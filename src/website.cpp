#include "website.h"

WebServer *_server;

void WebsiteInit(WebServer *server){
    _server = server;
    _server->on(REQ_START, WebsiteStartPage);
    _server->on(REQ_CONFIG, WebsiteConfigPage);    
    _server->on(REQ_CONF_NETWORK, WebsiteNetworkConfigPage);    
    _server->on(REQ_CONF_MQTT, WebsiteMQTTConfigPage);        
    _server->on(REQ_CONF_MISC, WebsiteMiscConfigPage);          
    _server->on(REQ_INFO, WebsiteInfoPage);
    _server->on(REQ_FACTORY_RESET, WebsiteFactoryResetPage);  
    _server->on(REQ_OTA_SELECT, WebsiteFirmwareUpdate);  
    _server->on(REQ_CONSOLE, WebsiteConsolePage);      
}

void WebsideApplyArgs(){
    for (int i = 0; i < _server->args(); i++) {
        SettingsSetValue(_server->argName(i),_server->arg(i));
    }
}

void WebsiteAction(){
    if (_server->hasArg("ACTION") == true){
        String message = "";
        uint8_t reqReboot = 0;
        if(_server->arg("ACTION").equalsIgnoreCase("SAVE")){
            WebsideApplyArgs();
            SettingsWrite();  
        } else if(_server->arg("ACTION").equalsIgnoreCase("SAVER")){
            WebsideApplyArgs();
            SettingsWrite();            
            reqReboot = 1;
            message = "Settings saved -> Reboot";
        } else if(_server->arg("ACTION").equalsIgnoreCase("APPLY")){
            WebsideApplyArgs();            
        } else if(_server->arg("ACTION").equalsIgnoreCase("REBOOT")){            
            reqReboot = 1;
            message = "Manual Reboot";
        } else if(_server->arg("ACTION").equalsIgnoreCase("RESETS")){
            SettingsClear();                  
            reqReboot = 1;
            message = "Settings Reset";
        } else if(_server->arg("ACTION").equalsIgnoreCase("RESETW")){
            SettingsWifiReset();                  
            reqReboot = 1;
            message = "Wifi Reset; Wifimanager enabled";
        }

        if(reqReboot > 0 ){
            //Seite muss neu geladen werden. Nach X sekunden wird redirected
            String page = FPSTR(SITE_HEAD);    
            page += FPSTR(SITE_BGN); 

            page.replace("{phead}", message);    
            page.replace("{pcat}" , "Restarting...");
    
            page += FPSTR(SITE_RELOAD_WAIT); 
            page += FPSTR(SITE_END); 

            WebsiteSend(page);  
            
            SettingsSoftRestart();           
        }
    }
}

void WebsiteStartPage(){
    WebsiteAction();

    String page = FPSTR(SITE_HEAD);    
    page += FPSTR(SITE_BGN);  
    page.replace("{pcat}" , F("Home"));

    if(settings.u_DISPLAYON){
        page += FPSTR(SITE_HREF_EXT);  
        page.replace("{tit}", F("ON"));
        page.replace("{id}",  F("ACTION"));
        page.replace("{val}", F("ONOFF"));
        page.replace("{col}", F("bgrn"));
        page.replace("{dest}", REQ_START);
    }else{
        page += FPSTR(SITE_HREF_EXT);  
        page.replace("{tit}", F("OFF"));
        page.replace("{id}",  F("ACTION"));
        page.replace("{val}", F("ONOFF"));
        page.replace("{col}", F("bred"));
        page.replace("{dest}", REQ_START);
    }

    page += FPSTR(SITE_HREF);  
    page.replace("{tit}", F("Modes"));
    page.replace("{dest}", REQ_MODES);

    page += FPSTR(SITE_HREF);  
    page.replace("{tit}", F("Modes Settings"));
    page.replace("{dest}", REQ_CONF_MODES);

    page += FPSTR(SITE_HREF);  
    page.replace("{tit}", F("Display Settings"));
    page.replace("{dest}", REQ_CONF_DISPLAY);

    page += FPSTR(SITE_HREF);  
    page.replace("{tit}", F("Actions"));
    page.replace("{dest}", REQ_ACTIONS);

    page += FPSTR(SITE_HREF);  
    page.replace("{tit}", F("Infos"));
    page.replace("{dest}", REQ_INFO);
    
    page += FPSTR(SITE_HREF);  
    page.replace("{tit}", F("Console"));
    page.replace("{dest}", REQ_CONSOLE);

    page += FPSTR(SITE_HREF);  
    page.replace("{tit}", F("Config"));
    page.replace("{dest}", REQ_CONFIG);

    page += FPSTR(SITE_END); 
    
    WebsiteSend(page);    
}

