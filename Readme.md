L3++: Lightweight Logging Library for C++
=====

L3++ is a self-contained, single-header, cross-platform logging library for C++.

The main goals for this library are simplicity, modularity and ease of use.
This library is released under the MIT License.

Copyright (C) 2015 Gereon Kremer


Concepts
=====

L3++ is based on the following conceptual components:

* RecordInfo: A record info stores auxiliary information of a log message like the filename, line number and function name where the log message was emitted.
* Channel: A channel categorizes log messages, usually according to logical components or modules in the source code.
* Sink: A sink represents a logging output, for example the terminal or a log file.
* Filter: A filter is associated with a sink and decides which messages are forwarded to the sink.
* Formatter: A formatter is associated with a sink and converts a log message into an actual string.


Log levels
-----
The following log levels exist:
* LVL_ALL
* LVL_TRACE
* LVL_DEBUG
* LVL_INFO
* LVL_WARN
* LVL_ERROR
* LVL_FATAL
* LVL_OFF

Hierarchical Channels
-----
Channels are hierarchically structured strings like `"app.module.submodule"`.
In this example, `submodule` is considered a subchannel of `module` and `app` the parent of `module`.
A subchannel implicitly inherits all filter rules unless a filter rule is explicitly specified.


Filter rules
-----
A filter object is given the channel and the level of a log message.
Based on these information, it decides whether a log message is actually sent to the respective sink.

A filter consists of an arbitrary number rules that map a channel to a minimal log level.
If the log level of a message is at least the one stored for this channel, it gets passed to the sink.
If no rule exists for the respective channel, the filter looks for the parent channel.
Every filter object defines a rule for the empty channel `""` as a safe fallback.



Basic Usage
=====

The logger object itself can be accessed via `l3pp::logger()`.
It provides two methods that are important for configuration:
* `configure()`: Configures sinks.
* `filter()`: Retrieves filter objects.

`configure()` comes in different flavours, depending on the type of sink you want to create. The first argument is always a string that identifies this sink.
* `std::string, std::shared_ptr<Sink>`: Creates an arbitrary sink.
* `std::string, std::string`: Creates a FileSink using the second argument as file name.
* `std::string, std::ostream`: Creates a StreamSink.

`filter()` expects a sink identifier as single argument and returns a reference to the respective filter object.
Note that the sink must be created beforehand by using `configure()`.

An initial configuration may look like this:

	l3pp::logger().configure("logfile", "demo.log");
	l3pp::logger().filter("logfile")
		("demo", l3pp::LogLevel::LVL_INFO)
		("demo.core", l3pp::LogLevel::LVL_DEBUG)
	;

In this demo, a single sink `logfile` is created that passes log messages to the file `demo.log`.
All messages in channel `demo` must have at least level `LVL_INFO` while messages in channel `demo.core` only need `LVL_DEBUG`.

The actual logging is performed using a handful of macros.
These macros


Considerations
=====

Performance
-----

While the use of hierarchical channels and multiple sinks with associated filters gives a lot of flexibility, it comes at a certain price.
As the configuration is done at runtime (and may even change at runtime), the question whether a certain message is printed can only be answered at runtime.
Therefore, every message, whether you will ever see it or not, has to pass through the logger and cost runtime.

To mitigate this, we suggest the following:

Create a preprocessor flag (like `ENABLE_LOGGING`) and define your own set of logging macros.
If this flag is defined, make your macros forward to the `__L3PP_LOG_*` macros.
If this flag is not defined, make your macros do nothing.


Multiple usages in the same project
-----
Assume you have an application that uses L3++ for logging as well as some other library that also uses L3++.
L3++ will play nicely in this scenario (partly, it was designed for this case).

However, you should take care of a few things:
* Duplicate sinks: Sink identifiers like `stdout` or `logfile` may already be defined. Use `Logger::has()` to check for this.
* Colliding channels: Prefix your channels with some unique prefix.
* Colliding macros: If you implement the aforementioned `ENABLE_LOGGING` macro, prefix your macros with your project name. Otherwise, these macros will collide.


Implementation Details
=====

Sinks
-----
A sink is a class that provides some `std::ostream`.
Any class that inherits from `l3pp::Sink` can be used.

As of now, two implementations are available: 
* FileSink: Writes to a output file.
* StreamSink: Writes to any given `std::ostream`, for example to `std::cout`.
