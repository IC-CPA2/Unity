#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <iostream>
#include <WiFi.h>
#define VSPI_MISO MISO
#define VSPI_MOSI MOSI
#define VSPI_SCLK   SCK
#define VSPI_SS     SS
#define SCK 18
#define MISO 19
#define MOSI 23
#define CS 5
#define RST_PIN 4
#define SS_PIN 2
IPAddress gateway(192, 168, 14, 224);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);
WiFiClient client;
MFRC522 mfrc522(SS_PIN, RST_PIN);
const char *ssid = "kelvinmang";
const char *password = "98300683";
const uint16_t port = 12000;
const char *host = "192.168.14.148";
SPISettings settings(100000, MSBFIRST, SPI_MODE0);
uint8_t spi_counter[6];
uint16_t spi_val;
uint8_t spi_reg;
uint16_t spi_returnval;
void calcDistance();
void resetCounter();
void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
  }//removed local IP to avoid confusion.
}
void setup() {
  Serial.begin(9600);
  pinMode(VSPI_SS, OUTPUT);
  SPI.begin();
  spi_returnval = 0;
  mfrc522.PCD_Init();
  initWiFi();
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
}
void loop(){
  int nocol = 0;
  int redcount = 0;
  int bluecount = 0;
  int pinkcount = 0;
  int orangecount = 0;
  int greencount = 0;
  int structcount = 0;
  int dist = 0;
  int X = 0;
  int Y = 0;
  int reddist = 0;
  int bluedist = 0;
  int pinkdist = 0;
  int orgdist = 0;
  int grndist = 0;
  for(int i=0; i<10000; i++){
    SPI.beginTransaction(settings);
    digitalWrite(VSPI_SS, LOW);
    spi_val = SPI.transfer16(spi_returnval);
    spi_returnval = 0;
    digitalWrite(VSPI_SS, HIGH);
    SPI.endTransaction();
    if(spi_val%8==0){
      nocol++;
      int spi_val2 = spi_val/8;
    dist = dist + spi_val2%32;
    int spi_val3 = spi_val2/32;
    Y = Y + spi_val3%16;
    int spi_val4 = spi_val3/16;
    X= X + spi_val4;
    }
    if(spi_val%8==1){
      redcount++;
      int spi_val2 = spi_val/8;
      dist = dist + spi_val2%32;
      reddist = reddist + spi_val2%32;
      int spi_val3 = spi_val2/32;
      Y = Y + spi_val3%16;
      int spi_val4 = spi_val3/16;
      X= X + spi_val4;
    }
    else if(spi_val%8==2){
      bluecount++;
      int spi_val2 = spi_val/8;
      dist = dist + spi_val2%32;
      bluedist = bluedist + spi_val2%32;
      int spi_val3 = spi_val2/32;
      Y = Y + spi_val3%16;
      int spi_val4 = spi_val3/16;
      X= X + spi_val4;
    }
    else if(spi_val%8==4){
      pinkcount++;
      int spi_val2 = spi_val/8;
      dist = dist + spi_val2%32;
      pinkdist = pinkdist + spi_val2%32;
      int spi_val3 = spi_val2/32;
      Y = Y + spi_val3%16;
      int spi_val4 = spi_val3/16;
      X= X + spi_val4;
    }
    else if(spi_val%8==3){
      orangecount++;
      int spi_val2 = spi_val/8;
      dist = dist + spi_val2%32;
      orgdist = orgdist + spi_val2%32;
      int spi_val3 = spi_val2/32;
      Y = Y + spi_val3%16;
      int spi_val4 = spi_val3/16;
      X= X + spi_val4;
    }
    else if(spi_val%8==5){
      greencount++;
    }
    else if(spi_val%8==5){
      structcount++;
    }
    }  /*  
  Serial.print(dist);
  Serial.print("Red: ");
  Serial.println(redcount);
  Serial.print("Blue: ");
  Serial.println(bluecount);
  Serial.print("Pink: ");
  Serial.println(pinkcount);
  Serial.print("Orange: ");
  Serial.println(orangecount);
  Serial.print("Green: ");
  Serial.println(greencount);*/
  Serial.println(structcount);
  if(pinkcount + orangecount + redcount + bluecount + greencount < 50){
    Serial.println("No Ball");
  }
  else if(pinkcount > orangecount && pinkcount*25 >redcount  && bluecount < pinkcount * 0.5){
    Serial.println("Pink");
    Serial.println(pinkdist/pinkcount);
  }
  else if(orangecount > pinkcount && orangecount*15 > redcount && bluecount < orangecount * 0.5){
    Serial.println("Orange");
    Serial.println(orgdist/orangecount);
  }
  else if(redcount >= bluecount && redcount >= pinkcount && redcount >= orangecount && redcount >= greencount && redcount>0 && bluecount < 25){
    Serial.println("Red");
    Serial.println(reddist/redcount);
  }
  else if (bluecount>25){
    Serial.println("Blue");
    Serial.println(bluedist/bluecount);
  }
  else if(greencount >= bluecount && greencount >= pinkcount && greencount >= orangecount && greencount >= redcount && greencount>0){
    Serial.println("Green");
  }
  unsigned long duration_prior = 0;
  unsigned long interval = 30000;
  unsigned long pres_time = millis();
  if ((WiFi.status() != WL_CONNECTED) && (pres_time - duration_prior >= interval))
  {
    Serial.print(millis());
    Serial.println("Reconnecting to Wifi....");
    WiFi.disconnect();
    initWiFi();
    duration_prior = pres_time;
  }
  // Serial.println("debug1)");
  // Serial.println(client.connect(host, port));
  bool connected;
  std::string msg;
  connected = client.connect(host, port);
  int i = 0;
  int counter = 0;
  while (i==0)
  {
    if (!connected){
      // Serial.println("debug2)");
      // Serial.println("connection to host server failed");
      delay(1000);
      return;
    }
    // Serial.println("Connected to server successful!");
    if(pinkcount + orangecount + redcount + bluecount + greencount < 50){ client.println("T");}
    else if(pinkcount > orangecount && pinkcount*25 >redcount  && bluecount < pinkcount * 0.5){ client.println("PA");}
    else if(orangecount > pinkcount && orangecount*15 > redcount && bluecount < orangecount * 0.5){ client.println("OA");}
    else if(redcount >= bluecount && redcount >= pinkcount && redcount >= orangecount && redcount >= greencount && redcount>0 && bluecount < 25){client.println("RA");}
    else if (bluecount>25){client.println("BA");}
    else if(greencount >= bluecount && greencount >= pinkcount && greencount >= orangecount && greencount >= redcount && greencount>0){client.println("GA");}

    
    connected = true;
    while (client.available())
    {
      char c = client.read();
      msg.push_back(c);
      // Serial.println(c);
      if (c=='>'){
        Serial.println("in loop");
        i = 1;
      }
    }
    // client.stop();
    delay(1000);
    // Serial.println("debug4)");
  }
    // std::cout<<msg<<std::endl;
}





