#include "Fixture.h"

Fixture::Fixture(uint8_t channels[5]){

  for(int i=0;i<5;i++){
    channelSetup[i] = channels[i];
    // check if we have an intensity channel
    if(channels[i] == INTENS){
      intensityChan = true;
    }
    // check if we have a white channel
    if(channels[i] == WHITE){
      whiteChan = true;
    }
  }
}

void Fixture::print(){
  // loop over channel footprint
  for(int i = 0; i < 5; i++){
    switch (channelSetup[i]) {
      case INTENS:
        Serial.print("i:" + String(iVal) + " ");
        break;
      
      case RED:
        Serial.print("r:" + String(rVal) + " ");
        break;
      
      case GREEN:
        Serial.print("g:" + String(gVal)+ " ");
        break;

      case BLUE:
        Serial.print("b:" + String(bVal) + " ");
        break;
      
      case WHITE:
        Serial.print("w:" + String(wVal) + " ");
        break;
    }
  }

  Serial.println("");
}

void Fixture::dimColor(){
  // calculate intensity percentage
  double percent = iVal/255.0;

  // scale the color values by percent
  rVal = (int)(percent * rVal);
  gVal = (int)(percent * gVal);
  bVal = (int)(percent * bVal);
  wVal = (int)(percent * wVal);
}

void Fixture::setIntens(uint16_t faderVal){
  iVal = faderVal;

  // if there's not a dedicated intensity channel, dim the color channels respectively
  if(!intensityChan){
    dimColor();
  }
}

void Fixture::setColor(bool red, bool green, bool blue, bool white){
  if(red){
    rVal = 255;
  }
  else{
    rVal = 0;
  }

  if(green){
    gVal = 255;
  }
  else{
    gVal = 0;
  }

  if(blue){
    bVal = 255;
  }
  else{
    bVal = 0;
  }

  if(white){
    wVal = 255;
  }
  else{
    wVal = 0;
  }

  // if we need to dim it
  if(!intensityChan){
    dimColor();
  }
}

uint8_t *Fixture::getDMX(uint8_t frame[513]){
   // start value is always 0
  frame[0] = 0;

  // loop over channel footprint
  for(int i = 0; i < 5; i++){
    switch (channelSetup[i]) {
      case INTENS:
        frame[i+1] = iVal;
        break;
      
      case RED:
        frame[i+1] = rVal;
        break;
      
      case GREEN:
        frame[i+1] = gVal;
        break;

      case BLUE:
        frame[i+1] = bVal;
        break;
      
      case WHITE:
        frame[i+1] = wVal;
        break;
    }
  }

  return frame;
}

void Fixture::disable(){
  iVal = 0;
  rVal = 0;
  gVal = 0;
  bVal = 0;
  wVal = 0;

}