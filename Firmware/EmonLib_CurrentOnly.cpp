/*
  Emon.cpp - Library for openenergymonitor
  Created by Trystan Lea, April 27 2010
  GNU GPL
  modified to use up to 12 bits ADC resolution (ex. Arduino Due)
  by boredman@boredomprojects.net 26.12.2013 [removed Jan 2023 as unecessary]
  Low Pass filter for offset removal replaces HP filter 1/1/2015 - RW

  Simplified for current measurement only with SCT-013-000 YHDC Current Transformer
  by CplSyx Jan 2023
*/

// Proboscide99 10/08/2016 - Added ADMUX settings for ATmega1284 e 1284P (644 / 644P also, but not tested) in readVcc function

//#include "WProgram.h" un-comment for use on older versions of Arduino IDE
#include "EmonLib_CurrentOnly.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


//--------------------------------------------------------------------------------------
// Sets the pins to be used for voltage and current sensors
//--------------------------------------------------------------------------------------
void EnergyMonitor::current(unsigned int _inPinI, double _ICAL)
{
  inPinI = _inPinI;
  ICAL = _ICAL;
  offsetI = ADC_COUNTS>>1;
}

//--------------------------------------------------------------------------------------
// emon_calc procedure
// Calculates Irms
// From a sample window of the mains AC current.
// The Sample window length is defined by the number of half wavelengths or crossings we choose to measure.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
double EnergyMonitor::calcIrms(unsigned int Number_of_Samples)
{

  for (unsigned int n = 0; n < Number_of_Samples; n++)
  {
    sampleI = analogRead(A0);

    // Digital low pass filter extracts the 2.5 V or 1.65 V dc offset,
    //  then subtract this - signal is now centered on 0 counts.
    offsetI = (offsetI + (sampleI-offsetI)/1024);
    filteredI = sampleI - offsetI;

    // Root-mean-square method current
    // 1) square current values
    sqI = filteredI * filteredI;
    // 2) sum
    sumI += sqI;
  }

  double I_RATIO = ICAL *((SupplyVoltage) / (ADC_COUNTS));
  Irms = I_RATIO * sqrt(sumI / Number_of_Samples);

  //Reset accumulators
  sumI = 0;
  //--------------------------------------------------------------------------------------

  return Irms;
}

void EnergyMonitor::serialprint()
{
  Serial.print("Irms: ");
  Serial.println(Irms);
  delay(100);
}

