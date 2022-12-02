#include <Arduino.h>
#include "FastLED.h"
#include "OTA.h"
#include "credentials.h"

#define LED_DATA_PIN 4
#define NUM_LEDS 350
// #define NUM_LEDS 150
#define ONBOARD_LED_PIN 2

CRGB leds[NUM_LEDS];
int ledState = LOW;

void setup()
{
  Serial.begin(115200);
  Serial.println("Happy solstice!");

  setupOTA("solstice-lights", WIFI_SSID, WIFI_PASSWORD);

  pinMode(ONBOARD_LED_PIN, OUTPUT);

  FastLED.addLeds<WS2811, LED_DATA_PIN, RGB>(leds, NUM_LEDS);
  // set brightness to half
  // FastLED.setBrightness(64);
}

void handleBlink()
{
  static unsigned long lastBlink = 0;
  static bool ledState = LOW;

  if (millis() - lastBlink > 100)
  {
    lastBlink = millis();
    ledState = !ledState;
    digitalWrite(ONBOARD_LED_PIN, ledState);
  }
}

void handleColor()
{
  static unsigned long lastColor = 0;
  static int color = 0;

  if (millis() - lastColor > 5000)
  {
    lastColor = millis();
    color = (color + 1) % 4;
    switch (color)
    {
    case 0:
      fill_solid(leds, NUM_LEDS, CRGB::White);
      break;
    case 1:
      fill_solid(leds, NUM_LEDS, CRGB::Red);
      break;
    case 2:
      fill_solid(leds, NUM_LEDS, CRGB::Green);
      break;
    case 3:
      fill_solid(leds, NUM_LEDS, CRGB::Blue);
      break;
    }
    FastLED.show();
  }
}

// count up and then back down
int countUPAndDown(int min, int max, int step)
{
  static int count = min;
  static bool up = true;

  if (up)
  {
    count += step;
    if (count >= max)
    {
      count = max;
      up = false;
    }
  }
  else
  {
    count -= step;
    if (count <= min)
    {
      count = min;
      up = true;
    }
  }
  return count;
}

// A function that moves a rainbow from one end of the strip to the other
void rainbow()
{
  static unsigned long lastRainbow = 0;
  static int hue = 0;

  if (millis() - lastRainbow > 20)
  {
    uint8_t deltaHue = countUPAndDown(0, 10, 1);
    lastRainbow = millis();
    hue = (hue + 1) % 256;
    fill_rainbow(leds, NUM_LEDS, hue, 1);
    FastLED.show();
  }
}

// A function that continually moves white pixels along the strip
void white()
{
  static unsigned long lastWhite = 0;
  static int whiteIndex = 0;

  if (millis() - lastWhite > 40)
  {
    lastWhite = millis();
    whiteIndex = (whiteIndex + 1) % NUM_LEDS;
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    leds[whiteIndex] = CRGB::White;
    for (int i = 0; i < 10; i++)
    {
      static int tailIndex = (whiteIndex - i);
      leds[tailIndex] = CRGB::White / (i + 1);
    }
    FastLED.show();
  }
}

// color fade
void fadeAllColors()
{
  static unsigned long lastFade = 0;
  static int fade = 0;

  if (millis() - lastFade > 400)
  {
    lastFade = millis();
    fade = (fade + 1) % 256;
    fill_solid(leds, NUM_LEDS, CHSV(fade, 255, 255));
    FastLED.show();
  }
}

void spaceySquares()
{
  static unsigned long nextSpacey = 0;
  static int spacey = 0;

  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i].fadeToBlackBy(1);
  }
  FastLED.show();

  if (millis() > nextSpacey)
  {
    nextSpacey = millis() + 1000;

    int numPixels = random(0, NUM_LEDS);
    int offset = random(0, NUM_LEDS);
    fill_solid(leds + offset, numPixels, CRGB::Gold);

    FastLED.show();
  }
}

// marching ants
void ants()
{
  static unsigned long nextAnts = 0;
  static int antIndex = 0;

  if (millis() > nextAnts)
  {
    nextAnts = millis() + 50;

    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i].fadeToBlackBy(1);
    }

    for (int i = 0; i < 10; i++)
    {
      leds[(antIndex + i) % NUM_LEDS] = CRGB::White;
    }

    antIndex = (antIndex + 1) % NUM_LEDS;

    FastLED.show();
  }
}

// Move all pixels over one and add a new one at the end
void movePixels()
{
  static unsigned long nextMove = 0;
  static int moveIndex = 0;

  if (millis() > nextMove)
  {
    nextMove = millis() + 50;

    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = leds[(i + 1) % NUM_LEDS];
    }

    leds[moveIndex] = CRGB::White;
    moveIndex = (moveIndex + 1) % NUM_LEDS;

    FastLED.show();
  }
}

void redGreenSlide()
{
  static unsigned long moveTime = 0;
  static int moveIndex = 0;
  static int bandCounter = 0;
  int bandWidth = 20;

  if (millis() > moveTime)
  {
    moveTime = millis() + 100;

    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = leds[(i + 1) % NUM_LEDS];
    }

    bandCounter = (bandCounter + 1) % bandWidth;

    if (bandCounter < (bandWidth / 2))
    {
      leds[NUM_LEDS - 1] = CRGB::Red;
    }
    else
    {
      leds[NUM_LEDS - 1] = CRGB::Green;
    }

    FastLED.show();
  }
}

void handleAnimations()
{
  static unsigned long lastAnimation = 0;
  static int animation = 0;

  if (millis() - lastAnimation > 60000)
  {
    lastAnimation = millis();
    animation = (animation + 1) % 5;
  }

  switch (animation)
  {
  case 0:
    redGreenSlide();
    break;
  case 1:
    rainbow();
    break;
  case 2:
    ants();
    break;
  case 3:
    fadeAllColors();
    break;
  case 4:
    spaceySquares();
    break;
  }
}

void loop()
{
  ArduinoOTA.handle();
  handleAnimations();
  // rainbow();
}
