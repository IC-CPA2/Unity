
//Include the libraries that we need
#include <Wire.h>
#include <INA219_WE.h>

INA219_WE ina219; // this is the instantiation of the library for the current sensor

float open_loop; // Duty Cycles
float vcout, vref, iL, dutyref,current_mA; /// Measurement Variables
unsigned int sensorValue0; //ADC sample values declaration
float v_diff = 0; 
float Ts = 0.001; // 1kHz control frequency

unsigned int loopTrigger; 
unsigned int com_count=0; // a variables to count the interrupts. Used for program debugging.




void setup() {
  // put your setup code here, to run once:
  //Basic pin setups
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
  TCB0.CTRLA=TCB_CLKSEL_CLKDIV1_gc | TCB_ENABLE_bm; //62.5kHz
  analogWrite(6,120); 

  interrupts();  //enable interrupts.
  Wire.begin(); // We need this for the i2c comms for the current sensor
  ina219.init(); // this initiates the current sensor
  Wire.setClock(700000); // set the comms speed for i2c
}



void loop() {
  // put your main code here, to run repeatedly:
  if(loopTrigger) { // This loop is triggered, it wont run unless there is an interrupt
    
    digitalWrite(13, HIGH);   // set pin 13. Pin13 shows the time consumed by each control cycle. It's used for debugging.

  // Sample all of the measurements 
  sampling();
  
  //Open loop Buck Implementation
  v_diff = vcout-vref; // Calculate the difference between output voltage and 5V
  if (v_diff>0){
    open_loop = open_loop-0.01;  // We are above vref so less duty cycle
  } else if (v_diff<0) {
    open_loop = open_loop+0.01;  // We are below vref so more duty cycle
  } else{
    open_loop = open_loop; 
  }
  open_loop = saturation(open_loop,0.98,0.02);// saturate the duty cycle at the reference or a min of 0.02
  pwm_modulate(open_loop); // send the pwm signal out
  //Serial.print("Vb: ");
  //Serial.println(vcout);
  
    digitalWrite(13, LOW);   // reset pin13.
    loopTrigger = 0;
  }
}

// Timer A CMP1 interrupt. Every 800us the program enters this interrupt. 
// This, clears the incoming interrupt flag and triggers the main loop.

ISR(TCA0_CMP1_vect){
  TCA0.SINGLE.INTFLAGS |= TCA_SINGLE_CMP1_bm; //clear interrupt flag
  loopTrigger = 1;
}

void sampling(){
  
  // Make the initial sampling operations for the circuit measurements
  sensorValue0 = analogRead(A0); //sample Vout;
  current_mA = ina219.getCurrent_mA(); // sample the inductor current (via the sensor chip)
  
  vcout = sensorValue0 * (4.096/1023) *2 ; // Convert the Vb sensor reading to volts
  vref = 3;
  iL = current_mA/1000;
  //Serial.println(vcout);
  Serial.print("Inductor Current: ");
  Serial.println(iL);
}

// Saturation function
float saturation( float sat_input, float uplim, float lowlim){ 
  if (sat_input > uplim) sat_input=uplim;
  else if (sat_input < lowlim ) sat_input=lowlim;
  else;
  return sat_input;
}

// PWM function
void pwm_modulate(float pwm_input){ 
  analogWrite(6,(int)(255-pwm_input*255)); 
}
