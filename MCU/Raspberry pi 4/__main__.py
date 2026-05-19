import os
import torch
import cv2
import serial

SERIAL_PORT       = os.getenv("SERIAL_PORT", "/dev/ttyUSB0")
CAMERA_INDEX      = int(os.getenv("CAMERA_INDEX", "0"))
YOLO_MODEL        = os.getenv("YOLO_MODEL", "yolov5s")
CONFIDENCE_THRESH = float(os.getenv("CONFIDENCE_THRESH", "0.2"))
PERSON_CLASS_ID   = int(os.getenv("PERSON_CLASS_ID", "0"))

try:
    seri = serial.Serial(
        port=SERIAL_PORT,
        baudrate=9600,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=2,
    )
except serial.SerialException as e:
    print(f"Error: 시리얼 포트 연결 실패 ({SERIAL_PORT}): {e}")
    raise SystemExit(1)

model = torch.hub.load('ultralytics/yolov5', YOLO_MODEL)


def detect_object(image):
    results = model(image)
    detections = results.xyxy[0].cpu().numpy()

    object_positions = []
    object_classes = []

    for detection in detections:
        class_id = int(detection[5])
        confidence = detection[4]

        if confidence > CONFIDENCE_THRESH:
            center_x = int((detection[0] + detection[2]) / 2)
            center_y = int((detection[1] + detection[3]) / 2)
            object_positions.append((center_x, center_y))
            object_classes.append(class_id)

    return object_positions, object_classes


def sendToArduino(led_status):
    comm = f"LED{led_status}\n"
    print(f"Send: LED{led_status}")
    seri.write(comm.encode("ascii"))


def main():
    cap = cv2.VideoCapture(CAMERA_INDEX)
    if not cap.isOpened():
        print(f"Error: 카메라를 열 수 없습니다. (index={CAMERA_INDEX})")
        return

    last_led_status = None

    while True:
        ret, frame = cap.read()
        if not ret:
            print("Error: 프레임을 읽을 수 없습니다.")
            break

        object_positions, object_classes = detect_object(frame)

        led_status = 0 if PERSON_CLASS_ID in object_classes else 1

        if led_status != last_led_status:
            sendToArduino(led_status)
            last_led_status = led_status

        cv2.imshow('Object Detection', frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
