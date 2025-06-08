#include "netntp.h"
#include <FreeRTOS.h>
#include <time.h>
#include <task.h>
#include <string.h>
#include <nettransport.h>
#include <netwifi.h>
#include "rtosespserial.h"
#include "rtostimeout.h"

typedef struct {
	uint8_t livnmode; 	// Eight bits, li, vn, and mode.
						// li. Two bits. Leap indicator.
						// vn. Three bits. Version number of the protocol.
						// mode. Three bits. Client will pick mode 3 for client.

	uint8_t stratum; 	// Eight bits. Stratum level of the local clock.
	uint8_t poll; 		// Eight bits. Maximum interval between successive messages.
	uint8_t precision; 	// Eight bits. Precision of the local clock.
	uint32_t rootDelay; // 32 bits. Total round trip delay time.
	uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
	uint32_t refid; // 32 bits. Reference clock identifier.
	uint32_t refTms; // 32 bits. Reference time-stamp seconds.
	uint32_t refTmf; // 32 bits. Reference time-stamp fraction of a second.
	uint32_t origTms;// 32 bits. Originate time-stamp seconds.
	uint32_t origTm_f;// 32 bits. Originate time-stamp fraction of a second.
	uint32_t rxTms;	// 32	bits.	Received	time-stamp	seconds.
	uint32_t rxTmf;// 32	bits.	Received	time-stamp	fraction	of	a	second.
	uint32_t txTms;// 32	bits.	Transmit	time-stamp	seconds.	Important
	uint32_t txTmf;// 32	bits.	Transmit	time-stamp	fraction	of	a	second.
} NTPPacket_t; // Total: 384 bits or 48 bytes.


static const NTPPacket_t NTP_PACKET = {0xe3, 0, 6, 0x3c, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};//__htonl(0xEC0600E3); // header pedido ntp
static const int NTP_PACKET_SIZE = sizeof(NTPPacket_t);
static struct tm a = {0, 0, 0, 1, 1, 1900, 0}; //
static struct tm b = {0, 0, 0, 1, 1, 1970, 0}; //ref time_t
#define SEVENTYYEARS  (mktime(&b) - mktime(&a))
static bool init = false;
static NTPPacket_t ntp_packet;

QueueHandle_t ntp_send_queue = NULL;
QueueHandle_t ntp_recv_queue = NULL;

static void netNTP_Task(void * arg);
time_t netNTP_xGetTime(int hour_adjust);


bool netNTP_Init(){
	if(init)
		return true;

	if((ntp_send_queue = xQueueCreate(NETWORK_NTP_QUEUES_SIZE, sizeof(int))) == NULL)
		return false;

	if((ntp_recv_queue = xQueueCreate(NETWORK_NTP_QUEUES_SIZE, sizeof(time_t))) == NULL)
		return false;

	if(xTaskCreate(netNTP_Task, "NTP_Task", NETWORK_NTP_TASK_STACK_SIZE, NULL, 1, NULL) != pdPASS)
		return false;

	init = true;
	return true;

}

time_t netNTP_GetTime(int hour_adjust){

	int * a = pvPortMalloc(sizeof(int));
	time_t r;

	*a = hour_adjust;

	xQueueSend(ntp_send_queue, a, portMAX_DELAY);
	xQueueReceive(ntp_recv_queue, &r, portMAX_DELAY);

	vPortFree(a);

	return r;
}

static void netNTP_Task(void * arg){

	int instruction;
	time_t res;
	while(1){
		xQueueReceive(ntp_send_queue, &instruction, portMAX_DELAY);
		res = netNTP_xGetTime(instruction);
		xQueueSend(ntp_recv_queue, &res, portMAX_DELAY);
	}


}

time_t netNTP_xGetTime(int hour_adjust){

	bool wificonnected = false;
	time_t secs_since_1970 = 0;
	time_t secs_since_1900 = 0;
	int state = 0;
	memset(&ntp_packet, 0, NTP_PACKET_SIZE);
	int bytes_recv = 0;
	int bytes_sent = 0;
	bool loop = true;
	while (loop){
		switch (state){
			case 0:
				netWIFI_Check(&wificonnected);
				if(wificonnected)
					state++;
				else
					loop = false;
				break;

			case 1:
				if(netUDP_Start("pool.ntp.org", 123))
					state++;
				else
					state = 5;
				break;
			case 2:
				bytes_sent = netTRANSPORT_Send((unsigned char*)&NTP_PACKET, NTP_PACKET_SIZE);
				if(bytes_sent == NTP_PACKET_SIZE)//NTP_PACKET_SIZE
					state++;
				else
					state = 5;
				break;
			case 3:
				bytes_recv += netTRANSPORT_Recv((uint8_t*)(&ntp_packet + bytes_recv), NTP_PACKET_SIZE - bytes_recv);
				if(bytes_recv == 0)
					state = 5;
				else if(bytes_recv == NTP_PACKET_SIZE)
					state++;
				break;
			case 4:
				secs_since_1970 = __ntohl((uint32_t)(ntp_packet.rxTms));
				//converte de big-endian para o formato do sistema, filtra os
				//bytes relevantes (segundos desde 1900, bytes 40-43) e ajusta
				//time_t (segundos desde 1970)
				secs_since_1900 = secs_since_1970 - SEVENTYYEARS + 3600 * hour_adjust;

				state++;
				break;
			case 5:
				netUDP_Close();
				loop = false;
				break;

		}
	}
	return secs_since_1900;
}

