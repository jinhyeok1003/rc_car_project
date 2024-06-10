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
    
    angle   = 4
    f_or_b  = 1
    speed   = 0
    
    if lines == b'0':       #후진
      f_or_b = -1
      speed = 1
    elif lines == b'1':     #75도
      angle = 3
    elif lines == b'2':     #60도
      angle = 2
    elif lines == b'3':     #45도
      angle = 1
    elif lines == b'4':     #105도
      angle = 5
    elif lines == b'5':     #120도
      angle = 6
    elif lines == b'6':     #135도
      angle = 7
    elif lines == b'7':     #정지
      speed = 0
    elif lines == b'8':     #전진
      f_or_b = 1
      speed = 1
    elif lines == b'9':     #90도
      angle = 4
    else :
      angle = 4
      f_or_b = 1
      speed = 0
    
    if angle == 1:
      angle_f = 45
    elif angle == 7:
      angle_f = 135
    else :
      angle_f = 90 + 15 * (angle - 4)
      
    if speed == 1: 
        speed_f = 15
    else :
        speed_f = 0
      
    print("speed = %d", speed)
    print("angle = %d", angle_f)
    
    set_angle(0, angle_f)
    Motor(1, f_or_b, speed)
    
  except KeyboardInterrupt:
    destroy()
