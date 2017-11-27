/*
    Author: Martin Picek
    Last edit: 15.8. 2016
*/

/*MOZNOSTI CHYB:
debouncing doba je moc kratka
podminka, ze vsechny rychlosti musi byt vetsi nez 0 (obecne divny, upravit)
*/
/*
Zmeny: je to debouncingem. Potrebuje to delsi interval, debouncing je tottiz velice dlouhy. TODO:jeste zvetsit cislo udavajici velikost intervalu.
Proto DEBOUNC_TIME by melo byt cca 520, ale timer ma velikost jenom 8 bitu, proto jsem zavedl prom. needsToBeOne, ktera indikuje, ze uz to jednou bylo
vic nez 240, vynuluje se timer a needsToBeOne je jedna. Pak kdyz je timer opet 240, tak povoli promennou pro sepnuti senzoru.
*/
/*
Proc to predtim fungovalo, ale zaroven to ukazovalo o dost vetsi cas? Je to proto, ze kdyz jsem tam mel:
if (t3 < (t1+t2)/5){
    t3 = (t1+t2)/2;
}
Tak ono to precetlo t3 nulu, ale zvedlo se to treba na 4000, no a pak jsem udelal:
todayTimeInTimerPulses += t3;
a hele, todayTimeInTimerPulses je zase o pul sekundy vetsi, nez by realne mel byt.
JE MI ALE STALE ZAHADOU, PROC TO NA TIMERU CETLO NULA, KDYZ PROMENNA magneticSensorTickAllowed BY TO NIKDY NEMELA DOVOLIT. MIN HODNOTA, KTEROU TO MOHLO PRECIST BYLA NEJAKYCH 80 (po upraveni 180 a pak treba 500)
*/

/*
TIMERY
0 - 8-bit - debouncing
1 - 16-bit - cteni casu mezi jednotlivymi otackami
2 - 8-bit - vypis na obrazovku
*/

#define DEBUG
#define F_CPU 12000000L

#define DEBOUNC_TIME 50
#define BETWEEN_UPDATES_LCD_TIME 400
#define MAXIMAL_TIME_FROM_LAST_ROTATION_TO_BE_UNDER_MINIMAL_SPEED 1200 //aprox. 6km/h on bike with circumference of 2000mm
#define ticksPerSecond1024 11719
#define minSpeed 20000 //means max period for turning the wheel

#define PORTY_D
#define DDRREGISTR DDRD
#define PORTREGISTR PORTD

#define RSTpin PD6
#define CEpin PD7
#define DCpin PD5
#define DINpin PD4
#define CLKpin PD1
#define BADISR_vect

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include "Nokia3310.h"
#include "tacho.h"


volatile uint32_t numberOfRotations = 0; //counts turns of the wheel
volatile uint16_t t1 = 0; //time of 1st turn of wheel
volatile uint16_t t2 = 0; //turn of 2nd turn of wheel
volatile uint16_t t3 = 0;
volatile uint8_t underMinimalSpeed = 1; //indicates, that bike is under minimal speed
volatile uint32_t todayTimeInTimerPulses = 0; //whole time of the day (in timer pulses, so: 1sec = 11718.75 ~= 11719)
volatile uint64_t todayTimeInMilliSeconds = 0;
volatile uint8_t magneticSensorTickAllowed = 1;
volatile uint64_t lastRotation = 0;
uint64_t lastLcdUpdate = 0;

#ifdef DEBUG
volatile int prectenaNula = 0;
#endif

uint8_t tim2_ov = 0;


uint16_t wheelCircumference = 2068; //in mm ... 2136 ... obvod kola

uint8_t needsToBeOne = 0;

#ifdef DEBUG
volatile uint8_t chyb = 0;
#endif

uint16_t a = 0;

/*uint32_t mCountSpeed() //m because the speed is 1000x smaller (we return 1000x bigger number than the real number)
{
    if(t1 > 0 && t2 > 0 && t3 > 0){
        //vim proc mi to neukazuje ze zacatku rychlost (kdyz se rozjizdim), je to proto, ze tady je podminka,
        //ze vsechny tri casy jsou vetsi nez 0 -> upravit
        //return (3*(uint32_t)wheelCircumference*3600)/((((uint32_t)(t1+t2+t3)*1000)/ticksPerSecond1024));
        return t1+t2+t3;
        return (3*(uint32_t)wheelCircumference*3600)/((uint32_t)t1+(uint32_t)t2+(uint32_t)t3);
    }
    else{
        return 0;
    }
}*/

