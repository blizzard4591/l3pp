= L3++: Lightweight Logging Library for C++

L3++ is a self-contained, single-header, cross-platform logging library for C++.

The main goals for this library are simplicity, modularity and ease of use.
This library is released under the MIT License.

L3++ is based on the following conceptual components:

* RecordInfo: A record info stores auxiliary information of a log message like the filename, line number and function name where the log message was emitted.
* Channel: A channel categorizes log messages, usually according to logical components or modules in the source code.
* Sink: A sink represents a logging output, for example the terminal or a log file.
* Filter: A filter is associated with a sink and decides which messages are forwarded to the sink.
* Formatter: A formatter is associated with a sink and converts a log message into an actual string.


Copyright (C) 2015 Gereon Kremer
