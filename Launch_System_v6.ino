

#include <Wire.h>
#include <SparkFun_Qwiic_Button.h>
#include <SparkFun_Alphanumeric_Display.h>  
#include <Servo.h>
//#include "BlinkM_funcs.h"
#include "SparkFun_Qwiic_Relay.h"

//Use Board Arduino Uno on USB Serial, to connect to RedBoard Edge

#define RELAY_ADDR 0x18 // Alternate address 0x19
Qwiic_Relay relay(RELAY_ADDR); 

HT16K33 display;
QwiicButton button;

uint32_t next;

//Define LED characteristics
uint8_t brightness = 250;   //The maximum brightness of the pulsing LED. Can be between 0 (min) and 255 (max)
uint16_t cycleTime = 1000;   //The total time for the pulse to take. Set to a bigger number for a slower pulse, or a smaller number for a faster pulse
uint16_t offTime = 200;     //The total time to stay off between pulses. Set to 0 to be pulsing continuously.

boolean statusLaunched = false;   // tracks if launch has occurred

// set all pins
const int buzzer = 6; 
const int pinLaunch = 12;   // this pin lights fuse that fires the rocket motor through the relay
const int pinFire = 5;     // this pin has an LED that tells if the fuse is LIT or not

//new 10/13/21 for BlinkM
//const boolean BLINKM_ARDUINO_POWERED = true;
//const int CMD_START_BYTE = 0x01;


int pos = 0;
Servo servo;

void setup() {
  // put your setup code here, to run once:

  //************INIT*************
  
  //Initialise the Pyro & buzzer pins)
  pinMode(pinLaunch, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(pinFire, OUTPUT);

  //Make sure that the output are turned off
  digitalWrite(pinLaunch, LOW);
  digitalWrite(buzzer, LOW);
  digitalWrite(pinFire, LOW);  

  Serial.begin(9600);
  Wire.begin(); //Join I2C bus  
  Serial.println("Buzzer and Launch Pin Reset");  
  
  servo.attach(9);
  Serial.println("Launch Servo Attached");  


  //check if display will acknowledge
  if (display.begin() == false) {
    Serial.println("Display did not acknowledge! Freezing.");
    while(1);
  }
  
  //check if button will acknowledge over I2C
  if (button.begin() == false) {
    Serial.println("Button did not acknowledge! Freezing.");
    while (1);
  }

  // ready the relay
  if(!relay.begin())
    Serial.println("Check connections to Qwiic Relay.");
  else
    Serial.println("Qwiic Relay is ready."); 
  float version = relay.singleRelayVersion();
  Serial.print("Firmware Version: ");
  Serial.println(version);    
  
  Serial.println("Button acknowledged.");
  button.LEDoff();  //start with the LED off
  Serial.println("Launch_System_V5 Arduino Code - BM - 10/16/21");  
  display.print("RDY");
  Serial.println("Startup Complete.");

}

void loop() {

  fireCheck(12);
  
  if (button.isPressed() == true && !statusLaunched) {
    button.LEDconfig(brightness, cycleTime, offTime);
    goLaunch();

  }   
    
    while (button.isPressed() == true)
      delay(10);  //wait for user to stop pressing
    button.LEDoff();
  
  delay(20); //let's not hammer too hard on the I2C bus  

}

void goLaunch()
{
    display.print("GOGO");
    delay(5000);
    longBeep();
    display.print("STRT");    
    delay(1000);
    
    display.print("10");
    shortBeep();
    delay(700);
    display.print("9");
    shortBeep();
    delay(700);
    display.print("8");
    shortBeep();
    delay(700);
    display.print("7");
    shortBeep();
    delay(700);
    display.print("6");
    shortBeep();
    delay(700);
    display.print("5");
    shortBeep();
    delay(700);
    display.print("4");
    shortBeep();
    delay(700);
    display.print("3");
    shortBeep();
    shortBeep();    
    delay(500);
    display.print("2");
    shortBeep();
    shortBeep();    
    delay(500);
    display.print("1");
    shortBeep();
    shortBeep();  
    
    // fire the servo
    servo.write(180);

    // short delay for clamps to open
    delay(1000);  
    
    // fire the pyro
    display.print("FIRE");  
    digitalWrite(pinLaunch, HIGH);
    // Turn relay on
    relay.turnRelayOn();   
    Serial.println("The launch pyro has been fired");
    longBeep();
    delay (1000);
    statusLaunched = !statusLaunched; 
    digitalWrite(pinLaunch, LOW);
    // Let's turn that relay off...
    relay.turnRelayOff();       
    Serial.println("The launch pyro has been reset");
    Serial.println("The launch has ended.");
    display.print("DONE");  
    doneBeep(); 
}

void shortBeep()
{
  tone(buzzer, 659, 100);
  delay(300);
  noTone(buzzer);
}

void longBeep()
{
  tone(buzzer, 523, 2000);
  delay(2000);  
  noTone(buzzer);
}

void doneBeep()
{
  tone(buzzer, 880, 3000);
  delay(3000);  
  noTone(buzzer);
}

void fireCheck(int pin)   // this function checks if the motor fuse is active or not, for safety
{
  int val = 0;     // variable to store the read value
  // read the input pin to check if the fuse is lit
    val = digitalRead(pin);
    if (val == 0)
    {
      //fuse is not lit
      //longBeep();
      digitalWrite(pinFire, LOW);
      //Serial.println("Fuse is not lit");
      //Serial.println(val);
    }
    else
    {
      //fuse is lit
      digitalWrite(pinFire, HIGH);
      display.print("FIRE");  
      Serial.println("CAUTION: Fuse IS LIT");
      Serial.println(val);  
    }
}
