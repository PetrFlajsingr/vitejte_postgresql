//
// Created by Petr on 31.03.2020.
//

#ifndef LIB_TEST_COMMUNICATOR_H
#define LIB_TEST_COMMUNICATOR_H

#include "VitejteLibTypes.h"
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

namespace vitejte {

class VitejteException : public std::runtime_error {
 public:
  explicit VitejteException(const std::string &arg);
};

enum class ErrorCodes { OK = 0, InvalidAccess = 1 };
ErrorCodes intToErrorCode(int code);

enum class VitejteState { None = 0, Normal, Off, Settings, Break, Message };

class Patient {
 public:
  enum class State { InQueue = 0, Deleted, Added, Cleared, Returned };
  enum class CardType { None = 0, Other, Insurance };
  Patient() = default;
  explicit Patient(const RawPatient &other);

  [[nodiscard]] int getId() const;
  [[nodiscard]] double getRegistrationTime() const;
  [[nodiscard]] double getClearanceTime() const;
  [[nodiscard]] const std::string &getTitlePre() const;
  [[nodiscard]] const std::string &getTitlePost() const;
  [[nodiscard]] const std::string &getName() const;
  [[nodiscard]] const std::string &getLastName() const;
  [[nodiscard]] const std::string &getRc() const;
  [[nodiscard]] const std::string &getZp() const;
  [[nodiscard]] const std::string &getDuvod() const;
  [[nodiscard]] int getIdFronty() const;
  [[nodiscard]] State getState() const;
  [[nodiscard]] const std::string &getLanguage() const;
  [[nodiscard]] CardType getCardType() const;

  friend std::ostream &operator<<(std::ostream &stream, const Patient &patient);

  [[nodiscard]] bool operator==(const Patient &rhs) const;
  [[nodiscard]] bool operator!=(const Patient &rhs) const;

  [[nodiscard]] inline static auto getIdSortFnc() {
    return [] (const Patient &lhs, const Patient &rhs) {
      return lhs.getId() < rhs.getId();
    };
  }

 private:
  int id;
  double registrationTime;
  double clearanceTime;
  std::string titlePre;
  std::string titlePost;
  std::string name;
  std::string lastName;
  std::string rc;
  std::string zp;
  std::string duvod;
  int idFronty;
  State state;
  std::string language;
  CardType cardType;
};

std::string patientStateToString(Patient::State state);
std::string patientCardTypeToString(Patient::CardType type);

class Communicator final {
 public:
  /**
   * Loads VitejteLib.dll on creation of first instance and loads internal
   * function pointers.
   */
  Communicator();
  /**
   * Unloads VitejteLib.dll on deletion of last instance.
   */
  ~Communicator();
  /**
   * Get list of unique IDs of devices found using broadcast. May take a while,
   * blocking.
   * @return unique IDs of found devices
   */
  [[nodiscard]] std::vector<std::string> findDevices();
  /**
   * Choose unique device ID to communicate with. Must be one of returned from
   * findDevices().
   * @param deviceID unique device ID
   */
  void setDevice(const std::string &deviceID);
  /**
   * Send register command to device.
   * Registers messages about incoming patients, status change etc.
   *
   * @param port
   * @param queues ids of queues about which this Communicator gets information
   */
  void registerClient(int port = 44444, const std::vector<int> &queues = {0});

  void unregisterClient();

  void clearPatient(int id);
  /**
   * Set callbacks for network messages from device
   * @param onPatientsChange called when there is a change in queue (new patient
   * etc.)
   */
  void setPatientChangeCallback(PtrFncPatientChangedCallback onPatientsChange, void *customPayload);
  void setErrorCallback(PtrFncErrorCallback onError, void *customPayload);
  void setStatChangeCallback(PtrFncStateChangeCallback onStateChange, void *customPayload);

  std::vector<Patient> getPatients();

  [[nodiscard]] std::string getIP();
  [[nodiscard]] std::string getID();
  [[nodiscard]] std::string getInfo();

 private:
  CommHandle handle;
  CallbackHandle callbackHandle;

  bool isDeviceSet = false;
  bool isRegistered = false;

  void startPing();
  void stopPing();
};
}// namespace vitejte
#endif// LIB_TEST_COMMUNICATOR_H
