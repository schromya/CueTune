#include <WiFi.h>
#include <WebServer.h>

/* Put your SSID & Password */
const char* ssid = "CueTune";  // Enter SSID here
const char* password = "password";  //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192,168,1,2);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

void server_setup() {
  Serial.begin(9600);
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  Serial.println("Wifi has started");
  server.begin();
  
}


void server_loop() {
  server.handleClient();
}


void handle_OnConnect() {
  server.send(200, "text/html", SendHTML()); 
}

void handle_NotFound() {
}

String SendHTML(){
  String ptr = R"delimiter(
    <!DOCTYPE html>
    <html lang="en">
      <head>
          <meta name="viewport" content="width=device-width, initial-scale=1.0" charset="UTF-8">
          <title>CueTune</title>
          <style>
          </style>
      </head>
      <body>
          <div id="main">
              <H1> Cue Tune</H1>
          </div>
          <script>
          </script>
      </body>
    </html>
  )delimiter";
  return ptr;
}