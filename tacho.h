#ifndef tacho_h
#define tacho_h

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "Nokia3310.h"

void setInterrupt(); //sets INT0 interrupt

void setupTimer(uint8_t number); //sets one of three timers (0 = 8bit, 1 = 16bit, 2 = 8bit)

uint16_t readTimer(uint8_t number); //reads timer value (0 = 8bit, 1 = 16bit, 2 = 8bit)

void nullTimer(uint8_t number); //nulls timer value (0 = 8bit, 1 = 16bit, 2 = 8bit)

void intToString(char* cil, uint32_t cislo); //converts int to string (function from string.h is too big)



void setInterrupt(){
	DDRD &= ~(1<<PD2); //set pin as input for interrupt (INT0)
	PORTD |= 1<<PD2; //activate pull-up resistor
	
	GICR |= (1<<INT0); //enable int0 external interrupt
	MCUCR |= (1<<ISC01); //falling edge

	DDRD &= ~(1<<PD3); //set pin as input for interrupt (INT1)
	PORTD |= 1<<PD3; //activate pull-up resistor
	
	GICR |= (1<<INT1); //enable int0 external interrupt
	MCUCR |= (1<<ISC11); //falling edge

	sei();
}

void setupTimer(uint8_t number){
	if (number == 0){
		TCCR0 |= 1<<CS02 | 1<<CS00; //prescaling 1024 ... 8bit
	}
	else if(number == 1){
		TCCR1B |= 1<<CS12 | 1<<CS10; //prescaling 1024 ... 16bit
	}
	else if(number == 2){
		TCCR2 |= 1<<CS22 | 1<<CS20; //prescaling 1024 ... 16bit
	}
}

uint16_t readTimer(uint8_t number){
	if (number == 0){
		return (uint16_t)TCNT0;
	}
	else if(number == 1){
		return TCNT1;
	}
	else if(number == 2){
		return (uint16_t)TCNT2;
	}
}

void nullTimer(uint8_t number){
	if (number == 0){
		TCNT0 = 0;
	}
	else if(number == 1){
		TCNT1 = 0;
	}
	else if(number == 2){
		TCNT2 = 0;
	}

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