void WebsiteConsolePage(){
    WebsiteAction();

    String page = FPSTR(SITE_HEAD);    
    page += FPSTR(SITE_BGN_FULL); 
    page.replace("{phead}", "Console");    

    page += FPSTR(SITE_CONSOLE);
    
    page += FPSTR(SITE_HREF);  
    page.replace("{tit}", F("Back"));
    page.replace("{dest}", REQ_START);

    page += FPSTR(SITE_END_FULL); 

    WebsiteSend(page);  
}

void WebsiteFirmwareUpdate(){
    WebsiteAction();

    String page = FPSTR(SITE_HEAD);    
    page += FPSTR(SITE_BGN);  
    page.replace("{pcat}" , F("Fimrwareupdate"));

    page += FPSTR(SITE_UPDATE_FORM);
    page.replace("{dest}" , REQ_OTA);

    page += FPSTR(SITE_HREF);  
    page.replace("{tit}", F("Back"));
    page.replace("{dest}", REQ_CONFIG);

    page += FPSTR(SITE_END); 
    
    WebsiteSend(page); 
    
}

void WebsiteFactoryResetPage(){
    WebsiteAction();

    String page = FPSTR(SITE_HEAD);    
    page += FPSTR(SITE_BGN);  
    page.replace("{pcat}" , F("Factory Reset"));

    page += FPSTR(SITE_FORM_BGN);
    page.replace("{dest}", REQ_CONFIG);

    page += FPSTR(SITE_BUTTON);  
    page.replace("{tit}", F("Abort"));
    page.replace("{id}",  F("ACTION"));
    page.replace("{val}", F(""));
    page.replace("{col}", F("bgrn"));

    page += FPSTR(SITE_BUTTON);  
    page.replace("{tit}", F("Reset Settings"));
    page.replace("{id}",  F("ACTION"));
    page.replace("{val}", F("RESETS"));
    page.replace("{col}", F("bred"));

    page += FPSTR(SITE_BUTTON);  
    page.replace("{tit}", F("Reset WiFi"));
    page.replace("{id}",  F("ACTION"));
    page.replace("{val}", F("RESETW"));
    page.replace("{col}", F("bred"));

    page += FPSTR(SITE_FORM_END);

    page += FPSTR(SITE_END); 
    
    WebsiteSend(page);  
}

