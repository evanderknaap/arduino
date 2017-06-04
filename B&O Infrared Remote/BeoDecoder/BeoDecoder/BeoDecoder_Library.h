//
// File			BeoDecoder_Library.h
// Brief		Library header
//
// Project	 	BeoDecoder
// Developed with [embedXcode](http://embedXcode.weebly.com)
//
// Author		Eric van der Knaap
// 				Eric van der Knaap
// Date			04/06/2017 15:29
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

#ifndef BeoDecoder_RELEASE
#define BeoDecoder_RELEASE 100


#endif // BeoDecoder_RELEASE
