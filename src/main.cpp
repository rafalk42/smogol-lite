#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>

#include "pms.h"
#include "DebugOverSerial.h"
#include "Wifi.h"
#include "StatusBlinker.h"
#include "Version.h"

#if defined(ESP8266) && !defined(D5)
  #define D5 (14)
  #define D6 (12)
  #define D7 (13)
  #define D8 (15)
  #define TX (1)
#endif


bool handleFileRead(String path);       // send the right file to the client (if it exists)
void handleSerial();
SoftwareSerial serialSensor(D7, D8);
ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80
PmsData recentFrame;
unsigned long lastPmsFrameTime = 0;
Debug *debug;
StatusBlinker *statusBlinker;
Wifi *wifi;

char hackTempSsid[64];
char hackTempPass[64];

void pmsOnFrame(PmsData frame)
{
  recentFrame = frame;
  lastPmsFrameTime = millis();
}

StatusBlinker::Status resolveStatus()
{
  auto timeSinceLastPmsFrame = millis() - lastPmsFrameTime;
  if (timeSinceLastPmsFrame > (5 * 1000))
  {
    return StatusBlinker::Status::NoSensorData;
  }
  else if (!wifi->isOnline())
  {
    return StatusBlinker::Status::NoConnection;
  }

  return StatusBlinker::Status::Nominal;
}

void setup()
{
  memset(hackTempSsid, 0, sizeof(hackTempSsid));
  memset(hackTempPass, 0, sizeof(hackTempPass));

  debug = new DebugOverSerial([](){ return millis(); }, &Serial);
  debug->info("Starting Smogol lite", VERSION_TEXT);
  
  statusBlinker = new StatusBlinker(debug, LED_BUILTIN, true);
  statusBlinker->initialize();
  statusBlinker->setStatus(StatusBlinker::Status::Initializing);
  
  debug->info("Initializing WiFi");
  wifi = new Wifi(debug);
  wifi->initialize();
  debug->info("WiFi initialized");

  ///////////////////////////////////////////
  // Initialize debug output
  // Initialize WiFi(debug)
  // Initialize filesystem(debug)
  // Initialize web server(debug, fs)
  // Initialize PMS receiver(debug, fs)
  // Initialize PMS web API
  ///////////////////////////////////////////
  //
  serialSensor.begin(9600);

  pmsRegisterFrameCallback(&pmsOnFrame);

  debug->info("Initializing SPIFFS");
  SPIFFS.begin();                           // Start the SPI Flash Files System
  debug->debug("Filesystem content listing:");
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next())
    {
      size_t fileSize = -1;
      if(dir.fileSize())
      {
          File f = dir.openFile("r");
          fileSize = f.size();
      }

      if (fileSize >= 0)
      {
        debug->debug(dir.fileName().c_str(), ", size:", fileSize, "bytes");
      }
      else
      {
        debug->debug(dir.fileName().c_str());
      }
    }
  }
  debug->debug("End of filesystem content listing");

  Dir dirWeb = SPIFFS.openDir("/web");
  while (dirWeb.next())
  {
    String filePath = dirWeb.fileName();
    String fileUri = filePath.substring(strlen("/web"));
    debug->debug("Serving", fileUri.c_str(), "from", dirWeb.fileName().c_str());
    server.serveStatic(fileUri.c_str(), SPIFFS, dirWeb.fileName().c_str());
  }
  server.serveStatic("/", SPIFFS, "/web/index.html");
  server.on(
    "/api/sensor/getRecent", HTTP_GET, []()
    {
      String content = String() + "{\"pm1\": " + recentFrame.atmPm01 + ",\"pm25\": " + recentFrame.atmPm25 + ",\"pm10\": " + recentFrame.atmPm10 + "}";
      server.sendHeader("Access-Control-Allow-Origin", "*", true);
      server.send(200, "application/json", content);
    }
  );

  server.begin();
  debug->info("HTTP server started");

  // delay(1000);

  debug->info("App initialization finished");
}

void loop()
{
  if (serialSensor.available())
  {
    pmsPushByte(serialSensor.read());
  }

  server.handleClient();

  auto currentStatus = resolveStatus();
  statusBlinker->setStatus(currentStatus);
  statusBlinker->handle();

  handleSerial();

  MDNS.update();
}

void handleCommand(const char* command)
{
  debug->debug("New command:", command);

  char *firstSpace = strchr(command, ' ');
  size_t eofCommandName = firstSpace - command;

  char commandName[32];
  char commandParam[32];
  if (firstSpace != NULL)
  {
      strncpy(commandName, command, eofCommandName);
      commandName[eofCommandName] = 0x00;
      strcpy(commandParam, firstSpace + 1);
  }
  else
  {
      strcpy(commandName, command);
  }

  if (!strcmp(commandName, "SETSSID"))
  {
    strcpy(hackTempSsid, commandParam);
  }
  else if (!strcmp(commandName, "SETPASS"))
  {
    strcpy(hackTempPass, commandParam);
  }
  else if (!strcmp(commandName, "APPLY"))
  {
    if (strlen(hackTempSsid) == 0)
    {
      debug->error("Temporary SSID value not set, use SETSSID");
    }
    else if (strlen(hackTempPass) == 0)
    {
      debug->error("Temporary password value not set, use SETPASS");
    }
    else
    {
      wifi->configure(hackTempSsid, hackTempPass);
    }
  }
}

char commandBuffer[64];
int commandBufferIndex = 0;
void handleSerial()
{
  if (!Serial.available())
  {
    return;
  }
  
  int chr = Serial.read();
  if (chr == '\n')
  {
    // null terminate the command
    commandBuffer[commandBufferIndex] = 0x00;
    handleCommand(commandBuffer);

    // reset index for a new command
    commandBufferIndex = 0;
  }
  else
  {
    commandBuffer[commandBufferIndex++] = chr;
  }
  
}

bool handleFileRead(String path)
{
  Serial.println("handleFileRead: " + path);

  if (SPIFFS.exists(path))
  {                            // If the file exists
    File file = SPIFFS.open(path, "r");                 // Open it
    size_t sent = server.streamFile(file, "text/html"); // And send it to the client
    file.close();                                       // Then close the file again
    Serial.printf("Sent %d bytes to the client\n", sent);
    
    return true;
  }

  Serial.println("\tFile Not Found");

  return false;                                         // If the file doesn't exist, return false
  // server.onNotFound([]()
  // {                              // If the client requests any URI
  //   String filePath = "/web";
  //   if (server.uri().isEmpty()
  //       || server.uri().equals("/"))
  //   {
  //     filePath += "/index.html";
  //   }
  //   else
  //   {
  //     filePath += server.uri();
  //   }

  //   if (!handleFileRead(filePath))                  // send it if it exists
  //   {
  //     server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  //   }
  // });

}