/**
 * @file formatter.h
 * @author Harold Bruintjes <h.bruintjes@cs.rwth-aachen.de>
 *
 * Define the Formatter class, used to format output of a sink
 */

#pragma once

#include <string>
#include <tuple>

namespace l3pp {

/**
 * Formats a log messages. This is a base class that simply print the message
 * with the log level prefix, see derived classes such as TemplatedFormatter
 * for more interesting data.
 */
template<typename char_t>
class basic_formatter {
	friend class basic_logger<char_t>;

	virtual std::basic_string<char_t> format(basic_log_entry<char_t> const& context) const;
public:
	virtual ~basic_formatter() {}

	std::basic_string<char_t> operator()(basic_log_entry<char_t> const& context) {
		return format(context);
	}
};

/**
 * Possible fields for FieldStr instance
 */
enum class Field {
	/// Name of the file (everything following the last path separator)
	FileName,
	/// Full path of the file
	FilePath,
	/// Line number
	Line,
	/// Name of function currently executed
	Function,
	/// Name of the logger
	LoggerName,
	/// Message to be logged
	Message,
	/// Level of the log entry
	LogLevel,
	/// Number of milliseconds since the logger was initialized
	WallTime,
};

/**
 * Controls justification of formatted log fields.
 */
enum class Justification {
	/// Left align field
	LEFT,
	/// Right align field
	RIGHT
};



/**
 * Formatter for log entry fields, with the exception of time stamp formatting
 * (see TimeStr for that). The Field template argument determines which field
 * is printed, see logging::Field.
 * The other template arguments control the alignment of the output string.
 */
template<typename char_t, Field field, int Width = 0, Justification j = Justification::RIGHT, char_t Fill = ' '>
class basic_field {
public:
	void stream(std::basic_ostream<char_t>& os, basic_log_entry<char_t> const& context) const;
};

/**
 * Formatter for log time stamps. The constructor expects a single string
 * argument which is a formatter for the time stamp. For the specification of
 * this format string see the documentation for std::put_time . You can use for
 * example "%c" or "%T".
 */
template<typename char_t>
class basic_time_field {
	std::basic_string<char_t> formatStr;

public:
	basic_time_field(char const* format) : formatStr(format) {
	}
	basic_time_field(std::basic_string<char_t> const& format) : formatStr(format) {
	}

	void stream(std::basic_ostream<char_t>& os, basic_log_entry<char_t> const& context) const;
};

/**
 * Formatter which formats the output based on the (templated) arguments given.
 * The arguments can be anything that implements the stream operator <<, but
 * more interestingly also the various FormatField subclasses. These classes
 * can output the various fields associated with a log entry.
 */
template<typename char_t, typename ... Formatters>
class basic_template_formatter : public basic_formatter<char_t> {
	std::tuple<Formatters...> formatters;

	template <int N>
	typename std::enable_if<N < (sizeof...(Formatters))>::type
	formatTuple(basic_log_entry<char_t> const& context, std::basic_ostream<char_t>& os) const {
		formatElement(std::get<N>(formatters), os, context);
		formatTuple<N+1>(context, os);
	}

	template <int N>
	typename std::enable_if<(N >= sizeof...(Formatters))>::type
	formatTuple(basic_log_entry<char_t> const&, std::basic_ostream<char_t>&) const {
	}

	template<Field field, int Width, Justification j, char_t Fill>
	void formatElement(basic_field<char_t, field, Width, j, Fill> const& t, std::basic_ostream<char_t>& stream, basic_log_entry<char_t> const& context) const {
		t.stream(stream, context);
	}

	void formatElement(basic_time_field<char_t> const& t, std::basic_ostream<char_t>& stream, basic_log_entry<char_t> const& context) const {
		t.stream(stream, context);
	}

	template<typename T>
	void formatElement(T const& t, std::basic_ostream<char_t>& stream, basic_log_entry<char_t> const&) const {
		stream << t;
	}
public:
	basic_template_formatter(Formatters ... formatters) :
		formatters(std::forward<Formatters>(formatters)...)
	{
	}

	std::basic_string<char_t> format(basic_log_entry<char_t> const& context) const override;
};

/**
 * Helper function to create a TemplateFormatter. Simply call with some
 * formatable arguments, e.g.,
 * @code{.cpp}
 * logging::makeTemplateFormatter(
 *     logging::FieldStr<logging::Field::LogLevel>(), " - ",
 *     logging::FieldStr<logging::Field::Message>(), "\n");
 * @endcode
 */
template<typename ... Formatters>
auto makeTemplateFormatter(Formatters&& ... formatters) {
	return std::make_shared<basic_template_formatter<char, Formatters...>>(std::forward<Formatters>(formatters)...);
}

}
