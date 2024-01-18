#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h> // Include the EEPROM library

// Define the LED and button pins
int buttonPin = D1;

// Variables to track button state and click count
int buttonState = LOW;
int lastButtonState = LOW;
int clickCount = 0;

// Create an instance of the web server
ESP8266WebServer server(80);

//------------------------------------------------------------Handler for serving the HTML page
void handleRoot()
{

    String page = "<!DOCTYPE html><html><head><title>Click Counter</title></head><body>";
    page += "<h1>Count: <span id=\"clickCount\">" + String(clickCount) + "</span></h1>";

    // --------------------JavaScript code embedded in the HTML response
    page += "<script>";
    page += "function updateClickCount(count) {";
    page += "  document.getElementById('clickCount').innerText = count;";
    page += "}";
    page += "function getClickCount() {";
    page += "  fetch('/counter')";
    page += "    .then(response => response.text())";
    page += "    .then(data => updateClickCount(data));";
    page += "}";
    page += "setInterval(getClickCount, 1000);";
    page += "</script>";

    page += "</body></html>";
    server.send(200, "text/html", page);
}

// ---------------------------------------------------------------Handler for getting the click count value
void handleGetClickCount()
{
    server.send(200, "text/plain", String(clickCount));
}

void setup()
{
    // -------------------------------------------------------------------Set button pin as input
    pinMode(buttonPin, INPUT);

    //----------------------------------------------------------------- Initialize the serial communication
    Serial.begin(57600);

    //------------------------------------------------------------------Create a WiFi access point
    WiFi.softAP("ESP8266", "12345678");
    // -----------------------------------------------------------------Print the My IP address
    Serial.print("Web server IP address: ");
    Serial.println(WiFi.softAPIP());

    // -----------------------------------------------------------------Set up the web server routes
    server.on("/", handleRoot);
    server.on("/counter", handleGetClickCount);

    // -----------------------------------------------------------------Start the web server
    server.begin();

    // -----------------------------------------------------------------Read the click count from EEPROM
    EEPROM.begin(512);           // Initialize the EEPROM library
    clickCount = EEPROM.read(0); // Read the click count from EEPROM adress 0
}

void loop()
{
    // ------------------------------------------------------------------Read the button state
    buttonState = digitalRead(buttonPin);
    // -----------------------------------------------------------------Check for button release
    if (buttonState == LOW && lastButtonState == HIGH)
    {
        // If the button is pressed, increment the click count
        clickCount++;

        // Update the click count in the EEPROM
        EEPROM.write(0, clickCount);
        EEPROM.commit(); // Save the changes to the EEPROM
    }
    // -----------------------------------------------------------------Update the last button state
    lastButtonState = buttonState;

    // ------------------------------------------------------------------Handle incoming client requests
    server.handleClient();
}
