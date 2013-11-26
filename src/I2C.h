#pragma once

#include <vector>

#include <boost/shared_ptr.hpp>

class I2C {
public:
	typedef boost::shared_ptr<I2C> shared_ptr;

	void write(unsigned int value);
	void write_byte(unsigned int &addr, unsigned int value);

	void write_block_data(unsigned int addr, std::vector<unsigned int> data);
};