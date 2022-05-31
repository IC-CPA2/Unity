// #include <Arduino.h>

// #include <SPI.h>
// #include <Wire.h>
// #include <MFRC522.h>
// #include <Wifi.h>

// #define SCK 18
// #define MISO 19
// #define MOSI 23
// #define CS 5

// #define RST_PIN 4
// #define SS_PIN 2

// MFRC522 mfrc522(SS_PIN, RST_PIN);

// //Set your Static IP address
// IPAddress local_IP (192, 168, 1, 184);
// //Set uoir Gateway IP Address
// IPAddress gateway(192, 168, 1, 1);

// IPAddress subnet (255, 255, 0, 0);
// IPAddress primaryDNS(8,8,8,8); //optional
// IPAddress secondaryDNS(8,8,4,4); //optional

// const char* ssid = "Muli-iPhone";
// const char* password = "123456789";
// const uint16_t port = 5000;
// const char* host = "172.20.10.14";
// //Serial.print("initialising Wifi . . .");
// void initWifi(){
//     Serial.print("Connecting to Wifi . . .");
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);

//   while (WiFi.status() != WL_CONNECTED){
//     Serial.print('.');
//     delay(1000);
//   }
//   Serial.print("WiFi connected with IP: ");
//   Serial.println(WiFi.localIP());
// }

// void setup() {
//   SPI.begin(); //Init SPI bus
//   mfrc522.PCD_Init(); //Init MFRC522
  


// //Configure static IP address
//   if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)){
//     Serial.println("STA failed to configure");
//   }

//   initWifi();
// }

// void loop() {

// unsigned long currentMillis = millis();
// unsigned long previousMillis = 0;
// unsigned long interval = 30000;

// WiFiClient client;
 
//   if (!client.connect(host, port)) {
//     Serial.println("Connection to host failed");
//     delay(1000);
//     return;
//   }
 
//   Serial.println("Connected to server successful!");
 
//   client.print("Hello from ESP32!");


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

// //if WiFi is down, try reconnecting
// if((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval)){
//   Serial.print(millis());
//   Serial.println("Reconnecting to WiFi...");
//   WiFi.disconnect();
//   WiFi.reconnect();
//   previousMillis = currentMillis;
// }



// }
#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <WiFiUdp.h>


const char* ssid = "Muli-iPhone";
const char* password = "123456789";
const uint16_t port = 5000;
const char* host = "172.20.10.14";
const char* reconnecting = "false";

//Set your Static IP address
IPAddress local_IP (172, 20, 10, 3);
//Set uoir Gateway IP Address
IPAddress gateway(172, 20, 10, 1);

IPAddress subnet (255, 255, 0, 0);
IPAddress primaryDNS(8,8,8,8); //optional
IPAddress secondaryDNS(8,8,4,4); //optional

char packetBuffer[255]; //buffer to hold incoming packet
char  ReplyBuffer[] = "acknowledged"; // a string to send back

//WiFiUDP Udp;




WiFiServer server(5000);


void setup(){
    Serial.begin(115200);
    delay(1000);

    WiFi.mode(WIFI_STA); //Optional
      if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)){
    Serial.println("STA failed to configure");
  }


    WiFi.begin(ssid, password);
    server.begin();





    Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
    Serial.println(WiFi.SSID());
    Serial.println(WiFi.gatewayIP());


   // Udp.begin(port);    
}


unsigned long previousMillis = 0;
unsigned long interval = 1000;



void loop(){
    unsigned long currentMillis = millis();

    //listening to packet from server

  //   int packetSize = Udp.parsePacket();  

  // if (packetSize) {

  //   Serial.print("Received packet of size ");

  //   Serial.println(packetSize);

  //   Serial.print("From ");

  //   IPAddress remoteIp = Udp.remoteIP();

  //   Serial.print(remoteIp);

  //   Serial.print(", port ");

  //   Serial.println(Udp.remotePort());

  

  //   // read the packet into packetBufffer

  //   int len = Udp.read(packetBuffer, 255);

  //   if (len > 0) {

  //     packetBuffer[len] = 0;

  //   }

  //   Serial.println("Contents:");

  //   Serial.println(packetBuffer);

  // }
  

  


WiFiClient client = server.available();

// if(client.connect(host, port)){

//   while(client.connected()){



// //Serial.println(client.connected());
//     if(client.connected()){
//       client.stop();
//       while(!client.available()){
//       }
//       msg_rec = client.read();
//     }
// break;

//   }
// }






//

if((currentMillis - previousMillis >= interval)){
 
  if (!client.connect(host, port)) {
    Serial.println("Connection to host failed");

  } else {

 
  Serial.println("Connected to server successfully!");
 
  client.write("Hello from ESP32!");
  }

if((WiFi.status() != WL_CONNECTED)){
  Serial.print(millis());
  Serial.println("Reconnecting to WiFi...");
  WiFi.disconnect();
  WiFi.reconnect();
  reconnecting = "true";
}

  previousMillis = currentMillis;


}     

//if WiFi is down, try reconnecting

if((WiFi.status() == WL_CONNECTED) && (reconnecting == "true")){
    Serial.println("Reconnected successfully!");
    reconnecting = "false";
}

}
