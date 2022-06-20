
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <iostream>
#include <WiFi.h>
#include <drive.h>
#define SCK 18
#define MISO 19
#define MOSI 23
#define CS 5
#define RST_PIN 4
#define SS_PIN 2
WiFiClient client;
MFRC522 mfrc522(SS_PIN, RST_PIN);
const char *ssid = "JamesO"; // the ssid field here is the name of your wifi
const char *password = "123456789";
const uint16_t port = 12000;
// const char *host = "192.168.14.148";
const char *host = "192.168.137.131"; // this is the host address of the LAN
// total translation of the rover
//  double total_translation_x,total_translation_y;
// Motors motors;
Drive driveUnity;
void initWiFi()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print('.');
        delay(1000);
    }
}
void setup()
{
    Serial.begin(9600);
    SPI.begin();
    mfrc522.PCD_Init();
    initWiFi();
    Serial.print("RRSI: ");
    Serial.println(WiFi.RSSI());
    driveUnity.setup();
    // delay to initialise everyything properly
    delay(2000);
}
void loop()
{
    unsigned long previousMillis = 0;
    unsigned long interval = 30000;
    unsigned long currentMillis = millis();
    if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval))
    {
        Serial.print(millis());
        Serial.println("Need To Reconnect....");
        WiFi.disconnect();
        initWiFi();
        previousMillis = currentMillis;
    }

    String msg = "";
    bool connected;
    connected = client.connect(host, port);
    int i = 0;
    while (i == 0)
    {
        if (!connected)
        {
            Serial.println("connection to host server failed");
            delay(1000);
            return;
        }
        Serial.println("Connected to server successful!");
        client.print("Hello from ESP32!");
        connected = true;
        while (client.available())
        {
            char c = client.read();
            Serial.println(c);
            // msg.concat(c);
            if (c == '>')
            {
                i = 1;
                break;
            }
        }
        client.stop();
        delay(1000);
    }
    Serial.println("DEG");

    int ind_semicolon = msg.indexOf(";", 0);
    String str_param = msg.substring(0, ind_semicolon); // get the
    Serial.println("Hello");
    Serial.println(msg);

    Serial.println(str_param);
    int straight_turn = str_param.toInt();
    msg = msg.substring(ind_semicolon + 1, msg.length());

    ind_semicolon = msg.indexOf(";", 0);
    str_param = msg.substring(0, ind_semicolon);
    Serial.println(str_param);
    // get the
    int straight_speed = str_param.toInt();
    msg = msg.substring(ind_semicolon + 1, msg.length());

    ind_semicolon = msg.indexOf(";", 0);
    str_param = msg.substring(0, ind_semicolon);

    Serial.println(str_param);
    // get the
    int turnsp = str_param.toInt();
    msg = msg.substring(ind_semicolon + 1, msg.length());

    ind_semicolon = msg.indexOf(";", 0);
    str_param = msg.substring(0, ind_semicolon);
    Serial.println(str_param);
    // get the
    float motorprop = str_param.toFloat();
    msg = msg.substring(ind_semicolon + 1, msg.length());
    delay(500);

    //  optical_measurements();
    //  Serial.println("Forward distance:");
    //  Serial.println(driveUnity.coord_y);
    // optical_measurements();
    // 666 coordinate_y difference translates to around 15cm of translation
    // driveUnity.forward_distance(4, 20);//Kp
    // driveUnity.turn(90, true);
    // driveUnity.forward_distance(4, 20);//
    // driveUnity.turn(90, false);
    // if (driveUnity.coord_y < 66600)
    // {
    //     //   driveUnity.forward_distance(2, 1000);
    // }
    // else
    // {
    //     driveUnity.turn(90, true);
    //     driveUnity.turn(90, false);
    //     driveUnity.brake();
    //     Serial.println("Heading angle");
    //     Serial.println(roverUnity.head_angle);
    // }
}

// #include <Arduino.h>

// #include <drive.h>

// // total translation of the rover
// //  double total_translation_x,total_translation_y;

// // Motors motors;

// Drive driveUnity;

// void setup()
// {

//     driveUnity.setup();
//     // delay to initialise everyything properly
//     delay(2000);
// }

// void loop()
// {
//     delay(500);
//     //  optical_measurements();
//     //  Serial.println("Forward distance:");
//     //  Serial.println(driveUnity.coord_y);

//     // optical_measurements();

//     // 666 coordinate_y difference translates to around 15cm of translation
//     driveUnity.forward_distance(4, 20);
//     driveUnity.turn(90, true);
//     driveUnity.forward_distance(4, 20);
//     driveUnity.turn(90, false);

//     if (driveUnity.coord_y < 66600)
//     {

//         //   driveUnity.forward_distance(2, 1000);
//     }
//     else
//     {
//         driveUnity.turn(90, true);
//         driveUnity.turn(90, false);
//         driveUnity.brake();
//         Serial.println("Heading angle");
//         Serial.println(roverUnity.head_angle);
//     }
// }

// void loop(){

// total_translation_x,total_translation_y = optical_measurements();

//     //speed is to be between -10 and 10
//     int rover_speed = 5;

//     //translation in centimetres
//     int rover_translation = 10000;

//     //relative turning angle compared to current position
//     int rover_rotation_angle = 0;

//     //the straightness error
//     int error = 0; //calculate this based on total_translations and knowledge on the current direction

//     //motors_control(rover_speed,rover_translation,rover_rotation_angle,error);
//     if(coord_y > 10000){
//         motors.brake();
//     }else{
//         Serial.println("total translation x:");
//         Serial.println(coord_x);
//        motors.forward(3,coord_x);
//         //motors.turn(90);
//         //motors.turn(90,false);
//     }
// }
