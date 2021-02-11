//
// Created by xflajs00 on 05.02.2021.
//

#ifndef VITEJTE_POSTGRESQL_SERVICE__DEVICEFINDER_H
#define VITEJTE_POSTGRESQL_SERVICE__DEVICEFINDER_H

#include "lib_interface/Communicator.h"

namespace vitejte {
struct DeviceFinder {
 public:
  std::vector<std::string> findDevices();

 private:
  vitejte::Communicator communicator;

};
}

#endif//VITEJTE_POSTGRESQL_SERVICE__DEVICEFINDER_H
