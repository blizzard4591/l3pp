/**
 * @file sink.h
 *
 * Defines classes for log sinks (i.e. outputs)
 */

#pragma once

#include <ostream>
#include <fstream>

namespace l3pp {

/**
 * Base class for a logging sink. It can only log some log entry to which some
 * formatting is applied (see Formatter).
 */
template<typename char_t>
class basic_sink {
public:
	typedef std::shared_ptr<basic_formatter<char_t>> formatter_ptr;

private:
	formatter_ptr formatter;

public:
	basic_sink() : formatter(std::make_shared<basic_formatter<char_t>>()) {

	}
	basic_sink(formatter_ptr formatter) : formatter(formatter) {

	}
	/**
	 * Default destructor.
     */
	virtual ~basic_sink() {}

	formatter_ptr getFormatter() const {
		return formatter;
	}

	void setFormatter(formatter_ptr formatter) {
		this->formatter = formatter;
	}

	std::basic_string<char_t> formatMessage(basic_log_entry<char_t> const& context) const {
		return (*formatter)(context);
	}

	/**
	 * Logs the given message with context info
	 */
	virtual void log(basic_log_entry<char_t> const& context) const = 0;
};

/**
 * Logging sink that wraps an arbitrary `std::basic_ostream<char_t>`.
 * It is meant to be used for streams like `std::cout` or `std::cerr`.
 * A StreamSink may be given a log level, which filters out all entries
 * below that level. By default is logs all entries.
 */
template<typename char_t>
class basic_stream_sink: public basic_sink<char_t> {
	typedef std::shared_ptr<basic_stream_sink<char_t>> sink_ptr;

	/// Filtered loglevel
	LogLevel level;
	/// Output stream.
	mutable std::unique_ptr<std::basic_ostream<char_t>> os;

	LogLevel getLevel() const {
		return level;
	}

	void setLevel(LogLevel level) {
		this->level = level;
	}

	explicit basic_stream_sink(std::basic_ostream<char_t>& _os) :
			level(LogLevel::ALL),
			os(new std::basic_ostream<char_t>(_os.rdbuf())) {}

	explicit basic_stream_sink(std::basic_string<char_t> const& filename) :
			level(LogLevel::ALL),
			os(new std::basic_ofstream<char_t>(filename, std::ios::out)) {}

public:
	void log(basic_log_entry<char_t> const& context) const override {
		if (context.level >= this->level) {
			*os << this->formatMessage(context) << std::flush;
		}
	}

	/**
	 * Create a StreamSink from some output stream.
     * @param os Output stream.
     */
	static auto create(std::basic_ostream<char_t>& os) {
		return sink_ptr(new basic_stream_sink<char_t>(os));
	}

	/**
	 * Create a StreamSink from some output file.
     * @param filename Filename for output file.
     */
	static auto create(std::basic_string<char_t> const& filename) {
		return sink_ptr(new basic_stream_sink<char_t>(filename));
	}
};

typedef basic_stream_sink<char> stream_sink;

}

