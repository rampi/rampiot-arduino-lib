#ifndef RampIOTControl_H
#define RampIOTControl_H

#include "Arduino.h"
#include "ArduinoJson.h"
#include "Storage.h"

class RampIOTControl{
public:
    /*Type definition*/
    typedef void (*OnMessageCallback)(const char* topic, JsonObject& json, const char* fireUserId);
    typedef void (*OnPropertiesCallback)(JsonObject& json);
    typedef void (*OnResetCallback)();
    RampIOTControl();
    void begin(HardwareSerial *hwdSerial, byte resetPin, uint32_t tId, const char* _thingType);
    void publishEvent(JsonObject& status, const char* fireUserId);
    void publishEvent(const char* status, const char* fireUserId);
    void setMessageCallback(OnMessageCallback messageCallback);
    void setResetCallback(OnResetCallback resetCallback);
    void setPropertiesCallback(OnPropertiesCallback onPropertiesCallback);
    void handleThing();
    void reset();
};

#endif