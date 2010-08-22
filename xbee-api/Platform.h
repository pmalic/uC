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

#ifndef PLATFORM_H_
#define PLATFORM_H_

#if defined(SERIAL)
	#include <inttypes.h>
	#include <WProgram.h>
	#include <HardwareSerial.h>
#elif defined(__ARMCC_VERSION)
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
#if defined(SERIAL)
		unsigned long _start;
#elif defined(__ARMCC_VERSION)
		Timer _timer;
#else
		boost::posix_time::ptime _start;
#endif

	public:
		Stopwatch ()
		{
#if defined(SERIAL)
			_start = millis();
#elif defined(__ARMCC_VERSION)
			_timer.start();
#else
			_start = boost::posix_time::microsec_clock::universal_time();
#endif
		}

#if defined(__ARMCC_VERSION)
		~Stopwatch ()
		{
			_timer.stop();
		}
#endif

		unsigned long read ()
		{
#if defined(SERIAL)
			return millis() - _start;
#elif defined(__ARMCC_VERSION)
			return _timer.read_ms();
#else
		using namespace boost::posix_time;

		const time_duration diff = ptime(microsec_clock::universal_time()) - _start;

		return static_cast<unsigned long>(diff.total_milliseconds());
#endif
		}
	};

	class SerialPortConf
	{
	public:
#if defined(SERIAL)
		const unsigned short number;

		SerialPortConf (number = 0)
		: number(number)
		{
		}
#elif defined(__ARMCC_VERSION)
		const PinName tx, rx;

		SerialPortConf (const PinName& tx = p28, const PinName& rx = p27)
		: tx(tx), rx(rx)
		{
		}
#else
		const std::string name;

		SerialPortConf (const std::string& name = "/dev/tty.usbserial-A700eX8n")
		: name(name)
		{
		}
#endif
	};

	class SerialPort
	{
	private:
		SerialPortConf _conf;
#if defined(SERIAL)
		HardwareSerial _port;
#elif defined(__ARMCC_VERSION)
		Serial _port;
#else
		boost::asio::io_service _io;
		boost::asio::serial_port _port;
#endif

	public:
#if defined(SERIAL)
		SerialPort (const SerialPortConf& conf)
		: _conf(conf), _port(Serial)
		{
			if (conf.number)
				switch (conf.number)
				{
					case 1:
						_port = Serial1;
						break;

					case 2:
						_port = Serial2;
						break;

					case 3:
						_port = Serial3;
						break;
				}
		}
#elif defined(__ARMCC_VERSION)
		SerialPort (const SerialPortConf& conf)
		: _conf(conf), _port(Serial(conf.tx, conf.rx))
		{
		}
#else
		SerialPort (const SerialPortConf& conf)
		: _conf(conf), _io(), _port(_io, conf.name)
		{
		}

		SerialPort (const SerialPort& serialPort)
		: _conf(serialPort._conf), _io(), _port(_io, serialPort._conf.name)
		{
		}
#endif

		void begin (long speed)
		{
#if defined(SERIAL)
			_port.begin(speed);
#elif defined(__ARMCC_VERSION)
			_port.baud(speed);
#else
			_port.set_option(boost::asio::serial_port_base::baud_rate(speed));
#endif
		}

		int readable ()
		{
#if defined(SERIAL)
			return _port.available();
#elif defined(__ARMCC_VERSION)
			return _port.readable();
#else
			return 1;
#endif
		}

		int read ()
		{
#if defined(SERIAL)
			return _port.read();
#elif defined(__ARMCC_VERSION)
			return _port.getc();
#else
			char c;

			boost::asio::read(_port, boost::asio::buffer(&c, 1));

			return static_cast<int>(c);
#endif
		}

		void flush ()
		{
#if defined(SERIAL)
			_port.flush();
#elif defined(__ARMCC_VERSION)
			while(_port.readable())
				_port.getc();
#else
			// TODO
#endif
		}

		int write (int i)
		{
#if defined(SERIAL)
			_port.print(i, BYTE);
#elif defined(__ARMCC_VERSION)
			_port.putc(i);
#else
			char c = static_cast<char>(i);

			std::cerr << i << " ";

			boost::asio::write(_port, boost::asio::buffer(&c, 1));
#endif

			return i;
		}

	};

};

#endif /* PLATFORM_H_ */
