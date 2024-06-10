void setup() {
    Serial.begin(9600);
}

void loop() {
  char rx_val = 'z';
  unsigned int asd = 20;
  //Serial.println(Serial.available());
  //delay(1000);
  //Serial.println(rx_val);
  //delay(1000);
  if (Serial.available()) {
    rx_val = Serial.read();
    //Serial.println(rx_val);
    //delay(1000);

    if (rx_val == 'a') { //손 핌 후진
      asd = 10;
    } else if(rx_val == 'b') { //조향각 3 75도
      asd = 11;
    } else if(rx_val == 'c') { //조향각 2 60도
      asd = 12;
    } else if(rx_val == 'd') { //조향각 1 45도
      asd = 13;
    } else if(rx_val == 'e') { //조향각 5 105도
      asd = 14;
    } else if(rx_val == 'f') { //조향각 6 120도
      asd = 15;
    } else if(rx_val == 'g') { //조향각 7 135도
      asd = 16;
    } else if(rx_val == 'h') { //정지
      asd = 17;
    } else if(rx_val == 'i') { //전진
      asd = 18;
    } else if(rx_val == 'j') { //90도
      asd = 19;
    } else {
      asd = 20;
    }
  }

  if (asd != 20) {
    Serial.println(asd);
    delay(1000);
  }

}
