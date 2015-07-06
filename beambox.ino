#include <FastLED.h>

#define NUM_LEDS 12
#define DATA_PIN 6

#define E_PIN_A 9
#define E_PIN_B 10
#define SWITCH_PIN 11

#define SENSOR_DELAY 5 //TODO Still ok?

CRGB leds[NUM_LEDS];
byte mode;

unsigned long currentTime;
unsigned long lastTime;
unsigned long deltaT;

boolean pinA, pinB, pSwitch;
boolean lastA, lastB, lastSwitch;

//Color picker
byte hue;

//Fade //TODO

//Chase //TODO

void setup() {
	FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

	pinMode(E_PIN_A, INPUT_PULLUP);
	pinMode(E_PIN_B, INPUT_PULLUP);
	pinMode(SWITCH_PIN, INPUT_PULLUP);

	lastA = pinA = (HIGH == digitalRead(E_PIN_A));
	lastB = pinB = (HIGH == digitalRead(E_PIN_B));
	lastSwitch = pSwitch = (HIGH == digitalRead(SWITCH_PIN));

	mode = 0;
	hue = 0;

	lastTime = currentTime = millis();
	deltaT = 0;
}

void loop() {
	currentTime = millis();
	pinA = (HIGH == digitalRead(E_PIN_A));
	pinB = (HIGH == digitalRead(E_PIN_B));
	pSwitch = (HIGH == digitalRead(SWITCH_PIN));

	deltaT = (unsigned long)(currentTime - lastTime);
	if(deltaT >= SENSOR_DELAY) {
		if(pSwitch != lastSwitch) {
			if(pSwitch) {
				mode++;
				if(mode > 4) mode = 0;
			}

			lastSwitch = pSwitch;
		}

		if(pinA != lastA) {
			//TODO Encoder stuff

			lastA = pinA;
			lastB = pinB;
		} else if(pinB != lastB) {
			//TODO Encoder stuff

			lastA = pinA;
			lastB = pinB;
		}

		lastTime = currentTime;
	}

	switch(mode) {
		case 0: //Off
			fill_solid(leds, NUM_LEDS, CRGB::Black);

			break;
		case 1: //White
			fill_solid(leds, NUM_LEDS, CRGB::White);

			break;
		case 2: //Color picker
			fill_solid(leds, NUM_LEDS, CHSV(hue, 255, 255));

			break;
		case 3: //Color fade
			fill_solid(leds, NUM_LEDS, CRGB::Red); //TODO

			break;
		case 4: //Rainbow chase
			fill_rainbow(leds, NUM_LEDS, 0); //TODO

			break;
	}

	FastLED.show();
}