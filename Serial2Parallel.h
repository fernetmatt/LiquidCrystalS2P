/*
	
	Serial2Parallel addon driver for the LiquidCrystal library

	Library originally added 31 Jan 2013
	by Matteo Benetti (http://www.alpheratz1991.com)
	
	This example code is in the public domain.

*/

#ifndef Serial2Parallel_h
#define Serial2Parallel_h

#include <inttypes.h>

#define SR_DB(x) x
#define SR_E   2
#define SR_RS  3

class Serial2Parallel
{
	public:
		Serial2Parallel();
		Serial2Parallel(int rck, int sck, int si);
		
		void init(int rck, int sck, int si);
		
		void set(int position, bool state);
		void set(uint8_t data);
		void clear();
		void write();
		uint8_t get();
		
	private:
		int _RCK, _SCK, _SI;
		uint8_t _data_register;
};
#endif
