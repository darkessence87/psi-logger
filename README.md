# Description

A lightweight, context-tagged logging library built on top of **psi-comm** and **psi-shared**.

Each log source identifies itself with a short context label (default `"LOGG"`, overridden by `#define LOG_CTX "MY_CTX"` before including the header). Multiple independent logger contexts can coexist in the same process — each gets its own `LoggerSingleton` instance.

Messages are transported via an IPC client/server pair (`LoggerClient` / `LoggerServer`) using shared memory. If no logger server is attached, messages are still emitted locally. A server can be a separate process that aggregates and displays logs from multiple clients.

**Log levels** (low → high): `TRACE`, `DEBUG`, `INFO`, `WARNING`, `ERROR`

**Message format** (comma-separated fields):
```
timestampId, local_time, logLevel, processId, threadName, address, fileName, fnName, line, msg
```

### Macros

**Instance methods** (capture `this` address):
```cpp
LOG_TRACE(x)   LOG_DEBUG(x)   LOG_INFO(x)   LOG_WARN(x)   LOG_ERROR(x)
```

**Static / free-function context** (address reported as 0):
```cpp
LOG_TRACE_STATIC(x)   LOG_DEBUG_STATIC(x)   LOG_INFO_STATIC(x)
LOG_WARN_STATIC(x)    LOG_ERROR_STATIC(x)
```

The `x` argument is streamed into a `std::ostringstream`, so any `operator<<`-capable value is accepted.

# Usage examples
* [1 Simple Logger](https://github.com/darkessence87/psi-logger/tree/master/psi/examples/1_Simple_Logger)