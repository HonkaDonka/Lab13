#include "oled-wing-adafruit.h"
#include "MQTT.h"

SYSTEM_THREAD(ENABLED);

// Declaring functions
void callback(char *topic, byte *payload, unsigned int length);
void requestData();
void resetDisplay();

// Declaring variables
Timer publish_timer(200, requestData, false); // Creates a timer that calls the function requestData every 200ms repeatedly
MQTT client("lab.thewcl.com", 1883, callback);
OledWingAdafruit display;

volatile bool shouldRequest = false;
bool islong = false;
float currentlat = 0;
float currentlon = 0;

void setup()
{
  display.setup();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.display();

  publish_timer.start(); // Starts the timer
}

void loop()
{
  if (client.isConnected())
  {
    client.loop();
    display.loop();

    if (shouldRequest) // If they should request data, then publish a message to the topic, which will be read by Node-Red
    {
      client.publish("VinsonLab13/Req", "GIMME DATA");
      shouldRequest = false;
    }

    resetDisplay();
    display.print("LATITUDE: ");
    display.println(currentlat);
    display.print("LONGITUDE: ");
    display.println(currentlon);
    display.display();
  }
  else
  {
    client.connect(System.deviceID());
    client.subscribe("VinsonLab13/Returns");
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String s = p;

  // Since MQTT sends the data in two parts, we can alternate between setting for latitude and longitude
  if (!islong)
  {
    currentlat = s.toFloat();
    islong = true;
  }
  else
  {
    currentlon = s.toFloat();
    islong = false;
  }
}

void resetDisplay()
{
  display.clearDisplay();
  display.setCursor(0, 0);
}

void requestData()
{
  shouldRequest = true;
}