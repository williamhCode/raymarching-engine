#pragma once

#include <atomic>
#include <mutex>
#include <string>
#include <filesystem>
#include <fstream>

struct Logger {
  std::atomic_bool enabled = true;
  std::mutex mutex;

  std::ofstream logFile;
  void RedirToPath(const std::filesystem::path& path);

  void Log(const std::string& message);

  void LogInfo(const std::string& message);
  void LogWarn(const std::string& message);
  void LogErr(const std::string& message);
  // void LogTrace();
};

inline Logger logger;

// helpers
#include <format>
#define LOG_ENABLE() logger.enabled = true
#define LOG_DISABLE() logger.enabled = false
#define LOG(...) logger.Log(std::format(__VA_ARGS__))
#define LOG_INFO(...) logger.LogInfo(std::format(__VA_ARGS__))
#define LOG_WARN(...) logger.LogWarn(std::format(__VA_ARGS__))
#define LOG_ERR(...) logger.LogErr(std::format(__VA_ARGS__))
// #define LOG_TRACE() logger.LogTrace()

#include <sstream>
inline std::string ToString(auto&& obj) {
  return (std::ostringstream() << obj).str();
}

