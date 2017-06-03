//
//  SonyTVMote.cpp
//  
//
//  Created by Eric van der Knaap on 01/06/2017.
//
//

#include <stdio.h>
#include "SonyTVMote.h"
#include "Arduino.h"
#include "Ethernet.h"


SonyTvMote :: SonyTvMote()
{
    
}

void SonyTvMote :: begin(String pwd)
{
    _password = pwd;
}

void SonyTvMote :: sendPostRequest(EthernetClient client, String cmd)
{
    
    String package = getCommandPackage(cmd);
    
    // Make a HTTP request:
    client.println("POST /sony/IRCC HTTP/1.1");
    client.println("Host: 192.168.178.31");
    client.println("User-Agent: Arduino/1.0");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
    client.print("Content-Length: ");
    client.println(package.length());
    client.print("X-Auth-PSK: ");
    client.println(_password);
    client.println(); // end of the header
    client.println(package);
}


String SonyTvMote:: getCommandPackage(String command)
{
    String data ="<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n";
    data.concat("<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">");
    data.concat("<s:Body>");
    data.concat("<u:X_SendIRCC xmlns:u=\"urn:schemas-sony-com:service:IRCC:1\">");
    data.concat("<IRCCCode>");
    data.concat(command);
    data.concat("</IRCCCode>");
    data.concat("</u:X_SendIRCC>");
    data.concat("</s:Body>");
    data.concat("</s:Envelope>");
    return data;
}

