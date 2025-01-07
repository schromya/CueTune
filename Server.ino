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
  
  server.on("/", handle_on_connect);
  server.on("/upload", handle_upload);
  server.on("/rfid", handle_rfid);
  server.onNotFound(handle_not_found);
  Serial.println("Wifi has started");
  server.begin();
  
}


void server_loop() {
  server.handleClient();
}


void handle_on_connect() {
  server.send(200, "text/html", SendHTML()); 
}

void handle_upload() {
  Serial.print(F("Made it here!\n"));
  String str = "YOLO";
  write_data(str);
  server.send(200, "application/json", "{\"data\": \"Uploaded data\"}");
}

void handle_rfid() {
  String data = read_data();
  Serial.print("HERE data " + data);
  server.send(200, "application/json", "{\"data\": \"" + data + "\"}");

}


void handle_not_found() {
  Serial.println("404 Not Found: " + server.uri()); // Print the request URI
  server.send(404, "text/plain", "Not Found");
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
              <button id="upload_button" type="button">Upload Data!</button>
              <p id="rfid_data"><p>
              <button id="rfid_button" type="button">Get RFID data!</button>
          </div>
          <script>
            document.getElementById('upload_button').addEventListener('click', () => {
              fetch("upload", {
                headers: { "Content-Type": "application/json"},
                method: "POST",
                body: "{}"
              })
              .then(res => {
                  if (res.status != 200) alert("Sorry, there was an error uploading!")
              })
            });

            document.getElementById('rfid_button').addEventListener('click', () => {
              console.log("Clicked button!");
              fetch("rfid", {
                headers: { "Content-Type": "application/json"},
                method: "GET",
              })
              .then(res => {
                  if (res.status != 200) alert("Sorry, there was an error uploading!")
                  else return res.json();
              })
              .then(json => {
                console.log(json)
                document.getElementById('rfid_data').innerText = "Data:" + json.data;
              })
            });
          </script>
      </body>
    </html>
  )delimiter";
  return ptr;
}

