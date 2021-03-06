/*
debug.h
Copyright (C) 2013 celeron55, Perttu Ahola <celeron55@gmail.com>
*/

/*
This file is part of Freeminer.

Freeminer is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Freeminer  is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Freeminer.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DEBUG_HEADER
#define DEBUG_HEADER

#include <iostream>
#include <exception>
#include <assert.h>
#include "gettime.h"

#if (defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE))
	#define WIN32_LEAN_AND_MEAN
	#ifndef _WIN32_WINNT
		#define _WIN32_WINNT 0x0501
	#endif
	#include <windows.h>
	#ifdef _MSC_VER
		#include <eh.h>
	#endif
	#define __NORETURN __declspec(noreturn)
	#define __FUNCTION_NAME __FUNCTION__
#else
	#define __NORETURN __attribute__ ((__noreturn__))
	#define __FUNCTION_NAME __PRETTY_FUNCTION__
#endif

// Whether to catch all std::exceptions.
// When "catching", the program will abort with an error message.
// In debug mode, leave these for the debugger and don't catch them.
#ifdef NDEBUG
	#define CATCH_UNHANDLED_EXCEPTIONS 1
#else
	#define CATCH_UNHANDLED_EXCEPTIONS 0
#endif

/*
	Debug output
*/

#define DTIME (getTimestamp()+": ")

extern void debugstreams_init(bool disable_stderr, const char *filename);
extern void debugstreams_deinit();

// This is used to redirect output to /dev/null
class Nullstream : public std::ostream {
public:
	Nullstream():
		std::ostream(0)
	{
	}
private:
};

extern std::ostream dstream;
extern std::ostream dstream_no_stderr;
extern Nullstream dummyout;


/* Abort program execution immediately
 */
__NORETURN extern void fatal_error_fn(
		const char *msg, const char *file,
		unsigned int line, const char *function);

#define FATAL_ERROR(msg) \
	fatal_error_fn((msg), __FILE__, __LINE__, __FUNCTION_NAME)

#define FATAL_ERROR_IF(expr, msg) \
	((expr) \
	? fatal_error_fn((msg), __FILE__, __LINE__, __FUNCTION_NAME) \
	: (void)(0))

/*
	sanity_check()
	Equivalent to assert() but persists in Release builds (i.e. when NDEBUG is
	defined)
*/

__NORETURN extern void sanity_check_fn(
		const char *assertion, const char *file,
		unsigned int line, const char *function);

#define SANITY_CHECK(expr) \
	((expr) \
	? (void)(0) \
	: sanity_check_fn(#expr, __FILE__, __LINE__, __FUNCTION_NAME))

#define sanity_check(expr) SANITY_CHECK(expr)


void debug_set_exception_handler();

/*
	DebugStack
*/

#define DEBUG_STACK_SIZE 50
#define DEBUG_STACK_TEXT_SIZE 300

extern void debug_stacks_init();
extern void debug_stacks_print_to(std::ostream &os);
extern void debug_stacks_print();

struct DebugStack;
class DebugStacker
{
public:
	DebugStacker(const char *text);
	~DebugStacker();

private:
	DebugStack *m_stack;
	bool m_overflowed;
};

#define DSTACK(msg) \
	DebugStacker __debug_stacker(msg);

#define DSTACKF(...) \
	char __buf[DEBUG_STACK_TEXT_SIZE];                   \
	snprintf(__buf, DEBUG_STACK_TEXT_SIZE, __VA_ARGS__); \
	DebugStacker __debug_stacker(__buf);

/*
	These should be put into every thread
*/

#if CATCH_UNHANDLED_EXCEPTIONS == 1
	#define BEGIN_DEBUG_EXCEPTION_HANDLER try {
	#define END_DEBUG_EXCEPTION_HANDLER(logstream)           \
		} catch (std::exception &e) {                        \
			logstream << "An unhandled exception occurred: " \
				<< e.what() << std::endl;                    \
			FATAL_ERROR(e.what());                           \
		}
#else
	// Dummy ones
	#define BEGIN_DEBUG_EXCEPTION_HANDLER
	#define END_DEBUG_EXCEPTION_HANDLER(logstream)
#endif

#endif // DEBUG_HEADER


