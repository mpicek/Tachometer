#ifndef tacho_h
#define tacho_h

#include "Nokia3310.h"

void setInterrupt(); //sets INT0 interrupt

void setupTimer(uint8_t number); //sets one of three timers (0 = 8bit, 1 = 16bit, 2 = 8bit)

uint16_t readTimer(uint8_t number); //reads timer value (0 = 8bit, 1 = 16bit, 2 = 8bit)

void nullTimer(uint8_t number); //nulls timer value (0 = 8bit, 1 = 16bit, 2 = 8bit)

void intToString(char* cil, uint32_t cislo); //converts int to string (function from string.h is too big)


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
