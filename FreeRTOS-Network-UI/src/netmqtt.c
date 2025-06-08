#include <FreeRTOS.h>
#include "netmqtt.h"
#include "MQTTPacket.h"
#include "transport.h"
#include <stdbool.h>
#include <time.h>
#include "rtosespserial.h"
#include <string.h>
#include <task.h>
#include <queue.h>
#include "netwifi.h"
#include "nettransport.h"

typedef struct {
	uint8_t id;
	char * payload;
	int size;
} MQTTSend_t;

static TickType_t refresh_rate_tick = pdMS_TO_TICKS(NETWORK_MQTT_REFRESH_PERIOD_MS);


static TaskHandle_t xMQTTTask = NULL;
static QueueHandle_t xMQTTRECVQueue = NULL;
static QueueHandle_t xMQTTSENDQueue = NULL;
static bool init = false;


static unsigned long connection_keepalive_s;
static unsigned long mqtt_port;
static char mqtt_address[NETWORK_MQTT_ADDRESS_LENGTH + 1];
static char mqtt_device_token[NETWORK_MQTT_DEVICE_TOKEN_LENGTH + 1];

static void netMQTT_Task(void *pvParameters);

enum mqttstate_t {BOOT, IDLE, WIFI_CHECK, TCP_START, TCP_CLOSE, CONN, WAIT_CONNECT, READY, PUB, SUB, UNSUB, SUSPEND};
static int state;



bool netMQTT_Init(){
	if(init)
		return true;

	if((xMQTTSENDQueue = xQueueCreate(NETWORK_MQTT_QUEUES_SIZE, sizeof(MQTTSend_t))) == NULL)
		return false;
	vQueueAddToRegistry(xMQTTSENDQueue, "MQTT_SENDQueue");

	if((xMQTTRECVQueue = xQueueCreate(1, sizeof(bool))) == NULL)
		return false;
	vQueueAddToRegistry(xMQTTRECVQueue, "MQTT_RECVQueue");

	if(xTaskCreate(netMQTT_Task, "MQTT_Task", NETWORK_MQTT_TASK_STACK_SIZE, NULL, 1, &xMQTTTask) != pdPASS)
		return false;

	init = true;
	return true;
}


void netMQTT_Config(char* address, char* token, unsigned long port, unsigned long keepalive){

	connection_keepalive_s = keepalive;
	mqtt_port = port;
	strcpy(mqtt_address, address);
	strcpy(mqtt_device_token, token);
}

bool netMQTT_Ready(){
	return (state == READY) ? true : false;
}

void netMQTT_Start(){
	if(state != IDLE)
		return;
	xTaskNotifyGive(xMQTTTask);
}

void netMQTT_Stop(){
	if(!netMQTT_Ready() )
		return;

	MQTTSend_t * messageToSend = pvPortMalloc(sizeof(MQTTSend_t));

	messageToSend->id = SUSPEND;

	if(xQueueSend(xMQTTSENDQueue, messageToSend, portMAX_DELAY) == pdPASS);

	bool res;
	if (xQueueReceive(xMQTTRECVQueue, &res, portMAX_DELAY) == pdPASS);


	vPortFree(messageToSend);

}



bool netMQTT_Publish(char * key, char * value){

	if((key == NULL) || (value == NULL))
		return false;

	MQTTSend_t * messageToSend = pvPortMalloc(sizeof(MQTTSend_t));

	messageToSend->id = PUB;

	messageToSend->payload = pvPortMalloc(7 + strlen(key) + strlen(value) + 1);

	messageToSend->size = sprintf((char *) (messageToSend->payload), "{\"%s\":%s}", key, value);


	xQueueSend(xMQTTSENDQueue, messageToSend, 0);

	vPortFree(messageToSend);

	return true;
}



