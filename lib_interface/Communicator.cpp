//
// Created by Petr on 31.03.2020.
//

#include "Communicator.h"
#include "Finally.h"
#include "LibInterface.h"

using namespace std::string_literals;

namespace vitejte {

Patient::Patient(const RawPatient &other)
    : id(other.id), registrationTime(other.registrationTime), clearanceTime(other.clearanceTime),
      titlePre(other.titlePre), titlePost(other.titlePost), name(other.name), lastName(other.lastName), rc(other.rc),
      zp(other.zp), duvod(other.duvod), idFronty(other.idFronty), state(static_cast<State>(other.state)),
      language(other.language), cardType(static_cast<CardType>(other.cardType)) {}


std::ostream &operator<<(std::ostream &stream, const Patient &patient) {
  return stream << "id: " << patient.id << " r\n"
         "namespace vitejte {\n"
         "\n"
         "Patient::Patient(const RawPatient &other)\n"
         "    : id(other.id), registrationTime(other.registrationTime), clearanceTime(other.clearanceTime),\n"
         "      titlePre(other.titlePre), titlePost(other.titlePost), name(other.name), lastName(other.lastName), rc(other.rc),\n"
         "      zp(other.zp), duvod(other.duvod), idFronty(other.idFronty), state(static_cast<State>(other.state)),\n"
         "      language(other.language), cardType(staegistrationTime: " << patient.registrationTime
                << " clearanceTime: " << patient.clearanceTime << " titlePre: " << patient.titlePre
                << " titlePost: " << patient.titlePost << " name: " << patient.name << " lastName: " << patient.lastName
                << " rc: " << patient.rc << " zp: " << patient.zp << " duvod: " << patient.duvod
                << " idFronty: " << patient.idFronty << " state: " << patientStateToString(patient.state)
                << " language: " << patient.language << " cardType: " << patientCardTypeToString(patient.cardType);
}

int Patient::getId() const { return id; }

double Patient::getRegistrationTime() const { return registrationTime; }

double Patient::getClearanceTime() const { return clearanceTime; }

const std::string &Patient::getTitlePre() const { return titlePre; }

const std::string &Patient::getTitlePost() const { return titlePost; }

const std::string &Patient::getName() const { return name; }

const std::string &Patient::getLastName() const { return lastName; }

const std::string &Patient::getRc() const { return rc; }

const std::string &Patient::getZp() const { return zp; }

const std::string &Patient::getDuvod() const { return duvod; }

int Patient::getIdFronty() const { return idFronty; }

Patient::State Patient::getState() const { return state; }

const std::string &Patient::getLanguage() const { return language; }

Patient::CardType Patient::getCardType() const { return cardType; }

bool Patient::operator==(const Patient &rhs) const {
  return id == rhs.id && registrationTime == rhs.registrationTime && clearanceTime == rhs.clearanceTime
      && titlePre == rhs.titlePre && titlePost == rhs.titlePost && name == rhs.name && lastName == rhs.lastName
      && rc == rhs.rc && zp == rhs.zp && duvod == rhs.duvod && idFronty == rhs.idFronty && state == rhs.state
      && language == rhs.language && cardType == rhs.cardType;
}

bool Patient::operator!=(const Patient &rhs) const { return !(rhs == *this); }

Communicator::Communicator() {
  loadVitejteLibrary();
  handle = libCreateCommunicationHandle();
  callbackHandle = libCreateCallbackHandle();
}

Communicator::~Communicator() {
  libDeleteCommunicationHandle(handle);
  libDeleteCallbackHandle(callbackHandle);
}

std::vector<std::string> Communicator::findDevices() {
  char **deviceList;
  int size = 0;
  const auto errC = intToErrorCode(libFindDevices(handle, deviceList, size));
  if (errC != ErrorCodes::OK) {
    throw VitejteException{"Error in lib, code: "s + std::to_string(static_cast<int>(errC))};
  }
  Finally freeCStringDeviceID{[&deviceList, size] { libFreeStringArray(deviceList, size); }};
  std::vector<std::string> result;
  result.reserve(size);
  for (int i = 0; i < size; ++i) { result.emplace_back(deviceList[i]); }
  return result;
}

void Communicator::setDevice(const std::string &deviceID) {
  auto *cStringDeviceID = new char[deviceID.size() + 1];
  Finally freeCStringDeviceID{[&cStringDeviceID] { delete[] cStringDeviceID; }};
  std::copy(deviceID.begin(), deviceID.end(), cStringDeviceID);
  cStringDeviceID[deviceID.size()] = '\0';
  const auto errC = intToErrorCode(libSetDevice(handle, cStringDeviceID));
  if (errC != ErrorCodes::OK) {
    throw VitejteException{"Error in lib, code: "s + std::to_string(static_cast<int>(errC))};
  }
  isDeviceSet = true;
  isRegistered = false;
}

std::string Communicator::getIP() {
  if (!isDeviceSet) { throw std::logic_error{"Communicator::getIP device not set"}; }
  char *ip;
  const auto errC = intToErrorCode(libGetVitejteIp(handle, ip));
  if (errC != ErrorCodes::OK) {
    throw VitejteException{"Error in lib, code: "s + std::to_string(static_cast<int>(errC))};
  }
  Finally freeCStringDeviceID{[&ip] { libFreeString(ip); }};
  std::string result{ip};
  return result;
}

std::string Communicator::getID() {
  if (!isDeviceSet) { throw std::logic_error{"Communicator::getID device not set"}; }
  char *ip;
  const auto errC = intToErrorCode(libGetVitejteID(handle, ip));
  if (errC != ErrorCodes::OK) {
    throw VitejteException{"Error in lib, code: "s + std::to_string(static_cast<int>(errC))};
  }
  Finally freeCStringDeviceID{[&ip] { libFreeString(ip); }};
  std::string result{ip};
  return result;
}

std::string Communicator::getInfo() {
  if (!isDeviceSet) { throw std::logic_error{"Communicator::getInfo device not set"}; }
  char *ip;
  const auto errC = intToErrorCode(libGetInfo(handle, ip));
  if (errC != ErrorCodes::OK) {
    throw VitejteException{"Error in lib, code: "s + std::to_string(static_cast<int>(errC))};
  }
  Finally freeCStringDeviceID{[&ip] { libFreeString(ip); }};
  std::string result{ip};
  return result;
}

void Communicator::registerClient(int port, const std::vector<int> &queues) {
  if (!isDeviceSet) { throw std::logic_error{"Communicator::registerClient device not set"}; }
  std::string qs;
  for (auto queueIdx : queues) { qs += std::to_string(queueIdx) + ","; }
  qs = qs.substr(0, qs.size() - 1);
  if (qs.empty()) { qs = "0"; }

  auto *cStringDeviceID = new char[qs.size() + 1];
  Finally freeCStringDeviceID{[&cStringDeviceID] { delete[] cStringDeviceID; }};
  std::copy(qs.begin(), qs.end(), cStringDeviceID);
  cStringDeviceID[qs.size()] = '\0';
  const auto errC = intToErrorCode(libRegisterClient(handle, port, cStringDeviceID));
  if (errC != ErrorCodes::OK) {
    throw VitejteException{"Error in lib, code: "s + std::to_string(static_cast<int>(errC))};
  }
  isRegistered = true;
  startPing();
}

void Communicator::unregisterClient() {
  if (!isDeviceSet) { throw std::logic_error{"Communicator::unregisterClient device not set"}; }
  stopPing();
  const auto errC = intToErrorCode(libUnRegisterClient(handle));
  if (errC != ErrorCodes::OK) {
    throw VitejteException{"Error in lib, code: "s + std::to_string(static_cast<int>(errC))};
  }
  isRegistered = false;
}

void Communicator::startPing() {
  if (!isDeviceSet) { throw std::logic_error{"Communicator::startPing device not set"}; }
  const auto errC = intToErrorCode(libStartPingTimer(handle));
  if (errC != ErrorCodes::OK) {
    throw VitejteException{"Error in lib, code: "s + std::to_string(static_cast<int>(errC))};
  }
}

void Communicator::stopPing() {
  if (!isDeviceSet) { throw std::logic_error{"Communicator::stopPing device not set"}; }
  const auto errC = intToErrorCode(libStopPingTimer(handle));
  if (errC != ErrorCodes::OK) {
    throw VitejteException{"Error in lib, code: "s + std::to_string(static_cast<int>(errC))};
  }
}

void Communicator::setPatientChangeCallback(PtrFncPatientChangedCallback onPatientsChange, void *customPayload) {
  if (!isDeviceSet) { throw std::logic_error{"Communicator::setCallbacks device not set"}; }
  auto errC = intToErrorCode(libPatientChangedCallbackSet(callbackHandle, onPatientsChange, customPayload));
  if (errC != ErrorCodes::OK) {
    throw VitejteException{"Error in lib, code: "s + std::to_string(static_cast<int>(errC))};
  }
  errC = intToErrorCode(libSetCallbackForComm(handle, callbackHandle));
  if (errC != ErrorCodes::OK) {
    throw VitejteException{"Error in lib, code: "s + std::to_string(static_cast<int>(errC))};
  }
}
void Communicator::setErrorCallback(PtrFncErrorCallback onError, void *customPayload) {
  if (!isDeviceSet) { throw std::logic_error{"Communicator::setCallbacks device not set"}; }
  auto errC = intToErrorCode(libErrorCallbackSet(callbackHandle, onError, customPayload));
  if (errC != ErrorCodes::OK) {
    throw VitejteException{"Error in lib, code: "s + std::to_string(static_cast<int>(errC))};
  }
  errC = intToErrorCode(libSetCallbackForComm(handle, callbackHandle));
  if (errC != ErrorCodes::OK) {
    throw VitejteException{"Error in lib, code: "s + std::to_string(static_cast<int>(errC))};
  }
}
void Communicator::setStatChangeCallback(PtrFncStateChangeCallback onStateChange, void *customPayload) {
  if (!isDeviceSet) { throw std::logic_error{"Communicator::setCallbacks device not set"}; }
  auto errC = intToErrorCode(libStateChangeCallbackSet(callbackHandle, onStateChange, customPayload));
  if (errC != ErrorCodes::OK) {
    throw VitejteException{"Error in lib, code: "s + std::to_string(static_cast<int>(errC))};
  }
  errC = intToErrorCode(libSetCallbackForComm(handle, callbackHandle));
  if (errC != ErrorCodes::OK) {
    throw VitejteException{"Error in lib, code: "s + std::to_string(static_cast<int>(errC))};
  }
}

std::vector<Patient> Communicator::getPatients() {
  if (!isRegistered) { throw std::logic_error{"Communicator::getPatients device not set"}; }
  RawPatient **pRawPatient;
  int patientCount;
  auto errC = intToErrorCode(libGetPatients(handle, pRawPatient, patientCount));
  if (errC != ErrorCodes::OK) {
    throw VitejteException{"Error in lib, code: "s + std::to_string(static_cast<int>(errC))};
  }
  Finally freePatients{[&pRawPatient, patientCount] { libFreepatients(pRawPatient, patientCount); }};
  std::vector<Patient> result;
  std::transform(pRawPatient, pRawPatient + patientCount, std::back_inserter(result),
                 [](RawPatient *rawPatient) { return Patient{*rawPatient}; });
  return result;
}

void Communicator::clearPatient(int id) {
  if (!isRegistered) { throw std::logic_error{"Communicator::getPatients device not set"}; }
  auto errC = intToErrorCode(libClearPatient(handle, id));
  if (errC != ErrorCodes::OK) {
    throw VitejteException{"Error in lib, code: "s + std::to_string(static_cast<int>(errC))};
  }
}

ErrorCodes intToErrorCode(int code) { return static_cast<ErrorCodes>(code); }

std::string patientStateToString(Patient::State state) {
  switch (state) {
    case Patient::State::InQueue: return "InQueue";
    case Patient::State::Deleted: return "Deleted";
    case Patient::State::Added: return "Added";
    case Patient::State::Cleared: return "Cleared";
    case Patient::State::Returned: return "Returned";
  }
}
std::string patientCardTypeToString(Patient::CardType type) {
  switch (type) {
    case Patient::CardType::None: return "None";
    case Patient::CardType::Other: return "Other";
    case Patient::CardType::Insurance: return "Insurance";
  }
}
VitejteException::VitejteException(const std::string &arg) : runtime_error(arg) {}

}// namespace vitejte