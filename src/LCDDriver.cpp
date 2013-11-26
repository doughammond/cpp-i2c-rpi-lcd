#include "LCDDriver.h"

LCDDriver::LCDDriver(I2C::shared_ptr pI2C) :
	m_pI2C(pI2C),
	m_portA(0), m_portB(0),
	m_ddrb(BOOST_BINARY(00010000))
{
	// Set MCP23017 IOCON register to Bank 0 with sequential operation.
	// If chip is already set for Bank 0, this will just write to OLATB,
	// which won't seriously bother anything on the plate right now
	// (blue backlight LED will come on, but that's done in the next
	// step anyway)
	m_pI2C->write_byte(MCP23017_IOCON_BANK1, (unsigned)0x00);

	// Brute force reload ALL registers to known state.  This also
	// sets up all the input pins, pull-ups, etc. for the Pi Plate.
	unsigned int _bData[] = {
		BOOST_BINARY(00111111),   // IODIRA    R+G LEDs=outputs, buttons=inputs
		m_ddrb,                   // IODIRB    LCD D7=input, Blue LED=output
		BOOST_BINARY(00111111),   // IPOLA     Invert polarity on button inputs
		BOOST_BINARY(00000000),   // IPOLB
		BOOST_BINARY(00000000),   // GPINTENA  Disable interrupt-on-change
		BOOST_BINARY(00000000),   // GPINTENB
		BOOST_BINARY(00000000),   // DEFVALA
		BOOST_BINARY(00000000),   // DEFVALB
		BOOST_BINARY(00000000),   // INTCONA
		BOOST_BINARY(00000000),   // INTCONB
		BOOST_BINARY(00000000),   // IOCON
		BOOST_BINARY(00000000),   // IOCON
		BOOST_BINARY(00111111),   // GPPUA     Enable pull-ups on buttons
		BOOST_BINARY(00000000),   // GPPUB
		BOOST_BINARY(00000000),   // INTFA
		BOOST_BINARY(00000000),   // INTFB
		BOOST_BINARY(00000000),   // INTCAPA
		BOOST_BINARY(00000000),   // INTCAPB
		m_portA,                  // GPIOA
		m_portB,                  // GPIOB
		m_portA,                  // OLATA     0 on all outputs; side effect of
		m_portB                   // OLATB     turning on R+G+B backlight LEDs.
	};
	uint_vector _data;
	m_pI2C->write_block_data((unsigned)0x00, _data);

	// Switch to Bank 1 and disable sequential operation.
	// From this point forward, the register addresses do NOT match
	// the list immediately above.  Instead, use the constants defined
	// at the start of the class.  Also, the address register will no
	// longer increment automatically after this -- multi-byte
	// operations must be broken down into single-byte calls.
	m_pI2C->write_byte(MCP23017_IOCON_BANK0, (unsigned)BOOST_BINARY(10100000));

	m_pI2C->write(0x33);	// Init
	m_pI2C->write(0x32);	// Init
	m_pI2C->write(0x28);	// 2 line 5x8 matrix
	m_pI2C->write(LCD_CLEARDISPLAY);
	m_pI2C->write(LCD_CURSORSHIFT | LCD_DISPLAYSHIFT);
	m_pI2C->write(LCD_ENTRYMODESET | LCD_DISPLAYMODE);
	m_pI2C->write(LCD_DISPLAYCONTROL | LCD_DISPLAYCONTROL_2);
	m_pI2C->write(LCD_RETURNHOME);

};