static void netMQTT_Task(void *pvParameters) {
	TickType_t refresh_start_time;
	bool wificonnected = false;
	MQTTSend_t receivedMessage;
	unsigned char buffer[128];
	MQTTTransport transporter;
	int result;
	int length;
	bool res;
	// ESP8266 Transport Layer
	static transport_iofunctions_t iof = { netTRANSPORT_Send, netTRANSPORT_Recv };
	int transport_socket = transport_open(&iof);
	TickType_t start;
	state = BOOT;
	while (true) {
		switch (state) {
		case BOOT:
			start = xTaskGetTickCount();
			vTaskDelayUntil(&start, pdMS_TO_TICKS(MQTT_NETWORK_CONNECT_TIMEOUT));
			state = WIFI_CHECK;
			break;

		case IDLE:

			//printf("blocked\n");
			ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
			//printf("unblocked\n");
			state = WIFI_CHECK;
			break;

		case WIFI_CHECK:
			netWIFI_Check(&wificonnected);
			if (wificonnected)
				state = TCP_START;
			else
				state = IDLE;
			break;
		case TCP_START:
			if (netTCP_Start(mqtt_address, mqtt_port, connection_keepalive_s)){
				state = CONN;
			}
			else
				state = TCP_CLOSE;
			break;
		case CONN:
			transporter.sck = &transport_socket;
			transporter.getfn = transport_getdatanb;
			transporter.state = 0;

			MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
			connectData.MQTTVersion = 3;
			connectData.username.cstring = mqtt_device_token;
			connectData.clientID.cstring = mqtt_device_token;
			connectData.keepAliveInterval = connection_keepalive_s * 2;
			length = MQTTSerialize_connect(buffer, sizeof(buffer), &connectData);
			// Send CONNECT to the mqtt broker.
			if ((result = transport_sendPacketBuffer(transport_socket, buffer, length)) == length) {
				state = WAIT_CONNECT;
			} else {
				state = TCP_CLOSE;
			}
			break;
		case WAIT_CONNECT:
			// Wait for CONNACK response from the MQTT broker.
			while (true) {
				if ((result = MQTTPacket_readnb(buffer, sizeof(buffer), &transporter)) == CONNACK) {
					// Check if the connection was accepted.
					unsigned char sessionPresent, connack_rc;
					if ((MQTTDeserialize_connack(&sessionPresent, &connack_rc, buffer,
							sizeof(buffer)) != 1) || (connack_rc != 0)) {
						state = TCP_CLOSE;
						break;
					} else {
						state = READY;
						break;
					}
				} else if (result == -1) {
					state = TCP_CLOSE;
					break;
				}
			}
			break;
		case READY:
			//printf("ready\n");
			if (xQueueReceive(xMQTTSENDQueue, &receivedMessage, portMAX_DELAY) == pdPASS) {
				switch(receivedMessage.id){
				case PUB:
					;MQTTString topicString = MQTTString_initializer;
					topicString.cstring = "v1/devices/me/telemetry";
					unsigned char  *payload = pvPortMalloc(receivedMessage.size);
					strcpy((char *)payload, receivedMessage.payload);
					vPortFree(receivedMessage.payload);
					length = receivedMessage.size;
					length = MQTTSerialize_publish(buffer, sizeof(buffer), 0, 0, 0, 0, topicString, payload, length);
					// Send PUBLISH to the MQTT broker.
					if ((result = transport_sendPacketBuffer(transport_socket, buffer, length)) != length){
						state = TCP_CLOSE;
					}
					vPortFree(payload);
					break;
				case SUB:

					break;
				case UNSUB:

					break;
				case SUSPEND:
					res = true;
					xQueueSend(xMQTTRECVQueue, &res, portMAX_DELAY);
					state = TCP_CLOSE;
					break;
				}
			}
			break;
		case TCP_CLOSE:
			netTCP_Close();
			state = IDLE;
			break;
		default:
			state = IDLE;
			break;
		}
		refresh_start_time = xTaskGetTickCount();
		vTaskDelayUntil(&refresh_start_time, refresh_rate_tick);
	}
}
