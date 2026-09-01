# gesture-classifier
Two-node ESP32-S3 gesture classifier, which fuses transmitter and receiver MPU6050 IMU readings via ESPNOW. Edge Impulse model runs on-device ML inference with LED indicators.

## LED Color Codes

| Color | Meaning | 
| :--- | :---: | 
| Dim red (blink, then solid) | The classifier is about to take a capture. Data starts being collected during the LED's solid red glow. | 
| Red  | Indicates left (Tx module) "chop" gesture. |
| Orange | Indicates left (Tx module) "jab" gesture. | 
| Yellow | Indicates left (Tx module) "wave" gesture. | 
| Green | Indicates right (Rx module) "chop" gesture. | 
| Blue | Indicates right (Rx module) "jab" gesture. | 
| Violet | Indicates right (Rx module) "wave" gesture. | 

## Connection Table

| Component Pin | ESP32-S3 Pin | 
| :--- | :---: | 
| MPU6050 VCC | 3v3 | 
| MPU6050 GND | GND |
| MPU6050 SDA | GPIO4 | 
| MPU6050 SCL | GPIO5 | 
| SPDT NO | GND | 
| SPDT COM | GPIO6 | 
| SPDT NC | Disconnected | 

## Current Deliverables
### v0.1.0
Working version of one-node gesture classifier. Classifier outputs are printed out on the terminal after a one second sample is taken. Must restart the serial monitor in order to run the classifier again.

### v0.1.1 
One-node gesture classifier with extra improvements. Classifier is now triggered by a switch instead of having to restart the serial monitor. Additionally, the onboard LED now has the capability to display color codes according to classifier output.

### v1.0.0 (Current)
Two-node gesture classifier, which is wirelessly connected by ESP-NOW protocol. Has different gestures compared to the original classifier, allowing for 12 axes of sensor data instead of the original 6. Includes button control and LED indicator lights (features from last release).

### v1.1.0 (Planned)
Added BLE GATT phone connectivity to display sensor outputs.
