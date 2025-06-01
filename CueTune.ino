
void setup()  {
  rfid_setup();
  server_setup();
  speaker_setup();
}

void loop() {
  server_loop();
  rfid_loop();
  speaker_loop();
}
