### hpp_log: Universal C++ logging helper
This is just a simple universal logging helper for cross-platform C++ applications that have to run in various environments.

It defines 4 log levels. Usage:

```
int i = 5;
logd << "This is a string " << i;
```

To set the reporting level:

```
Log().ReportingLevel() = LOG_DEBUG;
```
