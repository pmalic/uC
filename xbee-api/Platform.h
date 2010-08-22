/**
 * Copyright (c) 2010 Predrag Malicevic. All rights reserved.
 *
 * This file is part of XBee-Arduino.
 *
 * XBee-Arduino is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * XBee-Arduino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XBee-Arduino.  If not, see <http://www.gnu.org/licenses/>.
 */

#if defined SERIAL
	#include <inttypes.h>
	#include <WProgram.h>
	#include <HardwareSerial.h>
#elif defined MBED_LIBRARY_VERSION
	#include <mbed.h>
#else
	#include <boost/asio.hpp>
	#include <boost/date_time/posix_time/posix_time.hpp>
#endif


class Platform
{
public:
	class Stopwatch
	{
	private:
#if defined SERIAL
		unsigned long _start;
#elif defined MBED_LIBRARY_VERSION
		Timer _timer;
#else
		boost::posix_time::ptime _start;
#endif

	public:
		Stopwatch ()
		{
#if defined SERIAL
			_start = millis();
#elif defined MBED_LIBRARY_VERSION
			_timer.start();
#else
			_start = boost::posix_time::microsec_clock::universal_time();
#endif
		}

#if defined MBED_LIBRARY_VERSION
		~Stopwatch ()
		{
			_timer.stop();
		}
#endif

		unsigned long read ()
		{
#if defined SERIAL
			return millis() - _start;
#elif defined MBED_LIBRARY_VERSION
			return _timer.read_us();
#else
		using namespace boost::posix_time;

		const time_duration diff = ptime(microsec_clock::universal_time()) - _start;

		return static_cast<unsigned long>(diff.total_milliseconds());
#endif
		}
	};

	class SerialPort
	{
	private:
#if defined SERIAL
		HardwareSerial _port;
#elif defined MBED_LIBRARY_VERSION
		Serial _port;
#else
		boost::asio::io_service _io;
		boost::asio::serial_port _port;
		std::string _name;
#endif

	public:
#if defined SERIAL
		SerialPort (HardwareSerial port = Serial)
		: _port(port)
		{
		}

		SerialPort (const SerialPort& serialPort)
		: _port(serialPort._port)
		{
		}
#elif defined MBED_LIBRARY_VERSION
		SerialPort (PinName tx = p28, PinName rx = p27)
		: _port(Serial(tx, rx))
		{
		}

		SerialPort (const SerialPort& serialPort)
		: _port(serialPort._port)
		{
		}

		SerialPort (Serial port)
		: _port(port)
		{
		}
#else
		SerialPort (const std::string& name = "COM3")
		: _io(), _port(_io, name), _name(name)
		{
		}

		SerialPort (const SerialPort& serialPort)
		: _io(), _port(_io, serialPort._name), _name(serialPort._name)
		{
		}
#endif

		void begin (long speed)
		{
#if defined SERIAL
			_port.begin(speed);
#elif defined MBED_LIBRARY_VERSION
			_port.baud(speed);
#else
			_port.set_option(boost::asio::serial_port_base::baud_rate(speed));
#endif
		}

		int readable ()
		{
#if defined SERIAL
			return _port.available();
#elif defined MBED_LIBRARY_VERSION
			return _port.readable();
#else
			return 1;
#endif
		}

		int read ()
		{
#if defined SERIAL
			return _port.read();
#elif defined MBED_LIBRARY_VERSION
			return _port.getc();
#else
			char c;

			boost::asio::read(_port, boost::asio::buffer(&c, 1));

			return static_cast<int>(c);
#endif
		}

		void flush ()
		{
#if defined SERIAL
			_port.flush();
#elif defined MBED_LIBRARY_VERSION
			while(_port.readable())
				_port.getc();
#else
			// TODO
#endif
		}

		int write (int i)
		{
#if defined SERIAL
			_port.print(i, BYTE);
#elif defined MBED_LIBRARY_VERSION
			_port.putc(i);
#else
			char c = static_cast<char>(i);

			boost::asio::write(_port, boost::asio::buffer(&c, 1));
#endif

			return i;
		}

	};

};

