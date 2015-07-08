#include <FastLED.h>

#define NUM_LEDS 12
#define DATA_PIN 6

#define E_PIN_A 9
#define E_PIN_B 10
#define SWITCH_PIN 11

#define NUM_MODES 4
#define NUM_SCHEMES 7
#define PER_SCHEME 5

#define OFF 0
#define WHITE 1
#define PICK 2
#define FADE 3

#define SENSOR_DELAY 5

enum direction {
	still,
	cw,
	ccw
};

CRGB leds[NUM_LEDS];
byte mode;

unsigned long lastTime;

boolean pinA, pinB, pSwitch;
boolean lastA, lastB, lastSwitch;

direction dir;

//Color picker
byte hue;

//Fade
byte scheme;
byte sub;
byte frac;
CRGB currentFade;
const unsigned long fadeDelay = 100;
unsigned long lastTimeFade;
const CRGB schemes[NUM_SCHEMES][PER_SCHEME] = {
	{CRGB::Red, CRGB::Orange, CRGB::Yellow, CRGB::DeepPink, CRGB::DarkViolet},
	{CRGB::Chartreuse, CRGB::Cyan, CRGB::DarkGreen, CRGB::MediumSpringGreen, CRGB::DodgerBlue},
	{CRGB::GreenYellow, CRGB::Gold, CRGB::Green, CRGB::Orange, CRGB::Red},
	{CRGB::Purple, CRGB::Red, CRGB::Teal, CRGB::Navy, CRGB::MediumVioletRed},
	{CRGB::Olive, CRGB::OliveDrab, CRGB::Orange, CRGB::OrangeRed, CRGB::Orchid},
	{CRGB::GreenYellow, CRGB::Yellow, CRGB::YellowGreen, CRGB::Chartreuse, CRGB::Gold},
	{CRGB::DarkOrange, CRGB::DarkOrchid, CRGB::Cyan, CRGB::Crimson, CRGB::Lime}
};

void setup() {
	FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

	pinMode(E_PIN_A, INPUT_PULLUP);
	pinMode(E_PIN_B, INPUT_PULLUP);
	pinMode(SWITCH_PIN, INPUT_PULLUP);

	lastA = pinA = (HIGH == digitalRead(E_PIN_A));
	lastB = pinB = (HIGH == digitalRead(E_PIN_B));
	lastSwitch = pSwitch = (HIGH == digitalRead(SWITCH_PIN));

	mode = OFF;
	dir = still;
	hue = 0;
	scheme = 0;
	sub = 0;
	frac = 0;

	lastTimeFade = lastTime = millis();
}

void loop() {
	pinA = (HIGH == digitalRead(E_PIN_A));
	pinB = (HIGH == digitalRead(E_PIN_B));
	pSwitch = (HIGH == digitalRead(SWITCH_PIN));

	if((unsigned long)(millis() - lastTime) >= SENSOR_DELAY) {
		if(pSwitch != lastSwitch) {
			if(pSwitch) {
				mode = (mode + 1) % NUM_MODES;

				if(mode == FADE) {
					lastTimeFade = millis();
					sub = frac = 0;
					currentFade = blend(schemes[scheme][sub], schemes[scheme][(sub + 1) % PER_SCHEME], frac);
				}
			}

			lastSwitch = pSwitch;
		}

		if(lastA && lastB) {
			if(!pinA && pinB) dir = cw;
			else if(pinA && !pinB) dir = ccw;
		}

		lastTime = millis();
		lastA = pinA;
		lastB = pinB;
	}

	if(dir != still) {
		switch(mode) {
			case PICK: //Color picker
				if(dir == cw) hue++; //hue will wrap around after 255
				else hue--;

				break;
			case FADE: //Color fade
				if(dir == cw) {
					if(scheme == NUM_SCHEMES - 1) scheme = 0;
					else scheme++;
				} else {
					if(scheme == 0) scheme = NUM_SCHEMES - 1;
					else scheme--;
				}

				sub = 0;
				frac = 0;

				lastTimeFade = millis();
				currentFade = blend(schemes[scheme][sub], schemes[scheme][(sub + 1) % PER_SCHEME], frac);

				break;
		}

		dir = still;
	}

	switch(mode) {
		case OFF: //Off
			fill_solid(leds, NUM_LEDS, CRGB::Black);

			break;
		case WHITE: //White
			fill_solid(leds, NUM_LEDS, CRGB::White);

			break;
		case PICK: //Color picker
			fill_solid(leds, NUM_LEDS, CHSV(hue, 255, 255));

			break;
		case FADE: //Color fade
			if((unsigned long)(millis() - lastTimeFade) >= fadeDelay) {
				if(frac == 255) {
					frac = 0;
					sub = (sub + 1) % PER_SCHEME;
				} else {
					frac++;
				}

				currentFade = blend(schemes[scheme][sub], schemes[scheme][(sub + 1) % PER_SCHEME], frac);

				lastTimeFade = millis();
			}

			fill_solid(leds, NUM_LEDS, currentFade);

			break;
	}

	FastLED.show();
}
