
#ifndef Fixture_h
#define Fixture_h

#include <Arduino.h>

#define NONE 0
#define INTENS 1
#define RED 2
#define GREEN 3
#define BLUE 4
#define WHITE 5

class Fixture {
  private:
    bool intensityChan = false;
    bool whiteChan = false;

    uint8_t rVal = 0;
    uint8_t gVal = 0;
    uint8_t bVal = 0;

    uint8_t iVal = 0;
    uint8_t wVal = 0;

    uint8_t channelSetup[5];

    void dimColor();

  public:
    Fixture(uint8_t channels[5]);
    
    void setIntens(uint16_t faderVal);
    void setColor(bool red, bool green, bool blue, bool white);

    uint8_t *getDMX(uint8_t dmx[513]);

    void print();

    void disable();
};
#endif