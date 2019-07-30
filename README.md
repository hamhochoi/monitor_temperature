# monitor_temperature

## Thông tin hệ thống

- Hệ thống gồm 3 cảm biến nhiệt độ: MLX96014, DHT22 và SHT10
- Một IP camera Yoosee : Hướng dẫn sử dụng: https://www.youtube.com/watch?v=qY-DHCGaM38
- Mỗi cảm biến được điều khiển bởi một mạch vi xử lý esp8266 có nhiệm vụ đọc dữ liệu và gửi dữ liệu về  cho Raspberry Pi 3 (đóng vai trò một máy chủ)
- Dữ liệu được truyền theo cơ chế publish / subscripbe (giao thức MQTT)
    - Máy chủ MQTT: "broker.hivemq.com"
    - Topic nhận dữ liệu từ cảm biến MLX96014: "server_room/mlx96014/pub"
    - Topic nhận dữ liệu từ cảm biến DHT22: "server_room/dht22/pub"
    - Topic nhận dữ liệu từ cảm biến SHT10: "server_room/sht10/pub"

- Máy chủ (Raspberry Pi 3) cài IoT Platform Openhab để quản lý các thiết bị. Openhab sẽ nhận dữ liệu từ các thiết bị bằng cách lắng nghe trên máy chủ MQTT "broker.hivemq.com" tại các topic của các cảm biến phía trên.
    - Phiên bản docker openhab (trên Pi): openhab:2.2.0-armhf-debian
    - Phiên bản docker openhab (trên Linux): openhab:2.2.0-amd64-debian
    - Các addons trên Openhab cần cài đặt: MQTT bindings (cho phép nhận dữ liệu bằng giao thức MQTT); JSONPath Transformation (cho phép đọc json) 
    - OpenHab platform chạy trong mạng CDCT, địa chỉ 192.168.1.157 cổng 8080
    - API lấy ra các cảm biến trong hệ thống:
        - ví dụ: curl -X GET --header "Accept: application/json" "http://192.168.1.157:8080/rest/items?recursive=false"
    - Để kết nối tới internet, hiện tại ngrok được sử dụng để tạo public port cho Pi để  kết nối tới internet. 
        - Địa chỉ public hiện tại của giao diện quản lý: http://e69d3329.ngrok.io/basicui/app?fbclid=IwAR1hJhlYqEAxsTCMBEYryPahvtDnXjc8498-miQFb18pOPrjLvSQpbTr_Mk

## Code arduino
- Các code cho mạch điều khiển esp8266 (một loại arduino) được đặt trong: dht22_webserver_esp8266, mlx90614_esp8266_webserver, sht10_webserver_esp8266


## Thông tin về  Raspberry Pi 3
- Địa chỉ: 192.168.1.157
- user: pi
- Pass: raspberry
- ssh: ssh pi@r192.168.1.157  - pass: raspberry
- Thư mục chứa các file của openhab: /opt/openhab
- Cách chạy ngrok: /home/pi/rule_server/ngrok http 8080


# Lưu ý
- Các cảm biến gửi dữ liệu lên MQTT free server nên cần kết nối tới internet. Nếu mất mạng internet, sẽ không gửi được dữ liệu.
- Hiện tại các cảm biến được cài đặt mạng wifi CDCT (mật khẩu: cdct2019), nếu mất mạng sẽ không hoạt động được. Các mạch điều khiển sẽ liên tục thử  kết nối tới mạng wifi CDCT cho tới khi kết nối được mạng và MQTT server.
- Các mạch điều khiển cho phép thay đổi mạng wifi mà không cần phải nạp lại code bằng cách kết nối tới mạng Switch (được tạo ra bởi các esp8266 khi không kết nối được tới wifi ngay sau khi reset), vào địa chỉ 192.168.1.4, nhập địa chỉ thông tin mạng mới muốn kết nối, rồi gửi dữ liệu. Sau đó đợi thông báo thành công trên giao diện rồi rút nguồn của mạch esp8266 ra cắm lại để reset lại mạch. Thông tin mạng wifi mới sẽ được lưu vào ROM và được cập nhập khi esp8266 reset thành công.
- ngrok là 1 tools miễn phí nên có một số nhược điểm: Khi Pi phải khởi động lại, ta phải chạy lại ngrok để tạo public IP. Do đó địa chỉ giao diện quản lý sẽ bị thay đổi.
- Nếu ngay sau khi reset, esp8266 không bắt được wifi đã cài đặt, sẽ tạo ra một mạng tên Switch, và 1 web server tại địa chỉ 192.168.1.4 để cho người dùng nhập thông tin wifi mới.
- Nếu esp8266 đã bắt được mạng wifi, nhưng trong quá trình chạy, kết nối wifi bị mất, sẽ liên tục kết nối lại wifi mà không tạo ra mạng mới Switch.

# Repo:
- https://github.com/hamhochoi/monitor_temperature