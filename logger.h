/**
 * @file logging.h
 *
 * Defines the base Logger class
 */

#pragma once

#include "logging.h"
#include "sink.h"

#include <vector>
#include <sstream>

namespace l3pp {

/**
 * LogStream is a logger object that can be streamed into, writing an entry
 * to the logger associated upon destruction. Instances of this classer are
 * returned by Logger log() functions, so they can be used as such:
 * logger->debug() << "Message";
 */
class LogStream {
	friend class Logger;

	Logger& logger;
	LogLevel level;
	mutable EntryContext context;
	mutable std::ostringstream stream;
	mutable bool has_context;

	LogStream(Logger& logger, LogLevel level) :
		logger(logger), level(level), has_context(false)
	{
	}

	LogStream(const LogStream&) = delete;
	LogStream& operator=(const LogStream&) = delete;
public:
	LogStream(LogStream&& other) :
		logger(other.logger), level(other.level), context(std::move(other.context)),
		stream(std::move(other.stream)), has_context(other.has_context)
	{
	}
	~LogStream();

	friend LogStream const& operator<<(LogStream const& stream, EntryContext context);

	template<typename T>
	friend LogStream const& operator<<(LogStream const& stream, T const& val);
	friend LogStream const& operator<<(LogStream const& stream, std::ostream& (*F)(std::ostream&));
};

/**
 * Main logger class. Keeps track of all Logger instances, and can be used to
 * log various messages. Before the logging library is used, make sure to
 * call Logger::initialize(). Loggers are hierarchically nested, by means of
 * names separated by a period. All loggers are a (indirect) child of the root
 * logger, see Logger::getRootLogger() and Logger::getLogger().
 * A logger is associated with a LogLevel. Any entry with a level below this
 * level will be filtered out. A LogLevel of INHERIT means the parent log
 * level will be compared against instead.
 * A logger can be associated with 1 or more Sinks. A log entry is printed to
 * each associated sink. If the Logger is set additive (see getAdditive(),
 * setAdditive()) parent sinks are logged to as well (by default true).
 * Logging can be performed either as a single string message, or by using a
 * stream. The latter requires the end() method to be called before the entry
 * is logged. For convenience, various logging macros are defined at the end
 * of this header.
 */
class Logger {
	friend class Formatter;

	typedef std::shared_ptr<Logger> LogPtr;

	LogPtr parent;
	std::string name;
	LogLevel level;
	std::vector<SinkPtr> sinks;
	bool additive;

	// Logger constructors are private
	Logger() : parent(nullptr), name(""), level(LogLevel::DEFAULT),
		additive(true)
	{

	}

	Logger(std::string const& name, LogPtr parent) : parent(parent), name(name),
		level(LogLevel::INHERIT), additive(true)
	{
	}

	void logEntry(EntryContext const& context, std::string const& msg);

public:
	void addSink(SinkPtr sink) {
		sinks.push_back(sink);
	}

	void removeSink(SinkPtr sink);

	void setLevel(LogLevel level) {
		if (level == LogLevel::INHERIT && !parent) {
			return;
		}
		this->level = level;
	}

	LogLevel getLevel() const {
		if (level == LogLevel::INHERIT) {
			return parent->getLevel();
		}
		return level;
	}

	std::string const& getName() const {
		return name;
	}

	bool getAdditive() const {
		return additive;
	}

	void setAdditive(bool additive) {
		this->additive = additive;
	}

	void log(LogLevel level, std::string const& msg);
	void log(LogLevel level, std::string const& msg, EntryContext context);

	void trace(std::string const& msg) {
		log(LogLevel::TRACE, msg);
	}
	void debug(std::string const& msg) {
		log(LogLevel::DEBUG, msg);
	}
	void info(std::string const& msg) {
		log(LogLevel::INFO, msg);
	}
	void warn(std::string const& msg) {
		log(LogLevel::WARN, msg);
	}
	void error(std::string const& msg) {
		log(LogLevel::ERROR, msg);
	}
	void fatal(std::string const& msg) {
		log(LogLevel::FATAL, msg);
	}

	LogStream log(LogLevel level);

	LogStream trace() {
		return log(LogLevel::TRACE);
	}
	LogStream debug() {
		return log(LogLevel::DEBUG);
	}
	LogStream info() {
		return log(LogLevel::INFO);
	}
	LogStream warn() {
		return log(LogLevel::WARN);
	}
	LogStream error() {
		return log(LogLevel::ERROR);
	}
	LogStream fatal() {
		return log(LogLevel::FATAL);
	}

	static void initialize();
	static void deinitialize();

	static LogPtr getRootLogger();

	static LogPtr getLogger(LogPtr logger) {
		return logger;
	}

	static LogPtr getLogger(std::string name);
};
typedef std::shared_ptr<Logger> LogPtr;

}
