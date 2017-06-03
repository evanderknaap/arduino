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
    
    const String tv_NUM0 = "AAAAAQAAAAEAAAAJAw==";
    const String tv_NUM1 = "AAAAAQAAAAEAAAAAAw==";
    const String tv_NUM2 = "AAAAAQAAAAEAAAABAw==";
    const String tv_NUM3 = "AAAAAQAAAAEAAAADAw==";
    const String tv_NUM4 = "AAAAAQAAAAEAAAADAw==";
    const String tv_NUM5 = "AAAAAQAAAAEAAAAEAw==";
    const String tv_NUM6 = "AAAAAQAAAAEAAAAFAw==";
    const String tv_NUM7 = "AAAAAQAAAAEAAAAGAw==";
    const String tv_NUM8 = "AAAAAQAAAAEAAAAHAw==";
    const String tv_NUM9 = "AAAAAQAAAAEAAAAIAw==";
    
    const String tv_HOME = "AAAAAQAAAAEAAABgAw==";
    const String tv_CHUP = "AAAAAQAAAAEAAAAQAw==";
    const String tv_CHDOWN = "AAAAAQAAAAEAAAARAw==";
    
private:
    String getCommandPackage(String command);
    String _password;
};


#endif /* SonyTVMote_h */