void WebsiteInfoPage(){
    WebsiteAction();

    String page = FPSTR(SITE_HEAD);    
    page += FPSTR(SITE_BGN);  
    page.replace("{pcat}" , F("Infos"));

    page += FPSTR(SITE_DL_BGN);  

    page += FPSTR(SITE_DL_LINE);  
    page.replace("{tit}", F("Firmware Version"));
    page.replace("{val}", String(settings.version) );

    page += F("<br/>"); 
    page += F("<br/>"); 

    page += FPSTR(SITE_DL_LINE);  
    page.replace("{tit}", F("Chip ID"));
    page.replace("{val}", String("TODO CHIPID") );
    page += FPSTR(SITE_DL_LINE);  
    page.replace("{tit}", F("Flash Chip ID"));
    page.replace("{val}", String("TODO CHIPID") );
    page += FPSTR(SITE_DL_LINE);  
    page.replace("{tit}", F("IDE Flash Size"));
    page.replace("{val}", String(ESP.getFlashChipSize() / 1024)+F("kB"));    
    page += FPSTR(SITE_DL_LINE);  
    page.replace("{tit}", F("Program Size"));
    page.replace("{val}", String(ESP.getSketchSize() / 1024)+F("kB") );
    page += FPSTR(SITE_DL_LINE);  
    page.replace("{tit}", F("Free Program Space"));
    page.replace("{val}", String(ESP.getFreeSketchSpace() / 1024)+F("kB") );
    page += FPSTR(SITE_DL_LINE);  
    page.replace("{tit}", F("Free Memory"));
    page.replace("{val}", String(ESP.getFreeHeap() / 1024)+F("kB") );

    if(settings.u_MQTT){
        page += F("<br/>");  
    
        page += FPSTR(SITE_DL_LINE);  
        page.replace("{tit}", F("MQTT Status"));
        page.replace("{val}", MQTTStatus()  );

        page += FPSTR(SITE_DL_LINE);  
        page.replace("{tit}", F("MQTT state topic"));
        page.replace("{val}", stateTopic );

        page += FPSTR(SITE_DL_LINE);  
        page.replace("{tit}", F("MQTT cmd topic"));
        page.replace("{val}", cmdTopic );    
    }
    

    page += F("<br/>");  

    page += FPSTR(SITE_DL_LINE);  
    page.replace("{tit}", F("CPU Freq"));
    page.replace("{val}", String(ESP.getCpuFreqMHz())+F("Mhz")  );
    page += FPSTR(SITE_DL_LINE);  
    page.replace("{tit}", F("Model"));
    page.replace("{val}", ESP.getChipModel() );
    page += FPSTR(SITE_DL_LINE);  
    page.replace("{tit}", F("Core"));
    page.replace("{val}", ESP.getSdkVersion() );
    page += FPSTR(SITE_DL_LINE);  
    page.replace("{tit}", F("SDK"));
    page.replace("{val}", ESP.getSdkVersion() );
    page += FPSTR(SITE_DL_LINE);  
    page.replace("{tit}", F("Time"));
    page.replace("{val}", String(TimeformatedDateTime()) );
    
    page += F("<br/>");  

    page += FPSTR(SITE_DL_LINE);  
    page.replace("{tit}", F("IP"));
    page.replace("{val}", WiFi.localIP().toString() );
    page += FPSTR(SITE_DL_LINE);  
    page.replace("{tit}", F("MASK"));
    page.replace("{val}", WiFi.subnetMask().toString() );
    page += FPSTR(SITE_DL_LINE);  
    page.replace("{tit}", F("GATEWAY"));
    page.replace("{val}", WiFi.gatewayIP().toString() );
    page += FPSTR(SITE_DL_LINE);  
    page.replace("{tit}", F("DNS"));
    page.replace("{val}", WiFi.dnsIP().toString() );
    page += FPSTR(SITE_DL_LINE);  
    page.replace("{tit}", F("MAC"));
    page.replace("{val}", WiFi.macAddress());
    page += FPSTR(SITE_DL_LINE);  
    page.replace("{tit}", F("SSID"));
    page.replace("{val}", WiFi.SSID());
    
    page += FPSTR(SITE_DL_END);  
    
    page += FPSTR(SITE_HREF);  
    page.replace("{tit}", F("Back"));
    page.replace("{dest}", REQ_START);

    page += FPSTR(SITE_END); 
    
    WebsiteSend(page);    
}


void WebsiteConfigPage(){
    WebsiteAction();

    String page = FPSTR(SITE_HEAD);    
    page += FPSTR(SITE_BGN);   
    page.replace("{pcat}" , F("Config"));

    page += FPSTR(SITE_HREF);  
    page.replace("{tit}", F("Network Config"));
    page.replace("{dest}", REQ_CONF_NETWORK);

    if(settings.u_MQTT > 0){
        page += FPSTR(SITE_HREF);  
        page.replace("{tit}", F("MQTT Config"));
        page.replace("{dest}", REQ_CONF_MQTT);
    }

    page += FPSTR(SITE_HREF);  
    page.replace("{tit}", F("Misc. Config"));
    page.replace("{dest}", REQ_CONF_MISC);

    page += FPSTR(SITE_HREF);  
    page.replace("{tit}", F("Firmware Update"));
    page.replace("{dest}", REQ_OTA_SELECT);

    page += FPSTR(SITE_HREF_EXT);  
    page.replace("{tit}", F("Restart"));
    page.replace("{id}",  F("ACTION"));
    page.replace("{val}", F("REBOOT"));
    page.replace("{col}", F("bred"));
    page.replace("{dest}", REQ_START);

    page += FPSTR(SITE_HREF_EXT);  
    page.replace("{tit}", F("Factory Reset"));
    page.replace("{id}",  F("ACTION"));
    page.replace("{val}", F(""));
    page.replace("{col}", F("bred"));
    page.replace("{dest}", REQ_FACTORY_RESET);

    page += FPSTR(SITE_HREF);  
    page.replace("{tit}", F("Home"));
    page.replace("{dest}", REQ_START);
   
    page += FPSTR(SITE_END); 

    WebsiteSend(page); 
}

