/*
 * HardwareSerial.h
 *
 *  Created on: Aug 15, 2010
 *      Author: pm
 */

#ifndef HARDWARESERIAL_H_
#define HARDWARESERIAL_H_

#include <boost/asio.hpp>
#include <iostream>
#include <iomanip>
#include <string>
#include "WProgram.h"

class HardwareSerial
{
private:
	boost::asio::io_service m_io;
	boost::asio::serial_port m_port;

public:
	/**
	 * Constructor.
	 * \param port device name, example "/dev/ttyUSB0" or "COM4"
	 * \throws boost::system::system_error if cannot open the serial device
	 */
	HardwareSerial (std::string name)
	: m_io(), m_port(m_io, name)
	{
	}

	/**
	 * Sets the data rate in bits per second (baud) for serial data transmission.
	 * \param speed communication speed, example 9600 or 115200
	 * \throws boost::system::system_error if cannot open the serial device
	 */
	void begin (unsigned int speed)
	{
		m_port.set_option(boost::asio::serial_port_base::baud_rate(speed));
	}

	/**
	 * Dummy.
	 */
	void end ()
	{
	}

	/**
	 * Dummy.
	 */
	unsigned int available ()
	{
		return 1;
	}

	/**
	 * Reads incoming serial data.
	 * \return the first byte of incoming serial data available
	 * \throws boost::system::system_error on failure
	 */
	char read ()
	{
		char c;

		boost::asio::read(m_port, boost::asio::buffer(&c, 1));

		return c;
	}

	/**
	 * Dummy.
	 */
	void flush ()
	{
	}

	/**
	 * Writes a char to the serial port.
	 * \param c char to write
	 * \throws boost::system::system_error on failure
	 */
	void write (char c)
	{
		boost::asio::write(m_port, boost::asio::buffer(&c, 1));
	}

	/**
	 * Writes a string to the serial port.
	 * \param c string to write
	 * \throws boost::system::system_error on failure
	 */
	void write (std::string str)
	{
		boost::asio::write(m_port, boost::asio::buffer(str, str.size()));
	}

	/**
	 * Writes a buffer to the serial port.
	 * \param buf buffer to write
	 * \param len length of buffer
	 * \throws boost::system::system_error on failure
	 */
	void write (void* buf, size_t len)
	{
		boost::asio::write(m_port, boost::asio::buffer(buf, len));
	}

	/**
	 * Prints data to the serial port as human-readable ASCII text.
	 * \param i int to write
	 * \throws boost::system::system_error on failure
	 */
	void print (int i, int format)
	{
		std::ostringstream oss;
		char c;

		switch (format)
		{
			case BYTE:
				c = static_cast<char> (i);
				oss << c;
				break;

			case OCT:
				oss << std::oct << i;
				break;

			case HEX:
				oss << std::hex << i;
				break;
		}

		const std::string& str = oss.str();
		boost::asio::write(m_port, boost::asio::buffer(str, str.size()));
	}

	/**
	 * Prints data to the serial port as human-readable ASCII text.
	 * \param c char to write
	 * \throws boost::system::system_error on failure
	 */
	void print (char c, int format)
	{
		std::ostringstream oss;
		int i;

		switch (format)
		{
			case BYTE:
				oss << c;
				break;

			case OCT:
				i = static_cast<int> (c);
				oss << std::oct << i;
				break;

			case HEX:
				i = static_cast<int> (c);
				oss << std::hex << i;
				break;
		}

		const std::string& str = oss.str();
		boost::asio::write(m_port, boost::asio::buffer(str, str.size()));
	}

	/**
	 * Prints data to the serial port as human-readable ASCII text.
	 * \param d double to write
	 * \throws boost::system::system_error on failure
	 */
	void print (double d, int decimals = 2)
	{
		std::ostringstream oss;

		oss << std::fixed << std::setprecision(decimals) << d;

		const std::string& str = oss.str();
		boost::asio::write(m_port, boost::asio::buffer(str, str.size()));
	}

	/**
	 * Prints data to the serial port as human-readable ASCII text.
	 * \param d double to write
	 * \throws boost::system::system_error on failure
	 */
	void print (char* str, int format)
	{
		boost::asio::write(m_port, boost::asio::buffer(str, strlen(str)));
	}

	/**
	 * Prints data to the serial port as human-readable ASCII text.
	 * \param d double to write
	 * \throws boost::system::system_error on failure
	 */
	void print (std::string str, int format)
	{
		boost::asio::write(m_port, boost::asio::buffer(str, str.size()));
	}

};

HardwareSerial Serial("/dev/tty.usbserial-A700eX8n");

#endif /* HARDWARESERIAL_H_ */
