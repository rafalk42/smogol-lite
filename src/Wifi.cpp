#include <ESP8266mDNS.h>

#include "Wifi.h"


Wifi::Wifi(Debug *debug) :
    debug(debug),
    statusIsOnline(false)
{
}

void Wifi::initialize()
{
    WiFi.enableAP(false);
    // WiFi.begin("idiota", "idiotassdick");

    // WiFiEventHandler onStationModeAuthModeChanged(std::function<void(const WiFiEventStationModeAuthModeChanged&)>);
    // WiFiEventHandler onStationModeDHCPTimeout(std::function<void(void)>);
    // WiFiEventHandler onSoftAPModeStationConnected(std::function<void(const WiFiEventSoftAPModeStationConnected&)>);
    // WiFiEventHandler onSoftAPModeStationDisconnected(std::function<void(const WiFiEventSoftAPModeStationDisconnected&)>);
    // WiFiEventHandler onSoftAPModeProbeRequestReceived(std::function<void(const WiFiEventSoftAPModeProbeRequestReceived&)>);
    // WiFiEventHandler onWiFiModeChange(std::function<void(const WiFiEventModeChange&)>);

    onConnectedHandler = WiFi.onStationModeConnected([=](const WiFiEventStationModeConnected &info) { this->_onConnected(info); });
    onDisconnectHandler = WiFi.onStationModeDisconnected([=](const WiFiEventStationModeDisconnected &info) { this->_onDisconnected(info); });
    onGotIpAddressHandler = WiFi.onStationModeGotIP([=](const WiFiEventStationModeGotIP &ipInfo) { this->_onGotIPAddress(ipInfo); });
}

bool Wifi::isOnline()
{
    return statusIsOnline;
}

void Wifi::configure(char *ssid, char *password)
{
    debug->info("Configuring WiFi for SSID", DebugFormat::Quote, ssid, "and password", DebugFormat::Quote, password);
    WiFi.enableAP(false);
    WiFi.begin(ssid, password);
    WiFi.reconnect();
}

void Wifi::_onConnected(const WiFiEventStationModeConnected &info)
{
    debug->debug("WiFi connected to SSID", DebugFormat::Quote, DebugFormat::NoSpaceSingle, info.ssid.c_str(), ", channel ", info.channel);
}

void Wifi::_onDisconnected(const WiFiEventStationModeDisconnected &info)
{
    debug->debug("WiFi disconnected from SSID", DebugFormat::Quote, DebugFormat::NoSpaceSingle, info.ssid.c_str(),
        ", reason: ", _disconnectResonText(info.reason), DebugFormat::NoSpace , "(", info.reason, ")");
    
    if (statusIsOnline)
    {
        MDNS.end();
    }
    statusIsOnline = false;
}

void Wifi::_onGotIPAddress(const WiFiEventStationModeGotIP &ipInfo)
{
    debug->debug("IP address assigned", ipInfo.ip.toString().c_str(),
        "mask", ipInfo.mask.toString().c_str(),
        "gateway", ipInfo.gw.toString().c_str());
    
    statusIsOnline = true;

    debug->debug("Initializing mDNS responder");
    if (!MDNS.begin("Smogol-lite"))
    {
      debug->error("Error setting mDNS responder");
    }
    MDNS.addService("http", "tcp", 80);
}

const char *Wifi::_disconnectResonText(WiFiDisconnectReason reason)
{
    switch(reason)
    {
        default:
        case WIFI_DISCONNECT_REASON_UNSPECIFIED: return "unspecified";
        case WIFI_DISCONNECT_REASON_AUTH_EXPIRE: return "auth expired";
        case WIFI_DISCONNECT_REASON_AUTH_LEAVE: return "auth leave";
        case WIFI_DISCONNECT_REASON_ASSOC_EXPIRE: return "assoc expire";
        case WIFI_DISCONNECT_REASON_ASSOC_TOOMANY: return "assoc too many";
        case WIFI_DISCONNECT_REASON_NOT_AUTHED: return "not authed";
        case WIFI_DISCONNECT_REASON_NOT_ASSOCED: return "not assoced";
        case WIFI_DISCONNECT_REASON_ASSOC_LEAVE: return "assoc leave";
        case WIFI_DISCONNECT_REASON_ASSOC_NOT_AUTHED: return "assoc not authed";

        case WIFI_DISCONNECT_REASON_DISASSOC_PWRCAP_BAD: return "disassoc pwrcap bad";
        case WIFI_DISCONNECT_REASON_DISASSOC_SUPCHAN_BAD: return "dissasoc supchan bad";
        case WIFI_DISCONNECT_REASON_IE_INVALID: return "ie invalid";
        case WIFI_DISCONNECT_REASON_MIC_FAILURE: return "mic failure";
        case WIFI_DISCONNECT_REASON_4WAY_HANDSHAKE_TIMEOUT: return "4 way handshake timeout";
        case WIFI_DISCONNECT_REASON_GROUP_KEY_UPDATE_TIMEOUT: return "group key update timeout";
        case WIFI_DISCONNECT_REASON_IE_IN_4WAY_DIFFERS: return "ie in 4 way differs";
        case WIFI_DISCONNECT_REASON_GROUP_CIPHER_INVALID: return "group cipher invalid";
        case WIFI_DISCONNECT_REASON_PAIRWISE_CIPHER_INVALID: return "pairwise cipher invalid";
        case WIFI_DISCONNECT_REASON_AKMP_INVALID: return "akmp invalid";
        case WIFI_DISCONNECT_REASON_UNSUPP_RSN_IE_VERSION: return "unsupp rsn ie version";
        case WIFI_DISCONNECT_REASON_INVALID_RSN_IE_CAP: return "invalid rsn ie cap";
        case WIFI_DISCONNECT_REASON_802_1X_AUTH_FAILED: return "802.1x auth failed";
        case WIFI_DISCONNECT_REASON_CIPHER_SUITE_REJECTED: return "cipher suite rejected";

        case WIFI_DISCONNECT_REASON_BEACON_TIMEOUT: return "beacon timeout"; // 200
        case WIFI_DISCONNECT_REASON_NO_AP_FOUND: return "no ap found"; // 201
        case WIFI_DISCONNECT_REASON_AUTH_FAIL: return "auth fail"; // 202
        case WIFI_DISCONNECT_REASON_ASSOC_FAIL: return "assoc fail"; // 203
        case WIFI_DISCONNECT_REASON_HANDSHAKE_TIMEOUT: return "handshake timeout"; // 204
    }
}
