#pragma once

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/utility/binary.hpp>

typedef std::vector<unsigned int> uint_vector;

#include "I2C.h"


// The LCD data pins (D4-D7) connect to MCP pins 12-9 (PORTB4-1), in
// that order.  Because this sequence is 'reversed,' a direct shift
// won't work.  This table remaps 4-bit data values to MCP PORTB
// outputs, incorporating both the reverse and shift.
static const unsigned int FLIP[16] = {
	BOOST_BINARY(00000000),
	BOOST_BINARY(00010000),
	BOOST_BINARY(00001000),
	BOOST_BINARY(00011000),
	BOOST_BINARY(00000100),
	BOOST_BINARY(00010100),
	BOOST_BINARY(00001100),
	BOOST_BINARY(00011100),
	BOOST_BINARY(00000010),
	BOOST_BINARY(00010010),
	BOOST_BINARY(00001010),
	BOOST_BINARY(00011010),
	BOOST_BINARY(00000110),
	BOOST_BINARY(00010110),
	BOOST_BINARY(00001110),
	BOOST_BINARY(00011110)
};

class LCDDriver {

public:
	typedef boost::shared_ptr<LCDDriver> shared_ptr;

	LCDDriver(I2C::shared_ptr pI2C);


protected:

	// Port expander registers
	static const unsigned int MCP23017_IOCON_BANK0 = 0x0A;    // IOCON when Bank 0 active
	static const unsigned int MCP23017_IOCON_BANK1 = 0x15;    // IOCON when Bank 0 active
	// Register addresses for Bank1 only
	static const unsigned int MCP23017_GPIOA  = 0x09;
	static const unsigned int MCP23017_GPIOB  = 0x19;
	static const unsigned int MCP23017_IODIRB = 0x10;

	// Port expander input pins
	static const unsigned int SELECT = 0;
	static const unsigned int RIGHT  = 1;
	static const unsigned int DOWN   = 2;
	static const unsigned int UP     = 3;
	static const unsigned int LEFT   = 4;

	// LED colours
	static const unsigned int OFF    = 0x00;
	static const unsigned int RED    = 0x01;
	static const unsigned int GREEN  = 0x02;
	static const unsigned int BLUE   = 0x04;
	static const unsigned int YELLOW = RED + GREEN;
	static const unsigned int TEAL   = GREEN + BLUE;
	static const unsigned int VIOLET = RED + BLUE;
	static const unsigned int WHITE  = RED + GREEN + BLUE;
	static const unsigned int ON     = RED + GREEN + BLUE;

	// LCD Commands
	static const unsigned int LCD_CLEARDISPLAY   = 0x01;
	static const unsigned int LCD_RETURNHOME     = 0x02;
	static const unsigned int LCD_ENTRYMODESET   = 0x04;
	static const unsigned int LCD_DISPLAYCONTROL = 0x08;
	static const unsigned int LCD_CURSORSHIFT    = 0x10;
	static const unsigned int LCD_FUNCTIONSET    = 0x20;
	static const unsigned int LCD_SETCGRAMADDR   = 0x40;
	static const unsigned int LCD_SETDDRAMADDR   = 0x80;

	// Flags for display on/off control
	static const unsigned int LCD_DISPLAYON  = 0x04;
	static const unsigned int LCD_DISPLAYOFF = 0x00;
	static const unsigned int LCD_CURSORON   = 0x02;
	static const unsigned int LCD_CURSOROFF  = 0x00;
	static const unsigned int LCD_BLINKON    = 0x01;
	static const unsigned int LCD_BLINKOFF   = 0x00;

	// Flags for display entry mode
	static const unsigned int LCD_ENTRYRIGHT          = 0x00;
	static const unsigned int LCD_ENTRYLEFT           = 0x02;
	static const unsigned int LCD_ENTRYSHIFTINCREMENT = 0x01;
	static const unsigned int LCD_ENTRYSHIFTDECREMENT = 0x00;

	// Flags for display/cursor shift
	static const unsigned int LCD_DISPLAYMOVE = 0x08;
	static const unsigned int LCD_CURSORMOVE  = 0x00;
	static const unsigned int LCD_MOVERIGHT   = 0x04;
	static const unsigned int LCD_MOVELEFT    = 0x00;

	// Compound LCD commands
	static const unsigned int LCD_DISPLAYSHIFT     = LCD_CURSORMOVE | LCD_MOVERIGHT;
	static const unsigned int LCD_DISPLAYMODE      = LCD_ENTRYLEFT  | LCD_ENTRYSHIFTDECREMENT;
	// XXX conflicting name
	static const unsigned int LCD_DISPLAYCONTROL_2 = LCD_DISPLAYON  | LCD_CURSOROFF | LCD_BLINKOFF;

	// Write operations



	// Low-level 4-bit interface for LCD output.  This doesn't actually
	// write data, just returns a byte array of the PORTB state over time.
	// Can concatenate the output of multiple calls (up to 8) for more
	// efficient batch write.
	uint_vector out4(unsigned int bitmask, unsigned int value) {
		unsigned int hi = bitmask | FLIP[value >> 4];
		unsigned int lo = bitmask | FLIP[value & 0x0F];
		uint_vector out;
		out.push_back(hi | BOOST_BINARY(00100000));
		out.push_back(hi);
		out.push_back(lo | BOOST_BINARY(00100000));
		out.push_back(lo);
		return out;
	};

private:

	// I2C interface
	I2C::shared_ptr m_pI2C;

	// private data
	unsigned int m_portA, m_portB;
	unsigned int m_ddrb;
};
