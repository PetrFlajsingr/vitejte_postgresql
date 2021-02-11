//
// Created by xflajs00 on 05.02.2021.
//

#include "Service.h"
#include "global_logger.h"
#include "magic_enum.hpp"
#include "range/v3/action/sort.hpp"
#include "range/v3/view/filter.hpp"
#include "range/v3/view/transform.hpp"
#include "range/v3/view/zip.hpp"
#include <algorithm>
#include <range/v3/range/conversion.hpp>

namespace vitejte {

void Service::onPatientsChanged() {}

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
  const auto result = ranges::views::zip(oldPatients, newPatients) | ranges::views::filter([](const auto &patients) {
                        const auto &[oldPatient, newPatient] = patients;
                        return oldPatient != newPatient;
                      })
      | ranges::views::transform([](const auto &patients) {
                        const auto &[oldPatient, newPatient] = patients;
                        return newPatient;
                      })
      | ranges::to_vector;
  if (!result.empty()) { oldPatients = newPatients; }
  return result;
}

}// namespace vitejte