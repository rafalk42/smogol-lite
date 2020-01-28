#pragma once

#include <functional>
#include <ESP8266WiFi.h>

#include "rkDebug.h"
#include "StatusBlinker.h"


class Wifi
{
    public:
        Wifi(Debug *debug);
        void initialize();
        bool isOnline();
        void configure(char *ssid, char *password);
    
    private:
        Debug *const debug;
        WiFiEventHandler onConnectedHandler;
        WiFiEventHandler onDisconnectHandler;
        WiFiEventHandler onGotIpAddressHandler;
        bool statusIsOnline;

        void _onConnected(const WiFiEventStationModeConnected &info);
        void _onDisconnected(const WiFiEventStationModeDisconnected &info);
        void _onGotIPAddress(const WiFiEventStationModeGotIP &ipInfo);
        const char *_disconnectResonText(WiFiDisconnectReason reason);
};
