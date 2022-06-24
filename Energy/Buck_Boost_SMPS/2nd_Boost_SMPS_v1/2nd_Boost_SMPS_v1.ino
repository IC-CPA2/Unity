//Include the libraries that we need
#include <Wire.h>
#include <INA219_WE.h>

INA219_WE ina219; // this is the instantiation of the library for the current sensor

float pwm_out; 
float vin_present, vin_old, vpd, vout;
float v_diff = 0; 
float vref= 4700;
float current_present, current_old;
float power_present, power_old;


float current_limit = 2.0;
unsigned int sensorValue0, sensorValue1; //ADC sample values declaration
unsigned int loop_trigger; 
unsigned int int_count=0; // a variables to count the interrupts. Used for program debugging.
unsigned int con_count = 0; // a variables to count the interrupts. Used for program debugging 

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
}

void loop() {
  // put your main code here, to run repeatedly:
  if (loop_trigger == 1) { // FAST LOOP (1kHz)

    // output voltage from port A
    vpd = analogRead(A3) * (4.096/1.03) * 2.05 ; // I am using 2 470k resistor as the potential divider
    vout = vpd * 2.70;
    
    v_diff = vout - vref; //Calculate the difference between output voltage and voltage reference

    if (v_diff>0){
      pwm_out = pwm_out - 0.01; //We are above vref so less duty cycle 
    }else if (v_diff<0){
      pwm_out = pwm_out + 0.01; // We are below vref so more duty cycle
    } else {
      pwm_out = pwm_out ; 
    }
    pwm_out = saturation(pwm_out,0.98,0.02); //Saturate the duty cycle at the reference of a min of 0.02
    pwm_modulate(pwm_out); // send the pwm signal out 

   //Serial.print("Voltage reference: ");
   //Serial.println(vref);

   //Serial.print("Vout: ");
   //Serial.println(vout);
   
   //Serial.print("v_diff:  ");
   //Serial.println(v_diff);

    int_count++ ;
    loop_trigger = 0 ; 
  }

  if (int_count == 1000) { // SLOW LOOP (1Hz)
    //Measure input current from port B in Amp
    current_present = -(ina219.getCurrent_mA()); 

    //for debugging - print present input current
    Serial.print("Present Current: (mA)");
    Serial.println(current_present);

    // Measure input voltage from port B 
    sensorValue0 = analogRead(A0);
    vin_present = sensorValue0 * 4.096/1.03 * 3.546; // using 560k and 200k resistor as potential divider

    // input power from port B 
    power_present = vin_present * current_present ; 

    if (vref >= 4500 && vref <= 5200){ // When voltage referenece between 4.5V and 5.2V
      if (power_present > power_old){
        vref = vref + 50 ; //Increase voltage reference by 50mV
      } else if (power_present < power_old){
        vref = vref - 50 ; //Decrease voltage reference by 50mV
      } else{
        vref = vref; // voltage reference remains unchange 
      }
    }

    vin_old = vin_present ; 
    power_old = power_present;

   //for debugging - print voltage reference 
   Serial.print("Voltage reference: ");
   Serial.println(vref);
   
   //for bebugging - print output voltage 
   Serial.print("Vout: ");
   Serial.println(vout);

   //for bebugging - print duty cycle 
   Serial.print("Duty Cycle: ");
   Serial.println(pwm_out);

    int_count = 0; 
  }



}

// Timer A CMP1 interrupt. Every 1000us the program enters this interrupt. This is the fast 1kHz loop
ISR(TCA0_CMP1_vect) {
  loop_trigger = 1; //trigger the loop when we are back in normal flow
  TCA0.SINGLE.INTFLAGS |= TCA_SINGLE_CMP1_bm; //clear interrupt flag
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
