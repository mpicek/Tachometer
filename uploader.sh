#!/bin/bash
#UPLOADER


echo "Working with main.c"

echo "Compiling..."
rm main.elf
rm main.o
rm main.hex
avr-gcc -Wall -g -Os -mmcu=atmega8 -std=c99 -c main.c              # -std=c99 je tam kvuli tomu, aby to zvladlo inicializaci v blocich, napr. for (int i ...) .... a -Wall je to jako Warning all - da kecy o compilingu
avr-gcc -g -mmcu=atmega8 -std=c99 -o main.elf main.o
avr-objcopy -j.text -j.data -O ihex main.elf main.hex


echo "Processor: AVR Atmel Attiny2313"
echo "Programmer: avrisp"
echo "Port /dev/ttyUSB0"
echo "Baudrate 19200"
echo "Uploading..."

avrdude -p m8 -c avrisp -P /dev/ttyUSB0 -b 19200 -U flash:w:main.hex
echo "Made by Martin Picek."
echo "Version 1.0 (12th of February 2015)."
echo "Thank you."
