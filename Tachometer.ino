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
volatile int numberOfPressesRight = 0;
volatile int numberOfPressesLeft = 0;
volatile int firstLeftPress = 0;
volatile int firstRightPress = 0;
volatile int normalMode = 1;
volatile uint8_t changeWheel[4];
volatile uint8_t positionSettings = 3;

uint16_t wheelCircumference = 2000; //in mm ... 2136 ... obvod kola

uint32_t countSpeed(){ // returns 1000x bigger speed (but in km/h) .. but only if t1, t2 and t3 > 0
    if(t1 > 0 && t2 > 0 && t3 > 0){
        return (3*(uint32_t)wheelCircumference*3600)/((uint32_t)(t1+t2+t3));
    }
    else{
        return 0;
    }
}
void setWheelCircumferenceVariable(){
    wheelCircumference = 0;
    wheelCircumference += 1000*EEPROM.read(0);
    wheelCircumference += 100*EEPROM.read(1);
    wheelCircumference += 10*EEPROM.read(2);
    wheelCircumference += EEPROM.read(3);
}
void initializeValues(){
    numberOfRotations = 0; //counts turns of the wheel
    t1 = 0; //time of 1st turn of wheel
    t2 = 0; //turn of 2nd turn of wheel
    t3 = 0;
    underMinimalSpeed = 0; //indicates, that bike is under minimal speed
    todayTime = 0; //whole time of the day (in timer pulses, so: 1sec = 11718.75 ~= 11719)
    numberOfPressesRight = 0;
    numberOfPressesLeft = 0;
    setWheelCircumferenceVariable();
    changeWheel[0] = EEPROM.read(0);
    changeWheel[1] = EEPROM.read(1);
    changeWheel[2] = EEPROM.read(2);
    changeWheel[3] = EEPROM.read(3);
    positionSettings = 3;
}

void printOnLCD(){
    char stringed[11];
    if(normalMode){
    
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
    
    else{ //show changing wheel circumference
        LcdClear();
        GotoXY(0,0);
        LcdString("hello");
        GotoXY(25, 4);
        stringed[0] = '0'+ changeWheel[0];
        stringed[1] = '0'+ changeWheel[1];
        stringed[2] = '0'+ changeWheel[2];
        stringed[3] = '0'+ changeWheel[3];
        stringed[4] = '\0';
        LcdString(stringed);

        GotoXY(25, 5);
        if(positionSettings == 3){
            stringed[0] = ' ';
            stringed[1] = ' ';
            stringed[2] = ' ';
            stringed[3] = '"';
            stringed[4] = '\0';
        }
        if(positionSettings == 2){
            stringed[0] = ' ';
            stringed[1] = ' ';
            stringed[2] = '"';
            stringed[3] = ' ';
            stringed[4] = '\0';
        }
        if(positionSettings == 1){
            stringed[0] = ' ';
            stringed[1] = '"';
            stringed[2] = ' ';
            stringed[3] = ' ';
            stringed[4] = '\0';
        }
        if(positionSettings == 0){
            stringed[0] = '"';
            stringed[1] = ' ';
            stringed[2] = ' ';
            stringed[3] = ' ';
            stringed[4] = '\0';
        }
        LcdString(stringed);
    }
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
    EEPROM.write(0, 1);
    EEPROM.write(1, 2);
    EEPROM.write(2, 3);
    EEPROM.write(3, 4);
    
    InitialiseTachometer();
    initializeValues();
    normalMode = 1;
        
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);

    pinMode(9, OUTPUT);
    digitalWrite(9, LOW);
    pinMode(10, OUTPUT);
    digitalWrite(10, LOW);
    
}

void loop(void){
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
            if(normalMode){
               if(firstLeftPress + 500 < millis()){ //new row of presses
                    firstLeftPress = millis();
                    numberOfPressesLeft = 1;
                }
                else{
                    numberOfPressesLeft++;
                    firstLeftPress = millis();
               }
                if(numberOfPressesLeft > 4){
                    normalMode = 0;
                    numberOfPressesLeft = 0;
                    //adjust wheel circumference
                }
            }
            else{
                if(positionSettings == 0){
                    EEPROM.write(0, changeWheel[0]);
                    EEPROM.write(1, changeWheel[1]);
                    EEPROM.write(2, changeWheel[2]);
                    EEPROM.write(3, changeWheel[3]);
                    setWheelCircumferenceVariable();
                    normalMode = 1;
                    positionSettings = 3;
                }
                else{
                    positionSettings--;
                }
            }
            
        }
        else{
            if(normalMode){
               if(firstRightPress + 500 < millis()){ //new row of presses
                    firstRightPress = millis();
                    numberOfPressesRight = 1;
               }
                 else{
                   numberOfPressesRight++;
                   firstRightPress = millis();
               }
               if(numberOfPressesRight > 4){ // delete stats
                    initializeValues();
               }
            }
            else{
                if(changeWheel[positionSettings] < 9){
                    changeWheel[positionSettings]++;
                }
                else{
                    changeWheel[positionSettings] = 0;
                }
            }
        }
        digitalWrite(butt1, LOW);
    }
}
