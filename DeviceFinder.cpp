//
// Created by xflajs00 on 05.02.2021.
//

#include "DeviceFinder.h"

std::vector<std::string> vitejte::DeviceFinder::findDevices() {
  return communicator.findDevices();
}
