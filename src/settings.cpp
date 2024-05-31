#include "settings.h"
#include "timeNTP.h"
#include "logging.h"

static Ticker SettingsTimer;

Settings_t settings;

uint8_t doRestart = 0;

void SettingsUpdate(){
    //Need Restart
    if(doRestart){
        doRestart = 0;
        ESP.restart();
    }

    //Time changed -> Auto On Off?
    if(settings.u_AUTO_OFF != settings.u_AUTO_ON){
        if(TimeMinutes() == 0){
            if(TimeHours() == settings.u_AUTO_OFF){
                settings.u_ONSTATE = 0;
                WebLogInfo("AUTO OFF"); 
            }
            if(TimeHours() == settings.u_AUTO_ON){
                settings.u_ONSTATE = 1;
                WebLogInfo("AUTO ON"); 
            }
        }    
    }
}

void    SettingsInit(){
    SettingsRead();
    if(!String(settings.version).equals(FW_VERSION)){
        WebLogInfo("Settings Version mismatch!");   
        settings = {};     
        SettingsClear();        
        SettingsSetDefaults();
        SettingsWrite();
    }else{
        WebLogInfo("Done");        
    }
    SettingsTimer.attach_ms(1000,SettingsUpdate);
}

void SettingsSetDefaults(){
    settings.u_chipid = getChipID();
    
    strcpy(settings.version,FW_VERSION); 
    
    settings.u_MQTT                  = 0;
    settings.u_MDNS                  = 1;
    settings.u_LOGGING               = 2;
    
    settings.n_ntpinterval           = 60000;    
    strcpy(settings.n_ntpserver,    "de.pool.ntp.org");    
    strcpy(settings.n_hostname ,    String("mightyESP32"+String(settings.u_chipid)).c_str() );
    settings.m_port                  = 1883;    
    strcpy(settings.m_host,         "192.168.33.253");    
    strcpy(settings.m_client_id,    String("mightyESP32"+String(settings.u_chipid)).c_str() );    
    strcpy(settings.m_topic,        String("mightyESP32"+String(settings.u_chipid)).c_str() );        
}

void    SettingsWrite(){    
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.put(EEPROM_START_ADDRESS,settings);
    delay(200);
    EEPROM.commit();
    EEPROM.end();
    WebLogInfo("Wrote Settings Version: " + String(settings.version));
}

void    SettingsRead(){
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.get(EEPROM_START_ADDRESS,settings);
    delay(200);
    EEPROM.end();
    WebLogInfo("Read Settings Version: " + String(settings.version));
}

void    SettingsClear(){    
    EEPROM.begin(EEPROM_SIZE);
    for (int i = EEPROM_START_ADDRESS ; i < EEPROM_SIZE ; i++) {
        EEPROM.write(i, 0);
    }
    delay(200);
    EEPROM.commit();
    EEPROM.end();
    WebLogInfo("EEPROM Cleared");
}

void    SettingsWifiReset(){    
    //Erase Wifi
    WiFi.persistent(false);     
    WiFi.disconnect(true);
    WiFi.persistent(true);

}

uint32_t SettingsGetChecksum(){
    uint32_t checksum = 0;
    uint8_t bufferP[sizeof(settings)];
    memcpy(bufferP, &settings, sizeof(settings));

    for (unsigned int i=0; i<sizeof(bufferP); i++){
        checksum += bufferP[i];
    }     
    return checksum;
}

void    SettingsSetValue(String key, String value){
    if(key.equals(U_MQTT_TAG)){
        settings.u_MQTT = (uint8_t)value.toInt();
    }else if(key.equals(U_LOGG_TAG)){
        settings.u_LOGGING = (uint8_t)value.toInt();
    }else if(key.equals(U_ONOFF_TAG)){
        settings.u_ONSTATE = (uint8_t)value.toInt();
    }else if(key.equals(U_AUTOON_TAG)){
        settings.u_AUTO_ON = (uint8_t)value.toInt();
    }else if(key.equals(U_AUTOOFF_TAG)){
        settings.u_AUTO_OFF = (uint8_t)value.toInt();    
    }else if(key.equals(N_NTPINTERVAL_TAG)){    
        settings.n_ntpinterval = (uint32_t)value.toInt();
    }else if(key.equals(N_NTPSERVER_TAG)){    
        strcpy(settings.n_ntpserver, value.c_str());
    }else if(key.equals(N_HOSTNAME_TAG)){    
        strcpy(settings.n_hostname, value.c_str());
    }else if(key.equals(M_PORT_TAG)){
        settings.m_port = (uint16_t)value.toInt();
    }else if(key.equals(M_HOST_TAG)){    
        strcpy(settings.m_host, value.c_str());
    }else if(key.equals(M_CLIENT_ID_TAG)){    
        strcpy(settings.m_client_id, value.c_str());
    }else if(key.equals(M_USER_TAG)){    
        strcpy(settings.m_user, value.c_str());
    }else if(key.equals(M_PASS_TAG)){    
        strcpy(settings.m_pass, value.c_str());
    }else if(key.equals(M_TOPIC_TAG)){
        strcpy(settings.m_topic, value.c_str());
    }
}

String  getPropInt(String key, int32_t val){
    String ret = FPSTR(PROP_INT);
    ret.replace("{key}", key);
    ret.replace("{val}", String(val) );
    return ret;
}  

String  getPropStr(String key, String val){
    String ret = FPSTR(PROP_STR);
    ret.replace("{key}", key);
    ret.replace("{val}", val );
    return ret;
}

String    SettingsToJson(){ 
    String jsonDest = "{";       
    jsonDest += getPropStr(N_HOSTNAME_TAG,              String(settings.n_hostname));
    jsonDest += getPropStr(G_VERSION,                   String(settings.version));
    jsonDest += getPropStr(G_TIME,                      String(TimeformatedTime()));

    jsonDest.remove(jsonDest.length()-1);   

    jsonDest += "}";

    return jsonDest;
}

//##############
//### misc. 
//##############

uint32_t getChipID(){
    uint32_t uid = 0; 
    for(int i=0; i<17; i=i+8) {
	  uid |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
	}
    return uid;
}

void SettingsSoftRestart(){
    doRestart = 1;
}

