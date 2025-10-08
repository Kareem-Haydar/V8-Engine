#include <Core/Logger.h>

V8_Logger::V8_Logger(std::ostream* out) : output_(out), isRunning_(true) {
  logThread_ = std::thread(&V8_Logger::ProcessQueue, this);
}

V8_Logger::~V8_Logger() {
  {
    std::lock_guard<std::mutex> lock(logMutex_);
    isRunning_ = false;
  }

  logCondition_.notify_one();
  if (logThread_.joinable())
    logThread_.join();
}

void V8_Logger::ProcessQueue() {
  while (true) {
    std::unique_lock<std::mutex> lock(logMutex_);
    logCondition_.wait(lock, [this] { return !logQueue_.empty() || !isRunning_; });

    while (!logQueue_.empty()) {
      std::tuple<std::string, std::string, LogLevel> log = logQueue_.front();
      std::string message = std::get<0>(log);
      std::string funcName = std::get<1>(log);
      LogLevel level = std::get<2>(log);

      switch (level) {
        case LogLevel::Debug:
          message = "[DEBUG] " + message;
          (*output_) << "\033[36m" << message << "\033[0m" << " (from: " << funcName << ")" << std::endl;
          break;
        case LogLevel::Info:
          message = "[INFO] " + message;
          (*output_) << "\033[34m" << message << "\033[0m" << " (from: " << funcName << ")" << std::endl;
          break;
        case LogLevel::Warning:
          message = "[WARNING] " + message;
          (*output_) << "\033[33m" << message << "\033[0m" << " (from: " << funcName << ")" << std::endl;
          break;
        case LogLevel::Error:
          message = "[ERROR] " + message;
          (*output_) << "\033[35m" << message << "\033[0m" << " (from: " << funcName << ")" << std::endl;
          break;
        case LogLevel::Fatal:
          message = "[FATAL] " + message;
          (*output_) << "\033[31m" << message << "\033[0m" << " (from: " << funcName << ")" << std::endl;
          exit(1);
          break;
      }

      logQueue_.pop();
    }

    if (!isRunning_) 
      break;
  }
}

V8_Logger logger = V8_Logger(&std::clog);
