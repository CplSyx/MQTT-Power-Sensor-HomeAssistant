/*
  Emon.h - Library for openenergymonitor
  Created by Trystan Lea, April 27 2010
  GNU GPL
  modified to use up to 12 bits ADC resolution (ex. Arduino Due)
  by boredman@boredomprojects.net 26.12.2013 [removed Jan 2023 as unecessary]
  Low Pass filter for offset removal replaces HP filter 1/1/2015 - RW

  Simplified for current measurement only with SCT-013-000 YHDC Current Transformer
  by CplSyx Jan 2023
*/

//#ifndef EmonLib_h
//#define EmonLib_h

//Support for newer IDEs
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// measured voltage at 3.3V pin (I am using a Wemos D1 mini with a measured supply voltage of 3.36V)
#define SupplyVoltage 3.36                        

// Default to 10 bits, as in regular Arduino-based boards.
#define ADC_BITS 10

#define ADC_COUNTS  (1<<ADC_BITS) //This bitwise operation equals 1024


class EnergyMonitor
{
  public:

    void current(unsigned int _inPinI, double _ICAL);
    void current(double _ICAL);

    double calcIrms(unsigned int NUMBER_OF_SAMPLES);
    void serialprint();

    //Useful value variables
    double Irms;

  private:

    //Set current input pin
    unsigned int inPinI;

    //Calibration coefficients
    //Need to be set in order to obtain accurate results
    double ICAL;

    //--------------------------------------------------------------------------------------
    // Variable declaration for emon_calc procedure
    //--------------------------------------------------------------------------------------
    //sample_ holds the raw analog read value
    int sampleI;

    //Filtered_ is the raw analog value minus the DC offset
    double filteredI;
    double offsetI;                          //Low-pass filter output

    double sqI,sumI;            //sq = squared, sum = Sum, inst = instantaneous


};

//#endif
