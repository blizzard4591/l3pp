/**
 * @file l3pp.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * @author Harold Bruintjes <h.bruintjes@cs.rwth-aachen.de>
 *
 * The lightweight logging library for C++.
 *
 * This logging facility is fairly generic and is used as a simple and
 * header-only alternative to more advanced solutions like log4cplus or
 * boost::log.
 *
 * The basic components are Sinks, Formatters and Loggers.
 *
 * A Sink represents a logging output like a terminal or a log file.
 * This implementation provides a FileSink and a StreamSink, but the basic
 * Sink class can be extended as necessary.
 *
 * A Formatter is associated with a Sink and produces the actual string that is
 * sent to the Sink.
 * Usually, it adds auxiliary information like the current time, LogLevel and
 * source location to the string logged by the user.
 * The Formatter implements a reasonable default behavior for simple logging,
 * but it can be subclassed and modified as necessary.
 *
 * The Logger class finally plugs all these components together.
 * Individual loggers can log to one or more sinks (inheritable) and are
 * associated with an (inheritable) log level.
 *
 * Initial configuration may look like this:
 * @code{.cpp}
 * l3pp::Logger::initialize();
 * l3pp::SinkPtr sink = l3pp::StreamSink::create(std::clog);
 * l3pp::Logger::getRootLogger()->addSink(sink);
 * l3pp::Logger::getRootLogger()->setLevel(l3pp::LogLevel::INFO);
 * @endcode
 * 
 * Macro facilitate the usage:
 * <ul>
 * <li>`L3PP_LOG_<LVL>(logger, msg)` produces a normal log message where
 * logger should be string identifying the logger (or a LogPtr) and msg is the
 * message to be logged.</li>
 * </ul>
 * Any message (`msg`) can be an arbitrary expression that one would
 * stream to an `std::ostream` like `stream << (msg);`. The default formatter
 * adds newlines.
 * Manipulators are generally supported. However, for performance avoid std::endl
 * and use '\n' directly.
 */

#pragma once

#include <chrono>
#include <memory>

namespace l3pp {

	/**
	 * Indicates which log messages should be forwarded to some sink.
	 *
	 * All messages which have a level that is equal or greater than the specified
	 * value will be forwarded.
	 */
	enum class LogLevel {
		/// Log messages used for tracing the program flow in detail.
		TRACE,
		/// Log messages used for debugging.
		DEBUG,
		/// Log messages used for information.
		INFO,
		/// Log messages used to warn about an undesired state.
		WARN,
		/// Log messages used for errors that can be handled.
		ERR,
		/// Log messages used for errors that lead to program termination.
		FATAL,
		/// Log no messages.
		OFF,
		/// Parent level
		INHERIT,
		/// Default log level.
		DEFAULT = WARN,
		/// All log messages.
		ALL = TRACE
	};

	/**
	 * Streaming operator for LogLevel.
	 * @param os Output stream.
	 * @param level LogLevel.
	 * @return os.
	 */
	template<typename char_t>
	inline std::basic_ostream<char_t>& operator<<(std::basic_ostream<char_t>& os, LogLevel level);

	template<typename T>
	using ptr = std::shared_ptr<T>;

	class Logger;
}

#include "context.h"
#include "formatter.h"
#include "sink.h"
#include "logger.h"

#include "impl/logging.h"
#include "impl/logger.h"
#include "impl/formatter.h"

namespace l3pp {
	template<typename char_t = char>
	inline std::shared_ptr<basic_logger<char_t>> getRootLogger() {
		return basic_logger<char_t>::getRootLogger();
	}

	template<typename char_t>
	inline auto getLogger(std::basic_string<char_t> name) {
		return basic_logger<char_t>::getLogger(name);
	}

	template<typename char_t>
	inline auto getLogger(std::shared_ptr<basic_logger<char_t>> logger) {
		return logger;
	}

	inline void initialize() {
		// Setup root logger
		//getRootLogger<char>();
		// Set wall time
		detail::GetStartTime();
	}

	inline void deinitialize() {
		//detail::GetLoggers<char_t>().clear();
		//getRootLogger<char_t>()->sinks.clear();
	}

	/**
	 * Helper class to initialize l3pp. Call get() will
	 * retrieve the singleton, which will initialize the
	 * library.
	 */
	class Initializer {
		Initializer() {
			initialize();
		}

	public:
		~Initializer() {
			deinitialize();
		}

		static Initializer const& get(){
			static Initializer instance;
			return instance;
		}
	};
}

#ifdef _MSC_VER
#define L3PP_FUNCTION __FUNCTION__
#else
#define L3PP_FUNCTION __func__
#endif

/// Create a record info.
#define L3PP_LOG_CONTEXT l3pp::log_context(__FILE__, __LINE__, L3PP_FUNCTION)
/// Basic logging macro.
#define _L3PP_LOG(level, channel, expr) do { \
    auto L3PP_channel = ::l3pp::getLogger(channel); \
    if (L3PP_channel->getLevel() <= level) { \
        L3PP_channel->log(level, L3PP_LOG_CONTEXT) << expr; \
    } \
} while(false)

/// Log with level TRACE.
#define L3PP_LOG_TRACE(channel, expr) _L3PP_LOG(::l3pp::LogLevel::TRACE, channel, expr)
/// Log with level DEBUG.
#define L3PP_LOG_DEBUG(channel, expr) _L3PP_LOG(::l3pp::LogLevel::DEBUG, channel, expr)
/// Log with level INFO.
#define L3PP_LOG_INFO(channel, expr) _L3PP_LOG(::l3pp::LogLevel::INFO, channel, expr)
/// Log with level WARN.
#define L3PP_LOG_WARN(channel, expr) _L3PP_LOG(::l3pp::LogLevel::WARN, channel, expr)
/// Log with level ERROR.
#define L3PP_LOG_ERROR(channel, expr) _L3PP_LOG(::l3pp::LogLevel::ERR, channel, expr)
/// Log with level FATAL.
#define L3PP_LOG_FATAL(channel, expr) _L3PP_LOG(::l3pp::LogLevel::FATAL, channel, expr)