void printOnLcd()
{
    char stringed[11];
    //uint32_t speed = mCountSpeed();

    LcdClear();

    /*GotoXY(0,0);
    intToString(stringed, speed); //v per day
    LcdString(stringed);
    LcdString(" Km/h");*/
    

    GotoXY(0,1);
    /*intToString(stringed, (numberOfRotations*wheelCircumference)/1000); //s per day
    LcdString(stringed);
    LcdString(" m");*/
    intToString(stringed, t1);
    LcdString(stringed);


    GotoXY(0,2);
    /*intToString(stringed, todayTimeInTimerPulses/ticksPerSecond1024); //t per day
    LcdString(stringed);
    LcdString(" s");*/
    intToString(stringed, t2);
    LcdString(stringed);

    GotoXY(0,3);
    intToString(stringed, t3);
    LcdString(stringed);
    /*LcdString(stringed);
    LcdString("AVG: ");
    if(todayTimeInTimerPulses == 0){
        intToString(cas, 0);
    }
    else{
        intToString(cas, ((numberOfRotations*2136)/1000)/(todayTimeInTimerPulses/ticksPerSecond1024)*3.6);
    }
    LcdString(cas);
    LcdString(" km/h");*/

    #ifdef DEBUG

    GotoXY(0,5);
    intToString(stringed, chyb);
    LcdString("Chyb: ");
    LcdString(stringed);
    #endif
}

void InitialiseTachometer()
{
    resetMillis();
    setupTimer(0); //for debouncing
    setupTimer(1); //for counting time periods of wheel turn and then velocity counting
    setupTimer(2); //for printint on LCD (cca every second)
    setInterrupt(); //setup INT0 interrupt
    sei();
    setPins(RSTpin, CEpin, DCpin, DINpin, CLKpin);
    InitialiseLcd();
    LcdClear();
}



int main(void)
{
    DDRB |= 1 << PB1;
    DDRB |= 1 << PB2;
    InitialiseTachometer();
    PORTB |= 1 << PB1;
    PORTB ^= 1<<PB1;

    //printOnLcd();
    
    //PORTB |= 1 << PB1;
    

    //uint64_t lastMillis = millis();
    PORTB |= 1 << PB2;
        a = millis();
        while(millis() < a+1000);
        PORTB ^= 1<<PB2;

	while(1){
        //if(lastMillis = millis()) PORTB ^= 1<<PB1;

        if(millis() > lastRotation + MAXIMAL_TIME_FROM_LAST_ROTATION_TO_BE_UNDER_MINIMAL_SPEED){ //min speed
            underMinimalSpeed = 1;
            t1 = 0;
            t2 = 0;
            t3 = 0;
        }


        if(lastLcdUpdate + BETWEEN_UPDATES_LCD_TIME < millis()){ //print on lcd
            lastLcdUpdate = millis();
            
            printOnLcd();
        }

        /*if(lastRotation + DEBOUNC_TIME < millis()){ //sensor debouncing
            magneticSensorTickAllowed = 1;
        }*/
        /*if(lastRotation + 1000 < millis()){
            lastRotation = millis();
            PORTB ^= 1<<PB1;
        }*/
/*
        if(readTimer(1) > 1880){
            nullTimer(1);
        }
        */
        if(readTimer(0) > 50){
            if(readTimer(1) > 1880){
                nullTimer(1);
            }
        }

    }
    return 0;
}

ISR(INT0_vect)
{
    cli();
    if(millis() - lastRotation > DEBOUNC_TIME){
        //PORTB ^= 1<<PB1;

        if (underMinimalSpeed == 1){
            underMinimalSpeed = 0;
            lastRotation = millis();
        }

        else{
            numberOfRotations++;
            t1 = t2;
            t2 = t3;
            t3 = millis() - lastRotation;
            lastRotation = millis();

            numberOfRotations++;

            todayTimeInMilliSeconds += t3;
        }

    }
    sei();
}

/*
ISR(INT1_vect)
{

}*/

ISR (TIMER1_COMPA_vect)
{
    cli();
    incrementMillis(1); //10 
    nullTimer(1);
    sei();
    /*a++; 
    if(a > 30){
        a = 0;
        PORTB ^= 1<<PB1;
    }*/
}

ISR(BADISR_vect)
{
    PORTB ^= 1<<PB1;
}