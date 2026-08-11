# gesture-classifier
An ESP32-S3 powered hand gesture classifier, which takes samples from an MPU6050 sensor and feeds them into an Edge Impulse-trained ML model. 

## LED Color Codes

| Color | Meaning | 
| :--- | :---: | 
| Red (blink) | The classifier is about to take a capture. | 
| Red (solid) | The classifier is taking a data capture, or is making a prediction using the ML model. |
| Cyan | Indicates "chop" gesture. | 
| Yellow | Indicates "circle" gesture. | 
| Purple | Indicates "jab" gesture. | 
| Green | Indicates "wave" gesture.| 
| Off | Error: classifier could not come to a conclusion. | 

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

### v0.1.1 (Finished, unmerged)
One-node gesture classifier with extra improvements. Classifier is now triggered by a switch instead of having to restart the serial monitor. Additionally, the onboard LED now has the capability to display color codes according to classifier output.

### v1.0.0 (Planned)
Two-node gesture classifier, which is wirelessly connected by ESP-NOW protocol. Has different gestures compared to the original classifier, allowing for 12 axes of sensor data instead of the original 6.

### v1.1.0 (Planned)
Added BLE GATT phone connectivity to display sensor outputs.
