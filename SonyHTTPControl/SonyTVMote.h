//
//  SonyTVMote.h
//  Arduino Libaries
//  This Library allows you to send commands to a Sony TV over HTTP
//
//  You connect to the TV as a EthernetClient using the default Ethernet Library
//  You pass the TV EthernetClient object, desired command and TV password to the SonyTVMote object
//  It will creat a post request, incorporating the command and password to control your TV
//
//  Some TV models require a password, so an optional password string of four numbers can be passed
//  along
//
//  Wake On Lan is not yet supported. So starting up the TV from off is not yet supported
//
//  Created by Eric van der Knaap on 01/06/2017.




#ifndef SonyTVMote_h
#define SonyTVMote_h

#include "Arduino.h"
#include "Ethernet.h"

#define SUCCESS 1
#define TIMED_OUT -1
#define INVALID_SERVER -2
#define TRUNCATED -3
#define INVALID_RESPONSE -4

class SonyTvMote
{
public:
    SonyTvMote();
    void sendTVCommand(String cmd, EthernetClient client);
private:
    void sendPostRequest(EthernetClient client, String package)
    String getCommandPackage(String command)
};


#endif /* SonyTVMote_h */