// #include <Arduino.h>
// #include <SPI.h>
// #include <Wire.h>
// #include <MFRC522.h>
// #include <iostream>
// #include <WiFi.h>
// #define SCK 18
// #define MISO 19
// #define MOSI 23
// #define CS 5
// #define RST_PIN 4
// #define SS_PIN 2
// // IPAddress local_IP(192, 168, 14, 184);
// IPAddress gateway(192, 168, 14, 224);
// IPAddress subnet(255, 255, 255, 0);
// IPAddress primaryDNS(8, 8, 8, 8);
// IPAddress secondaryDNS(8, 8, 4, 4);
// WiFiClient client;
// MFRC522 mfrc522(SS_PIN, RST_PIN);
// const char *ssid = "kelvinmang";
// const char *password = "98300683";
// const uint16_t port = 12000;
// const char *host = "192.168.14.148";
// // IPAddress gateway(192, 168, 14, 224);
// // IPAddress subnet(255, 255, 255, 0);
// // IPAddress primaryDNS(8, 8, 8, 8);
// // IPAddress secondaryDNS(8, 8, 4, 4);
// // WiFiClient client;
// // MFRC522 mfrc522(SS_PIN, RST_PIN);
// // const char *ssid = "kelvinmang";
// // const char *password = "98300683";
// // const uint16_t port = 12000;
// // const char *host = "192.168.14.148";
// void initWiFi()
// {
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
//   Serial.println("connecting to WiFi ..");
//   while (WiFi.status() != WL_CONNECTED)
//   {
//     Serial.println('.');
//     delay(1000);
//   }
//   // Serial.println(WiFi.localIP());
// }
// void setup()
// {
//   Serial.begin(115200);
//   SPI.begin();
//   mfrc522.PCD_Init();
//   // if(!WiFi.config(local_IP,gateway,subnet,primaryDNS,secondaryDNS)){
//   //  Serial.println("STA Failed to configure");
//   // }
//   initWiFi();
//   Serial.print("RRSI: ");
//   Serial.println(WiFi.RSSI());
// }
// void loop()
// {
//   unsigned long previousMillis = 0;
//   unsigned long interval = 30000;
//   unsigned long currentMillis = millis();
//   if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval))
//   {
//     Serial.print(millis());
//     Serial.println("Reconnecting to Wifi....");
//     WiFi.disconnect();
//     initWiFi();
//     previousMillis = currentMillis;
//   }
//   Serial.println("debug1)");
//   // Serial.println(client.connect(host, port));
//   bool connected;
//   std::string msg;
//   connected = client.connect(host, port);
//   // while (i==0){
//   //   Serial.println("Enter While Loop");
//   //   connected = true;
//   //     while (client.available())
//   //     {
//   //       i = 1;
//   //       Serial.println("WHAT's Not READ");
//   //       char c = client.read();
//   //       if(c=='>'){
//   //         i=1;
//   //       }
//   //       msg.push_back(c);
//   //       Serial.print(c);
//   //     }
//   //   }
//   // std::cout<<msg<<std::endl;
//   int i = 0;
//   int counter = 0;

