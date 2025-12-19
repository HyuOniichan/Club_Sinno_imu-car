**Setup**
+ Mở file bằng Arduino IDE
+ Chạy host MQTT server (bằng Docker trên laptop hoặc bằng mosquitto trên Raspberry Pi)
+ Sửa file `config.h`
  - Wifi: SSID (tên wifi) và password (nếu không có thì để chuỗi rỗng "")
  - MQTT client id, server ip & port, username & password
+ Nạp code
  - `club_imu_car_control_hand.ino` và `config.h` cho con esp nối với cảm biến gia tốc (cầm tay)
  - `club_imu_car_control.ino` và `config.h` cho con esp trên khung xe
+ Lắp 2 cục pin liion vào xe

**Giải pháp thay thế**
+ Thay sensor bằng điều khiển cầm tay
+ Thay MQTT bằng bluetooth giữa 2 con esp32