void WebsiteNetworkConfigPage(){
    WebsiteAction();

    String page = FPSTR(SITE_HEAD);    
    page += FPSTR(SITE_BGN);  
    page.replace("{pcat}" , F("Network Config"));
    
    page += FPSTR(SITE_FORM_BGN);  
    page.replace("{dest}", REQ_CONF_NETWORK);
    
    page += FPSTR(SITE_INP_T);  
    page.replace("{tit}", F("Hostname"));
    page.replace("{id}",  F(N_HOSTNAME_TAG)); 
    page.replace("{val}", String(settings.n_hostname));
    page.replace("{len}", F("32"));

    page += FPSTR(SITE_INP_N);  
    page.replace("{tit}", F("NTP Interval"));
    page.replace("{id}",  F(N_NTPINTERVAL_TAG)); 
    page.replace("{val}", String(settings.n_ntpinterval));
    page.replace("{min}", F("10000"));
    page.replace("{max}", F("60000"));
    page.replace("{step}", F("1000"));
    
    page += FPSTR(SITE_INP_T);  
    page.replace("{tit}", F("NTP Server"));
    page.replace("{id}",  F(N_NTPSERVER_TAG)); 
    page.replace("{val}", String(settings.n_ntpserver));
    page.replace("{len}", F("32"));
    

    page += FPSTR(SITE_BUTTON);  
    page.replace("{tit}", F("Save & Restart"));
    page.replace("{id}",  F("ACTION"));
    page.replace("{val}", F("SAVER"));
    page.replace("{col}", F("bred"));

    page += FPSTR(SITE_FORM_END);  
    
    page += FPSTR(SITE_HREF);  
    page.replace("{tit}", F("Back"));
    page.replace("{dest}", REQ_CONFIG);

    page += FPSTR(SITE_END); 
     
    WebsiteSend(page); 
}

void WebsiteMQTTConfigPage(){
    WebsiteAction();

    String page = FPSTR(SITE_HEAD);    
    page += FPSTR(SITE_BGN);  
    page.replace("{pcat}" , F("MQTT Config"));
    
    page += FPSTR(SITE_FORM_BGN);  
    page.replace("{dest}", REQ_CONF_MQTT);
    
    page += FPSTR(SITE_INP_N);  
    page.replace("{tit}", F("Port"));
    page.replace("{id}",  F(M_PORT_TAG)); 
    page.replace("{val}", String(settings.m_port));
    page.replace("{min}", F("1"));
    page.replace("{max}", F("60000"));
    page.replace("{step}", F("1"));

    page += FPSTR(SITE_INP_T);  
    page.replace("{tit}", F("Broker"));
    page.replace("{id}",  F(M_HOST_TAG)); 
    page.replace("{val}", String(settings.m_host));
    page.replace("{len}", F("32"));

    page += FPSTR(SITE_INP_T);  
    page.replace("{tit}", F("Client ID"));
    page.replace("{id}",  F(M_CLIENT_ID_TAG)); 
    page.replace("{val}", String(settings.m_client_id ));
    page.replace("{len}", F("32"));

    page += FPSTR(SITE_INP_T);  
    page.replace("{tit}", F("User"));
    page.replace("{id}",  F(M_USER_TAG)); 
    page.replace("{val}", String(settings.m_user));
    page.replace("{len}", F("32"));

    page += FPSTR(SITE_INP_T);  
    page.replace("{tit}", F("Password"));
    page.replace("{id}",  F(M_PASS_TAG)); 
    page.replace("{val}", String(settings.m_pass));
    page.replace("{len}", F("32"));
    
    page += FPSTR(SITE_INP_T);  
    page.replace("{tit}", F("Topic unique part"));
    page.replace("{id}",  F(M_TOPIC_TAG)); 
    page.replace("{val}", String(settings.m_topic));
    page.replace("{len}", F("32"));

    page += FPSTR(SITE_BUTTON);  
    page.replace("{tit}", F("Save & Restart"));
    page.replace("{id}",  F("ACTION"));
    page.replace("{val}", F("SAVER"));
    page.replace("{col}", F("bred"));

    page += FPSTR(SITE_FORM_END); 

    page += FPSTR(SITE_HREF);  
    page.replace("{tit}", F("Back"));
    page.replace("{dest}", REQ_CONFIG);

    page += FPSTR(SITE_END); 
     
    WebsiteSend(page); 
}

