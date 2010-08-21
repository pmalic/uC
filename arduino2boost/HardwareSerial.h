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
	std::string m_name;

public:
	/**
	 * Constructor.
	 * \param port device name, example "/dev/ttyUSB0" or "COM4"
	 * \throws boost::system::system_error if cannot open the serial device
	 */
	HardwareSerial (const std::string& name)
	: m_io(), m_port(m_io, name), m_name(name)
	{
	}

	HardwareSerial (const HardwareSerial& serial)
	: m_io(), m_port(m_io, serial.m_name), m_name(serial.m_name)
	{
	}

	HardwareSerial& operator= (const HardwareSerial& serial)
	{
		return *this;
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
	void write (const std::string& str)
	{
		boost::asio::write(m_port, boost::asio::buffer(str, str.size()));
	}

	/**
	 * Writes a buffer to the serial port.
	 * \param buf buffer to write
	 * \param len length of buffer
	 * \throws boost::system::system_error on failure
	 */
	void write (const void* buf, size_t len)
	{
		boost::asio::write(m_port, boost::asio::buffer(buf, len));
	}

	/**
	 * Prints data to the serial port as human-readable ASCII text.
	 * \param i int to write
	 * \throws boost::system::system_error on failure
	 */
	void print (int i, unsigned int format = 0)
	{
		std::ostringstream oss;

		switch (format)
		{
			case BYTE:
			{
				char c = static_cast<char> (i);
				oss << c;
			}
				break;

			case OCT:
				oss << std::oct << i;
				break;

			case HEX:
				oss << std::hex << i;
				break;

			default:
				oss << i;
		}

		const std::string& str = oss.str();
		boost::asio::write(m_port, boost::asio::buffer(str, str.size()));
	}

	/**
	 * Prints data to the serial port as human-readable ASCII text.
	 * \param c char to write
	 * \throws boost::system::system_error on failure
	 */
	void print (char c, unsigned int format = 0)
	{
		std::ostringstream oss;

		switch (format)
		{
			case BYTE:
				oss << c;
				break;

			case OCT:
			{
				int i = static_cast<int> (c);
				oss << std::oct << i;
			}
				break;

			case HEX:
			{
				int i = static_cast<int> (c);
				oss << std::hex << i;
			}
				break;

			default:
				oss << c;
		}

		const std::string& str = oss.str();
		boost::asio::write(m_port, boost::asio::buffer(str, str.size()));
	}

	/**
	 * Prints data to the serial port as human-readable ASCII text.
	 * \param d double to write
	 * \throws boost::system::system_error on failure
	 */
	void print (double d, unsigned int decimals = 2)
	{
		std::ostringstream oss;

		oss << std::fixed << std::setprecision(decimals) << d;

		const std::string& str = oss.str();
		boost::asio::write(m_port, boost::asio::buffer(str, str.size()));
	}

	/**
	 * Prints data to the serial port as human-readable ASCII text.
	 * \param str char* to write
	 * \throws boost::system::system_error on failure
	 */
	void print (const char* str, unsigned int format = 0)
	{
		boost::asio::write(m_port, boost::asio::buffer(str, strlen(str)));
	}

	/**
	 * Prints data to the serial port as human-readable ASCII text.
	 * \param str string to write
	 * \throws boost::system::system_error on failure
	 */
	void print (const std::string& str, unsigned int format = 0)
	{
		boost::asio::write(m_port, boost::asio::buffer(str, str.size()));
	}

	/**
	 * Prints data to the serial port as human-readable ASCII text
	 * followed by a carriage return character (ASCII 13, or '\r')
	 * and a newline character (ASCII 10, or '\n').
	 * \param i int to write
	 * \throws boost::system::system_error on failure
	 */
	void println (int i, unsigned int format = 0)
	{
		print(i, format);
		write('\r');
		write('\n');
	}

	/**
	 * Prints data to the serial port as human-readable ASCII text
	 * followed by a carriage return character (ASCII 13, or '\r')
	 * and a newline character (ASCII 10, or '\n').
	 * \param c char to write
	 * \throws boost::system::system_error on failure
	 */
	void println (char c, unsigned int format = 0)
	{
		print(c, format);
		write('\r');
		write('\n');
	}

	/**
	 * Prints data to the serial port as human-readable ASCII text
	 * followed by a carriage return character (ASCII 13, or '\r')
	 * and a newline character (ASCII 10, or '\n').
	 * \param d double to write
	 * \throws boost::system::system_error on failure
	 */
	void println (double d, unsigned int decimals = 2)
	{
		print(d, decimals);
		write('\r');
		write('\n');
	}

	/**
	 * Prints data to the serial port as human-readable ASCII text
	 * followed by a carriage return character (ASCII 13, or '\r')
	 * and a newline character (ASCII 10, or '\n').
	 * \param str char* to write
	 * \throws boost::system::system_error on failure
	 */
	void println (const char* str, unsigned int format = 0)
	{
		print(str, format);
		write('\r');
		write('\n');
	}

	/**
	 * Prints data to the serial port as human-readable ASCII text
	 * followed by a carriage return character (ASCII 13, or '\r')
	 * and a newline character (ASCII 10, or '\n').
	 * \param str string to write
	 * \throws boost::system::system_error on failure
	 */
	void println (const std::string str, unsigned int format = 0)
	{
		print(str, format);
		write('\r');
		write('\n');
	}


};

#endif /* HARDWARESERIAL_H_ */
