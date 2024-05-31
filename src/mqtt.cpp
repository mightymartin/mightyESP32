#include "mqtt.h"

static Ticker MQTTTimer;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

uint32_t lastChecksum = 0;

String cmdTopic, stateTopic;
char *mqtt_user = NULL;
char *mqtt_pass = NULL;

unsigned long lastReconnectAt = millis();

void MQTTUpdate(){    
    MQTTPublish(true); 
}

boolean MQTTReconnect(){
    if ((millis() - lastReconnectAt) > 1000) {
        if(!mqttClient.connected()) {     
            WebLogInfo("Attempting MQTT connection...");        
            if (mqttClient.connect(settings.m_client_id, mqtt_user, mqtt_pass)) { 
                WebLogInfo("Connect succeeded");      
                mqttClient.subscribe(cmdTopic.c_str());
                WebLogDebug("subscribe to " + String(cmdTopic)); 

                //############ IF HA -> hier config publishen und subscriben
                if(settings.m_homeassistant > 0){
                    // Beim Connect Config pushen
                    //mqttClient.publish(HAM_ctrl->getConfigTopic().c_str(),HAM_ctrl->getConfigPayload().c_str());
                   
                    // Beim Connect LWT auf online setzen
                    //mqttClient.publish(HAM_ctrl->getLWTTopic().c_str(),LWT_ONLINE);
                    
                    //mqttClient.subscribe(HAM_ctrl->getCommandTopic().c_str());
                    //mqttClient.subscribe(HAM_targetTemp->getCommandTopic().c_str());
                    //mqttClient.subscribe(HAM_targetPower->getCommandTopic().c_str());
                }

            } else {
                WebLogError("Connect failed, rc=" + String(mqttClient.state()));                  
            }        
        }  
        lastReconnectAt = millis();
    }
    return mqttClient.connected();
}

String MQTTStatus(){    
    if(mqttClient.connected()){
        return F("Connected");
    }else{
        return "not Connected, rc=" + String(mqttClient.state());
    }    
}

void MQTTTick(){  
    mqttClient.loop();
    MQTTPublish(false);  
}



void MQTTSubCallback(char* topic, byte* payload, unsigned int length) {
    payload[length] = '\0'; //terminate that nasty boy    
    String value = String((char *)payload);  
    uint8_t intValue = (uint8_t)value.toInt();

    if(settings.m_homeassistant > 0 && String(topic).equals(""/*HAM_targetPower->getCommandTopic()*/)){
        //#### hass impl
    }else{
        //#### Lagacy Shit
        uint8_t   validCmd = 0;
        uint8_t   validValue = 0;
        String  command = String(strrchr(topic, '/')).substring(1);

        /* 
        if(command.equalsIgnoreCase(U_ONOFF_TAG)){
            validCmd = 1;
            if(length > 0 && intValue <= 1){                        
                validValue = 1;
            }        
        }
        if(command.equalsIgnoreCase(D_TEXT_TAG)){
            validCmd = 1;
            if(length > 0 && length < 255){    
                SettingsSetValue(D_MODE_TAG,String(DRAW_MODE_TEXT));            
                validValue = 1;                    
            }
        }*/

        if(validCmd == 1 && validValue == 1){
            SettingsSetValue(command,value);
            WebLogInfo(String(topic) + " = " +  value);        
        }
        if(validCmd == 0){
            WebLogInfo("MQTT invalid Command " + command + " : " +  value + ".");
        }
        if(validValue == 0){
            WebLogInfo("MQTT invalid Value " + command + " : " +  value + ".");
        }
    }
    
    
    
    
}

void MQTTPublish(bool force) {    
    if(MQTTReconnect()){      
        if(lastChecksum != SettingsGetChecksum() || force){                   
            WebLogInfo(stateTopic + " = " + SettingsToJson());
            mqttClient.publish(stateTopic.c_str(),SettingsToJson().c_str());                
            lastChecksum = SettingsGetChecksum();
        }  

        // payload
        if(settings.m_homeassistant > 0){
            // hass pub
            //mqttClient.publish(HAM_ctrl->getStateTopic().c_str(),             
        }   
    }    
}

void MQTTHAInit(){
    /*
    HAMqttDevice::HAMqttThing stoveThing = {"Micronova Stove Ctrl","Martin Krämer","MK2", String(getChipID()), "0.1", settings.version, "", LWT_ONLINE, LWT_OFFLINE};
    HAM_ctrl = new HAMqttDevice("Control", HAMqttDevice::SWITCH, stoveThing);
    HAM_targetTemp = new HAMqttDevice("Target Temperature", HAMqttDevice::NUMBER, stoveThing);
    HAM_targetPower = new HAMqttDevice("Target Powerlevel", HAMqttDevice::NUMBER, stoveThing);
    HAM_connetionState = new HAMqttDevice("Connection State", HAMqttDevice::SENSOR, stoveThing);
    HAM_state = new HAMqttDevice("Stove State", HAMqttDevice::SENSOR, stoveThing);
    HAM_ambientTemp = new HAMqttDevice("Ambient Temperature", HAMqttDevice::SENSOR, stoveThing);
    HAM_fumeTemp = new HAMqttDevice("Fume Temperature", HAMqttDevice::SENSOR, stoveThing);
    HAM_fanSpeed = new HAMqttDevice("Fan Speed", HAMqttDevice::SENSOR, stoveThing);
    HAM_flamePower = new HAMqttDevice("Flame Power", HAMqttDevice::SENSOR, stoveThing);
    HAM_waterTemp = new HAMqttDevice("Water Temperature", HAMqttDevice::SENSOR, stoveThing);
    HAM_waterPressure  = new HAMqttDevice("Water Pressure", HAMqttDevice::SENSOR, stoveThing);
    HAM_systime        = new HAMqttDevice("Time", HAMqttDevice::SENSOR, stoveThing);


    HAM_ctrl
        ->addConfigVar("icon","mdi:power");

    HAM_targetTemp
        ->addConfigVar("device_class","temperature")
        .addConfigVar("state_class","measurement")
        .addConfigVar("unit_of_measurement","°C")
        .addConfigVar("icon","mdi:thermometer")
        .addConfigVar("min_temp", String(MIN_TARGETTEMP))
        .addConfigVar("max_temp", String(MAX_TARGETTEMP))
        .addConfigVar("min", String(MIN_TARGETTEMP))
        .addConfigVar("max", String(MAX_TARGETTEMP))
        .addConfigVar("step","0.5");
    */
}

void MQTTInit(){
    if (strlen(settings.m_user) > 0) mqtt_user = settings.m_user;
    if (strlen(settings.m_pass) > 0) mqtt_pass = settings.m_pass;

    if(settings.m_homeassistant > 0){
        MQTTHAInit();
    }

    if(strlen(settings.m_topic) > 0 && strlen(settings.m_client_id) > 0 && strlen(settings.m_host) > 0 && settings.m_port > 0){
        cmdTopic = String(MQTT_TOPIC_PREFIX + String(settings.m_topic) + MQTT_CMD_TOPIC);
        stateTopic = String(MQTT_TOPIC_PREFIX + String(settings.m_topic) + MQTT_STATE_TOPIC);
                
        mqttClient.setServer(settings.m_host, settings.m_port);
        mqttClient.setCallback(MQTTSubCallback);
        MQTTTimer.attach_ms(60000,MQTTUpdate);

        MQTTPublish(true);
    }else{
        WebLogError("MQTT Missing Parameter"); 
    }
}


