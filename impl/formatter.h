/**
 * @file formatter.h
 * @author Harold Bruintjes <h.bruintjes@cs.rwth-aachen.de>
 *
 * Implementation of Formatter classes
 */

#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstring> //strrchr

namespace l3pp {

namespace detail {
	/**
	 * Internal function to get wall-time
	 */
	inline static std::chrono::system_clock::time_point GetStartTime() {
		static std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
		return startTime;
	}
}

template<typename char_t>
inline std::basic_string<char_t> basic_formatter<char_t>::format(basic_log_entry<char_t> const& context) const {
	std::basic_stringstream<char_t> stream;
	stream << context.level << " - " << context.msg << '\n';
	return stream.str();
}

template<typename char_t, Field field, int Width, Justification j, char_t Fill>
inline void basic_field<char_t, field, Width, j, Fill>::stream(std::basic_ostream<char_t>& os, basic_log_entry<char_t> const& context) const {
	os << std::setw(Width);
	os << std::setfill(Fill);
	switch(j) {
		case Justification::LEFT:
			os << std::left;
		case Justification::RIGHT:
			os << std::right;
	}

	switch(field) {
		case Field::FileName:
#ifdef _WIN32
			os << strrchr(context.filename, '\\')+1;
#else
			os << strrchr(context.filename, '/')+1;
#endif
			break;
		case Field::FilePath:
			os << context.filename;
			break;
		case Field::Line:
			os << context.line;
			break;
		case Field::Function:
			os << context.funcname;
			break;
		case Field::LoggerName:
			os << context.logger->getName();
			break;
		case Field::Message:
			os << context.msg;
			break;
		case Field::LogLevel:
			os << context.level;
			break;
		case Field::WallTime:
			auto runtime = context.timestamp - detail::GetStartTime();
			os << std::chrono::duration_cast<std::chrono::milliseconds>(runtime).count();
			break;
	}
}

template<typename char_t>
inline void basic_time_field<char_t>::stream(std::basic_ostream<char_t>& os, basic_log_entry<char_t> const& context) const {
	auto time = std::chrono::system_clock::to_time_t(context.timestamp);
	auto timeinfo = localtime (&time);
	os << std::put_time(timeinfo, formatStr.c_str());
}

template<typename char_t, typename ... Formatters>
inline std::basic_string<char_t> basic_template_formatter<char_t, Formatters...>::format(basic_log_entry<char_t> const& context) const {
	std::basic_stringstream<char_t> stream;

	formatTuple<0>(context, stream);

	return stream.str();
}

}
