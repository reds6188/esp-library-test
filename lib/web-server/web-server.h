#ifndef WEB_SERVER_H_
#define WEB_SERVER_H_

#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <console.h>

#define HTTP_T      "HTTP"  // HTTP console tag

void addGetCallback(const char * uri, String (*func)(uint8_t*));
extern void initWebServer(String (*func)(String, uint8_t*));

#endif  /* WEB_SERVER_H_ */