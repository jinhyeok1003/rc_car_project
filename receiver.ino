#include <SoftwareSerial.h>
SoftwareSerial bluetooth(8,9);

unsigned int reg_direction; // 방향 레지스터
unsigned int reg_speed;
unsigned int reg_driving;
int valid;                  // 에러 발생하면 valid = 0, 방향 초기 상태 도달 시 valid = 1

int b_reset;
int b_manual;

String step = "0123456789abcdefsq";   // get_data error : 0, valid == 0 : f, no error : 1, reset : r
                                    // bluetooth direction : 1 ~ 7
                                    // bluetooth speed : 1 ~ e

int get_data(int *direction_R_, int *speed_R_, int *direction_L_, int *speed_L_);
int get_tx_direction(int prev_valid, int *direction_R_, int *direction_L_, int *tx_direction_);
int get_tx_driving_speed(int prev_valid, int *speed_R_, int *speed_L_, int *tx_driving_, int *tx_speed_);
void send_data_to_RCcar(int reg_direction_, int reg_driving_, int reg_speed_);
void appinventor_interface(int *reg_direction_, int *reg_driving_, int *reg_speed_);
int start_manual_operation(void);
void get_manual_operation(int *b_direction_, int *b_speed_, int *b_driving_);
void reset(void);

void setup() {
  bluetooth.begin(9600);
  Serial.begin(9600); // 시리얼 통신 속도를 9600로 설정.
  reg_direction = 4;
  reg_speed = 1;
  reg_driving = 1;
  valid = 0;
  b_reset = 1;      // active low reset
  b_manual = 0;     // neither manual nor not_manual
}

void loop() {
  unsigned int direction_R;
  unsigned int direction_L;
  unsigned int speed_R;
  unsigned int speed_L;
  unsigned int tx_direction = reg_direction;
  unsigned int tx_speed;
  unsigned int tx_driving;

  int b_in;
  int b_direction;
  int b_speed;
  int b_driving;
  //Serial.print(b_reset);
  if (bluetooth.available() && b_reset) {
    Serial.println("not reset");
    b_in = bluetooth.read();
    b_reset = !(b_in == '0');
    reset();
  } else if (bluetooth.available() && (b_manual == 0)) {
    Serial.println("done reset");
    b_manual = start_manual_operation();

  }

  if (!b_reset && (b_manual == -1)) {                 // not using manual operation, but flex sensors
    Serial.println("not manual");
    while (!get_data(&direction_R, &speed_R, &direction_L, &speed_L)) {
      bluetooth.print(step.substring(0,1));
    }

    int prev_valid = valid;
    tx_direction = reg_direction; 
    tx_speed = reg_speed;
    tx_driving = reg_driving;

    valid = get_tx_direction(prev_valid, &direction_R, &direction_L, &tx_direction) &&
            get_tx_driving_speed(prev_valid, &speed_R, &speed_L, &tx_driving, &tx_speed);
    Serial.print("valid : ");
    Serial.println(valid);
    if (!valid)                     // if error, previous direction & stop
    {
      tx_direction = reg_direction;
      tx_speed = 1;
      tx_driving = 1;
      bluetooth.print(step.substring(15,16));
    } else {
      bluetooth.print(step.substring(1,2));
    }
    reg_direction = tx_direction;    // 조향각을 레지스터에 저장 
    reg_speed = tx_speed;
    reg_driving = tx_driving;
    appinventor_interface(tx_direction, tx_driving, tx_speed);
    b_manual = start_manual_operation();

  } else if (!b_reset && (b_manual == 1)) {           // not using flex sensors, but manual operation
    Serial.println("manual");
    //transform_data_appinventor(&b_direction, &b_speed);
    b_direction = reg_direction;
    b_speed = reg_speed;
    b_driving = reg_driving;

    get_manual_operation(&b_direction, &b_speed, &b_driving);
    
    reg_direction = b_direction;
    reg_speed = b_speed;
    reg_driving = b_driving;

    
  } else {
    Serial.println("error");
    //bluetooth.print('5');
  }

  send_data_to_RCcar(reg_direction, reg_driving, reg_speed);

  appinventor_interface(reg_direction, reg_driving, reg_speed);

  delay(1000);
}

