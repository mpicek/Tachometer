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

#define DEBOUNC_TIME 180
#define ticksPerSecond1024 11719
#define minSpeed 20000 //means max period for turning the wheel

#define RSTpin 7
#define CEpin 8
#define DCpin 6
#define DINpin 5
#define CLKpin 4

#define magSensor 3
#define buttonsPin 2

#include <EEPROM.h>
#include "Nokia3310.h"
#include "tacho.h"

volatile uint32_t numberOfRotations = 0; //counts turns of the wheel
volatile uint16_t t1 = 0; //time of 1st turn of wheel
volatile uint16_t t2 = 0; //turn of 2nd turn of wheel
volatile uint16_t t3 = 0;
volatile uint8_t underMinimalSpeed = 0; //indicates, that bike is under minimal speed
volatile uint32_t todayTimeInTimerPulses = 0; //whole time of the day (in timer pulses, so: 1sec = 11718.75 ~= 11719)
volatile uint8_t magneticSensorTickAllow = 1;

#ifdef DEBUG
volatile int prectenaNula = 0;
#endif

uint8_t tim2_ov = 0;


uint16_t wheelCircumference = 2068; //in mm ... 2136 ... obvod kola

uint8_t needsToBeOne = 0;

#ifdef DEBUG
volatile uint8_t chyb = 0;
#endif


uint32_t countSpeed(){
    if(t1 > 0 && t2 > 0 && t3 > 0){
        //vim proc mi to neukazuje ze zacatku rychlost (kdyz se rozjizdim), je to proto, ze tady je podminka,
        //ze vsechny tri casy jsou vetsi nez 0 -> upravit
        return (3*(uint32_t)wheelCircumference*3600)/((((uint32_t)(t1+t2+t3)*1000)/ticksPerSecond1024));
    }

    else{
        return 0;
    }
}

void printOnLcd(){

    char stringed[11];
    uint32_t speed = countSpeed();

    LcdClear();

    GotoXY(0,0);
    intToString(stringed, speed/1000); //v per day
    LcdString(stringed);
    LcdString(" Km/h");

    GotoXY(0,1);
    intToString(stringed, (numberOfRotations*wheelCircumference)/1000); //s per day
    LcdString(stringed);
    LcdString(" m");


    GotoXY(0,2);
    intToString(stringed, todayTimeInTimerPulses/ticksPerSecond1024); //t per day
    LcdString(stringed);
    LcdString(" s");

    #ifdef DEBUG

    GotoXY(0,5);
    intToString(stringed, chyb);
    LcdString("Chyb: ");
    LcdString(stringed);
    #endif

}

void InitialiseTachometer(){
    setPins(RSTpin, CEpin, DCpin, DINpin, CLKpin);
    InitialiseLcd();
    LcdClear();
}

volatile bool b = 0;
volatile uint64_t last = 0;
int butt1 = 11;
int butt2 = 12;

void setup(){
    InitialiseTachometer();
    pinMode(magSensor, INPUT);
    digitalWrite(magSensor, HIGH); 
    attachInterrupt(digitalPinToInterrupt(magSensor), magSensorISR, FALLING);
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
    last = millis();

    pinMode(buttonsPin, INPUT);
    digitalWrite(buttonsPin, HIGH); 
    attachInterrupt(digitalPinToInterrupt(buttonsPin), buttonsISR, FALLING);
    pinMode(butt1, OUTPUT);
    digitalWrite(butt1, LOW);
    pinMode(butt2, OUTPUT);
    digitalWrite(butt2, LOW);

    pinMode(9, OUTPUT);
    digitalWrite(9, LOW);
    pinMode(10, OUTPUT);
    digitalWrite(10, LOW);
}

void loop(void){
    GotoXY(0,0);
    LcdString(" Km/h");
    int val = EEPROM.read(0);
    if(val == 3){
        digitalWrite(9, HIGH);
    }
    EEPROM.write(0, 3);
    while(1){}
    /*
    while(1){
        if (readTimer(1) > minSpeed){ //we are under minimal speed
            underMinimalSpeed = 1;
            nullTimer(1);
            t1 = 0;
            t2 = 0;
            t3 = 0;
        }


        if (tim2_ov >= 90){ // 8-bit timeru - je to hodne rychly
            //TODO spocitat rychlost
            printOnLcd();
            tim2_ov = 0;
        }

        if (readTimer(2) > 240){ // 8-bit timeru - je to hodne rychly
            tim2_ov++;
            nullTimer(2);
        }



        if (readTimer(0) > DEBOUNC_TIME && magneticSensorTickAllow == 0 && needsToBeOne < 2){ //opet dve promenne, protoze 8-bit timer ma moc malou hodnotu na uchovani
            needsToBeOne++;
            nullTimer(0);
        }

        if (readTimer(0) > DEBOUNC_TIME && magneticSensorTickAllow == 0 && needsToBeOne == 2){
            magneticSensorTickAllow = 1;
        }
    }*/
}
void magSensorISR(){
    if(millis() - last > 50){
    last = millis();
    if(b){
      digitalWrite(13, LOW);
      b = 0;
    }
    else{
      digitalWrite(13, HIGH);
      b = 1;
    }
    }
}

void buttonsISR(){
    if(millis() - last > 50){
      last = millis();
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
