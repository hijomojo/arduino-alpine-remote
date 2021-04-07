/* Alpine Remote 2021, Jeremy F.
 *  
 *  Control an Alpine Car Stereo unit using the connector at the back
 * 
 * Using inspiration from the Wnthr/arduino_alpine_remote library I started the problem from scratch.
 * 
 * To use the Alpine remote wired connection connect a 5v Arduino pin 3 to the tip of a 3.5mm TRS plug
 * and the sleeve to ground.
 * 
 * This program copies the serial output code from the ArduinoIR library, but omits the 39kHz PWM
 * as we are not driving an IR LED. Using an IR Receiver I have sniffed all the buttons on my Alpine
 * remote and included the raw data here.
 *  
 * My car happens to use a resistor ladder to encode 6 buttons from my steering wheel.
 * 
 *  
 */



//Alpine remote signal raw data collected using IRreceive_dump and an IR receiver module
#define A_POWER 0xF6097286
#define A_MUTE 0xE9167286
#define A_VOLDOWN 0xEA157286
#define A_VOLUP 0xEB147286
#define A_SOURCE 0xF50A7286
#define A_BAND 0xF20D7286
#define A_PHONE 0xA9567286
#define A_TRACKPREV 0xED127286
#define A_PLAYPAUSE 0xF8077286
#define A_TRACKNEXT 0xED127286
#define A_ARROWUP 0x718E7286
#define A_ARROWDOWN 0x708F7286
#define A_ARROWLEFT 0x6C937286
#define A_ARROWRIGHT 0x6D927286
#define A_ENTER 0x5FA07286
#define A_UP 0xF10E7286
#define A_DN 0xF00F7286
#define A_AUDIO 0x5AA57286
#define A_SUBTITLE 0x5BA47286
#define A_MENU 0x5EA17286

//NEC IR timing info found within the IRremote library
#define NEC_UNIT                560
#define NEC_HEADER_MARK         (16 * NEC_UNIT) // 9000
#define NEC_HEADER_SPACE        (8 * NEC_UNIT)  // 4500
#define NEC_BIT_MARK            NEC_UNIT
#define NEC_ONE_SPACE           (3 * NEC_UNIT)  // 1690
#define NEC_ZERO_SPACE          NEC_UNIT
#define NEC_REPEAT_HEADER_SPACE (4 * NEC_UNIT)  // 2250

//Pin to output remote signal on. Connect this to the tip of a 3.5mm plug. 
#define APIN 3

//Resistor ladder voltage divider decision values for my car steering wheel buttons.
// I used the Arduino to measure each button push, and then took a value halfway between each
// one for these descision values.

#define R_VOLDOWN 313
#define R_VOLUP 444
#define R_MUTE 566
#define R_TRACKDOWN 688
#define R_TRACKUP 740
#define R_MODE 890

void setup() {
  pinMode(APIN, OUTPUT);
  Serial.begin(115200);
  delay(10);
  digitalWrite(APIN, LOW);
  Serial.println("Alpine Remote 2021");
  
  int sensorValue = analogRead(A4);
  while(sensorValue < 100){ //freeze program here if button wiring has shorted
  delay(1); 
  sensorValue = analogRead(A4);
  }
}

void loop() {
  int sensorValue = analogRead(A4);
  delay(1); //I find I get more consistent analog readings this way.
  sensorValue = analogRead(A4);
  
  if (sensorValue < R_VOLDOWN) { 
    sendNEC32_TTL(A_VOLDOWN);
    Serial.print(sensorValue);
    Serial.println(" Volume Down pushed");}
  else if (sensorValue < R_VOLUP) { 
    sendNEC32_TTL(A_VOLUP);
    Serial.print(sensorValue);
    Serial.println(" Volume Up pushed");}
  else if (sensorValue < R_MUTE) { 
    sendNEC32_TTL(A_PLAYPAUSE);
    Serial.print(sensorValue);
    Serial.println(" Mute pushed");}
  else if (sensorValue < R_TRACKDOWN) {
    sendNEC32_TTL(A_TRACKNEXT);
    Serial.print(sensorValue);
    Serial.println(" Track Down pushed"); }
  else if (sensorValue < R_TRACKUP) { 
    sendNEC32_TTL(A_TRACKPREV);
    Serial.print(sensorValue);
    Serial.println(" Track up pushed");}
  else if (sensorValue < R_MODE) { 
    sendNEC32_TTL(A_PHONE);
    Serial.print(sensorValue);
    Serial.println(" Mode pushed");}
}

void sendNEC32_TTL(uint32_t data) {
  //Send NEC style 32 bits (2x address bytes + 2x command bytes) using NEC timing to a pin.
  uint32_t mask = 0x00000001;
  
  //Send NEC header burst
  digitalWrite(APIN, HIGH);
  delayMicroseconds(NEC_HEADER_MARK);
  digitalWrite(APIN, LOW);
  //Send NEC header space
  delayMicroseconds(NEC_HEADER_SPACE);
  
  //Send the data LSB first
  for (int i=0;i<32;i++) {
     if((mask << i) & data) { //bitwise AND operation reveals each bit
      //Send a '1'  
      digitalWrite(APIN, HIGH);
      delayMicroseconds(NEC_BIT_MARK);
      digitalWrite(APIN, LOW);
      delayMicroseconds(NEC_ONE_SPACE); 
    }
    else { 
      //send a '0'
     digitalWrite(APIN, HIGH);
     delayMicroseconds(NEC_BIT_MARK);
     digitalWrite(APIN, LOW);
     delayMicroseconds(NEC_ZERO_SPACE); 
    }
  }
  //Send NEC stop bit
  digitalWrite(APIN, HIGH);
  delayMicroseconds(NEC_BIT_MARK);
  digitalWrite(APIN, LOW);
  delayMicroseconds(NEC_ZERO_SPACE);

 delay(200); //simple de-bounce, and NEC requires at least 5ms(?) gap otherwise the NEC "repeat" scheme must be used. 
}

void debugNEC32_TTL(uint32_t data) {
  //Print output for debugging
  uint32_t mask = 0x00000001;
  for (int i=0;i<32;i++) {
    if((mask << i) & data) {
      //Send a '1'  
      Serial.print("1"); 
    }
    else { 
      //send a '0'
     Serial.print("0");
    }
  }
 Serial.println("S");
}
