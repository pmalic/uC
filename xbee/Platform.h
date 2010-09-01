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

#if defined(__ARMCC_VERSION)
	#include <mbed.h>
#elif defined(_LPC2100)
	#include <cstddef>
	#include <stdint.h>
	#include <targets/LPC210x.h>
#elif defined(_BOOST)
	#include <boost/asio.hpp>
	#include <boost/bind.hpp>
	#include <boost/thread.hpp>
	#include <boost/date_time/posix_time/posix_time.hpp>
#else
	#include <inttypes.h>
	#include <WProgram.h>
	#include <HardwareSerial.h>
#endif

class Platform
{
public:
	class Stopwatch
	{
	private:
#if defined(__ARMCC_VERSION)
		Timer _timer;
#elif defined(_LPC2100)
		// nothing
#elif defined(_BOOST)
		boost::posix_time::ptime _start;
#else
		unsigned long _start;
#endif

	public:
		Stopwatch ()
		{
#if defined(__ARMCC_VERSION)
			_timer.start();
#elif defined(_LPC2100)
		  T0PR = 58982 / VPBDIV - 1; // prescale divider
		  T0TCR = 3; // reset counter
		  T0IR = 0xff; // clear interrupts
		  T0TCR = 1; // start counting
#elif defined(_BOOST)
			_start = boost::posix_time::microsec_clock::universal_time();
#else
			_start = millis();
#endif
		}

		~Stopwatch ()
		{
#if defined(__ARMCC_VERSION)
			_timer.stop();
#elif defined(_LPC2100)
			T0TCR = 2;
#endif
		}

		unsigned long read ()
		{
#if defined(__ARMCC_VERSION)
			return _timer.read_ms();
#elif defined(_LPC2100)
			return T0TC;
#elif defined(_BOOST)
			using namespace boost::posix_time;

			const time_duration diff = ptime(microsec_clock::universal_time()) - _start;

			return static_cast<unsigned long>(diff.total_milliseconds());
#else
			return millis() - _start;
#endif
		}
	};

	class SerialPortConf
	{
	public:
#if defined(__ARMCC_VERSION)
		const PinName tx, rx;

		SerialPortConf (const PinName& tx = p28, const PinName& rx = p27)
		: tx(tx), rx(rx)
		{
		}
#elif defined(_LPC2100)
		const unsigned short number;

		SerialPortConf (const unsigned short number = 1)
		: number(number)
		{
		}
#elif defined(_BOOST)
		const std::string name;

		SerialPortConf (const std::string& name = "/dev/tty.usbserial-A700eX8n")
		: name(name)
		{
		}
#else
		const unsigned short number;

		SerialPortConf (const unsigned short number = 1)
		: number(number)
		{
		}
#endif
	};

	class SerialPort
	{
	private:
		SerialPortConf _conf;
#if defined(__ARMCC_VERSION)
		Serial _port;
#elif defined(_LPC2100)
		// nothing
#elif defined(_BOOST)
		boost::asio::io_service _io;
		boost::asio::serial_port _port;

		boost::thread _thread;
		size_t _readable;
		char _rdbuf[128];
		unsigned short _rdbuf_pos;

		void _start ()
		{
			using namespace boost;

			_io.post(bind(&SerialPort::_read, this));

			thread t(bind(&asio::io_service::run, &_io));
			_thread.swap(t);
		}

		void _read ()
		{
			using namespace boost;

			_port.async_read_some(asio::buffer(_rdbuf, 128), bind(&SerialPort::_read_done, this, asio::placeholders::error, asio::placeholders::bytes_transferred));
		}

		void _read_done (const boost::system::error_code& error, size_t bytes_transferred)
		{
			if (!error)
			{
				_readable = bytes_transferred;
				_rdbuf_pos = 0;
			}

			if (_port.is_open())
				_read();
		}

		void _close ()
		{
			if (_port.is_open())
				_port.close();

			_io.reset();
		}
#else
		HardwareSerial _port;
#endif

	public:
#if defined(__ARMCC_VERSION)
		SerialPort (const SerialPortConf& conf)
		: _conf(conf), _port(Serial(conf.tx, conf.rx))
		{
		}
#elif defined(_LPC2100)
		SerialPort (const SerialPortConf& conf)
		: _conf(conf)
		{
		}
#elif defined(_BOOST)
		SerialPort (const SerialPortConf& conf)
		: _conf(conf), _io(), _port(_io, conf.name), _readable(0), _rdbuf_pos(0)
		{
			_start();
		}

		SerialPort (const SerialPort& serialPort)
		: _conf(serialPort._conf), _io(), _port(_io, serialPort._conf.name), _readable(0), _rdbuf_pos(0)
		{
			_start();
		}
#else
		SerialPort (const SerialPortConf& conf)
		: _conf(conf), _port(Serial)
		{
#if defined(USE_MEGA)
			switch (conf.number)
			{
				case 2:
					_port = Serial2;
					break;

				case 3:
					_port = Serial3;
					break;

				default:
					_port = Serial1;
					break;
			}
#else
			_port = Serial;
#endif
		}
#endif

#if defined(_BOOST)
		~SerialPort ()
		{
			_io.post(boost::bind(&SerialPort::_close, this));

			_thread.join();
		}
#endif

		void begin (long speed)
		{
#if defined(__ARMCC_VERSION)
			_port.baud(speed);
#elif defined(_LPC2100)
			unsigned int divider = VPBDIV & 3;

			if (divider == 0)
				divider = 4;

		  const unsigned int divisor = OSCILLATOR_CLOCK_FREQUENCY * (PLLCON & 1 ? (PLLCFG & 0xF) + 1 : 1) / divider / (16 * speed);

		  PINSEL0 = (1 << 0x12) | (1 << 0x10);
		  U1LCR = 0x83; // 8 bit, 1 stop bit, no parity, enable DLAB
		  U1DLL = divisor & 0xff;
		  U1DLM = (divisor >> 8) & 0xff;
		  U1LCR &= ~0x80; // disable DLAB
		  U1FCR = 1;
#elif defined(_BOOST)
			_port.set_option(boost::asio::serial_port_base::baud_rate(speed));
#else
			_port.begin(speed);
#endif
		}

		int readable ()
		{
#if defined(__ARMCC_VERSION)
			return _port.readable();
#elif defined(_LPC2100)
			return U1LSR & 0x01;
#elif defined(_BOOST)
			return static_cast<int>(_readable);
#else
			return _port.available();
#endif
		}

		int read ()
		{
#if defined(__ARMCC_VERSION)
			return _port.getc();
#elif defined(_LPC2100)
			while (!(U1LSR & 0x01));

			return static_cast<int>(U1RBR);
#elif defined(_BOOST)
			if (_readable)
			{
				--_readable;

				return static_cast<int>(_rdbuf[_rdbuf_pos++]);
			}
			else
				return -1;
#else
			return _port.read();
#endif
		}

		void flush ()
		{
#if defined(__ARMCC_VERSION)
			while(_port.readable())
				_port.getc();
#elif defined(_LPC2100)
			// TODO
#elif defined(_BOOST)
			_readable = 0;
#else
			_port.flush();
#endif
		}

		int write (int i)
		{
#if defined(__ARMCC_VERSION)
			_port.putc(i);
#elif defined(_LPC2100)
			while (!(U1LSR & 0x20));

			U1THR = static_cast<char>(i);
#elif defined(_BOOST)
			char c = static_cast<char>(i);

			boost::asio::write(_port, boost::asio::buffer(&c, 1));
#else
			_port.print(i, BYTE);
#endif

			return i;
		}

	};

};

#endif /* PLATFORM_H_ */
