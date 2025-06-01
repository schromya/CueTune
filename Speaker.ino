const int SPEAKER_PIN = 2;  // PAM8403


/*
  PUBLIC FUNCTION. Should be called in the primary arduino setup.
*/
void speaker_setup() {
  // Not needed yet
}


/*
  PUBLIC FUNCTION. Should be called in the primary arduino loop.
*/
void speaker_loop() {
  // Not needed yet
}



/*
  PUBLIC FUNCTION. Plays beep on speaker
*/
void speaker_beep() {
  tone(SPEAKER_PIN, 300, 50);  // 200 hz for 100 ms
}
