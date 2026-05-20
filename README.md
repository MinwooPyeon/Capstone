# AI 기반 우회전 차량 교통사고 예방 시스템

카메라와 YOLOv5 객체 탐지 모델을 활용하여 횡단보도 보행자를 실시간으로 감지하고, Dot-Matrix 신호등에 위험 여부를 표시해 우회전 차량으로 인한 교통사고를 예방하는 시스템입니다.

---

## 시스템 구조

```
┌─────────────┐     카메라 영상     ┌──────────────────┐
│   Camera    │ ─────────────────► │  MCU (Python)    │
└─────────────┘                    │  YOLOv5 탐지     │
                                   └────────┬─────────┘
                                            │ 시리얼 통신 (9600 baud)
                                            │ LED0 (보행자 감지)
                                            │ LED1 (보행자 없음)
                                            ▼
                                   ┌──────────────────┐
                                   │  Arduino Uno     │
                                   │  Dot-Matrix 제어 │
                                   └────────┬─────────┘
                                            │
                        ┌───────────────────┴───────────────────┐
                        ▼                                       ▼
               ┌─────────────────┐                   ┌─────────────────┐
               │  🔴 빨간 원      │                   │  🟢 초록 화살표  │
               │  (보행자 감지)   │                   │  (보행자 없음)   │
               └─────────────────┘                   └─────────────────┘
```

---

## 동작 원리

1. 카메라가 횡단보도를 실시간 촬영
2. YOLOv5 모델이 프레임에서 보행자(class 0) 감지
3. 감지 결과에 따라 Arduino로 시리얼 신호 전송
   - 보행자 감지 → `LED0` 전송 → **빨간 원** 표시 (우회전 주의)
   - 보행자 없음 → `LED1` 전송 → **초록 화살표** 표시 (우회전 가능)

---

## 기술 스택

| 분류 | 기술 |
|------|------|
| AI / 객체 탐지 | YOLOv5s (PyTorch) |
| 영상 처리 | OpenCV |
| MCU | Jetson Nano / Raspberry Pi 4,5 / PC |
| 하드웨어 | Arduino Uno, 16×16 RGB Dot-Matrix |
| 통신 | UART 시리얼 (9600 baud) |
| 언어 | Python 3, C++ (Arduino) |

---

## 디렉토리 구조

```
Capstone/
├── MCU/
│   ├── Jetson Nano/        # Jetson Nano용 Python 코드
│   ├── Raspberry pi 4/     # Raspberry Pi 4용 Python 코드
│   ├── Raspberry pi 5/     # Raspberry Pi 5용 Python 코드
│   └── PC/                 # Windows PC용 Python 코드
├── HW/
│   └── Dot Matrix/         # Arduino Dot-Matrix 제어 코드
├── Modeling/               # 신호등 케이스 3D 설계도
└── README.md
```

---

## 설치 및 실행 방법

### 1. YOLOv5 환경 설정

```bash
# YOLOv5 저장소 클론
git clone https://github.com/ultralytics/yolov5.git
cd yolov5

# 의존 라이브러리 설치
pip install -r requirements.txt

# 추가 라이브러리 설치 (시리얼 통신)
pip install pyserial
```

### 2. MCU 코드 이동

사용하는 MCU 폴더에서 `__main__.py`를 YOLOv5 폴더로 복사합니다.

```bash
# 예시: Jetson Nano 사용 시
cp "MCU/Jetson Nano/__main__.py" yolov5/
```

### 3. 환경 변수 설정 (선택)

| 환경 변수 | 기본값 | 설명 |
|-----------|--------|------|
| `SERIAL_PORT` | `/dev/ttyUSB0` (Jetson/RPi), `COM4` (PC) | Arduino 시리얼 포트 |
| `CAMERA_INDEX` | `0` | 카메라 장치 인덱스 |
| `YOLO_MODEL` | `yolov5s` | 사용할 YOLOv5 모델 |
| `CONFIDENCE_THRESH` | `0.2` | 탐지 신뢰도 임계값 |
| `PERSON_CLASS_ID` | `0` | 탐지 대상 클래스 ID |

```bash
# 예시
export SERIAL_PORT=/dev/ttyUSB1
export CONFIDENCE_THRESH=0.4
```

### 4. Arduino 코드 업로드

Arduino IDE를 사용하여 `HW/Dot Matrix/dot777.ino`를 Arduino Uno에 업로드합니다.

### 5. 실행

> **중요**: Arduino 코드를 먼저 실행한 후 Python 코드를 실행해야 합니다.

```bash
# YOLOv5 폴더에서 실행
cd yolov5
python __main__.py
```

- 종료: `q` 키 입력

---

## 주의 사항

- **Jetson Nano**: 최초 실행 및 재실행 시 모델 로딩에 시간이 오래 걸립니다. 미리 켜두는 것을 권장합니다.
- Arduino와 MCU 간 시리얼 포트 연결을 실행 전 반드시 확인하세요.
- 카메라 인덱스는 장치 환경에 따라 다를 수 있습니다.

---

## 하드웨어 설계

신호등 케이스 3D 모델링 및 설계도는 `Modeling/` 폴더에서 확인할 수 있습니다.

| 파일 | 설명 |
|------|------|
| `전체모델링.png` | 전체 신호등 모델 |
| `3D가상공간.png` | 3D 가상 배치 환경 |
| `객체탐지.png` | 객체 탐지 예시 화면 |
| `3차우회전 신호등 앞면/뒷면 설계도` | 최종 케이스 설계도 (PDF) |
