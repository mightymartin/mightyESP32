#ifndef SETTINGS_H
#define SETTINGS_H

#include <WiFi.h> 
#include <Arduino.h> 
#include <EEPROM.h>
#include <Ticker.h>

#define QUOTE(...) #__VA_ARGS__

#define EEPROM_START_ADDRESS    0
#define EEPROM_SIZE             4095

#define FW_VERSION              "0.1"
#define CONF_WEBSERVER_PORT     80
#define CONF_WEBSOCKET_PORT     81
#define CONF_SERIAL_BAUD        115200

#define G_VERSION                   "GVER"   
#define G_TIME                      "GTIME"   
 
#define U_MQTT_TAG                  "UMQTT" 
#define U_LOGG_TAG                  "ULOGG" 
#define U_ONOFF_TAG                 "UONOFF" 
#define U_AUTOON_TAG                "UAON" 
#define U_AUTOOFF_TAG               "UAOFF" 

#define N_NTPINTERVAL_TAG           "NNTPI" 
#define N_NTPSERVER_TAG             "NNTPS"
#define N_HOSTNAME_TAG              "NHOST"
#define M_PORT_TAG                  "MPORT"
#define M_HOST_TAG                  "MBROK"
#define M_CLIENT_ID_TAG             "MCLID"
#define M_USER_TAG                  "MUSER"
#define M_PASS_TAG                  "MPASS" 
#define M_TOPIC_TAG                 "MTOP"


struct Settings_t{
    //General
    char        version[5]              ; 
    uint8_t     u_LDR                   ;
    uint8_t     u_MQTT                  ;
    uint8_t     u_MDNS                  ;
    uint8_t     u_LOGGING               ;
    uint8_t     u_ONSTATE               ;
    uint8_t     u_AUTO_ON               ;
    uint8_t     u_AUTO_OFF              ;
    uint32_t    u_chipid                ;

    //network
    uint32_t    n_ntpinterval           ;
    char        n_ntpserver[32]         ;    
    char        n_hostname[32]          ;

    //MQTT
    uint16_t    m_port                  ;
    char        m_host[32]              ;    
    char        m_client_id[32]         ;
    char        m_user[32]              ;
    char        m_pass[32]              ;
    char        m_topic[32]             ;
    
}  __attribute__((packed));

extern Settings_t settings;

extern uint8_t    doRestart;

extern void     SettingsInit();

extern void     SettingsSetDefaults();

extern uint32_t SettingsGetChecksum();
extern void     SettingsSetValue(String key, String value);
extern String   SettingsToJson();

extern void     SettingsWrite();
extern void     SettingsRead();
extern void     SettingsClear();

extern void     SettingsTick();
extern void     SettingsUpdate();

extern void     SettingsWifiReset();
extern void     SettingsSoftRestart();
extern void     SettingsTick();

#endif