//   while (i==0)
//   {
//     if (!connected)
//     {
//       Serial.println("debug2)");
//       Serial.println("connection to host server failed");
//       delay(1000);
//       return;
//     }
//     Serial.println("Connected to server successful!");
//     client.print("T");
//     connected = true;
//     while (client.available())
//     {
//       char c = client.read();
//       msg.push_back(c);
//       // Serial.println(c);
//       if (c=='>'){
//         i = 1;
//       }
//     }
//     // client.stop();
//     delay(1000);
//     Serial.println("debug4)");
//   }
//     std::cout<<msg<<std::endl;  
//     if (msg == "tryingfrom>"){
//         std::cout<<"AHHA"<<std::endl;
//     }else{
//         std::cout<<"JJJJ"<<std::endl;
//     }
// }



// #include <Arduino.h>
// #include <SPI.h>
// #include <Wire.h>
// #include <iostream>
// #include <string>
// #include <MFRC522.h>
// #include <WiFi.h>
// #define CS 5
// #define RST_PIN 4
// #define SS_PIN 2
// #define SCK 18
// #define MISO 19
// #define MOSI 23

// // IPAddress local_IP(192, 168, 1, 200); 
// IPAddress local_IP(192, 168, 14, 148); 
// //KELVIN
// // IPAddress local_IP(192, 168, 137, 88); 

// IPAddress gateway(192, 168, 1, 1);
// // IPAddress gateway(192, 168, 1, 254);//home settings
// // IPAddress gateway(146, 169, 144, 1);


// IPAddress subnet(255, 255, 240, 0);

