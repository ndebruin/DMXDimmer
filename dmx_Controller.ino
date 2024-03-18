#include <Arduino.h>

#define DEBUG true

#include "DmxOutput.h"
DmxOutput dmxOutput;

#include "Fixture.h"

// fixture types
uint8_t dimmer1[] = {INTENS, NONE, NONE, NONE, NONE};
uint8_t dimmer4[] = {INTENS, INTENS, INTENS, INTENS, NONE};
uint8_t rgb[] = {RED, GREEN, BLUE, NONE, NONE};
uint8_t rgbw[] = {RED, GREEN, BLUE, WHITE, NONE};
uint8_t colorsource[] = {INTENS, RED, GREEN, BLUE, NONE};
uint8_t colorado[] = {INTENS, RED, GREEN, BLUE, WHITE};

Fixture Dimmer1(dimmer1);
Fixture Dimmer4(dimmer4);
Fixture RGB(rgb);
Fixture RGBW(rgbw);
Fixture IRGB(colorsource);
Fixture IRGBW(colorado);

// start with a single dimmer fixture
Fixture fixture = Dimmer1;

// fader pin
#define FADER 28
#define REVERSEFADER false

// dmx frame
uint8_t DMXFrame[513];

// array of dip switch pins
uint8_t DIP[] = {27,26,22,21,20,19,18,17,16,15};

bool enabled = false;
bool doubled = false;

// blink the led
uint16_t blinkInterval = 1000;
uint64_t lastBlink = 0;
boolean blinkState = LOW;
void Blink(){
  // verbaitum from "blink without delay" example
  uint64_t currentBlink = millis();

  if(currentBlink - lastBlink >= blinkInterval){
    lastBlink = currentBlink;

    blinkState = !blinkState;

    digitalWrite(LED_BUILTIN, blinkState);   
  }
}

void zero(){
  for(int i=1;i<513;i++){
    DMXFrame[i] = 0;
  }
}

void setup() {
  // setup dmx output on pin 1
    dmxOutput.begin(1);

    // Setup the onboard LED so that we can blink when we receives packets
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    // init dip switches
    for(int i=0;i<10;i++){
      pinMode(DIP[i], INPUT_PULLUP);
    }

    pinMode(FADER, INPUT);

    Serial.begin(115200);
}

void loop() {
  
  // figure out what fixture profile to use
  bool found = false;
  int idx = -1;
  for(int i = 0;i<6;i++){
    // if there's one on, and it's the first one
    if(!digitalRead(DIP[i]) && !found){
      found = true;
      idx = i;
    }
    // if we've already found one, then don't allow output, and show that
    else if(!digitalRead(DIP[i]) && found){
      enabled = false;
      idx = -1;
      doubled = true;
    }
  }
  if(!found){
    enabled = false;
    doubled = false;
  }

  // change fixture based on the selected dip
  switch(idx){
    case 0: // 1 dimmer
      enabled = true;
      fixture = Dimmer1;
      break;
    case 1: // 4 dimmers
      enabled = true;
      fixture = Dimmer4;
      break;
    case 2: // rgb
      enabled = true;
      fixture = RGB;
      break;
    case 3: // rgbw
      enabled = true;
      fixture = RGBW;
      break;
    case 4: // intensity rgb (colorsource 5ch)
      enabled = true;
      fixture = IRGB;
      break;
    case 5: // intensity rgbw (colorado arc2d)
      enabled = true;
      fixture = IRGBW;
      break;

    default:
      enabled = false;
      break;
  }

  // read the value on the fader
  uint16_t val = analogRead(FADER);
  // map from 0-1023 to 0-255
  if(REVERSEFADER){
    val = map(val, 0, 1023, 0, 255);
    if(val < 2){
      val = 0;
    }
  }
  else{
    val = map(val, 0, 1023, 255, 0);
    if(val > 253){ // noise prevention
      val = 255;
    }
  }
  
  // only output if we can
  if(enabled){
    // set intensity of fixture from the fader
    fixture.setIntens(val);

    // set RGBW from DIP 7-10
    fixture.setColor(!digitalRead(DIP[6]), !digitalRead(DIP[7]), !digitalRead(DIP[8]), !digitalRead(DIP[9]));
    
    if(DEBUG){
      fixture.print();
    }

    // write data
    fixture.getDMX(DMXFrame);
    dmxOutput.write(DMXFrame, 513);

    // wait until done
    while(dmxOutput.busy()){}
    // delay 1 ms for stability
    delay(1);

    // blink led
    Blink();
  }
  else if(doubled){ // if two fixtures are turned on
    digitalWrite(LED_BUILTIN, LOW);
    // zero the dmx frame
    zero();
    // send a zero frame (prevent fixtures from staying on, in case the switches are bumped)
    dmxOutput.write(DMXFrame, 513);
  }
  else{ // if disabled
    digitalWrite(LED_BUILTIN, HIGH);
    // zero the dmx frame
    zero();
    // send a zero frame (prevent fixtures from staying on, in case the switches are bumped)
    dmxOutput.write(DMXFrame, 513);
  }
}


