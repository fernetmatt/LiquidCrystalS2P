/*
	
	Serial2Parallel addon driver for the LiquidCrystal library

	Library originally added 31 Jan 2013
	by Matteo Benetti (http://www.alpheratz1991.com)
	
	This example code is in the public domain.

*/

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"
#include "Serial2Parallel.h"

/* 8bit shift register: { [], [], [], [], [], [], [], [] )
 *               					 0   1   2   3   4   5   6   7
 *              		  		 X   X   E  RS   DB4 DB5 DB6 DB7  
 */
// 7		6		5		4		3		2		1		0
// DB7	DB6 DB5	DB4 RS	E 	X		X

Serial2Parallel::Serial2Parallel()
{
  // NOP
}

Serial2Parallel::Serial2Parallel(int rck, int sck, int si)
{
	Serial2Parallel::init(rck, sck, si);
}

void Serial2Parallel::init(int rck, int sck, int si)
{
	_RCK = rck;
	_SCK = sck;
	_SI = si;
	
	pinMode(_RCK, OUTPUT);
	pinMode(_SCK, OUTPUT);
	pinMode(_SI, OUTPUT);
	
	_data_register = 0x00;
	Serial2Parallel::set(2, HIGH);
	Serial2Parallel::set(2, HIGH);
	
	Serial2Parallel::write();
	
}

void Serial2Parallel::set(int position, bool state)
{
	if (state)
	{
		_data_register |= (1 << position);
	}
	else
	{
    	_data_register &= ~(1 << position);
	}
}

void Serial2Parallel::set(uint8_t data)
{
	_data_register &= data;
}

uint8_t Serial2Parallel::get()
{
	return _data_register;
}

void Serial2Parallel::write()
{
	digitalWrite(_RCK, LOW); 
  
	for(int i = 7; i >= 0; i--)
	{
		digitalWrite(_SCK, LOW);
		digitalWrite(_SI, ((_data_register&(0x01<<i)) >> i));
		digitalWrite(_SCK, HIGH);
	}

	digitalWrite(_RCK, HIGH); 
}