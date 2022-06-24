
//Include the libraries that we need
#include <Wire.h>
#include <INA219_WE.h>
#include <SPI.h>
#include <SD.h>

INA219_WE ina219; // this is the instantiation of the library for the current sensor

float pwm_out; 
float vin_present, vin_old,vout;
float current_present, current_old;
float power_present, power_old;
String dataString;

unsigned int sensorValue0, sensorValue1; //ADC sample values declaration
unsigned int loop_trigger; 
unsigned int int_count=0; // a variables to count the interrupts. Used for program debugging.

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

const int chipSelect = 10; //hardwired chip select for the SD card
float Ts = 0.001; //1 kHz control frequency.

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  noInterrupts(); //disable all interrupts
  pinMode(13, OUTPUT); //LED on Pin13 to indicate status
  analogReference(EXTERNAL); // We are using an external analogue reference for the ADC

  // TimerA0 initialization for control-loop interrupt.
  
  TCA0.SINGLE.PER = 999; //
  TCA0.SINGLE.CMP1 = 999; //
  TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc | TCA_SINGLE_ENABLE_bm; //16 prescaler, 1M.
  TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP1_bm; 

  // TimerB0 initialization for PWM output
  
  pinMode(6, OUTPUT);
  pinMode(A0,INPUT);
  pinMode(A3,INPUT);
  TCB0.CTRLA=TCB_CLKSEL_CLKDIV1_gc | TCB_ENABLE_bm; //62.5kHz
  analogWrite(6,120); 

  interrupts();  //enable interrupts.
  Wire.begin(); // We need this for the i2c comms for the current sensor
  ina219.init(); // this initiates the current sensor
  Wire.setClock(700000); // set the comms speed for i2c

  //Check for the SD Card
  Serial.println("\nInitializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("* is a card inserted?");
    while (true) {} //It will stick here FOREVER if no SD is in on boot
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  if (SD.exists("SD_Test.csv")) { // Wipe the datalog when starting
    SD.remove("SD_Test.csv");
  }
}





void loop() {
    if (loop_trigger == 1){ // FAST LOOP (1kHz)
      
    pwm_out = saturation(pwm_out,0.98,0.02);
    pwm_modulate(pwm_out);
    
    int_count++;
    loop_trigger = 0;
    }
    
    if (int_count ==1000){ // SLOW LOOP (1Hz)
      
    // Measure input current from port B in Amp
    current_present = -(ina219.getCurrent_mA());

//    //for debugging - print present input current
//    Serial.print("Present Current: (mA)");
//    Serial.println(current_present);

    // Measure input voltage from port B 
    sensorValue0 = analogRead(A0);
    vin_present = sensorValue0 * 4.096/1.03 *3.546;

//    //for debugging - print old and present input voltage
//    Serial.print("Old input voltage (mV): ");
//    Serial.println(vin_old);
//    Serial.print("Present Vin (mV): ");
//    Serial.println(vin_present);
 

    // current power from solar panel
    power_present = vin_present*current_present;

//    //for bebugging - print old and present power
//    Serial.print("Old Power  (mW): ");
//    Serial.println(power_old/1000);
//    Serial.print("Present Power (mW): ");
//    Serial.println(power_present/1000);
//
//    Serial.print("Old Duty Cycle: ");
//    Serial.println(pwm_out);

    if(power_present>power_old){
      if(vin_present>vin_old){
        pwm_out = pwm_out - 0.01;
      } else{
        pwm_out = pwm_out + 0.01;
      }
    } else {
      if (vin_present>vin_old){
        pwm_out = pwm_out + 0.01;
      } else {
        pwm_out = pwm_out - 0.01;
      }
    }

    //Measure output voltage from port A
    
    vout = analogRead(A3) * (4.096/1.03) * 2.05 * 2.70 ; // I am using 2 470k resistor as the potential divider
//    Serial.print("Vout: ");
//    Serial.println(vout);
    vin_old = vin_present;
    power_old = power_present;

//    //Print updated duty cycle, used for debugging
//    Serial.print("New Duty Cycle: ");
//    Serial.println(pwm_out);

  dataString = String("Output Voltage from 1st SMPS (V)") + "," + String(vout/1000) + "," + String("Input Voltage (V)") + "," + String(vin_present/1000) + "," + String("Input Current(A) ") + "," + String(current_present/1000) + "," + String("1st SMPS Duty Cycle") + "," + String(pwm_out) + "," + String("Input Power(mW) ") + "," + String(power_present/1000);//build a datastring for the CSV file
   Serial.println(dataString); // send it to serial as well in case a computer is connected
    
    File dataFile = SD.open("SD_Test.csv", FILE_WRITE); // open our CSV file
   if (dataFile){ //If we succeeded (usually this fails if the SD card is out)
     dataFile.println(dataString); // print the data
    } else {
     Serial.println("File not open"); //otherwise print an error
    }
    dataFile.close(); // close the file
    int_count = 0;
    }
}

// Saturation function
float saturation( float sat_input, float uplim, float lowlim) { 

  if (sat_input > uplim) sat_input = uplim;
  else if (sat_input < lowlim ) sat_input = lowlim;
  else;
  return sat_input;
}

void pwm_modulate(float pwm_input){ // PWM function
  analogWrite(6,(int)(pwm_input*255)); 
}

// Timer A CMP1 interrupt. Every 1000us the program enters this interrupt. This is the fast 1kHz loop
ISR(TCA0_CMP1_vect) {
  loop_trigger = 1; //trigger the loop when we are back in normal flow
  TCA0.SINGLE.INTFLAGS |= TCA_SINGLE_CMP1_bm; //clear interrupt flag
}
