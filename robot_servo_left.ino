#include <Servo.h>
Servo servo_1;

int flexpin_1 = A4; //센서와 연결된 아날로그 핀 설정
int flexpin_2 = A3;
int flexpin_3 = A2;
int flexpin_4 = A1;
int flexpin_5 = A0;

void setup() {
    Serial.begin(9600); // 시리얼 통신 속도를 9600로 설정.
    servo_1.attach(8);
}

void loop() {
  int flexVal_1;  //센서값을 저장할 변수 설정
  int flexVal_2; 
  int flexVal_3;
  int flexVal_4;
  int flexVal_5;
    
  flexVal_1 = analogRead(flexpin_4);
  //Serial.println(flexVal_1);
  unsigned int servo_1_pos = map(flexVal_1, 780, 930, 0, 180);
  servo_1_pos = constrain(servo_1_pos, 0, 180);

  flexVal_2 = analogRead(flexpin_5);
  //Serial.println(flexVal_2);
  unsigned int dc_motor = map(flexVal_2, 740, 810, 0, 180);
  dc_motor = constrain(dc_motor, 0, 180);
  if     (0<=servo_1_pos && servo_1_pos <30)    servo_1_pos = 1; //서보 0도, 센서값 최소
  else if(30<=servo_1_pos && servo_1_pos <60)   servo_1_pos = 2; //서보 30도
  else if(60<=servo_1_pos && servo_1_pos <90)   servo_1_pos = 3; //서보 60도
  else if(90<=servo_1_pos && servo_1_pos <120)  servo_1_pos = 4; //서보 90도
  else if(120<=servo_1_pos && servo_1_pos <150) servo_1_pos = 5; //서보 120도
  else if(150<=servo_1_pos && servo_1_pos <180) servo_1_pos = 6; //서보 150도
  else if(180<=servo_1_pos && servo_1_pos <210) servo_1_pos = 7; //서보 180도, 센서값 최대

  if     (0<=dc_motor && dc_motor <80)    dc_motor = 1; //DC모터 정지, 센서값 최소
  else if(80<=dc_motor && dc_motor <120)   dc_motor = 2; //DC모터 움직임, 속도 1
  else if(120<=dc_motor && dc_motor <160)   dc_motor = 3; //DC모터 움직임, 속도 2
  else if(160<=dc_motor && dc_motor <=180)  dc_motor = 4; //DC모터 움직임, 속도 3
  //else if(120<=dc_motor && dc_motor <150) dc_motor = 5; //DC모터 움직임, 속도 4
  //else if(150<=dc_motor && dc_motor <180) dc_motor = 6; //DC모터 움직임, 속도 5
  //else if(180<=dc_motor && dc_motor <210) dc_motor = 7; //DC모터 움직임, 센서값 최대
  //초기 상태 0, 전진1, 후진2
  Serial.print(2);  //오른손 tag = 1, 왼손 tag = 2
  Serial.print(servo_1_pos);
  Serial.println(dc_motor);

  delay(1000);
}