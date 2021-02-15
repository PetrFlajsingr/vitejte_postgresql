//
// Created by xflajs00 on 05.02.2021.
//

#include "Service.h"
#include "fmt/ostream.h"
#include "global_logger.h"
#include "lib_interface/Finally.h"
#include "magic_enum.hpp"
#include "range/v3/action/sort.hpp"
#include "range/v3/view/filter.hpp"
#include "range/v3/view/transform.hpp"
#include "range/v3/view/zip.hpp"
#include <algorithm>
#include <range/v3/range/conversion.hpp>
#include <signal.h>
#include <toml++/toml.h>
#include "utils.h"

namespace vitejte {

Service::Service(toml::table &config, std::unique_ptr<VitejteDataSaver> saver) : saver(std::move(saver)) {
  if (!config.contains("vitejte")) { throw std::runtime_error("Config neobsahuje ID vitejte"); }
  auto vitejteConfig = *config["vitejte"].as_table();
  if (!vitejteConfig.contains("id")) { throw std::runtime_error("Config neobsahuje ID vitejte"); }
  const auto vitejteID = *vitejteConfig["id"].value<std::string>();
  const auto devices = communicator.findDevices();
  if (const auto iter = std::ranges::find(devices, vitejteID); iter == devices.end()) {
    throw std::runtime_error("Vybrane zarizeni nebylo nalezeno");
  }
  communicator.setDevice(vitejteID);
  communicator.setErrorCallback(callbackError, this);
  communicator.setPatientChangeCallback(callbackPatientChange, this);
  communicator.setStatChangeCallback(callbackStateChange, this);
}

void Service::onPatientsChanged() {
  logger->log(spdlog::level::trace, "Patients changed");
  const auto changedPatients = compareAndGetChangedPatients();
  for (const auto &patient : changedPatients) {
    logger->log(spdlog::level::debug, "{}\n", patient);
    auto ss = std::stringstream{};
    ss <<  delphiDateTimeToDateTime(patient.getRegistrationTime());
    const auto dateStr = ss.str();
    logger->log(spdlog::level::debug, "{}\n", dateStr);
  }
  saveOrUpdatePatients(changedPatients);
}

void Service::onError(const std::string &message) {
  logger->log(spdlog::level::critical, "Error from Vitejte: {}", message);
}

void Service::onStateChange(vitejte::VitejteState state, double until) {
  logger->log(spdlog::level::info, "Vitejte state changed: {}", magic_enum::enum_name(state));
}
std::vector<Patient> Service::getVitejtePatients() {
  return communicator.getPatients() | ranges::actions::sort(Patient::getIdSortFnc());
}
std::vector<Patient> Service::compareAndGetChangedPatients() {
  const auto newPatients = getVitejtePatients();
  auto result = ranges::views::zip(oldPatients, newPatients) | ranges::views::filter([](const auto &patients) {
                  const auto &[oldPatient, newPatient] = patients;
                  return oldPatient != newPatient;
                })
      | ranges::views::transform([](const auto &patients) {
                  const auto &[oldPatient, newPatient] = patients;
                  return newPatient;
                })
      | ranges::to_vector;
  if (newPatients.size() > oldPatients.size()) {
    std::ranges::copy(newPatients.begin() + oldPatients.size(), newPatients.end(), std::back_inserter(result));
  }
  if (!result.empty()) { oldPatients = newPatients; }
  return result;
}
void Service::run() {
  communicator.registerClient();
  auto communicatorStop = Finally([&] { communicator.unregisterClient(); });
  auto tmp = std::string();
  std::cin >> tmp;
}
void Service::saveOrUpdatePatients(const std::vector<Patient> &patients) {
  std::ranges::for_each(patients, [this] (const auto &patient) {
    saver->saveOrUpdate(patient);
  });
}

}// namespace vitejte