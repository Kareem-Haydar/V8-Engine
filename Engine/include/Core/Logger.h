#pragma once

#include <condition_variable>
#include <iostream>
#include <fstream>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>

#include <fmt/core.h>

#if defined(_MSC_VER)
  #define FUNC_NAME __FUNCSIG__
#elif defined(__GNUC__) || defined(__clang__)
  #define FUNC_NAME __PRETTY_FUNCTION__
#else
  #define FUNC_NAME __func__
#endif

static std::string_view TrimArgsFromFunctionName(std::string_view prettyFunc) {
    size_t parenPos = prettyFunc.find('(');
    if (parenPos != std::string_view::npos) {
      prettyFunc = prettyFunc.substr(0, parenPos); 
    }

    size_t lastSpace = prettyFunc.rfind(' ');
    if (lastSpace != std::string_view::npos) {
        prettyFunc = prettyFunc.substr(lastSpace + 1);
    }

    return prettyFunc;
}

class V8_Logger {
  public:
    enum class LogLevel {
      Debug,
      Info,
      Warning,
      Error,
      Fatal
    };

  private:
    std::thread logThread_;
    std::queue<std::tuple<std::string, std::string, LogLevel>> logQueue_;
    std::mutex logMutex_;
    std::condition_variable logCondition_;
    std::atomic<bool> isRunning_;

    void ProcessQueue();

  public:
    std::ostream* output_;

    V8_Logger(std::ostream* out = &std::clog);
    ~V8_Logger();

    void SetOutput(std::ostream* out) {
      std::unique_lock<std::mutex> lock(logMutex_);
      output_ = out;
    }

    template <typename... Args>
    void Log(LogLevel level, std::string_view funcName, fmt::format_string<Args...> fmt, Args&&... args) {
      std::unique_lock<std::mutex> lock(logMutex_);

      // immediately terminate if fatal
      if (level == LogLevel::Fatal) {
        std::string msg = fmt::format(fmt, std::forward<Args>(args)...);
        (*output_) << "\033[31m[FATAL] " << msg << "\033[0m (from: " << TrimArgsFromFunctionName(FUNC_NAME) << ")" << std::endl; 

        isRunning_ = false;
        std::terminate();
      }

      logQueue_.push(std::make_tuple(fmt::format(fmt, std::forward<Args>(args)...), std::string(TrimArgsFromFunctionName(funcName)), level));
      logCondition_.notify_one();
    }
};

extern V8_Logger logger;

#ifdef NDEBUG
  #define V_DEBUG(fmt_str, ...) (void)0
  #define V_INFO(fmt_str, ...) (void)0
#else
  #define V_DEBUG(fmt_str, ...) \
    logger.Log(V8_Logger::LogLevel::Debug, TrimArgsFromFunctionName(FUNC_NAME), fmt_str, ##__VA_ARGS__)

  #define V_INFO(fmt_str, ...) \
    logger.Log(V8_Logger::LogLevel::Info, TrimArgsFromFunctionName(FUNC_NAME), fmt_str, ##__VA_ARGS__)
#endif

#define V_WARNING(fmt_str, ...) \
  logger.Log(V8_Logger::LogLevel::Warning, TrimArgsFromFunctionName(FUNC_NAME), fmt_str, ##__VA_ARGS__)

#define V_ERROR(fmt_str, ...) \
  logger.Log(V8_Logger::LogLevel::Error, TrimArgsFromFunctionName(FUNC_NAME), fmt_str, ##__VA_ARGS__)

#define V_FATAL(fmt_str, ...) \
  logger.Log(V8_Logger::LogLevel::Fatal, TrimArgsFromFunctionName(FUNC_NAME), fmt_str, ##__VA_ARGS__)
