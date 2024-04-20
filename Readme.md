
# MAX30100 ESP8266 WebServer

This project implements a web server on an ESP8266 microcontroller to display sensor readings from a MAX30100 pulse oximeter and control the display remotely.

## Table of Contents
1. [Requirements](#requirements)
2. [Setup](#setup)
3. [Usage](#usage)
4. [Contributing](#contributing)
5. [License](#license)

## Requirements <a name="requirements"></a>
- **Hardware:**
  - ESP8266 microcontroller
  - MAX30100 pulse oximeter sensor
  - SSD1306 OLED display (optional for local display)
- **Software:**
  - Arduino IDE
  - Libraries:
    - ESP8266WebServer
    - Wire
    - MAX30100_PulseOximeter
    - ThingSpeak
    - Adafruit_GFX
    - Adafruit_SSD1306

## Setup 
1. Connect the hardware components as per the wiring instructions in the code.
2. Install the required libraries in the Arduino IDE.
3. Upload the code to your ESP8266 microcontroller.

## Usage 
1. Power on the ESP8266 microcontroller.
2. Connect to the Wi-Fi network hosted by the microcontroller.
3. Open a web browser and navigate to the IP address of the microcontroller.
4. You can view the sensor readings (BPM and SpO2) on the web interface.
5. Toggle the display on/off using the provided button.
6. Optionally, you can integrate with ThingSpeak to log sensor data.


## Features
1. The data is stored in the cloud for the data analysis.
2. The realtime data can be viewed in the localhost server.
3. If the readings cross the optimal the a mail will be sent to the user for precaution.



