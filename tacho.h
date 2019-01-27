#ifndef tacho_h
#define tacho_h

#include "Nokia3310.h"

void intToString(char* cil, uint32_t cislo); //converts int to string (function from string.h is too big

void secondsToTime(char* cil, uint32_t cislo);

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

void metersToKilometers(char* cil, uint32_t cislo){
  int ccopy = cislo;
  cislo -= cislo%10;
  cislo /=10;
  ccopy -= ccopy%10;
  ccopy /=10;
  if(cislo == 0){ //abychom dokazali zapsat nulu
    cil[0] = 0 + '0';
    cil[1] = '\0';
    return;
  }

  
  int cifer = 1, i = 0;
  cislo -= cislo%10;
  while(cislo != 0){
    cislo /=10;
    cifer++;
    cislo -= cislo%10;
  }

  bool dot = 0;
  for(i = 0; i <= cifer; i++){
    if(i != 2 || dot == 1){
      cil[cifer-i] = ccopy%10 + '0';
      ccopy -= ccopy%10;
      ccopy /=10;
    }
    else{
      dot = 1;
      cil[cifer-i] = '.';
    } 
  }
  cil[cifer+1] = '\0';
  if(cil[0] == '0'){
    for(i = 10; i>=0;i--){
      cil[i+1]=cil[i];
    }
    cil[0]='.';
  }
  if(cil[0] == '.'){
    for(i = 10; i>=0;i--){
      cil[i+1]=cil[i];
    }
    cil[0]='0';
  }
}

void secondsToTime(char* cil, uint32_t cislo){
  uint8_t i = 0;

  int s = cislo % 60;
  int m = ((cislo % 3600) - (cislo % 60))/60;
  int h = (cislo - (cislo % 3600))/3600;

  char hs[4];

  intToString(hs, h);
  i = 0;
  while(hs[i] != '\0'){
    cil[i] = hs[i];
    i++;
  }
  cil[i]=':';
  i++;
  cil[i]= '0' + (m-(m%10))/10;
  i++;
  cil[i]= '0' + m%10;
  i++;
  cil[i]=':';
  i++;
  cil[i]= '0' + (s-(s%10))/10;
  i++;
  cil[i]= '0' + s%10;  
  i++;
  cil[i] = '\0';
}

#endif
