//
// Created by xflajs00 on 05.02.2021.
//

#ifndef VITEJTE_POSTGRESQL_SERVICE__VITEJTESERVICE_H
#define VITEJTE_POSTGRESQL_SERVICE__VITEJTESERVICE_H

#include "lib_interface/Communicator.h"
#include "savers.h"
#include <toml++/toml_table.h>

namespace vitejte {

class Service {
 public:
  explicit Service(toml::table &config, std::unique_ptr<VitejteDataSaver> saver);

  void run();

  void onPatientsChanged();
  void onError(const std::string &message);
  void onStateChange(vitejte::VitejteState state, double until);

 private:
  void saveOrUpdatePatients(const std::vector<Patient> &patients);

  vitejte::Communicator communicator;
  std::vector<vitejte::Patient> oldPatients;

  std::vector<Patient> compareAndGetChangedPatients();
  std::vector<Patient> getVitejtePatients();
  std::unique_ptr<VitejteDataSaver> saver = nullptr;
};

inline __stdcall void callbackPatientChange(void *ptr) {
  auto &handler = *reinterpret_cast<Service *>(ptr);
  handler.onPatientsChanged();
}
inline __stdcall void callbackError(void *ptr, const char *msg) {
  auto &handler = *reinterpret_cast<Service *>(ptr);
  handler.onError(msg);
}
inline __stdcall void callbackStateChange(void *ptr, int state, double until) {
  auto &handler = *reinterpret_cast<Service *>(ptr);
  handler.onStateChange(static_cast<vitejte::VitejteState>(state), until);
}

}// namespace vitejte
#endif//VITEJTE_POSTGRESQL_SERVICE__VITEJTESERVICE_H