int get_data(int *direction_R_, int *speed_R_, int *direction_L_, int *speed_L_) {
  
  unsigned int flexVal_1 = Serial.parseInt(); // 태그+센서값 정수로 수신
  delay(500);
  unsigned int flexVal_2 = Serial.parseInt();

  Serial.print("flexVal_1 : ");
  Serial.println(flexVal_1);
  Serial.print("flexVal_2 : ");
  Serial.println(flexVal_2);

  int tag_1 = flexVal_1 / 100;      // 태그1
  int dat_1 = flexVal_1 % 100;      // 센서값1
  int tag_2 = flexVal_2 / 100;      // 태그2
  int dat_2 = flexVal_2 % 100;      // 센서값2
  
  if (tag_1 == 1 && tag_2 == 2)                  // 첫번째로 받은 값이 오른손인 경우
  {
    *direction_R_ = dat_1 / 10;
    *speed_R_ = dat_1 % 10;
    
    *direction_L_ = dat_2 / 10;
    *speed_L_ = dat_2 % 10;
    
    return 1;
  }
  else if (tag_1 == 2 && tag_2 == 1)             // 첫번째로 받은 값이 왼손인 경우
  {
    *direction_R_ = dat_2 / 10;
    *speed_R_ = dat_2 % 10;
    
    *direction_L_ = dat_1 / 10;
    *speed_L_ = dat_1 % 10;

    return 1;
  }
  else                             // 값을 제대로 받지 못함
  {
    return 0;
  }
}

int get_tx_direction(int prev_valid, int *direction_R_, int *direction_L_, int *tx_direction_) {
  if ((*direction_R_ == 7 || *direction_R_ == 6) 
   && (*direction_L_ == 7 || *direction_L_ == 6))  // 왼손 오른손 신호 주먹으로 동일(동작하기 위한 초기 상태)
  {
    *tx_direction_ = 4;              // 전방 
    return 1;                      // 동작 가능
  }

  else if ((*direction_L_ == 7 || *direction_L_ == 6) && *direction_R_ < 7) // 왼손 주먹 -> 마주볼 때 바퀴가 왼쪽으로 휨 
  {
    if((*direction_R_ == 5)|(*direction_R_ == 6))
    {
      *tx_direction_ = 3;
    }

    else if((*direction_R_ == 3)|(*direction_R_ == 4))
    {
      *tx_direction_ = 2;
    }

    else if((*direction_R_ == 1)|(*direction_R_ == 2))
    {
      *tx_direction_ = 1;
    }
    return prev_valid;
  } 

  else if((*direction_R_ == 7 || *direction_R_ == 6) && *direction_L_ < 7) // 오른손 주먹 -> 마주볼 때 바퀴가 오른쪽으로 휨
  {
    if((*direction_L_ == 5)|(*direction_L_ == 6))
    {
     *tx_direction_ = 5;
    }

    else if((*direction_L_ == 3)|(*direction_L_ == 4))
    {
     *tx_direction_ = 6;
    }

    else if((*direction_L_ == 1)|(*direction_L_ == 2))
    {
     *tx_direction_ = 7;
    }
    return prev_valid;
  }

  else // error state
  {
    return 0;
  }

}

