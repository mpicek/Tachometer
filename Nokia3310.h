#ifndef Nokia3310_h
#define Nokia3310_h

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define LCD_X     84
#define LCD_Y     48

void setPins(uint8_t RST, uint8_t CE, uint8_t DC, uint8_t DIN, uint8_t CLK); //sets pins (stores them to global variables) and sets them as outputs

void low(uint8_t pin); //set pin low

void high(uint8_t pin); //set pin high

void shiftOut(uint8_t val); //shift out data

void Write(uint8_t data); //write DATA to LCD (similar function is Command)

void LcdCharacter(uint8_t character); //prints character on LCD

void LcdString(char *characters); //prints string on LCD

void InitialiseLcd(); //initialises LCD (needs some commands)

void Command(uint8_t data); //writes COMMAND to LCD

void GotoXY(uint8_t x,uint8_t y); //goes on X an Y position on LCD

void LcdClear(void); //clears whole display

void writeBigChar(uint8_t character);

void LcdStringBig(char *characters);

void writeBigSpace();


const uint8_t ASCII[91][5] PROGMEM = //table of ascii characters
{
{0x00, 0x00, 0x00, 0x00, 0x00} // 20
,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
,{0x06, 0x06, 0x06, 0x06, 0x06} // 22 oznacovatko pozice
,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c ¥
,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j
,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
};

