#include "RampIOTControl.h"
#define BAUD_RATE 9600
#define PENDING_RESET_SLOT 5
#define RESET_TIMEOUT 3000

const char ON_MESSAGE[] PROGMEM = "onMessage";
const char ON_PROPERTIES[] PROGMEM = "onProperties";
const char ON_RESET[] PROGMEM = "onReset";

RampIOTControl::OnPropertiesCallback onPropertiesCallback = NULL;
RampIOTControl::OnMessageCallback onMessageCallback = NULL;
RampIOTControl::OnResetCallback onResetCallback = NULL;
HardwareSerial *hwdSerial;
bool dataAvailable;
bool pendingReset = false;
unsigned long lastResetRequest = 0;
byte rPin;
String Buff = "";

RampIOTControl::RampIOTControl(){}

void RampIOTControl::begin(HardwareSerial *hSerial, byte resetPin, uint32_t tId, const char* _thingType){
    hwdSerial = hSerial;
    rPin = resetPin;
    Storage storage;        
    pendingReset = !storage.isEmpty(PENDING_RESET_SLOT);    
    hwdSerial->begin(BAUD_RATE);
    hwdSerial->print("{\"method\":\"begin\",");
    hwdSerial->print("\"thingId\":");
    hwdSerial->print(tId);
    hwdSerial->print(",\"thingType\":\"");
    hwdSerial->print(_thingType);
    hwdSerial->println("\"}");
}

void RampIOTControl::publishEvent(JsonObject& status, const char* fireUserId){
    if( hwdSerial != NULL ){
        uint8_t size = status.measureLength()+1;
        char _status[size];
        status.printTo(_status, size);
        hwdSerial->print("{\"method\":\"publishEvent\",");
        hwdSerial->print("\"status\":");
        hwdSerial->print(_status);
        hwdSerial->print(",\"fireUserId\":\"");
        hwdSerial->print(fireUserId);
        hwdSerial->println("\"}");
    }    
}

void RampIOTControl::publishEvent(const char* status, const char* fireUserId){
    if( hwdSerial != NULL ){        
        hwdSerial->print("{\"method\":\"publishEvent\",");
        hwdSerial->print("\"status\":");
        hwdSerial->println(status);
        hwdSerial->print(",\"fireUserId\":\"");
        hwdSerial->print(fireUserId);
        hwdSerial->println("\"}");
    }    
}

void ___onReset__(){
    pendingReset = false;
    Storage storage;
    storage.clearSlot(PENDING_RESET_SLOT);
}

void RampIOTControl::setMessageCallback(OnMessageCallback messageCallback){
    onMessageCallback = messageCallback;
}

void RampIOTControl::setResetCallback(OnResetCallback resetCallback){
    onResetCallback = resetCallback;
}

void RampIOTControl::setPropertiesCallback(OnPropertiesCallback onPropsCallback){
    onPropertiesCallback = onPropsCallback;
}

void RampIOTControl::handleThing(){
    if( !pendingReset && digitalRead(rPin) == LOW ){
        pendingReset = true;
        Storage storage;
        storage.saveData(PENDING_RESET_SLOT, "P");
        reset();
    }
    if( pendingReset && (lastResetRequest == 0 || millis()-lastResetRequest >= RESET_TIMEOUT) ){
        lastResetRequest = millis();
        reset();
    }
    if( hwdSerial != NULL ){
        while(hwdSerial->available()) {
            char data = hwdSerial->read();
            if( data == '\n' ){
                dataAvailable = true;
            }else{
                Buff.concat(data);
            }
        }
    }    
    if( dataAvailable ){
        DynamicJsonBuffer jsonBuffer;
        char buff[Buff.length()+1];
        Buff.toCharArray(buff, Buff.length()+1);
        Buff = "";
        dataAvailable = false;
        JsonObject& root = jsonBuffer.parseObject(buff);
        if (root.success()) {
            const char* _method = root["method"];
            if( strcmp_P(_method, ON_MESSAGE) == 0 ){
                if( onMessageCallback != NULL ){
                    const char* topic = root["topic"];
                    const char* fireUserId = root["fireUserId"];
                    JsonObject& status = root["status"];
                    onMessageCallback(topic, status, fireUserId);
                }
            }
            else if( strcmp_P(_method, ON_PROPERTIES) == 0 ){
                if( onPropertiesCallback != NULL ){
                    JsonObject& properties = root["properties"];
                    onPropertiesCallback(properties);
                }
            }
            else if( strcmp_P(_method, ON_RESET) == 0 ){
                ___onReset__();
                if( onResetCallback == NULL ){
                    onResetCallback();
                }
            }
        }
    }
}

void RampIOTControl::reset(){
    if( hwdSerial != NULL ){
        hwdSerial->println("{\"method\":\"reset\"}");
    }
}