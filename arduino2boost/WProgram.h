/*
 * WProgram.h
 *
 *  Created on: Aug 15, 2010
 *      Author: pm
 */

#ifndef WPROGRAM_H_
#define WPROGRAM_H_

#include <boost/date_time/posix_time/posix_time.hpp>

enum PrintFormat
{
	BYTE = 1u
, BIN
, OCT
, DEC
, HEX
};

unsigned long millis ()
{
	using namespace boost::posix_time;

	static const ptime start(microsec_clock::universal_time());

	const time_duration diff = ptime(microsec_clock::universal_time()) - start;

	return static_cast<unsigned long>(diff.total_milliseconds());
}

#endif /* WPROGRAM_H_ */
