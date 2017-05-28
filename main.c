/*
 * atmega_to_esp.c
 *
 * Created: 22-2-2017 22:27:24
 * Author : Tom
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "esp8266_sm.h"
#include "fsm.h"
#include "Client.h"

//#include "clientSocket.h"
#include "PubSubClient.h"
#include "ServiceDispatcher.h"
#include "ServiceDispatcherRouter.h"
#include "ServiceDispatcherQueue.h"
#include "x86.h"
#include "Client.h"

#include "serviceRGBServer.h"
#include "serviceRGBClient.h"

#define LOG_ERROR(...)     \
perror(__FUNCTION__);                   \
printf("ERROR::%s:%s:", __FUNCTION__, strerror(errno) );    \
printf(__VA_ARGS__);                    \
printf("\r\n");                         \
fflush(stdout);

#define LOG_INFO(...)     \
printf("INFO ::%s: ",__FUNCTION__);     \
printf(__VA_ARGS__);                    \
printf("\r\n");                         \
fflush(stdout);


unsigned long clock_millis = 0; //= ~49 days

ISR(TIMER1_COMPA_vect)  //will interrupt every 1 millisecond
{
    clock_millis++;
}

void timer_config(void)
{
    #define F_CPU                   (16000000UL)
    #define INTERVAL_IN_MICROSEC    (1000) /* every 1 millisecond */
    #define PRESCALER               (1024)
    #define CTC_MATCH_OVERFLOW      ((F_CPU / INTERVAL_IN_MICROSEC) / PRESCALER)

    TCCR1A = 0;// | (0<<WGM01) | (0<<WGM00); //Normal port operation, CTC MODE
    TCCR1B = (1<<WGM12);
    TCCR1B |= (1<<CS12) | (0<<CS11) | (1<<CS10); //start timer, and set prescaler at FCPU/1024
    OCR1AH = (CTC_MATCH_OVERFLOW>>8);
    OCR1AL = (CTC_MATCH_OVERFLOW);
    TIMSK1 |= (1<<OCIE1A);
}

unsigned long millis()
{
    return clock_millis;
}

int8_t cbPublishData(SDHandle_t sdqHandle, uint8_t *topic, uint8_t topicLength, uint8_t *data, uint8_t dataLength);

int8_t cbPublishData(SDHandle_t sdqHandle, uint8_t *topic, uint8_t topicLength, uint8_t *data, uint8_t dataLength)
{
    char *t = (char*)topic;
    topic[topicLength] = 0x0;
    LOG_INFO("Send %s", t);
    if (PubSubClient_publish(t, (uint8_t*)data, dataLength, 1))
    {
        LOG_INFO("    done");
    }
    return 0;
}

int8_t subscribeToTopic(char *topic)
{
    LOG_INFO("Subscribe to: %s", topic);
    return PubSubClient_subscribe(topic);
}


static void receive(uint8_t *data, uint8_t n, int16_t handle);

commEngine_fpHandles_t comm =
{
    &esp8266_Receive
};


int main(void)
{
    //clear WatchDog reset Flag!
    MCUSR &= ~(1<<WDRF);
    
    // 0 == input
    // 1 == output
    DDRD = (0<<PIND0) | (1<<PIND1);

    FSM_Init();
    UART_Init("x", &comm);
    timer_config();

    SDR_Init();
    SDQ_Init(&cbPublishData);

    unsigned char myLamp[9] = { 0x5B, 0xBF, 0xD5, 0xBA, 0x56, 0x26, 0x67, 0xDB, 0x11 }; //Lamp Tom!
    LivingColors_init();
    LivingColors_clearLamps();
    LivingColors_addLamp(myLamp);
    LivingColors_turnLampOnRGB(0, 255/*RED*/, 0 /*GREEN*/, 0/*BLUE*/);
     msgRGBServer_fpHandles_t LivingColors_handles = 
    {
        NULL,                           //msgRGBServer_fpHandleSetWhite_t setWhite;
        &LivingColors_turnLampOnRGB,    //msgRGBServer_fpHandleSetRGB_t setRGB;
    };

    //Let services subscribe
    S_RGBServer_Init(&SDR_SubscribeService);
    S_RGBServer_SetCallbacks(&LivingColors_handles);
    S_RGBClient_Init(&SDR_SubscribeService);

    //Activate ESP8266
    esp8266_Init();

    sei();

    //Init MQTT
    PubSubClient_initHostCallback((Client_t*)&espClientSock, &millis, "192.168.5.10",   1883, SDQ_AddToRecvQueue);
    PubSubClient_setMyAddress("NL\0","EHV\0","LC_Client\0");

    PubSubClient_connectId("mqttClient");
//    PubSubClient_subscribe("NL\EHV\PCClient\1\+");
//    PubSubClient_subscribe("NL\EHV\PCClient\2\+");
    SDR_SubscribeAllServices(&PubSubClient_subscribe);
    PubSubClient_publish("1/1", (const uint8_t*)"LIVING CLRS!", 12, 1);

    LivingColors_turnLampOnRGB(0/*lampID*/, 0/*RED*/, 255/*GREEN*/, 0/*BLUE*/);

    while (1) 
    {
        FSM_Feed();
        //UART_Task();
        PubSubClient_loop();
        SDQ_Thread();
    }
}

