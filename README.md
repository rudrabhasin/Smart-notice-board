# Smart-notice-board

# Smart Notice Board using ESP8266

## Overview
This project is a **Smart Notice Board** controlled via a web interface, built using **ESP8266 WiFi module** and **MAX7219 LED Matrix Display**. Users can send messages remotely using a web page, and the messages are displayed on the LED matrix with customizable scrolling effects, speed, and text inversion.

## Features
- **WiFi Connectivity**: Allows remote control of the notice board through a web interface.
- **LED Matrix Display**: Uses **MAX7219** to display text messages.
- **Customizable Scrolling Effects**: Supports left and right scrolling.
- **Adjustable Speed**: Users can set the speed of text movement.
- **Invert Display**: Option to invert the text color.
- **End Task Feature**: Clears the display when needed.

## Components Required
- **ESP8266 NodeMCU**
- **MAX7219 LED Matrix Display (8 modules)**
- **Power Supply (5V)**
- **Jumper Wires**

## Circuit Connections
| ESP8266 Pin | MAX7219 Pin |
|-------------|------------=|
| D8 (GPIO15) | CS          |
| D7 (GPIO13) | DIN         |
| D5 (GPIO14) | CLK         |
| 3.3V        | VCC         |
| GND         | GND         |

## Installation & Setup
### 1. Install Required Libraries
Ensure you have the following libraries installed in the **Arduino IDE**:
- `ESP8266WiFi.h`
- `MD_Parola.h`
- `MD_MAX72XX.h`
- `SPI.h`

### 2. Upload the Code
- Open the **Arduino IDE**
- Select **ESP8266 NodeMCU** as the board
- Configure the **WiFi SSID and Password** in the code
- Upload the sketch to the ESP8266

### 3. Connect to WiFi
- Once uploaded, open the **Serial Monitor** at **115200 baud rate**.
- The IP address of the ESP8266 will be displayed.
- Open a web browser and enter the IP address to access the control panel.

## Web Interface
The web interface allows users to:
- Enter a **custom message** to be displayed.
- Select **scrolling direction** (left or right).
- Toggle **invert mode** (normal or inverse text).
- Adjust **scrolling speed**.
- Enable **end task** to clear the display.

## How It Works
1. The ESP8266 creates a **web server** on port **80**.
2. The web page provides a **form** for users to input messages and settings.
3. The ESP8266 processes the request and updates the **LED Matrix Display** accordingly.
4. Messages are displayed with the selected effects.

## Code Breakdown
### 1. WiFi Setup
```cpp
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) delay(500);
server.begin();
```

### 2. Handling Web Requests
```cpp
WiFiClient client = server.available();
if (client) {
    String req = client.readStringUntil('\r');
    if (req.indexOf("MSG=") != -1) {
        int msgStart = req.indexOf("MSG=") + 4;
        String message = req.substring(msgStart, req.indexOf("&", msgStart));
        message.toCharArray(newMessage, BUF_SIZE);
        newMessageAvailable = true;
    }
}
```

### 3. Displaying Message on LED Matrix
```cpp
if (newMessageAvailable) {
    strcpy(curMessage, newMessage);
    newMessageAvailable = false;
    P.displayClear();
    P.displayScroll(curMessage, PA_LEFT, scrollEffect, frameDelay);
}
```

## Future Improvements
- Add support for **multiple fonts and text styles**.
- Implement **MQTT support** for IoT integration.
- Include **mobile app control** using a dedicated app.

## License
This project is licensed under the **MIT License**.

## Author
**Rudra Bhasin**

## Contribution
Feel free to **fork** the repository and submit **pull requests** for improvements!

