# IoT project

## 2인 프로젝트, MQTT broker 서버 구현, 하드웨어, 소프트웨어 전반적으로 담당
## MQTT 통신 프로토콜 사용
## C언어 사용
## ESP8266보드 사용

## Diagram
![Image](https://github.com/user-attachments/assets/a3d35cb7-e3f3-472d-a007-46dd4b3c8004)
**ESP8266 라이브러리 활용 ESP 보드 와이파이 연결, 라즈베리파이, 스마트폰과 동일한 ip로 접속하도록 설정** </br>
**LED제어 : Payload값을 char데이터로 저장, 0의 값이 들어오면 LED OFF/ 1의 값이 들어오면 LED ON** </br>
**DHT11과 LED가 연결된 2개의 ESP8266보드는 MQTT통신을 위해 Topic을 Publish 해준다.** </br>
**스마트폰 MQTT DASH 어플리케이션에서 동일한 Topic을 Subscribe 하여 스마트폰에서 ESP8266의 GPIO 제어**</br>

## Opearating Video Link
[Blog](https://blog.naver.com/u_j00/223831536264)
