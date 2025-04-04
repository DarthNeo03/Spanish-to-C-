#include <Arduino.h>

int pin = 13;
int tipo = OUTPUT;

void setup() {
pinMode(pin, tipo);
}

void loop() {
digitalWrite(pin, ALTO);
delay(1000);
digitalWrite(pin, BAJO);
delay(1000);
}