void WebsiteMiscConfigPage(){
    WebsiteAction();

    String page = FPSTR(SITE_HEAD);    
    page += FPSTR(SITE_BGN);  
    page.replace("{pcat}" , F("Misc. Config"));
    
    page += FPSTR(SITE_FORM_BGN);  
    page.replace("{dest}", REQ_CONF_MISC);

    page += FPSTR(SITE_INP_CBX_BGN);  
    page.replace("{tit}", F("use MQTT"));
    page.replace("{val}", String(settings.u_MQTT));
    page.replace("{id}",  F(U_MQTT_TAG)); 
    page += FPSTR(SITE_INP_CBX_OPT);  
    page.replace("{otit}", F("yes"));
    page.replace("{oval}", F("1"));
    page.replace("{oopt}", (settings.u_MQTT == 1) ? F("selected") : F(""));
    page += FPSTR(SITE_INP_CBX_OPT);  
    page.replace("{otit}", F("no"));
    page.replace("{oval}", F("0"));
    page.replace("{oopt}", (settings.u_MQTT == 0) ? F("selected") : F(""));
    page += FPSTR(SITE_INP_CBX_END);  
    
    page += FPSTR(SITE_INP_CBX_BGN);  
    page.replace("{tit}", F("use LDR"));
    page.replace("{val}", String(settings.u_LDR));
    page.replace("{id}",  F(U_LDR_TAG)); 
    page += FPSTR(SITE_INP_CBX_OPT);  
    page.replace("{otit}", F("yes"));
    page.replace("{oval}", F("1"));
    page.replace("{oopt}", (settings.u_LDR == 1) ? F("selected") : F(""));
    page += FPSTR(SITE_INP_CBX_OPT);  
    page.replace("{otit}", F("no"));
    page.replace("{oval}", F("0"));
    page.replace("{oopt}", (settings.u_LDR == 0) ? F("selected") : F(""));
    page += FPSTR(SITE_INP_CBX_END);  
    
    page += FPSTR(SITE_INP_CBX_BGN);  
    page.replace("{tit}", F("use MDNS"));
    page.replace("{val}", String(settings.u_MDNS));
    page.replace("{id}",  F(U_MDNS_TAG)); 
    page += FPSTR(SITE_INP_CBX_OPT);  
    page.replace("{otit}", F("yes"));
    page.replace("{oval}", F("1"));
    page.replace("{oopt}", (settings.u_MDNS == 1) ? F("selected") : F(""));
    page += FPSTR(SITE_INP_CBX_OPT);  
    page.replace("{otit}", F("no"));
    page.replace("{oval}", F("0"));
    page.replace("{oopt}", (settings.u_MDNS == 0) ? F("selected") : F(""));
    page += FPSTR(SITE_INP_CBX_END);  

    page += FPSTR(SITE_INP_CBX_BGN);  
    page.replace("{tit}", F("Log Level"));
    page.replace("{id}",  F(U_LOGG_TAG)); 
    page.replace("{val}",  String(settings.u_LOGGING)); 
    page += FPSTR(SITE_INP_CBX_OPT);  
    page.replace("{otit}", F("off"));
    page.replace("{oval}", String(LOGLEVEL_OFF));
    page.replace("{oopt}", (settings.u_LOGGING == LOGLEVEL_OFF) ? F("selected") : F(""));
    page += FPSTR(SITE_INP_CBX_OPT);  
    page.replace("{otit}", F("error"));
    page.replace("{oval}", String(LOGLEVEL_ERR));
    page.replace("{oopt}", (settings.u_LOGGING == LOGLEVEL_ERR) ? F("selected") : F(""));
    page += FPSTR(SITE_INP_CBX_OPT);  
    page.replace("{otit}", F("info"));
    page.replace("{oval}", String(LOGLEVEL_INF));
    page.replace("{oopt}", (settings.u_LOGGING == LOGLEVEL_INF) ? F("selected") : F(""));
    page += FPSTR(SITE_INP_CBX_OPT);  
    page.replace("{otit}", F("debug"));
    page.replace("{oval}", String(LOGLEVEL_DBG));
    page.replace("{oopt}", (settings.u_LOGGING == LOGLEVEL_DBG) ? F("selected") : F(""));
    page += FPSTR(SITE_INP_CBX_END); 

    page += FPSTR(SITE_BUTTON);  
    page.replace("{tit}", F("Save & Restart"));
    page.replace("{id}",  F("ACTION"));
    page.replace("{val}", F("SAVER"));
    page.replace("{col}", F("bred"));

    page += FPSTR(SITE_FORM_END); 

    page += FPSTR(SITE_HREF);  
    page.replace("{tit}", F("Back"));
    page.replace("{dest}", REQ_CONFIG);

    page += FPSTR(SITE_END); 
     
    WebsiteSend(page); 
}

void WebsiteSend(String page){
    page.replace("{phead}", settings.n_hostname);    
    page.replace("{ptit}" , settings.n_hostname);
    
    _server->sendHeader("Content-Length", String(page.length()));
    _server->send(200, "text/html", page); 

}
