#include "DeviceFinder.h"
#include "fmt/core.h"
#include "fmt/ostream.h"
#include "global_logger.h"
#include "savers.h"
#include "utils.h"
#include "list_devices.h"
#include <argparse.hpp>
#include "toml++/toml.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <magic_enum.hpp>
#include <pqxx/pqxx>

using namespace std::string_literals;

/*
void setup(VitejteEventHandler &handler) {
  auto &commRef = handler.communicator;

  const auto devices = commRef.findDevices();

  commRef.setDevice(devices[1]);
  fmt::print("Set device\n");
  fmt::print("Registering\n");
  commRef.registerClient(44444);

  commRef.setPatientChangeCallback(callbackPatientChange, &handler);
  commRef.setErrorCallback(callbackError, &handler);
  commRef.setStatChangeCallback(callbackStateChange, &handler);
}*/

enum class AppMode { Devices, Test, Normal, InvalidMode };

argparse::ArgumentParser createArgumentParser() {
  auto parser = argparse::ArgumentParser("vitejte_service");
  parser.add_argument("--test").help("test funkce ukladani dat").default_value(false).implicit_value(true);
  parser.add_argument("--mode")
      .help("typ ukladace [postgres]")
      .action([](const std::string &value) {
        if (auto option = magic_enum::enum_cast<vitejte::SaverType>(value); option.has_value()) { return value; }
        throw std::runtime_error("Invalid option for mode: "s + value);
      })
      .default_value("none"s);
  parser.add_argument("--devices")
      .help("nalezeni a vypis zarizeni Vitejte na siti")
      .default_value(false)
      .implicit_value(true);
  parser.add_argument("-l", "--log").help("logovani na stdout").default_value(false).implicit_value(true);
  return parser;
}

AppMode decideAppMode(argparse::ArgumentParser &args) {
  if (args.get<bool>("--devices")) { return AppMode::Devices; }
  if (args["--mode"] == "none"s) { return AppMode::InvalidMode; }
  if (args["--test"] == true) { return AppMode::Test; }
  return AppMode::Normal;
}

int run(bool test, vitejte::SaverType saverType, const std::filesystem::path &exeFolder) {
  auto config = toml::parse_file((exeFolder / "config.toml").string());
  auto dataSaver = vitejte::createDataSaver(saverType, config);

  if (test) { return vitejte::runSavingTest(*dataSaver); }
}

int main(int argc, char *argv[]) {
  const auto exeFolder = getExeFolder(argv[0]);

  auto args = createArgumentParser();
  try {
    args.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    fmt::print("{}\n", err.what());
    fmt::print("{}", args);
    return 0;
  }
  createLogger(exeFolder, args.get<bool>("--log"));

  const auto appMode = decideAppMode(args);
  try {
    switch (appMode) {
      case AppMode::Devices: listDevices(); return 0;
      case AppMode::Test:
        return run(true, magic_enum::enum_cast<vitejte::SaverType>(args.get<std::string>("--mode")).value(), exeFolder);
      case AppMode::Normal:
        return run(false, magic_enum::enum_cast<vitejte::SaverType>(args.get<std::string>("--mode")).value(),
                   exeFolder);
      case AppMode::InvalidMode:
        fmt::print("Mode neni zadan\n");
        fmt::print("{}", args);
        return 3;
    }
  } catch (pqxx::sql_error const &e) {
    logger->log(spdlog::level::err, "SQL error: {}, query: {}", e.what(), e.query());
    return 2;
  } catch (std::exception const &e) {
    logger->log(spdlog::level::err, "Error: {}", e.what());
    return 1;
  }
  /*using namespace std::chrono_literals;
  Main main;

  setup(main);

  std::string input;
  fmt::print("Type 'exit' to quit, c<id> to clear a patient\n";
  while (input != "exit") {
    std::cin >> input;
    if (input.empty()) { continue; }
    if (input[0] == 'c') {
      const auto idStr = input.substr(1);
      if (!idStr.empty()) {
        const auto id = std::atoi(idStr.c_str());
        main.communicator.clearPatient(id);
      }
    }
  }
  main.communicator.unregisterClient();
  fmt::print("quitting..." << std::endl;
  std::this_thread::sleep_for(5s);
  vitejte::freeLibrary();
*/
  return 0;
}
