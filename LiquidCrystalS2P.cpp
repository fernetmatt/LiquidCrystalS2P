/*
	
	Serial to parallel modified LiquidCrystal library	
	
	Library originally added 18 Apr 2008
 	by David A. Mellis
 	library modified 5 Jul 2009
 	by Limor Fried (http://www.ladyada.net)
	Library modified 31 Jan 2013
	by Matteo Benetti (http://www.alpheratz1991.com)
	
	This example code is in the public domain.

*/

#include "LiquidCrystalS2P.h"
#include "Serial2Parallel.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1 
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

// output pins
// 8bit shift register: { [], [], [], [], [], [], [], [] )
//              					 0   1   2   3   4   5   6   7
//              					 X   X  DB4 DB5 DB6 DB7  E   RS
//

LiquidCrystal::LiquidCrystal(int s2p_rck, int s2p_sck, int s2p_si)
{
  s2pdriver.init(s2p_rck, s2p_sck, s2p_si);
  
  _displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
	
  begin(16, 2);  
}

void LiquidCrystal::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {

  _numlines = lines;
  _currline = 0;


  // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  // according to datasheet, we need at least 40ms after power rises above 2.7V
  // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
  
  delayMicroseconds(50000); 
  
  //put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46

	s2pdriver.set(SR_RS, LOW);
	s2pdriver.set(SR_E, LOW);
	s2pdriver.set(SR_DB(4), HIGH);
	s2pdriver.set(SR_DB(5), HIGH);
	
	s2pdriver.write();
	delayMicroseconds(4500); // wait min 4.1ms

	// second try
	s2pdriver.write();
	delayMicroseconds(4500); // wait min 4.1ms
    
  // third go!
	s2pdriver.write();
  delayMicroseconds(150);

  // finally, set to 4-bit interface
  s2pdriver.set(SR_DB(4), LOW);
	s2pdriver.write();
  
  // finally, set # lines, font size, etc.
  command(LCD_FUNCTIONSET | _displayfunction);  

  // turn the display on with no cursor or blinking default
  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
  display();

  // clear it off
  clear();

  // Initialize to default text direction (for romance languages)
  _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  command(LCD_ENTRYMODESET | _displaymode);

}

/********** high level commands, for the user! */
void LiquidCrystal::clear()
{
  command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void LiquidCrystal::home()
{
  command(LCD_RETURNHOME);  // set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void LiquidCrystal::setCursor(uint8_t col, uint8_t row)
{
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if ( row >= _numlines ) {
    row = _numlines-1;    // we count rows starting w/0
  }
  
  command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void LiquidCrystal::noDisplay() {
  _displaycontrol &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal::display() {
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LiquidCrystal::noCursor() {
  _displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal::cursor() {
  _displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void LiquidCrystal::noBlink() {
  _displaycontrol &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal::blink() {
  _displaycontrol |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LiquidCrystal::scrollDisplayLeft(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LiquidCrystal::scrollDisplayRight(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LiquidCrystal::leftToRight(void) {
  _displaymode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LiquidCrystal::rightToLeft(void) {
  _displaymode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LiquidCrystal::autoscroll(void) {
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LiquidCrystal::noAutoscroll(void) {
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LiquidCrystal::createChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7; // we only have 8 locations 0-7
  command(LCD_SETCGRAMADDR | (location << 3));
  for (int i=0; i<8; i++) {
    write(charmap[i]);
  }
}

/*********** mid level commands, for sending data/cmds */

inline void LiquidCrystal::command(uint8_t value) {
  send(value, LOW);
}

inline size_t LiquidCrystal::write(uint8_t value) {
  send(value, HIGH);
  return 1; // assume sucess
}

/************ low level data pushing commands **********/

// write either command or data, with automatic 4/8-bit selection
void LiquidCrystal::send(uint8_t value, uint8_t mode) {
  
	s2pdriver.set(SR_RS, mode);
	s2pdriver.write();
	
 	write4bits(value>>4);
	write4bits(value);
}

void LiquidCrystal::pulseEnable(void) {
	
	s2pdriver.set(SR_E, LOW);
	s2pdriver.write();
	
	delayMicroseconds(1);  
	  
	s2pdriver.set(SR_E, HIGH);
	s2pdriver.write();
	
  delayMicroseconds(1);    
	
	s2pdriver.set(SR_E, LOW);
	s2pdriver.write();
	
  delayMicroseconds(100);   // commands need > 37us to settle
}

void LiquidCrystal::write4bits(uint8_t value) {
  
	s2pdriver.set(SR_DB(4), (value >> 3) & 0x01);
	s2pdriver.write();
	s2pdriver.set(SR_DB(5), (value >> 2) & 0x01);
	s2pdriver.write();
	s2pdriver.set(SR_DB(6), (value >> 1) & 0x01);
	s2pdriver.write();
	s2pdriver.set(SR_DB(7), (value >> 0) & 0x01);
	s2pdriver.write();
	
	
  pulseEnable();
}