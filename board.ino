#include <ESP8266WiFi.h>
#include <MD_Parola.h>
#include <MD_MAX72XX.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 8
#define CS_PIN 15

MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

const char* ssid = "ENTER YOUR SSID";       //SSID
const char* password = "ENTER YOUR PASSWORD";      //PASSWORD

WiFiServer server(80);

uint8_t frameDelay = 25;
textEffect_t scrollEffect = PA_SCROLL_LEFT;

#define BUF_SIZE 512
char curMessage[BUF_SIZE] = "";
char newMessage[BUF_SIZE] = "";
bool newMessageAvailable = false;
bool endTask = false;

const char WebResponse[] = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
const char WebPage[] =
"<!DOCTYPE html>"
"<html>"
"<head>"
"<title>rb</title>"
"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
"<style>"
"  body { font-family: Arial, sans-serif; display: flex; justify-content: center; align-items: center; min-height: 100vh; margin: 0; background-color: #f9f9f9; }"
"  #container { max-width: 500px; width: 90%; padding: 20px; background-color: #ffffff; box-shadow: 0px 4px 8px rgba(0,0,0,0.1); border-radius: 8px; }"
"  h2 { color: #333; }"
"  input[type='text'], input[type='range'] { width: 100%; padding: 10px; margin: 10px 0; border: 1px solid #ccc; border-radius: 4px; }"
"  input[type='submit'], input[type='button'] { padding: 10px 20px; font-size: 16px; background-color: #4CAF50; color: white; border: none; border-radius: 4px; cursor: pointer; width: 100%; margin-top: 10px; }"
"  input[type='submit']:hover, input[type='button']:hover { background-color: #45a049; }"
"  label { display: block; margin-top: 10px; font-size: 16px; }"
"</style>"
"<script>"
"  function SendData() {"
"    let nocache = '&nocache=' + Math.random() * 1000000;"
"    let request = new XMLHttpRequest();"
"    let message = encodeURIComponent(document.getElementById('message').value);"
"    let scrollType = document.querySelector('input[name=\"ScrollType\"]:checked').value;"
"    let invert = document.querySelector('input[name=\"Invert\"]:checked').value;"
"    let speed = document.getElementById('speed').value;"
"    let endTask = document.getElementById('endTask').checked ? '1' : '0';"
"    let url = `/&MSG=${message}&SD=${scrollType}&I=${invert}&SP=${speed}&ET=${endTask}${nocache}`;"
"    request.open('GET', url, true);"
"    request.send();"
"  }"
"</script>"
"</head>"
"<body>"
"<div id='container'>"
"  <h2>Smart Notice Board</h2>"
"  <form id='data_form'>"
"    <label>Message:<br><input type='text' id='message' maxlength='255'></label>"
"    <label><input type='radio' name='Invert' value='0' checked> Normal</label>"
"    <label><input type='radio' name='Invert' value='1'> Inverse</label>"
"    <label><input type='radio' name='ScrollType' value='L' checked> Left Scroll</label>"
"    <label><input type='radio' name='ScrollType' value='R'> Right Scroll</label>"
"    <label>Speed:<br>Fast<input type='range' id='speed' name='Speed' min='10' max='200'>Slow</label>"
"    <label><input type='checkbox' id='endTask'> End Task</label>"
"    <input type='button' value='Send Data' onclick='SendData()'>"
"  </form>"
"</div>"
"</body>"
"</html>";

void handleWiFi() {
  WiFiClient client = server.available();
  if (client) {
    String req = client.readStringUntil('\r');
    client.flush();

    // Process request
    if (req.indexOf("GET /") != -1) {
      if (req.indexOf("MSG=") != -1) {
        int msgStart = req.indexOf("MSG=") + 4;
        int msgEnd = req.indexOf("&", msgStart);
        String message = req.substring(msgStart, msgEnd);
        message.replace("%20", " ");
        message.toCharArray(newMessage, BUF_SIZE);
        newMessageAvailable = true;
      }

      if (req.indexOf("SD=L") != -1) {
        scrollEffect = PA_SCROLL_LEFT;
      } else if (req.indexOf("SD=R") != -1) {
        scrollEffect = PA_SCROLL_RIGHT;
      }

      if (req.indexOf("I=1") != -1) {
        P.setInvert(true);
      } else {
        P.setInvert(false);
      }

      if (req.indexOf("SP=") != -1) {
        int speedStart = req.indexOf("SP=") + 3;
        int speedEnd = req.indexOf("&", speedStart);
        String speedStr = req.substring(speedStart, speedEnd);
        frameDelay = speedStr.toInt();
        P.setSpeed(frameDelay);
      }

      if (req.indexOf("ET=1") != -1) {
        endTask = true;
        P.displayClear();
      } else {
        endTask = false;
      }

      // Send web page response
      client.print(WebResponse);
      client.print(WebPage);
    }
    client.stop();
  }
}

void setup() {
  Serial.begin(115200);
  P.begin();
  P.displayClear();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  server.begin();
  sprintf(curMessage, "IP: %s", WiFi.localIP().toString().c_str());
  P.displayText(curMessage, PA_CENTER, frameDelay, 0, PA_SCROLL_LEFT);
}

void loop() {
  handleWiFi();

  if (endTask) {
    P.displaySuspend(true);
  } else if (newMessageAvailable) {
    strcpy(curMessage, newMessage);
    newMessageAvailable = false;
    P.displaySuspend(false);
    P.displayClear();
    P.displayScroll(curMessage, PA_LEFT, scrollEffect, frameDelay);
  }

  if (P.displayAnimate()) P.displayReset();
}
