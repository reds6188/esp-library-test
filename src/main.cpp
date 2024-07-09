#include "main.h"

Timer TimerBlink1, TimerBlink2;

void setup() {
	Serial.begin(115200);
	TimerBlink1.trigger();
	pinMode(LED_1, OUTPUT);
	pinMode(LED_2, OUTPUT);
	pinMode(LED_3, OUTPUT);
}

void loop() {
	if(TimerBlink1.elapsedX1s()) {
		TimerBlink1.trigger();
		digitalWrite(LED_1, !digitalRead(LED_1));
	}

	if(TimerBlink2.elapsedX100ms(2)) {
		TimerBlink2.trigger();
		digitalWrite(LED_2, !digitalRead(LED_2));
	}
}