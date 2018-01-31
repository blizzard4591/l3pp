/**
 * @file logger.h
 *
 * Defines the base basic_logger<char_t> class
 */

#pragma once

#include <vector>
#include <algorithm>
#include <map>
#include <sstream>

namespace l3pp {

namespace detail {
	/**
	 * Internal function to get all configured loggers. Should not be used
	 * directly, see basic_logger<char_t>::getbasic_logger<char_t>()
	 */
	template<typename char_t>
	static inline std::map<std::basic_string<char_t>, ptr<basic_logger<char_t>>>& GetLoggers() {
		static std::map<std::basic_string<char_t>, ptr<basic_logger<char_t>>> loggers;
		return loggers;
	}
}

template<typename char_t>
inline basic_log_stream<char_t>::~basic_log_stream<char_t>() {
	entry.msg = stream.str();
	entry.logger->logEntry(entry);
}

template<typename char_t>
inline void basic_logger<char_t>::logEntry(basic_log_entry<char_t> const& entry) const {
	for(auto& sink: sinks) {
		sink->log(entry);
	}
	if (additive && parent) {
		parent->logEntry(entry);
	}
}

template<typename char_t>
inline void basic_logger<char_t>::removeSink(sink_ptr sink) {
	typename std::vector<sink_ptr>::iterator pos = std::find(sinks.begin(), sinks.end(), sink);
	if (pos != sinks.end()) {
		sinks.erase(pos);
	}
}

template<typename char_t>
inline void basic_logger<char_t>::log(LogLevel level, std::basic_string<char_t> const& msg, log_context context) {
	if (level < getLevel()) {
		return;
	}
	logEntry(basic_log_entry<char_t>(std::move(context), this, level, msg));
}

template<typename char_t>
inline basic_log_stream<char_t> basic_logger<char_t>::log(LogLevel level, log_context context) {
	return basic_log_stream<char_t>(basic_log_entry<char_t>(std::move(context), this, level));
}

template<typename char_t>
inline typename basic_logger<char_t>::logger_ptr basic_logger<char_t>::getLogger(std::basic_string<char_t> name) {
	if (name.size() == 0) {
		// Root logger
		return getRootLogger();
	}
	auto& loggers = detail::GetLoggers<char_t>();
	auto it = loggers.find(name);
	if (it != loggers.end()) {
		return it->second;
	} else {
		auto n = name.rfind('.');
		logger_ptr parent;
		if (n == std::basic_string<char_t>::npos) {
			parent = getRootLogger();
		} else{
			parent = getLogger(name.substr(0, n));
		}
		auto newLogger = logger_ptr(new basic_logger<char_t>(name, parent));
		loggers.emplace(name, newLogger);
		return newLogger;
	}
}

template<typename char_t, typename T>
inline basic_log_stream<char_t> const& operator<<(basic_log_stream<char_t> const& stream, T const& val) {
	stream.stream << val;
	return stream;
}

template<typename char_t>
inline basic_log_stream<char_t> const& operator<<(basic_log_stream<char_t> const& stream, std::basic_ostream<char_t>& (*F)(std::basic_ostream<char_t>&)) {
	stream.stream << F;
	return stream;
}

}
