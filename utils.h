//
// Created by xflajs00 on 04.02.2021.
//

#ifndef VITEJTE_POSTGRESQL_SERVICE__UTILS_H
#define VITEJTE_POSTGRESQL_SERVICE__UTILS_H

#include <exception>
#include <numeric>
#include <ctime>
#include <string>
#include "date/date.h"
#include <toml++/toml.h>

template<typename T>
inline T getOrThrow(const toml::table &table, std::string_view key) {
  using namespace std::string_literals;
  if (auto val = table[key].value<T>(); val.has_value()) { return *val; }
  throw std::runtime_error("Value missing in config: "s + std::string(key));
}

inline std::filesystem::path getExeFolder(std::string_view exePath) {
  auto result = std::filesystem::path(exePath);
  result.remove_filename();
  return result;
}

inline date::hh_mm_ss<std::chrono::milliseconds> delphiTimeToTimeOfDay(double time) {
  using namespace std::chrono_literals;
  auto integral = 0.0;
  const auto fractional = std::modf(time, &integral);
  return date::hh_mm_ss<std::chrono::milliseconds>(std::chrono::milliseconds(
      uint32_t(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::days(1)).count() * fractional)));
}

inline date::hh_mm_ss<std::chrono::milliseconds> delphiDateToTimePoint(double date) {
  using namespace std::chrono_literals;
  using namespace date::literals;
  auto integral = 0.0;
  const auto fractional = std::modf(date, &integral);
  return date::year_month_day(1899_y, date::January, 1_d) + date::day(integral);
}

inline std::chrono::time_point<std::chrono::system_clock> delphiDateTimeToTimePoint(double dateTime) {}

#endif//VITEJTE_POSTGRESQL_SERVICE__UTILS_H
