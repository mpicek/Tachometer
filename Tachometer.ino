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
JE MI ALE STALE ZAHADOU, PROC TO NA TIMERU CETLO NULA, KDYZ PROMENNA magneticSensorTickAllow BY TO NIKDY NEMELA DOVOLIT. MIN HODNOTA, KTEROU TO MOHLO PRECIST BYLA NEJAKYCH 80 (po upraveni 180 a pak treba 500)
*/

/*
TIMERY
0 - 8-bit - debouncing
1 - 16-bit - cteni casu mezi jednotlivymi otackami
2 - 8-bit - vypis na obrazovku
*/

#define DEBUG

#define DEBOUNCE_TIME 60
#define minSpeed 1200 //means max period for turning the wheel in ms
#define printLCDInterval 1000

#define RSTpin 7
#define CEpin 8
#define DCpin 6
#define DINpin 5
#define CLKpin 4

#define magSensorPin 3
#define buttonsPin 2
#define butt1 11
#define butt2 12



#include <EEPROM.h>
#include "Nokia3310.h"
#include "tacho.h"

volatile uint32_t numberOfRotations = 0; //counts turns of the wheel
volatile uint16_t t1 = 0; //time of 1st turn of wheel
volatile uint16_t t2 = 0; //turn of 2nd turn of wheel
volatile uint16_t t3 = 0;
volatile uint8_t underMinimalSpeed = 0; //indicates, that bike is under minimal speed
volatile uint32_t todayTime = 0; //whole time of the day (in timer pulses, so: 1sec = 11718.75 ~= 11719)

uint16_t wheelCircumference = 2000; //in mm ... 2136 ... obvod kola

uint32_t countSpeed(){ // returns 1000x bigger speed (but in km/h) .. but only if t1, t2 and t3 > 0
    if(t1 > 0 && t2 > 0 && t3 > 0){
        return (3*(uint32_t)wheelCircumference*3600)/((uint32_t)(t1+t2+t3));
    }
    else{
        return 0;
    }
}

void initializeValues(){
    numberOfRotations = 0; //counts turns of the wheel
    t1 = 0; //time of 1st turn of wheel
    t2 = 0; //turn of 2nd turn of wheel
    t3 = 0;
    underMinimalSpeed = 0; //indicates, that bike is under minimal speed
    todayTime = 0; //whole time of the day (in timer pulses, so: 1sec = 11718.75 ~= 11719)
}

void printOnLCD(){

    char stringed[11];
    uint32_t speed = countSpeed();

    LcdClear();

    GotoXY(0,0);
    intToString(stringed, speed/1000); //v per day
    LcdStringBig(stringed);
    GotoXY(35,2);
    LcdString(" Km/h");

    GotoXY(0,3);
    metersToKilometers(stringed, (numberOfRotations*wheelCircumference)/1000); //s per day
    LcdString(stringed);
    LcdString(" km");

    GotoXY(0, 4);
    if(todayTime > 0) intToString(stringed, (numberOfRotations*wheelCircumference*3600)/(todayTime*1000)); //s per day
    else intToString(stringed, 0); //s per day
    LcdString(stringed);
    LcdString(" km/h");

    GotoXY(0,5);
    secondsToTime(stringed, todayTime/1000);
    LcdString(stringed);
}

volatile bool b = 0;
volatile uint64_t lastSpin;
uint64_t lastPrintLCD;
volatile uint64_t lastButtonPress;

volatile int chyb;

void setup(){
    lastSpin = millis();
    lastButtonPress = millis();
    lastPrintLCD = millis();
    
    InitialiseTachometer();
    initializeValues();
        
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);

    pinMode(9, OUTPUT);
    digitalWrite(9, LOW);
    pinMode(10, OUTPUT);
    digitalWrite(10, LOW);
}

void loop(void){
    /*GotoXY(0,0);
    LcdString(" Km/h");
    int val = EEPROM.read(0);
    if(val == 3){
        digitalWrite(9, HIGH);
    }
    EEPROM.write(0, 3);*/
    /*GotoXY(0,0);
    char stringed[11];
    intToString(stringed, 7890); //v per day
    LcdStringBig(stringed);
    while(1){}*/
    
    while(1){
        if(millis() - lastSpin > minSpeed){
            digitalWrite(9, HIGH);
            digitalWrite(10, HIGH);
            underMinimalSpeed = 1;
            t1 = 0;
            t2 = 0;
            t3 = 0;
        }else{
            digitalWrite(9, LOW);
            digitalWrite(10, LOW);
        }
        
        if(millis() - lastPrintLCD > printLCDInterval){
            printOnLCD();
            lastPrintLCD = millis();
        }

        
    }
}
void magSensorISR(){
    if(millis() - lastSpin > DEBOUNCE_TIME){
        if(b){
            digitalWrite(13, LOW);
            b = 0;
        }
        else{
            digitalWrite(13, HIGH);
            b = 1;
        }
        
        if (underMinimalSpeed == 1){
            underMinimalSpeed = 0;
        }
        else{
            numberOfRotations++;
            t1 = t2;
            t2 = t3;
            t3 = millis() - lastSpin;

            todayTime += t3;
            #ifdef DEBUG
            if(t3 == 0 || t3 < 3){
                chyb++;
            }
            #endif
        }
        lastSpin = millis();
    }
}
void buttonsISR(){
    if(millis() - lastButtonPress > 50){
      lastButtonPress = millis();
        digitalWrite(butt1, HIGH);
        if(digitalRead(buttonsPin) == HIGH){
            digitalWrite(9, HIGH);
            digitalWrite(10, LOW);
        }
        else{
            digitalWrite(10, HIGH);
            digitalWrite(9, LOW);
        }
        digitalWrite(butt1, LOW);
    }
}
/*
ISR(INT0_vect){
    if(magneticSensorTickAllow == 1){
        nullTimer(0);
        magneticSensorTickAllow = 0;
        needsToBeOne = 0;

        if (underMinimalSpeed == 1){
            underMinimalSpeed = 0;
            nullTimer(1);
        }
        else{
            numberOfRotations++;
            t1 = t2;
            t2 = t3;
            t3 = readTimer(1);

            nullTimer(1);
            todayTimeInTimerPulses += t3;
            #ifdef DEBUG
            if(t3 == 0 || t3 < 3){
                chyb++;
            }
            #endif
        }
    }
    //todayTimeInTimerPulses += readTimer(1);
    //nullTimer(1);
}
ISR(INT1_vect){
    PORTB ^= 1<<PB1;
    //todayTimeInTimerPulses = 0;
}*/
