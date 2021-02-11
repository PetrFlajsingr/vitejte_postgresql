//
// Created by Petr on 11.02.2021.
//
#include "list_devices.h"
#include "global_logger.h"
#include "DeviceFinder.h"
#include "indicators/indeterminate_progress_bar.hpp"
#include "range/v3/view/enumerate.hpp"
#include <future>
#include "tabulate/table.hpp"
#include "fmt/format.h"
#include "fmt/ostream.h"

void listDevices() {
  logger->log(spdlog::level::trace, "Vyhledavani Vitejte na siti");

  auto bar = indicators::IndeterminateProgressBar{
      indicators::option::BarWidth{40},
      indicators::option::Start{"["},
      indicators::option::Fill{"-"},
      indicators::option::Lead{"<==>"},
      indicators::option::End{"]"},
      indicators::option::PostfixText{"Vyhledavani zarizeni"},
      indicators::option::ForegroundColor{indicators::Color::yellow},
      indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}};

  auto job = [&] {
    auto devices = vitejte::DeviceFinder{}.findDevices();
    bar.mark_as_completed();
    return devices;
  };

  auto devFuture = std::async(std::launch::async, job);

  while (!bar.is_completed()) {
    bar.tick();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  const auto devices = devFuture.get();

  logger->log(spdlog::level::trace, "Vyhledavani dokonceno, pocet nalezenych zarizeni: {}", devices.size());

  auto table = tabulate::Table();
  table.format().font_style({tabulate::FontStyle::bold}).font_align(tabulate::FontAlign::left);
  fmt::print("Nalezena zarizeni: \n");
  table.add_row({"Index", "ID"});
  for (const auto &[idx, device] : ranges::views::enumerate(devices)) {
    table.add_row({fmt::format("#{}", 1), device});
  }
  fmt::print("{}\n", table);
  fmt::print("Pro vyber zarizeni jej pridejte do konfiguracniho souboru 'config.toml' ve tvaru:\n");
  fmt::print("[vitejte]\n"
             "id = \"id\"\n");
}
