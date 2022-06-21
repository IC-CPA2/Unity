#ifndef WIFI_H
#define WIFI_H

#include <string>
#include "mbed.h"
#include "ESP8266.h"

class Wifi
{
    public:
        Wifi();
        void RcvReply();
        void EnableWifi();
        void DisableWifi();
        void Connect(char * id, char * password);
        void GET(char * command);
        void StartAP(char * id, char * password);
    private:
        //PC communication
        Serial pc;//(P0_4, P0_0);
        
        //LEDS
        DigitalOut R;//(P0_23);
        DigitalOut B;//(P0_22);
        DigitalOut G;//(P0_14);
        
        //WIFI stuffs
        ESP8266 esp;//(P0_18,P0_17,115200);
        DigitalOut wifiEnable;//(P0_16);
        char ssid[32];// = "MadeOfCats";     // enter WiFi router ssid inside the quotes
        char pwd [32];// = "trueFactsAboutTheInternet"; // enter WiFi router password inside the quotes
         
        // Global variables
        char snd[255], rcv[1000]; // Strings for sending and receiving commands / data / replies
};

#endif
