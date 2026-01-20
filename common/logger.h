#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace openmeters::common {

/**
 * Log levels for filtering and categorization.
 */
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Fatal = 4
};

/**
 * Simple file-based logger with console output.
 * Thread-safe logging system for production use.
 * 
 * Thread safety: All logging operations are thread-safe.
 */
class Logger {
public:
    /**
     * Initialize the logger.
     * 
     * @param logFilePath Path to log file (e.g., "logs/openmeters.log")
     * @param minLevel Minimum log level to write (default: Info)
     * @param enableConsole Also write to console (default: true)
     * @return true if initialization succeeded, false otherwise
     */
    static bool initialize(
        const std::string& logFilePath,
        LogLevel minLevel = LogLevel::Info,
        bool enableConsole = true
    );
    
    /**
     * Shutdown the logger and close log file.
     */
    static void shutdown();
    
    /**
     * Log a message at the specified level.
     * 
     * @param level Log level
     * @param message Message to log
     * @param file Source file (use __FILE__)
     * @param line Source line (use __LINE__)
     */
    static void log(
        LogLevel level,
        const std::string& message,
        const char* file = nullptr,
        int line = 0
    );
    
    /**
     * Log a debug message.
     */
    static void debug(const std::string& message, const char* file = nullptr, int line = 0);
    
    /**
     * Log an info message.
     */
    static void info(const std::string& message, const char* file = nullptr, int line = 0);
    
    /**
     * Log a warning message.
     */
    static void warning(const std::string& message, const char* file = nullptr, int line = 0);
    
    /**
     * Log an error message.
     */
    static void error(const std::string& message, const char* file = nullptr, int line = 0);
    
    /**
     * Log a fatal error message.
     */
    static void fatal(const std::string& message, const char* file = nullptr, int line = 0);
    
    /**
     * Set minimum log level.
     * Messages below this level will be ignored.
     */
    static void setMinLevel(LogLevel level);
    
    /**
     * Get current minimum log level.
     */
    static LogLevel getMinLevel();

private:
    Logger() = default;
    ~Logger() = default;
    
    static void writeLog(
        LogLevel level,
        const std::string& message,
        const char* file,
        int line
    );
    
    static std::string levelToString(LogLevel level);
    static std::string getTimestamp();
    
    static std::unique_ptr<std::ofstream> s_logFile;
    static std::mutex s_logMutex;
    static LogLevel s_minLevel;
    static bool s_consoleEnabled;
    static bool s_initialized;
};

// Convenience macros
#define LOG_DEBUG(msg) openmeters::common::Logger::debug(msg, __FILE__, __LINE__)
#define LOG_INFO(msg) openmeters::common::Logger::info(msg, __FILE__, __LINE__)
#define LOG_WARNING(msg) openmeters::common::Logger::warning(msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) openmeters::common::Logger::error(msg, __FILE__, __LINE__)
#define LOG_FATAL(msg) openmeters::common::Logger::fatal(msg, __FILE__, __LINE__)

} // namespace openmeters::common