// // IPAddress subnet(255, 255, 0, 0);
// IPAddress primaryDNS(8, 8, 8, 8);
// IPAddress secondaryDNS(8, 8, 4, 4);
// WiFiClient client;
// MFRC522 mfrc522(SS_PIN, RST_PIN);
// const char *ssid = "kelvinmang";
// // const char *ssid = "BT-98CK36";
// // const char *ssid = "Charmaine's iPhone";


// const char *password = "98300683";
// // const char *password = "dguFny9FpJeHrF";//James home wifi
// // const char *password = "123456789";

// const uint16_t port = 12000;
// const char *host = "192.168.14.148";
// // const char *host = "192.168.1.76";
// // const char *host = "146.169.146.203";


// void initWiFi()
// {
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
//   Serial.print("connecting to WiFi ..");
//   while (WiFi.status() != WL_CONNECTED)
//   {
//     Serial.print('.');
//     delay(1000);
//   }
//   Serial.println(WiFi.localIP());
// }
// void setup()
// {
//   Serial.begin(115200);
//   SPI.begin();
//   mfrc522.PCD_Init();
//   // if(!WiFi.config(local_IP,gateway,subnet,primaryDNS,secondaryDNS)){
//   //  Serial.println("STA Failed to configure");
//   // }
//   initWiFi();
//   Serial.print("RRSI: ");
//   Serial.println(WiFi.RSSI());
// }
// void loop()
// {
//   unsigned long previousMillis = 0;
//   unsigned long interval = 30000;
//   unsigned long currentMillis = millis();
//   if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval))
//   {
//     Serial.print(millis());
//     Serial.println("Reconnecting to Wifi....");
//     WiFi.disconnect();
//     initWiFi();
//     previousMillis = currentMillis;
//   }

//   // Serial.println(client.connect(host, port));
//   bool connected;
//   connected = client.connect(host, port);
  
//     if (!connected)
//     {
//       Serial.println("Cannot connect to server");
//       delay(1000);
//       return;
//     }
//     Serial.println("connected succesful");
//     std::string msg;
//     int i=0;
//     while (i==0)
//     {
//     Serial.println("WHAT's Not READ");

//     connected = true;
//       while (client.available())
//       {
//         Serial.println("WHAT's Not READ");
//         char c = client.read();
//         if(c=='>'){
//           i=1;
//         }
//         msg.push_back(c);
//         Serial.print(c);
//       }
//     }
//     std::cout<<msg<<std::endl;

//     delay(1000);


//     // std::string my_str;
//     Serial.println("Enter Coordinate Information");
//     // std::cin>>my_str;
//     // String ent_val = my_str.c_str();

//     client.print("T");
//     int counter = 0;


// }
  




// #include <Arduino.h>
// #include <WiFi.h>
// #include <SPI.h>
// #include <Wire.h>
// #include <MFRC522.h>
// #include <WiFiUdp.h>

// #define SCK 18
// #define MISO 19
// #define MOSI 23
// #define CS 5
// #define RST_PIN 4
// #define SS_PIN 2

// MFRC522 mfrc522(SS_PIN,RST_PIN);

// // const char* ssid = "Muli-iPhone";
// const char* ssid = "kelvinmang";
// const char* password = "98300683";

// // const char* password = "Issiesucks";
// const uint16_t port = 12000;
// const char* host = "192.168.139.148";
// const char* reconnecting = "false";

// //Set your Static IP address
// IPAddress local_IP (192, 168, 139, 148);
// //Set uoir Gateway IP Address
// IPAddress gateway(172, 20, 10, 1);

// // IPAddress subnet (255, 255, 0, 0);
// IPAddress subnet (255, 255, 255, 0);

// IPAddress primaryDNS(8,8,8,8); //optional
// IPAddress secondaryDNS(8,8,4,4); //optional

// char packetBuffer[255]; //buffer to hold incoming packet
// char  ReplyBuffer[] = "acknowledged"; // a string to send back

// //WiFiUDP Udp;




// WiFiServer server(12000);


// void setup(){
//     SPI.begin();
//     mfrc522.PCD_Init();
//     Serial.begin(115200);
//     delay(1000);

