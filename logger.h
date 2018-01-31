/**
 * @file logger.h
 *
 * Defines the base Logger class
 */

#pragma once

#include <vector>
#include <sstream>

namespace l3pp {

/**
 * LogStream is a logger object that can be streamed into, writing an entry
 * to the logger associated upon destruction. Instances of this classer are
 * returned by Logger log() functions, so they can be used as such:
 * logger->debug() << "Message";
 */
template<typename char_t>
class basic_log_stream {
	friend class basic_logger<char_t>;

	basic_log_entry<char_t> entry;
	mutable std::basic_ostringstream<char_t> stream;

	basic_log_stream(basic_log_entry<char_t> entry) :
		entry(std::move(entry))
	{
	}

public:
	basic_log_stream(basic_log_stream const&) = default;
	basic_log_stream(basic_log_stream&&) = default;

	basic_log_stream& operator=(const basic_log_stream&) = default;
	basic_log_stream& operator=(basic_log_stream&&) = default;

	~basic_log_stream();

	template<typename char_tt, typename T>
	friend basic_log_stream<char_tt> const& operator<<(basic_log_stream<char_tt> const& stream, T const& val);
	template<typename char_tt>
	friend basic_log_stream<char_tt> const& operator<<(basic_log_stream<char_tt> const& stream, std::basic_ostream<char_t>& (*F)(std::basic_ostream<char_t>&));
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
template<typename char_t>
class basic_logger {
	friend class basic_log_stream<char_t>;

	typedef ptr<basic_logger<char_t>> logger_ptr;
	typedef ptr<basic_sink<char_t>> sink_ptr;

	logger_ptr parent;
	std::basic_string<char_t> name;
	LogLevel level;
	std::vector<sink_ptr> sinks;
	bool additive;

	// Logger constructors are private
	basic_logger() : parent(nullptr), name(), level(LogLevel::DEFAULT),
		additive(true)
	{

	}

	basic_logger(std::basic_string<char_t> const& name, logger_ptr parent) : parent(parent), name(name),
		level(LogLevel::INHERIT), additive(true)
	{
	}

	void logEntry(basic_log_entry<char_t> const& entry) const;

public:
	void addSink(sink_ptr sink) {
		sinks.push_back(sink);
	}

	void removeSink(sink_ptr sink);

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

	auto const& getName() const {
		return name;
	}

	bool getAdditive() const {
		return additive;
	}

	void setAdditive(bool additive) {
		this->additive = additive;
	}

	void log(LogLevel level, std::basic_string<char_t> const& msg, log_context context = log_context());

	void trace(std::basic_string<char_t> const& msg, log_context context = log_context()) {
		log(LogLevel::TRACE, msg, context);
	}
	void debug(std::basic_string<char_t> const& msg, log_context context = log_context()) {
		log(LogLevel::DEBUG, msg, context);
	}
	void info(std::basic_string<char_t> const& msg, log_context context = log_context()) {
		log(LogLevel::INFO, msg, context);
	}
	void warn(std::basic_string<char_t> const& msg, log_context context = log_context()) {
		log(LogLevel::WARN, msg, context);
	}
	void error(std::basic_string<char_t> const& msg, log_context context = log_context()) {
		log(LogLevel::ERR, msg, context);
	}
	void fatal(std::basic_string<char_t> const& msg, log_context context = log_context()) {
		log(LogLevel::FATAL, msg, context);
	}

	basic_log_stream<char_t> log(LogLevel level, log_context context = log_context());

	auto trace(log_context context = log_context()) {
		return log(LogLevel::TRACE, context);
	}
	auto debug(log_context context = log_context()) {
		return log(LogLevel::DEBUG, context);
	}
	auto info(log_context context = log_context()) {
		return log(LogLevel::INFO, context);
	}
	auto warn(log_context context = log_context()) {
		return log(LogLevel::WARN, context);
	}
	auto error(log_context context = log_context()) {
		return log(LogLevel::ERR, context);
	}
	auto fatal(log_context context = log_context()) {
		return log(LogLevel::FATAL, context);
	}

	static auto getRootLogger() {
		static auto root_logger = logger_ptr(new basic_logger<char_t>());
		return root_logger;
	}

	static logger_ptr getLogger(std::basic_string<char_t> name);
};

typedef basic_logger<char> logger;
typedef basic_logger<wchar_t> wlogger;

typedef ptr<logger> logger_ptr;
typedef ptr<wlogger> wlogger_ptr;

}

