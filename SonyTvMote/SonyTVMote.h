//
//  SonyTVMote.h
//  Arduino Libaries
//  This Library allows you to send commands to a Sony TV over HTTP
//
//  You connect to the TV as a EthernetClient using the default Ethernet Library
//  When creating the SonyTvMote object, you pass in the TV password, if your tv requires one
//  You set your Pwd in ..
//
//  You pass the TV EthernetClient object, desired command to the SonyTVMote object
//  It will creat a post request, incorporating the command and password to control your TV and send it.
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
#define DEFAULT_PORT 80

class SonyTvMote
{
public:
    SonyTvMote();
    void sendPostRequest(EthernetClient client, String cmd);
    void begin(String pwd);
    
    // Constants
    const String tv_off   = "AAAAAQAAAAEAAAAvAw==";
    const String tv_HDMI1 = "AAAAAgAAABoAAABaAw==";
    const String tv_HDMI2 = "AAAAAgAAABoAAABbAw==";
    const String tv_HDMI3 = "AAAAAgAAABoAAABcAw==";
    const String tv_HDMI4 = "AAAAAgAAABoAAABdAw==";
    
private:
    String getCommandPackage(String command);
    String _password;
};


#endif /* SonyTVMote_h */