int get_tx_driving_speed(int prev_valid, int *speed_R_, int *speed_L_, int *tx_driving_, int *tx_speed_) {
  if (*speed_R_ == *speed_L_)
  {
    *tx_driving_ = 1;
    *tx_speed_ = 1;                // 정지
    return 1;
  }
  
  else if (*speed_R_ > *speed_L_)     // 오른쪽 손목이 더 꺾여있을 때는 전진
  {
    *tx_driving_ = 1;               // 전진 = 1
    *tx_speed_ = *speed_R_ - *speed_L_ + 1;  // 오른 손목과 왼 손목의 차이만큼으로 속도 결정
    return 1;
  }

  else if (*speed_R_ < *speed_L_)     // 왼쪽 손목이 더 꺾여있을 때는 후진
  {
    *tx_driving_ = 2;               // 후진 = 2
    *tx_speed_ = *speed_L_ - *speed_R_ + 1;  // 왼 손목과 오른 손목의 차이만큼으로 속도 결정
    return 1;
  }

  else                          // error state
  {
    return 0;
  }
}

void send_data_to_RCcar(int reg_direction_, int reg_driving_, int reg_speed_) {
  Serial.print(reg_direction_);
  Serial.print(reg_driving_);
  Serial.println(reg_speed_);
  
  delay(1000);
}

void appinventor_interface(int reg_direction_, int reg_driving_, int reg_speed_) {

  String b_direction = step.substring(reg_direction_, reg_direction_ + 1);
  String b_speed = step.substring(reg_speed_ + (reg_driving_ - 1) * 7, reg_speed_ + (reg_driving_ - 1) * 7 + 1);

  bluetooth.print(b_direction + b_speed);
  //Serial.println(b_direction + b_speed);
}

int start_manual_operation(void) {
  if (bluetooth.available()) {
    int b_in = bluetooth.read();
    int manual = (b_in == 'm');
    int not_manual = (b_in == 'n');
    int reset = (b_in == '1');
    if (manual) {
      return 1;
    } else if (not_manual) {
      return -1;
    } else if (reset) {
      b_reset = 1;
      bluetooth.print(step.substring(17,18));     // 'q'uit
      return 0;
    } else return b_manual;
  }
  else return b_manual;
}

void get_manual_operation(int *b_direction_, int *b_speed_, int *b_driving_) {
  if (bluetooth.available()) {
    int b_in = bluetooth.read();
    if (b_in == 'a') {            // left
      if (*b_direction_ > 1) {    // not maximum left direction
        *b_direction_ -= 1;
      }
    } else if (b_in == 'b') {     // right
      if (*b_direction_ < 7) {    // not maximum right direction
        *b_direction_ += 1;
      }
    } else if (b_in == 'c') {     // forward
      if (*b_speed_ < 7 && *b_driving_ == 1) {          // moving forward, but not maximum speed
        *b_speed_ += 1;
      } else if (*b_speed_ > 1 && *b_driving_ == 2) {   // moving backward
        *b_speed_ -= 1;
      } else if (*b_speed_ == 1 && *b_driving_ == 2) {  // backward stop
        *b_speed_ = 2;
        *b_driving_ = 1;
      }
    } else if (b_in == 'd') {     // backward
      if (*b_speed_ < 7 && *b_driving_ == 2) {          // moving backward, but not maximum speed
        *b_speed_ += 1;
      } else if (*b_speed_ > 1 && *b_driving_ == 1) {   // moving forward
        *b_speed_ -= 1;
      } else if (*b_speed_ == 1 && *b_driving_ == 1) {  // forward stop
        *b_speed_ = 2;
        *b_driving_ = 2;
      }
    } else if (b_in == 'e') {      // immediate stop
      *b_speed_ = 1;
      *b_driving_ = 1;
    } else if (b_in == 'm') {
      b_manual = 1;
    } else if (b_in == 'n') {
      b_manual = -1;
    } else if (b_in == '1') {
      b_reset = 1;                   // reset
    }
  }
  //return 1;
}

void reset(void) {
  if (!b_reset) {
    reg_direction = 4;
    reg_speed = 1;
    reg_driving = 1;
    b_manual = 0;
    bluetooth.print(step.substring(16,17));     // 's'tart
  }
}
/*
void transform_data_appinventor(int *b_direction_, int *b_speed_) {
  *b_direction_ = reg_direction;
  *b_speed_ = reg_speed + (reg_driving - 1) * 7;
}*/
