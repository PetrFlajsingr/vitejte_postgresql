//
// Created by xflajs00 on 04.02.2021.
//

#ifndef VITEJTE_POSTGRESQL_SERVICE__UTILS_H
#define VITEJTE_POSTGRESQL_SERVICE__UTILS_H

#include "date/date.h"
#include "lib_interface/Finally.h"
#include "lib_interface/LibInterface.h"
#include <ctime>
#include <exception>
#include <numeric>
#include <string>
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

inline date::year_month_day delphiDateToYearMonthDay(double date) {
  using namespace std::string_literals;
  char *dateTimeCStr = nullptr;
  vitejte::libDateTimeAsString(date, dateTimeCStr);
  Finally freeStr([&] {vitejte::libFreeString(dateTimeCStr);});
  const auto dateTimeStr = std::string(dateTimeCStr);
  const auto dateStr = dateTimeStr.substr(0, "dd/mm/yyyy"s.size());
  const auto day = date::day(std::atoi(dateStr.substr(0, "dd"s.size()).c_str()));
  const auto month = date::month(std::atoi(dateStr.substr("dd/"s.size(), "mm"s.size()).c_str()));
  const auto year = date::year(std::atoi(dateStr.substr("dd/mm/"s.size(), "yyyy"s.size()).c_str()));

  return year / month / day;
}

struct DateTime {
  date::year_month_day date;
  date::hh_mm_ss<std::chrono::milliseconds> time;
};

inline DateTime delphiDateTimeToTimePoint(double dateTime) {
  return DateTime{delphiDateToYearMonthDay(dateTime), delphiTimeToTimeOfDay(dateTime)};
}

#endif//VITEJTE_POSTGRESQL_SERVICE__UTILS_H
