import torch
import cv2
import numpy as np
import serial
import threading

# 시리얼 통신 설정
seri = serial.Serial(port='COM4',
                     baudrate=9600,
                     parity=serial.PARITY_NONE,
                     stopbits=serial.STOPBITS_ONE,
                     bytesize=serial.EIGHTBITS)

# YOLOv5 모델 로드
model = torch.hub.load('ultralytics/yolov5', 'yolov5s')

# 객체 탐지 함수
def detect_object(frame):
    # 그레이스케일로 변환
    gray_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    # 객체 탐지
    results = model(gray_frame)

    # 탐지된 객체 추출
    detections = results.xyxy[0].detach().numpy()
    # 객체 탐지 결과 초기화
    object_classes = []

    # 탐지된 객체 반복
    for detection in detections:
        class_id = int(detection[5])
        confidence = detection[4]

        # 신뢰도가 일정 이상인 객체만 선택
        if confidence > 0.2:
            object_classes.append(class_id)

    return object_classes

# 메인 함수
def main():
    cap = cv2.VideoCapture(0)  # 웹캠 사용을 위한 비디오 캡처 객체 생성
    if not cap.isOpened():
        print("Error: Failed to open webcam.")
        return

    def process_frames():
        while True:
            ret, frame = cap.read()  # 프레임 읽기
            if not ret:
                print("Error: Failed to capture frame.")
                break

            # 객체 탐지
            object_classes = detect_object(frame)

            # 사람이 탐지된 경우 LED0 켜기
            if 0 in object_classes:
                send_to_arduino(0)
            else:
                # 사람이 탐지되지 않은 경우 LED1 켜기
                send_to_arduino(1)

    thread = threading.Thread(target=process_frames)
    thread.daemon = True
    thread.start()

    while True:
        # 'q' 키를 누르면 종료
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # 비디오 캡처 객체 해제
    cap.release()
    cv2.destroyAllWindows()

# 아두이노로 LED 상태 전송하는 함수
def send_to_arduino(led_status):
    comm = 'LED' + str(led_status)
    print('Send:', comm)
    comm += '\n'
    seri.write(bytes(comm, encoding='ascii'))

if __name__ == "__main__":
    main()
