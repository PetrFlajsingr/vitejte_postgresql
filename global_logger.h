//
// Created by xflajs00 on 03.02.2021.
//

#ifndef VITEJTE_POSTGRESQL_SERVICE__GLOBAL_LOGGER_H
#define VITEJTE_POSTGRESQL_SERVICE__GLOBAL_LOGGER_H

#include <filesystem>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

inline std::shared_ptr<spdlog::logger> logger;

inline void createLogger(const std::filesystem::path &folder, bool logToStdout) {
  auto sinks = std::vector<std::shared_ptr<spdlog::sinks::sink>>{};
  if (logToStdout) { sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>()); }
  const auto logFilePath = folder / "log.log";
  const auto maxSize = 1048576 * 5;
  const auto maxFiles = 3;
  sinks.emplace_back(
      std::make_shared<spdlog::sinks::rotating_file_sink<std::mutex>>(logFilePath.string(), maxSize, maxFiles));
  auto log = std::make_shared<spdlog::logger>("vitejte_service", sinks.begin(), sinks.end());
  log->set_level(spdlog::level::trace);
  spdlog::register_logger(log);
  logger = log;
  logger->log(spdlog::level::trace, "Logger created");
}

#endif//VITEJTE_POSTGRESQL_SERVICE__GLOBAL_LOGGER_H
