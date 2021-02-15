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
#include <ostream>
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

inline date::hh_mm_ss<std::chrono::milliseconds> delphiTimeToTimeOfDay(const std::string &time) {
  using namespace std::string_literals;
  const auto hours = std::chrono::hours(static_cast<unsigned>(std::atoi(time.substr(0, 2).c_str())));
  const auto minutes = std::chrono::minutes(static_cast<unsigned>(std::atoi(time.substr("hh:"s.size(), 2).c_str())));
  const auto seconds = std::chrono::seconds(static_cast<unsigned>(std::atoi(time.substr("hh:mm:"s.size(), 2).c_str())));
  return date::hh_mm_ss<std::chrono::milliseconds>{hours + minutes + seconds};
}

inline date::year_month_day delphiDateToYearMonthDay(const std::string &date) {
  using namespace std::string_literals;
  const auto day = date::day(static_cast<unsigned>(std::atoi(date.substr(0, "dd"s.size()).c_str())));
  const auto month = date::month(static_cast<unsigned>(std::atoi(date.substr("dd/"s.size(), "mm"s.size()).c_str())));
  const auto year = date::year(static_cast<unsigned>(std::atoi(date.substr("dd/mm/"s.size(), "yyyy"s.size()).c_str())));
  return year / month / day;
}

struct DateTime {
  date::year_month_day date{};
  date::hh_mm_ss<std::chrono::milliseconds> time{};
  inline friend std::ostream &operator<<(std::ostream &os, const DateTime &dateTime) {
    os << fmt::format("{}/{}/{} {}:{}", static_cast<int>(dateTime.date.year()),
                      static_cast<unsigned>(dateTime.date.month()), static_cast<unsigned>(dateTime.date.day()),
                      dateTime.time.hours().count(), dateTime.time.minutes().count(), dateTime.time.seconds().count());
    return os;
  }
};

inline DateTime delphiDateTimeToDateTime(const std::string &dateTime) {
  using namespace std::string_literals;
  return DateTime{delphiDateToYearMonthDay(dateTime.substr(0, "dd/mm/yyyy"s.size())),
                  delphiTimeToTimeOfDay(dateTime.substr("dd/mm/yyyy "s.size(), "hh:mm:ss"s.size()))};
}

#endif//VITEJTE_POSTGRESQL_SERVICE__UTILS_H
