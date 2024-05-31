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
Ticker restartTicker;

//##############################
//## Tools
//##############################
void restartOnConnectionLost(){
    //Connection Lost, Need Restart
    if (WiFi.status() != WL_CONNECTED){
        ESP.restart();
    }
}

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
  WiFiManager wifiManager;  
  wifiManager.setConnectTimeout(30);
  wifiManager.setConfigPortalTimeout(180);
  wifiManager.setWiFiAutoReconnect(true);
  
  if(settings.u_LOGGING >= LOGLEVEL_DBG){
    wifiManager.setDebugOutput(true);
  }  
  wifiManager.autoConnect(settings.n_hostname);

  //### Logging
  WebLogInit();
  WebLogInfo("Init Weblogging");

  //### NTP Zeit
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

  //### Start restartTicker
  restartTicker.attach_ms(30000,restartOnConnectionLost);


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