const uint8_t big_number[15][3][16] PROGMEM = {

{{0x00,0x00,0xc0,0xe0,0x70,0x30,0x30,0x30,0x70,0xe0,0xc0,0x00,0x00,0x00,0x00,0x00}, // '0'
 {0x00,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x00,0x00,0x00,0x00},
 {0x00,0x07,0x1f,0x38,0x70,0x60,0x60,0x60,0x70,0x38,0x1f,0x07,0x00,0x00,0x00,0x00}},

{{0x00,0x00,0x00,0xc0,0xc0,0xc0,0xf0,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // '1'
 {0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
 {0x00,0x00,0x00,0x60,0x60,0x60,0x7f,0x7f,0x60,0x60,0x60,0x00,0x00,0x00,0x00,0x00}},

{{0x00,0xe0,0x60,0x70,0x30,0x30,0x30,0x30,0x60,0xe0,0x80,0x00,0x00,0x00,0x00,0x00},  // '2'
 {0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xc0,0xf0,0x3f,0x1f,0x00,0x00,0x00,0x00,0x00},
 {0x00,0x70,0x78,0x7c,0x6e,0x67,0x63,0x61,0x60,0x60,0x60,0x60,0x00,0x00,0x00,0x00}},   

{{0x00,0xe0,0x60,0x70,0x30,0x30,0x30,0x30,0x30,0x60,0xe0,0xc0,0x00,0x00,0x00,0x00},  // '3'
 {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x18,0x3c,0x7c,0xe7,0xc3,0x00,0x00,0x00,0x00},
 {0x00,0x38,0x30,0x70,0x60,0x60,0x60,0x60,0x70,0x38,0x1f,0x0f,0x00,0x00,0x00,0x00}},
 
{{0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xc0,0x60,0xf0,0xf0,0x00,0x00,0x00,0x00,0x00},  // '4'
 {0x00,0xe0,0xf0,0xdc,0xce,0xc7,0xc1,0xc0,0xc0,0xff,0xff,0x00,0x00,0x00,0x00,0x00},
 {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0x7f,0x00,0x00,0x00,0x00,0x00}},

{{0x00,0x00,0xf0,0xf0,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x00,0x00,0x00,0x00,0x00},  // '5'
 {0x00,0x00,0x1f,0x1f,0x18,0x18,0x18,0x18,0x30,0xf0,0xc0,0x00,0x00,0x00,0x00,0x00},
 {0x00,0x30,0x70,0x60,0x60,0x60,0x60,0x70,0x38,0x1f,0x0f,0x00,0x00,0x00,0x00,0x00}},

{{0x00,0x00,0x80,0xc0,0xe0,0x60,0x30,0x30,0x30,0x30,0x30,0x00,0x00,0x00,0x00,0x00},  // '6'
 {0x00,0xfc,0xff,0x33,0x18,0x18,0x18,0x18,0x18,0x30,0xf0,0xc0,0x00,0x00,0x00,0x00},
 {0x00,0x07,0x1f,0x38,0x70,0x60,0x60,0x60,0x60,0x30,0x1f,0x0f,0x00,0x00,0x00,0x00}},

{{0x00,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0xb0,0xf0,0xf0,0x00,0x00,0x00,0x00,0x00},  // '7'
 {0x00,0x00,0x00,0x00,0xc0,0xf0,0x78,0x1e,0x07,0x01,0x00,0x00,0x00,0x00,0x00,0x00},
 {0x00,0x60,0x78,0x3e,0x0f,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{{0x00,0x80,0xe0,0x60,0x30,0x30,0x30,0x30,0x30,0x60,0xe0,0xc0,0x00,0x00,0x00,0x00},  // '8'
 {0x00,0x87,0xe7,0x6c,0x18,0x18,0x30,0x30,0x38,0x6c,0xc7,0x83,0x00,0x00,0x00,0x00},
 {0x00,0x0f,0x3f,0x38,0x70,0x60,0x60,0x60,0x60,0x30,0x1f,0x0f,0x00,0x00,0x00,0x00}},

{{0x00,0x80,0xc0,0x60,0x30,0x30,0x30,0x30,0x70,0xe0,0xc0,0x00,0x00,0x00,0x00,0x00},  // '9'
 {0x00,0x0f,0x3f,0x30,0x60,0x60,0x60,0x60,0x60,0x30,0xff,0xff,0x00,0x00,0x00,0x00},
 {0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x70,0x38,0x1e,0x0f,0x03,0x00,0x00,0x00,0x00}},

{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // '.'
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0x38,0x38,0x38,0,0,0,0,0,0,0,0,0,0,0,0}},

{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // '+'
 {0,0x40,0x40,0x40,0xF8,0x40,0x40,0x40,0,0,0,0,0,0,0},
 {0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0}},

{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},   // '-'
 {0,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},

{{0,0xE0,0xE0,0xE0,0,0,0,0,0,0,0,0,0,0,0,0},  // ':'
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0xE,0xE,0xE,0,0,0,0,0,0,0,0,0,0,0,0}},

{{0,0,0,0xF0,0,0,0,0,0,0,0,0,0,0,0,0},  // '/'
 {0,0,0xF8,0x7,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0xFC,0x3,0,0,0,0,0,0,0,0,0,0,0,0,0}}
};

uint8_t rst, ce, dc, din, clk;

void writeBigChar(uint8_t character, byte pos){
    for(byte row = 0; row < 3; row++){
        GotoXY(pos*18, row);
        for(byte col = 0; col < 16; col++){
            Write(pgm_read_byte(&big_number[character-'0'][row][col]));
        }
    }
}

void LcdStringBig(char *characters){
  byte i = 0;
  while (*characters)
  {
    writeBigChar(*characters++, i);
    i++;
  }
}

void writeBigSpace(){
    for(byte row = 0; row < 3; row++){
        GotoXY(4, row);
        Write(0);
        Write(0);
    }
}

void setPins(uint8_t RST, uint8_t CE, uint8_t DC, uint8_t DIN, uint8_t CLK){
    rst = RST;
    ce = CE;
    dc = DC;
    din = DIN;
    clk = CLK;
    pinMode(rst, OUTPUT);
    pinMode(ce, OUTPUT);
    pinMode(dc, OUTPUT);
    pinMode(din, OUTPUT);
    pinMode(clk, OUTPUT);
}

void low(uint8_t pin){
  digitalWrite(pin, LOW);
}

void high(uint8_t pin){
  digitalWrite(pin, HIGH);
}

void shiftOut(uint8_t val){
  uint8_t i;

  for (i = 0; i < 8; i++)  {

    if((!!(val & (1 << (7 - i)))) == 1){
      high(din);
    }
    else if((!!(val & (1 << (7 - i)))) == 0){
      low(din);
    }

    high(clk);
    low(clk);
  }
}

void Write(uint8_t data){
  high(dc);
  low(ce);
  shiftOut(data);
  high(ce);
}


void LcdCharacter(uint8_t character){
  Write(0x00);
  for (uint8_t index = 0; index < 5; index++)
  {
    Write(pgm_read_byte(&ASCII[character - 0x20][index]));
  }
  Write(0x00);
}


void LcdString(char *characters){
  while (*characters)
  {
    LcdCharacter(*characters++);
  }
}


void InitialiseLcd(){  //DONE
    low(rst);
    high(rst);

    Command(0x21); // set LCD Vop (contrast)
    Command(0xB1); // set temp coefficent
    Command(0x04); // LCD bias mode 1:40
    Command(0x14); // LCD basic commands
    Command(0x20);
    Command(0x0C);
}

void Command(uint8_t data){  //DONE
    low(dc);
    low(ce);
    shiftOut(data);
    high(ce);
}



void GotoXY(uint8_t x,uint8_t y){  //DONE
    x=x+128;
    y=y+64;
    Command(x);
    Command(y);
}

void LcdClear(void){
  for (int index = 0; index < LCD_X * LCD_Y / 8; index++)
  {
    Write(0x00);
  }
}


#endif
