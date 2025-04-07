#include <Arduino.h>

int pin1 = 1;
int pin2 = 2;
int tipo = OUTPUT;
int estadoAlto = HIGH;
int estadoBajo = LOW;

void setup() {
pinMode(pin1, tipo);
pinMode(pin2, tipo);
}

void loop() {
digitalWrite(pin1, estadoAlto);
digitalWrite(pin2, estadoBajo);
delay(1000);
digitalWrite(pin1, estadoBajo);
digitalWrite(pin2, estadoAlto);
delay(1000);
}
