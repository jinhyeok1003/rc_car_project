#!/usr/bin/env/python3
import serial
import time
from board import SCL, SDA
import busio
#from adafruit_motor import servo,motor
#from adafruit_motor import motor
from adafruit_pca9685 import PCA9685
from adafruit_motor import motor
from adafruit_motor import servo

# motor_EN_A: Pin7  |  motor_EN_B: Pin11
# motor_A:  Pin8,Pin10    |  motor_B: Pin13,Pin12


MOTOR_M1_IN1 =  15      #Define the positive pole of M1
MOTOR_M1_IN2 =  14      #Define the negative pole of M1

Dir_forward   = 0
Dir_backward  = 1

left_forward  = 1
left_backward = 0

right_forward = 0
right_backward= 1

pwn_A = 0
pwm_B = 0
  
def map(x,in_min,in_max,out_min,out_max):
  return (x - in_min)/(in_max - in_min) *(out_max - out_min) +out_min


#def setup():
i2c = busio.I2C(SCL, SDA)
  #i2c = busio.I2C()
  # Create a simple PCA9685 class instance.
#  pwm_motor.channels[7].duty_cycle = 0xFFFF
pca = PCA9685(i2c, address=0x5f) #default 0x40
pca.frequency = 50
 # time.sleep(0.1)
#  pwm_motor.channels[7].duty_cycle = 0xFFFF

#  motor11 = motor.DCMotor(pca.channels[5], pca.channels[6])
motor1 = motor.DCMotor(pca.channels[MOTOR_M1_IN1],pca.channels[MOTOR_M1_IN2] )
motor1.decay_mode = (motor.SLOW_DECAY)
#  motorStop()

def Motor(channel,direction,motor_speed):
  if motor_speed > 100:
    motor_speed = 100
  elif motor_speed < 0:
    motor_speed = 0
  speed = map(motor_speed, 0, 100, 0, 1.0)
  if direction == -1:
    speed = -speed

  if channel == 1:
    motor1.throttle = speed
    # print("1111")


def motorStop():#Motor stops
    motor1.throttle = 0

def destroy():
  motorStop()
  pca.deinit()

def set_angle(ID, angle):
    servo_angle = servo.Servo(pca.channels[ID], min_pulse=200, max_pulse=3000,actuation_range=180)
    servo_angle.angle = angle

port = serial.Serial("/dev/ttyACM0", baudrate=9600, timeout=None)

while True:
  try:
  
    lines = port.readline()
    lines = lines.strip()
    
    angle = 0;
    f_or_b = 0;
    speed = 0;
    
    angle_in  = lines[0:1]
    f_or_b_in = lines[1:2]
    speed_in  = lines[2:3]
    
    #if_error     = lines[0:2]
    
    #if if_error == b'00':
    
    if angle_in == b'1':
      angle = 1
    elif angle_in == b'2':
      angle = 2
    elif angle_in == b'3':
      angle = 3
    elif angle_in == b'4':
      angle = 4
    elif angle_in == b'5':
      angle = 5
    elif angle_in == b'6':
      angle = 6
    elif angle_in == b'7':
      angle = 7
    else :
      angle = 100
      
    if f_or_b_in == b'1':
      f_or_b = 1
    elif f_or_b_in == b'2':
      f_or_b = -1
    
    if speed_in == b'1':
      speed = 1
    elif speed_in == b'2':
      speed = 2
    elif speed_in == b'3':
      speed = 3
    elif speed_in == b'4':
      speed = 4
    elif speed_in == b'5':
      speed = 5
    elif speed_in == b'6':
      speed = 6
    elif speed_in == b'7':
      speed = 7
    else :
      speed = 100
    
    if speed == 1:
      speed_set = 0
    elif speed == 2:
      speed_set = 15
    else :
      speed_set = 70 * (speed - 1) / 7
    print(speed_set)
    if angle == 1:
      angle_f = 30
    elif angle == 7:
      angle_f = 150
    else :
      angle_f = 90 + 15 * (angle - 4)
    print(angle_f)
    
    
    
    set_angle(0, angle_f)
    Motor(1, f_or_b, speed_set)
    
  except KeyboardInterrupt:
    destroy()
