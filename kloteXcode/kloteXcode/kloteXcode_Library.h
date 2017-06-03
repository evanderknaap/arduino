//
// File			kloteXcode_Library.h
// Brief		Library header
//
// Project	 	kloteXcode
// Developed with [embedXcode](http://embedXcode.weebly.com)
//
// Author		Eric van der Knaap
// 				Eric van der Knaap
// Date			01/06/2017 23:56
// Version		<#version#>
//
// Copyright	© Eric van der Knaap, 2017
// Licence		<#licence#>
//
// See			ReadMe.txt for references
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

#include <SPI.h>
#include <Ethernet.h>

#ifndef kloteXcode_RELEASE
#define kloteXcode_RELEASE 100




#endif // kloteXcode_RELEASE
