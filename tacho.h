#ifndef tacho_h
#define tacho_h

#include "Nokia3310.h"

void intToString(char* cil, uint32_t cislo); //converts int to string (function from string.h is too big

void setSensors(int msPn, int buttPin, int butt1pin, int butt2pin);

extern void magSensorISR();

extern void buttonsISR();

void InitialiseTachometer();

void InitialiseTachometer(){
    setPins(RSTpin, CEpin, DCpin, DINpin, CLKpin);
    InitialiseLcd();
    LcdClear();
    setSensors(magSensorPin, buttonsPin, butt1, butt2);
}

void setSensors(int msPn, int buttPin, int butt1pin, int butt2pin){
    pinMode(msPn, INPUT);
    digitalWrite(msPn, HIGH); 
    attachInterrupt(digitalPinToInterrupt(msPn), magSensorISR, FALLING);

    pinMode(buttPin, INPUT);
    digitalWrite(buttPin, HIGH); 
    attachInterrupt(digitalPinToInterrupt(buttPin), buttonsISR, FALLING);
    
    pinMode(butt1pin, OUTPUT);
    digitalWrite(butt1pin, LOW);
    pinMode(butt2pin, OUTPUT);
    digitalWrite(butt2pin, LOW);
}

void intToString(char* cil, uint32_t cislo){
	char pole[11];
	uint8_t i = 0;
	if(cislo == 0){ //abychom dokazali zapsat nulu
		cil[0] = 0 + '0';
		cil[1] = '\0';
		return;
	}

	while (cislo != 0){
		pole[i] = cislo % 10;
		cislo -= pole[i];
		cislo /= 10;
		pole[i] = pole[i] + '0';
		i++;
	}
	uint8_t in = 0;
	while (i!=0){
		cil[in] = pole[i - 1];
		in++;
		i--;
	}
	cil[in] = '\0';
}

#endif
