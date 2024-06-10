void setup() {
  Serial.begin(9600);
}

void loop() {
  char motion_val = 'z';
  // Serial.println(motion_val);
  char motion_command = 'z';
    motion_val = Serial.read();
    

    if (motion_val == '0') //손 핌 후진
    {
      motion_command = 'a';
    }  
    else if (motion_val == '1') //조향각 3 75도
    {
      motion_command = 'b';
    }
    else if (motion_val == '2') //조향각 2 60도
    {
      motion_command = 'c';
    }
    else if (motion_val == '3') //조향각 1 45도
    {
      motion_command = 'd';
    }
    else if (motion_val == '4') //조향각 5 105도
    {
      motion_command = 'e';
    }
    else if (motion_val == '5') //조향각 6 120도
    {
      motion_command = 'f';
    }
    else if (motion_val == '6') //조향각 7 135도
    {
      motion_command = 'g';
    }
    else if (motion_val == '7') //정지
    {
      motion_command = 'h';
    }
    else if (motion_val == '8') //전진
    {
      motion_command = 'i';
    }
    else if (motion_val == '9') //90도
    {
      motion_command = 'j';
    }
    else
    {
      motion_command = 'z';
    }

  if (motion_command != 'z') {
    Serial.println(motion_command);
    delay(1000);
  }
  
}
