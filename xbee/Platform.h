/**
 * Copyright (c) 2014 Predrag Malicevic. All rights reserved.
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

#if defined(ARDUINO)
 	#if ARDUINO >= 100
		#include <Arduino.h>
	#else
 		#include <WProgram.h>
 	#endif
	#include <inttypes.h>
	#include <HardwareSerial.h>
#elif defined(__ARMCC_VERSION)
	#include <mbed.h>
#elif defined(_LPC2100)
	#include <cstddef>
	#include <stdint.h>
	#include <targets/LPC210x.h>
#elif defined(BOOST_GCC) || defined(BOOST_MSVC)
	#include <boost/asio.hpp>
	#include <boost/bind.hpp>
	#include <boost/thread.hpp>
	#include <boost/date_time/posix_time/posix_time.hpp>
#endif

class Stopwatch
{
private:
#if defined(ARDUINO)
	unsigned long _start;
#elif defined(__ARMCC_VERSION)
	Timer _timer;
#elif defined(_LPC2100)
	// nothing
#elif defined(BOOST_GCC) || defined(BOOST_MSVC)
	boost::posix_time::ptime _start;
#endif

public:
	Stopwatch ()
	{
#if defined(ARDUINO)
		_start = millis();
#elif defined(__ARMCC_VERSION)
		_timer.start();
#elif defined(_LPC2100)
		T0PR = 58982 / VPBDIV - 1; // prescale divider
		T0TCR = 3; // reset counter
		T0IR = 0xff; // clear interrupts
		T0TCR = 1; // start counting
#elif defined(BOOST_GCC) || defined(BOOST_MSVC)
		_start = boost::posix_time::microsec_clock::universal_time();
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
#if defined(ARDUINO)
		return millis() - _start;
#elif defined(__ARMCC_VERSION)
		return _timer.read_ms();
#elif defined(_LPC2100)
		return T0TC;
#elif defined(BOOST_GCC) || defined(BOOST_MSVC)
		using namespace boost::posix_time;

		const time_duration diff = ptime(microsec_clock::universal_time()) - _start;

		return static_cast<unsigned long>(diff.total_milliseconds());
#endif
	}
};

#if defined(ARDUINO)
	#define STREAM Stream
#else
class STREAM
{
public:
	virtual ~STREAM () {}

	virtual int available () = 0;
	virtual int read () = 0;
	virtual void flush () = 0;

	virtual void write (uint8_t) = 0; // from Print
};
#endif

#if !defined(ARDUINO)
class SERIAL : public STREAM
{
private:
#if defined(__ARMCC_VERSION)
	Serial _port;
#elif defined(_LPC2100)
	const unsigned short _number;
#elif defined(BOOST_GCC) || defined(BOOST_MSVC)
	const std::string _name;
	boost::asio::io_service _io;
	boost::asio::serial_port _port;

	boost::thread _rdthr;
	char _rdbuf[128];

	boost::mutex _rdmtx;
	std::vector<char> _rdque;

	void _start ()
	{
		using namespace boost;

		_io.post(bind(&SERIAL::_read, this));

		thread t(bind(&asio::io_service::run, &_io));

		_rdthr.swap(t);
	}

	void _read ()
	{
		using namespace boost;

		_port.async_read_some(asio::buffer(_rdbuf, 128), bind(&SERIAL::_read_done, this, asio::placeholders::error, asio::placeholders::bytes_transferred));
	}

	void _read_done (const boost::system::error_code& error, size_t bytes_transferred)
	{
		using namespace boost;

		if (error)
			this_thread::sleep(posix_time::milliseconds(10));
		else
		{
			lock_guard<mutex> lck(_rdmtx);

			_rdque.insert(_rdque.end(), _rdbuf, _rdbuf + bytes_transferred);
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
#endif

public:
#if defined(__ARMCC_VERSION)
	SERIAL (const PinName& tx = p28, const PinName& rx = p27)
    : _port(Serial(tx, rx))
	{
	}
#elif defined(_LPC2100)
	SERIAL (const unsigned short number = 1)
    : _number(number)
	{
	}
#elif defined(BOOST_GCC) || defined(BOOST_MSVC)
	SERIAL (const std::string& name)
    : _name(name)
    , _io()
    , _port(_io, name)
	{
		_start();
	}

	SERIAL (const SERIAL& serial)
    : _name(serial._name)
    , _io()
    , _port(_io, serial._name)
	{
		_start();
	}

	~SERIAL ()
	{
		_io.post(boost::bind(&SERIAL::_close, this));

		_rdthr.join();
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
#elif defined(BOOST_GCC) || defined(BOOST_MSVC)
		_port.set_option(boost::asio::serial_port_base::baud_rate(speed));
#endif
	}

	virtual int available ()
	{
#if defined(__ARMCC_VERSION)
		return _port.readable();
#elif defined(_LPC2100)
		return U1LSR & 0x01;
#elif defined(BOOST_GCC) || defined(BOOST_MSVC)
		boost::lock_guard<boost::mutex> lck(_rdmtx);

		return static_cast<int>(_rdque.size());
#endif
	}

	virtual int read ()
	{
#if defined(__ARMCC_VERSION)
		return _port.getc();
#elif defined(_LPC2100)
		while (!(U1LSR & 0x01));

		return static_cast<int>(U1RBR);
#elif defined(BOOST_GCC) || defined(BOOST_MSVC)
		boost::lock_guard<boost::mutex> lck(_rdmtx);

		if (_rdque.empty())
			return -1;

		char c = _rdque.front();

		_rdque.erase(_rdque.begin());

		return c;
#endif
	}

	virtual void flush ()
	{
#if defined(__ARMCC_VERSION)
		while(_port.readable())
			_port.getc();
#elif defined(_LPC2100)
		// TODO
#elif defined(BOOST_GCC) || defined(BOOST_MSVC)
		boost::lock_guard<boost::mutex> lck(_rdmtx);

		_rdque.clear();
#endif
	}

	virtual void write (uint8_t i)
	{
#if defined(__ARMCC_VERSION)
		_port.putc(i);
#elif defined(_LPC2100)
		while (!(U1LSR & 0x20));

		U1THR = static_cast<char>(i);
#elif defined(BOOST_GCC) || defined(BOOST_MSVC)
		char c = static_cast<char>(i);

		boost::asio::write(_port, boost::asio::buffer(&c, 1));
#endif
	}
};
#endif

#endif // PLATFORM_H_
