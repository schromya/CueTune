#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

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
  server.begin();

}


void server_loop() {
  server.handleClient();
}


void handle_on_connect() {
  server.send(200, "text/html", SendHTML()); 
}

void handle_upload() {

  String body = server.arg("plain");

  // Parse JSON
  DynamicJsonDocument data(1024);
  DeserializationError error = deserializeJson(data, body);
  if (error) {
    Serial.println("JSON Parsing Error: " + String(error.c_str()));
    server.send(400, "application/json", "{\"error\": \"Invalid JSON\"}");
    return;
  }
  String str = data["data"];

  rfid_set_next_write(str);

  Serial.println(F("Will upload data on next tap."));
  server.send(200, "application/json", "{\"data\": \"Will upload data on next tap.\"}");

}


void handle_rfid() {
  String data = rfid_get_last_read();
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
              <label for="rfid_input">RFID Data</label>
              <input type="text" id="rfid_input" />

              <button id="upload_button" type="button">Upload Data!</button>
              <br>
              <p> <b>Latest RFID Data:</b> <p>
              <p id="rfid_data"><p>
          </div>
          <script>
            document.getElementById('upload_button').addEventListener('click', () => {
              const rfid_data = document.getElementById("rfid_input").value;
              fetch("upload", {
                headers: { "Content-Type": "application/json"},
                method: "POST",
                body: JSON.stringify({data: rfid_data})
              })
              .then(res => {
                  if (res.status != 200) alert("Sorry, there was an error uploading!")
              })
            });

            function poll_rfid() {
              fetch("rfid", {
                headers: { "Content-Type": "application/json"},
                method: "GET",
              })
              .then(res => {
                  if (!res.ok) throw new Error("Fetch error");
                  return res.json();
              })
              .then(json => {
                document.getElementById('rfid_data').innerText = json.data;
              })
              .catch(err => {
                console.error("Polling error:", err);
              });
            }

            setInterval(poll_rfid, 300);  // Poll every 300 ms

          </script>
      </body>
    </html>
  )delimiter";
  return ptr;
}

