import cv2
import mediapipe as mp
import tensorflow as tf
import numpy as np
import serial
import serial.tools.list_ports
import time

# TensorFlow 모델 로드
model = tf.keras.models.load_model('hand_gesture_model.h5')

# MediaPipe Hands 초기화
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(max_num_hands=1, min_detection_confidence=0.7)
mp_drawing = mp.solutions.drawing_utils

# 시리얼 포트 찾기
def find_serial_port():
    ports = serial.tools.list_ports.comports()
    for port in ports:
        print(f"Found port: {port.device}")
        return port.device
    return None

serial_port = find_serial_port()
if serial_port is None:
    print("No serial port found.")
    exit()

# 시리얼 통신 설정
'''
try:
    arduino = serial.Serial('COM8', baudrate=9600, timeout=.1)
    #time.sleep(2)  # 시리얼 통신 안정화를 위해 잠시 대기
except serial.SerialException as e:
    print(f"SerialException: {e}")
    exit()
'''

# 웹캠 캡처 설정
cap = cv2.VideoCapture(0)

def process_hand_landmarks(hand_landmarks):
    # 21개의 손 랜드마크를 (x, y, z)로 분리하여 모델 입력 형식에 맞게 정렬
    data = []
    for landmark in hand_landmarks.landmark:
        data.extend([landmark.x, landmark.y, landmark.z])
    return np.array(data).reshape(1, -1)
'''
def send_to_arduino(label):
    # 라벨 값에 따라 특정 문자를 시리얼로 전송
    if label == 0:
        arduino.write(b'0')
    elif label == 1:
        arduino.write(b'1')
    elif label == 2:
        arduino.write(b'2')
    elif label == 3:
        arduino.write(b'3')
    elif label == 4:
        arduino.write(b'4')
    elif label == 5:
        arduino.write(b'5')
    elif label == 6:
        arduino.write(b'6')
    elif label == 7:
        arduino.write(b'7')
    elif label == 8:
        arduino.write(b'8')
    elif label == 9:
        arduino.write(b'9')
    else :
        arduino.write(b'10')
'''

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break
    
    # BGR 이미지를 RGB로 변환
    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    
    # MediaPipe로 손 랜드마크 검출
    result = hands.process(frame_rgb)
    
    if result.multi_hand_landmarks:
        for hand_landmarks in result.multi_hand_landmarks:
            
            data = process_hand_landmarks(hand_landmarks)
            
            prediction = model.predict(data)
            label = np.argmax(prediction, axis=1)[0]
            
            print(f'Predicted Label: {label}')
            time.sleep(1)
            
            # 예측 결과를 아두이노로 전송q
            '''
            send_to_arduino(label)
            time.sleep(1)
            if arduino.in_waiting > 0:
                incoming_data = arduino.readline().decode('utf-8').rstrip()
                print("Arduino:", incoming_data)
            '''
            # 손 랜드마크를 영상에 그림
            mp_drawing.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)
    
    # 결과 영상 출력
    cv2.imshow('Hand Gesture Recognition', frame)
    
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# 자원 해제
cap.release()
cv2.destroyAllWindows()
#arduino.close()
