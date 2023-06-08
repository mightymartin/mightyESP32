#include <Arduino.h>
#include <WiFiManager.h>
#include <WebServer.h>
#include <HTTPUpdateServer.h>

#include "logging.h"
#include "settings.h"
#include "website.h"
#include "mqtt.h"

//##############################
//## Instances
//##############################
WebServer server(CONF_WEBSERVER_PORT);
HTTPUpdateServer httpUpdaterServer;

//##############################
//## Setup
//##############################
void setup() {
  //### Starte Serial  
  Serial.begin(CONF_SERIAL_BAUD);

  //### Settings
  WebLogInfo("--- Init ---");  
  WebLogInfo("Settings init");
  SettingsInit();

  //### Hardware
  WebLogInfo("Init Hardware");
  //keine Hardware bislang:)  
  

  //### Wifimanager

  WebLogInfo("Init Wifimanger");
  //WiFi.hostname(settings.n_hostname);
  WiFiManager wifiManager;  
  wifiManager.setConnectTimeout(10);
  wifiManager.setConfigPortalTimeout(180);
  //wifiManager.setSaveConfigCallback(saveConfigCallback);
  if(settings.u_LOGGING >= LOGLEVEL_DBG){
    wifiManager.setDebugOutput(true);
  }  
  wifiManager.autoConnect(settings.n_hostname);

  WebLogInfo("Init Weblogging");
  WebLogInit();

  //NTP Zeit
  WebLogInfo("Init Time");
  TimeInit();

  //### Init Website
  WebLogInfo("Init Website");
  WebsiteInit(&server);  
  
  //### Init UpdateServer 
  WebLogInfo("Init Updateserver");
  httpUpdaterServer.setup(&server,REQ_OTA);
  
  //### Start Webserver
  WebLogInfo("Init Webserver");
  server.begin();

  if(settings.u_MQTT){
    //### Start MQTT
    WebLogInfo("Init MQTT");
    MQTTInit();
  }

  WebLogInfo("--- Init End ---");

}

void loop() {
    // put your main code here, to run repeatedly:   
    WebLogTick();
    TimeTick(); 
    
    if(settings.u_MQTT){
      MQTTTick(); 
    }

    server.handleClient();  
}