#include "Wifi.h"

Wifi::Wifi():pc(P0_4, P0_0),R(P0_23),B(P0_22),G(P0_14),esp(P0_18,P0_17,115200),wifiEnable(P0_16)
{
    pc.baud(9600);
}

void Wifi::EnableWifi()
{
    R = 0;
    G = 0;
    B = 1;
    wifiEnable = 1;
    wait(1);
}

void Wifi::DisableWifi()
{
    R = 1;
    G = 0;
    B = 1;
    wifiEnable = 0;
    wait(1);
}

void Wifi::RcvReply()
{
    esp.RcvReply(rcv,400);
    pc.printf("%s", rcv);
}

void Wifi::StartAP(char * id, char *password)
{
    EnableWifi();
    pc.printf("START\r\n");
    wait(0.1);
    
    pc.printf("Reset ESP\r\n");
    esp.Reset();
    esp.RcvReply(rcv, 400);
    pc.printf("%s", rcv);
    wait(0.2);
    
    pc.printf("Sending AT\r\n");
    strcpy(snd, "AT");
    esp.SendCMD(snd);
    esp.RcvReply(rcv, 400);
    pc.printf("%s", rcv);
    wait(0.1);
    
    pc.printf("Set mode to AP\r\n");
    esp.SetMode(3);
    esp.RcvReply(rcv, 1000);
    pc.printf("%s", rcv);
    wait(0.1);
    
    pc.printf("Reset ESP\r\n");
    esp.Reset();
    esp.RcvReply(rcv, 400);
    pc.printf("%s", rcv);
    wait(0.2);
    
    pc.printf("Creating Network\r\n");
    strcpy(snd,"AT+CWSAP=\"MEOW\",\"testing123\",5,3");
    esp.SendCMD(snd);
    //esp.StartAPMode(id,password, "1", "4");
    esp.RcvReply(rcv, 1000);
    pc.printf("%s", rcv);
    wait(1);
    
    pc.printf("Getting IP\r\n");
    esp.GetIP(rcv);
    pc.printf("%s", rcv);
    wait(0.1);
    //DisableWifi();
}

void Wifi::Connect(char * id, char * password)
{
    strcpy(ssid,"MadeOfCats");     // enter WiFi router ssid inside the quotes
    strcpy(pwd,"trueFactsAboutTheInternet"); // enter WiFi router password inside the quotes
        
    EnableWifi();
    pc.printf("START\r\n");
    wait(0.3);
    
    pc.printf("Reset ESP\r\n");
    esp.Reset();
    esp.RcvReply(rcv, 400);
    pc.printf("%s", rcv);
    wait(0.2);
    
    pc.printf("Sending AT\r\n");
    strcpy(snd, "AT");
    esp.SendCMD(snd);
    esp.RcvReply(rcv, 400);
    pc.printf("%s", rcv);
    wait(0.2);
    
    pc.printf("Set mode to AP\r\n");
    esp.SetMode(1);
    esp.RcvReply(rcv, 1000);
    pc.printf("%s", rcv);
    wait(0.2);
    
    /*pc.printf("Receiving Wifi List\r\n");
    esp.GetList(rcv);
    pc.printf("%s", rcv);
    wait(5);*/
    
    pc.printf("Connecting to AP\r\n");
    esp.Join(ssid, pwd); // Replace MyAP and MyPasswd with your SSID and password
    esp.RcvReply(rcv, 1000);
    pc.printf("%s", rcv);
    wait(8);
    
    pc.printf("Reset ESP\r\n");
    esp.Reset();
    esp.RcvReply(rcv, 400);
    pc.printf("%s", rcv);
    wait(0.2);
    
    pc.printf("Getting IP\r\n");
    esp.GetIP(rcv);
    pc.printf("%s", rcv);
    wait(0.2);
    DisableWifi();
}

//TODO make it so that this parses the GET command properly
void Wifi::GET(char * command)
{
    EnableWifi();
    //WIFI updates the Status to IFTTT servers//
    pc.printf("Setting into multichannel mode\r\n");
    strcpy(snd,"AT+CIPMUX=1");
    esp.SendCMD(snd);//Setting WiFi into MultiChannel mode
    esp.RcvReply(rcv, 1000);
    pc.printf("%s", rcv);
    wait(0.2);
    
    pc.printf("Establishing connection with ThingSpeak server\r\n");
    strcpy(snd,"AT+CIPSTART=4,\"TCP\",\"api.thingspeak.com\",80");
    esp.SendCMD(snd); //Initiate connection with IFTTT server 
    esp.RcvReply(rcv, 1000);
    pc.printf("%s", rcv);
    wait(0.5); 
    
    pc.printf("Sending 80 bytes\r\n");
    strcpy(snd,"AT+CIPSEND=4,80");
    esp.SendCMD(snd);
    esp.RcvReply(rcv, 1000);
    pc.printf("%s", rcv);
    wait(0.5);    
    
    pc.printf("Post 0 to ThingSpeak\r\n");
    sprintf(snd,"GET https://api.thingspeak.com/update?key=KG2Q7LSCFQ4PRNL8&field1=0 HTTP/1.0\r\n\r\n"); //Post values to IFTTT
    esp.SendCMD(snd);
    esp.RcvReply(rcv, 1000);
    pc.printf("%s", rcv);
    wait(0.3);
    
    pc.printf("Closing the connection\r\n");
    strcpy(snd,"AT+CIPCLOSE"); //Close the connection to server
    esp.SendCMD(snd);
    esp.RcvReply(rcv, 1000);
    pc.printf("%s", rcv);
    DisableWifi();
}
