#include "web-server.h"

AsyncWebServer server(80);


void addGetCallback(const char * uri, String (*func)(uint8_t*)) {
    server.on(uri, HTTP_GET, [func](AsyncWebServerRequest *request) {
        console.info(HTTP_T, "Received request: \"" + String(request->url()) + "\"");
        String payload = func(NULL);
        console.info(HTTP_T, payload);
        request->send(200, "text/json", payload);
    });
}

void initWebServer(String (*func)(String, uint8_t*))
{
    //if(!SPIFFS.begin())
    if(!SPIFFS.begin(false, "/spiffs", 10, "spiffs"))
    {
        console.error(HTTP_T, "An Error has occurred while mounting SPIFFS partition");
        return;
    }
    else {
        console.success(HTTP_T, "SPIFFS partition has mounted");
    }

    server.on("^(\\/.+\\/)*(.+)\\.(.+)$", HTTP_GET, [](AsyncWebServerRequest *request){
        String dir = request->pathArg(0);
        String name = request->pathArg(1);
        String ext = request->pathArg(2);
        String MIMEtype = "text/plain";
        String path = dir + name + "." + ext;
        if(!ext.compareTo("svg")) {
            console.log(HTTP_T, "Serving SVG file: " + dir + name);
            MIMEtype = "image/svg+xml";
        }
        else if(!ext.compareTo("css")) {
            console.log(HTTP_T, "Serving CSS file" + dir + name);
            MIMEtype = "text/css";
        }
        else if(!ext.compareTo("js")) {
            console.log(HTTP_T, "Serving JS file" + dir + name);
            MIMEtype = "text/javascript";
        }
        else if(!ext.compareTo("ico")) {
            console.log(HTTP_T, "Serving ICO file" + dir + name);
            MIMEtype = "image/x-icon";
        }
        else
            console.log(HTTP_T, "ERROR: path not recognized");
        request->send(SPIFFS, path, MIMEtype);
    });

    // Route for root / web page
    //server.on("/", HTTP_GET, [callback](AsyncWebServerRequest *request){
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        console.log(HTTP_T, "Serving index.html");
        request->send(SPIFFS, "/index.html", "text/html");
        //request->send(SPIFFS, "/index.html", String(), false, callback);
    });

    /*
    server.on("/log", HTTP_GET, [func](AsyncWebServerRequest *request) {
        console.log(HTTP_T, "Log request");
        request->send(SPIFFS, "/LOGS.txt", String(), true);
    });

    server.on("/set-credentials", HTTP_POST, [](AsyncWebServerRequest *request) {
    }, [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
    }, [func](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        //handleTest(request, data);
        //func(request->url(), data);
        //request->send(200, "text/plain", "true");
        String payload = func(request->url(), data);
        console.log(HTTP_T, payload);
        request->send(200, "text/json", payload);
    });

    server.on("/set-registration", HTTP_POST, [](AsyncWebServerRequest *request) {
    }, [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
    }, [func](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        String payload = func(request->url(), data);
        console.log(HTTP_T, payload);
        request->send(200, "text/json", payload);
    });
    */

    server.onNotFound([](AsyncWebServerRequest *request){
        console.log(HTTP_T, "ERROR 404 - The content you are looking for was not found.");
        request->send(404, "text/plain", "ERROR 404 - The content you are looking for was not found.");
    });

    // Start server
    server.begin();

    console.success(HTTP_T, "Web server was set successfully!");
}