//     WiFi.mode(WIFI_STA); //Optional
//       if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)){
//     Serial.println("STA failed to configure");
//   }


//     WiFi.begin(ssid, password);
//     server.begin();
//     Serial.print("DEBUG Local ESP32 IP: ");
//     Serial.println(WiFi.localIP());
//     Serial.println(WiFi.SSID());
//     Serial.println(WiFi.gatewayIP());






//     Serial.println("\nConnecting NEW SETS");

//     while(WiFi.status() != WL_CONNECTED){
//         Serial.print(".");
//         delay(100);
//     }

//     Serial.println("\nConnected to the WiFi network");
//     Serial.print("Local ESP32 IP: ");
//     Serial.println(WiFi.localIP());
//     Serial.println(WiFi.SSID());
//     Serial.println(WiFi.gatewayIP());
 


//    // Udp.begin(port);    
// }


// unsigned long previousMillis = 0;
// unsigned long interval = 250;



// void loop(){
//   // if (mfrc522.PICC_IsNewCardPresent()) //RFID read here
//   // {
//   //   if (mfrc522.PICC_ReadCardSerial())
//   //   {
//   //     idcard = "";
//   //     for (byte i = 0; i < mfrc522.uid.size; i++){
//   //       idcard +=(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "")
//   //       + String(mfrc522.uid.uidByte[i], HEX);
//   //     }
      
//   //     Serial.println("tag rfid :" + idcard);

//   //     mfrc522.PICC_HaltA();
//   //     mfrc522.PCD_StopCrypto1();
//   //   }
//   // }
//     unsigned long currentMillis = millis();

//     //listening to packet from server

//     // int packetSize = Udp.parsePacket();  

//   // if (packetSize) {

//   //   Serial.print("Received packet of size ");

//   //   Serial.println(packetSize);

//   //   Serial.print("From ");

//   //   IPAddress remoteIp = Udp.remoteIP();

//   //   Serial.print(remoteIp);

//   //   Serial.print(", port ");

//   //   Serial.println(Udp.remotePort());

  

//   //   // read the packet into packetBufffer

//   //   int len = Udp.read(packetBuffer, 255);

//   //   if (len > 0) {

//   //     packetBuffer[len] = 0;

//   //   }

//   //   Serial.println("Contents:");

//   //   Serial.println(packetBuffer);

//   // }
  

  


// WiFiClient client;
// // Serial.print("TMD read, WIFI CLI PATCH");
// // delay(10);
// // Serial.println(client.read());

// // if(client.connect(host, port)){

// //   while(client.connected()){



// // //Serial.println(client.connected());
// //     if(client.connected()){
// //       client.stop();
// //       while(!client.available()){
// //       }
// //       msg_rec = client.read();
// //     }
// // break;

// //   }
// // }






// //

// if((currentMillis - previousMillis >= interval)){

//  Serial.println("Entering the if-at Bottom");
//   if (!client.connect(host, port)) {
//     Serial.println("Connection to host failed");
//     char mes_reg = client.read();
//     Serial.println(mes_reg);


//   } else {

//   if (client.available()>0){
//     Serial.println("Connected to server HAHHAHA!");
//     delay(10);
//     Serial.println("Debuggging Client read");
//     Serial.println(client.read());
//     delay(10);
//   }else{
//     delay(10);
//     Serial.println(client.read());

//     Serial.println("READ CLIENT");
//   }

 
//   client.write("Hello from ESP44!");
//   }

// if((WiFi.status() != WL_CONNECTED)){
//   Serial.print(millis());
//   Serial.println("Reconnecting to WiFi...");
//   WiFi.disconnect();
//   WiFi.reconnect();
//   reconnecting = "true";
// }

//   previousMillis = currentMillis;


// }     

// //if WiFi is down, try reconnecting

// if((WiFi.status() == WL_CONNECTED) && (reconnecting == "true")){
//     Serial.println("Reconnected successfully!");
//     reconnecting = "false";
// }

// }
