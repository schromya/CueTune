
void setup()  {
  rfid_setup();
  server_setup();
}

void loop() {
  server_loop();
  rfid_loop();
}
