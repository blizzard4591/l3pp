/**
 * @file context.h
 *
 * Defines context class for log entries
 */

#pragma once

namespace l3pp {
	template<typename char_t>
	class basic_logger;

	/**
	 * Contextual information for a new log entry, contains such this as location,
	 * log info (level, logger) and the time of the event.
	 * A context will be created automatically by using the macros L3PP_LOG_*
	 */
	struct log_context {
		// Program location
		const char* filename;
		size_t line;
		const char* funcname;

		log_context() : filename(""), line(0), funcname("")
		{}

		log_context(const char* filename, size_t line, const char* funcname) :
				filename(filename), line(line), funcname(funcname)
		{}
	};

	template<typename char_t>
	struct basic_log_entry: log_context {
		// Time of entry
		std::chrono::system_clock::time_point timestamp;

		// Log event info
		basic_logger<char_t> const* logger;
		LogLevel level;

		// Log message
		std::basic_string<char_t> msg;

		basic_log_entry(log_context context, basic_logger<char_t> const* logger, LogLevel level) :
				log_context(std::move(context)),
				timestamp(std::chrono::system_clock::now()),
				logger(logger), level(level) {
		}
		basic_log_entry(log_context context, basic_logger<char_t> const* logger, LogLevel level, std::basic_string<char_t> msg) :
				log_context(std::move(context)),
				timestamp(std::chrono::system_clock::now()),
				logger(logger), level(level), msg(std::move(msg)) {
		}
	};
}
