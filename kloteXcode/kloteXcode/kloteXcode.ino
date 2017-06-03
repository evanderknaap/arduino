//
// kloteXcode
//
// Description of the project
// Developed with [embedXcode](http://embedXcode.weebly.com)
//
// Author 		Eric van der Knaap
// 				Eric van der Knaap
//
// Date			01/06/2017 23:56
// Version		<#version#>
//
// Copyright	© Eric van der Knaap, 2017
// Licence		<#licence#>
//
// See         ReadMe.txt for references
//


// Core library for code-sense - IDE-based
#if defined(ENERGIA) // LaunchPad specific
#include "Energia.h"
#elif defined(TEENSYDUINO) // Teensy specific
#include "Arduino.h"
#elif defined(ESP8266) // ESP8266 specific
#include "Arduino.h"
#elif defined(ARDUINO) // Arduino 1.8 specific
#include "Arduino.h"
#else // error
#error Platform not defined
#endif // end IDE

// Set parameters


// Include application, user and local libraries
#include "kloteXcode_Library.h"


// Define structures and classes


// Define variables and constants


// Prototypes


// Utilities


// Functions


// Add setup code
void setup()
{
    ;
}

// Add loop code
void loop()
{
    ;
}
