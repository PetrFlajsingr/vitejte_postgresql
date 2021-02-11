//
// Created by Petr on 27.03.2020.
//

#ifndef LIB_TEST_LIBINTERFACE_H
#define LIB_TEST_LIBINTERFACE_H
#include "VitejteLibTypes.h"
#include <optional>
#include <string>
#include <windows.h>

namespace vitejte {
static inline PtrFncCreateCommunicationHandle libCreateCommunicationHandle = nullptr;
static inline PtrFncDeleteCommunicationHandle libDeleteCommunicationHandle = nullptr;
static inline PtrFncCreateCallbackHandle libCreateCallbackHandle = nullptr;
static inline PtrFncDeleteCallbackHandle libDeleteCallbackHandle = nullptr;

static inline PtrFncSetCallbackForComm libSetCallbackForComm = nullptr;
static inline PtrFncPatientChangedCallbackSet libPatientChangedCallbackSet = nullptr;
static inline PtrFncErrorCallbackSet libErrorCallbackSet = nullptr;
static inline PtrFncStateChangeCallbackSet libStateChangeCallbackSet = nullptr;

static inline PtrFncDateTimeAsString libDateTimeAsString = nullptr;

static inline PtrFncFreeString libFreeString = nullptr;
static inline PtrFncFreeStringArray libFreeStringArray = nullptr;
static inline PtrFncFindDevices libFindDevices = nullptr;
static inline PtrFncSetDevice libSetDevice = nullptr;
static inline PtrFncGetVitejteIP libGetVitejteIp = nullptr;
static inline PtrFncGetVitejteID libGetVitejteID = nullptr;
static inline PtrFncGetInfo libGetInfo = nullptr;
static inline PtrFncStartPingTimer libStartPingTimer = nullptr;
static inline PtrFncStopPingTimer libStopPingTimer = nullptr;
static inline PtrFncRegisterClient libRegisterClient = nullptr;
static inline PtrFncUnRegisterClient libUnRegisterClient = nullptr;
static inline PtrFncGetPatients libGetPatients = nullptr;
static inline PtrFncFreePatients libFreepatients = nullptr;
static inline PtrFncClearPatient libClearPatient = nullptr;

namespace detail {
static inline bool isVitejteLibLoaded = false;
static inline HINSTANCE hinstLib;
}// namespace detail

inline std::optional<std::string> loadVitejteLibrary(const std::string &path = "VitejteLib.dll") {
  if (!detail::isVitejteLibLoaded) {
    detail::hinstLib = LoadLibrary(TEXT(path.c_str()));
    detail::isVitejteLibLoaded = true;
    libCreateCommunicationHandle =
        (PtrFncCreateCommunicationHandle) GetProcAddress(detail::hinstLib, "createCommunicationHandle");
    libDeleteCommunicationHandle =
        (PtrFncDeleteCommunicationHandle) GetProcAddress(detail::hinstLib, "deleteCommunicationHandle");
    libCreateCallbackHandle = (PtrFncCreateCallbackHandle) GetProcAddress(detail::hinstLib, "createCallbackHandle");
    libDeleteCallbackHandle = (PtrFncDeleteCallbackHandle) GetProcAddress(detail::hinstLib, "deleteCallbackHandle");

    libSetCallbackForComm = (PtrFncSetCallbackForComm) GetProcAddress(detail::hinstLib, "setCallbackForComm");
    libPatientChangedCallbackSet =
        (PtrFncPatientChangedCallbackSet) GetProcAddress(detail::hinstLib, "setPatientsChangedCallback");
    libErrorCallbackSet = (PtrFncErrorCallbackSet) GetProcAddress(detail::hinstLib, "setErrorCallback");
    libStateChangeCallbackSet =
        (PtrFncStateChangeCallbackSet) GetProcAddress(detail::hinstLib, "setStateChangedCallback");

    libDateTimeAsString = (PtrFncDateTimeAsString) GetProcAddress(detail::hinstLib, "dateTimeAsString");

    libFreeString = (PtrFncFreeString) GetProcAddress(detail::hinstLib, "freeString");
    libFreeStringArray = (PtrFncFreeStringArray) GetProcAddress(detail::hinstLib, "freeStringArray");
    libFindDevices = (PtrFncFindDevices) GetProcAddress(detail::hinstLib, "findDevices");
    libSetDevice = (PtrFncSetDevice) GetProcAddress(detail::hinstLib, "setDevice");
    libGetVitejteIp = (PtrFncGetVitejteIP) GetProcAddress(detail::hinstLib, "getVitejteIP");
    libGetVitejteID = (PtrFncGetVitejteID) GetProcAddress(detail::hinstLib, "getVitejteID");
    libGetInfo = (PtrFncGetInfo) GetProcAddress(detail::hinstLib, "getInfo");
    libStartPingTimer = (PtrFncStartPingTimer) GetProcAddress(detail::hinstLib, "startPingTimer");
    libStopPingTimer = (PtrFncStopPingTimer) GetProcAddress(detail::hinstLib, "stopPingTimer");
    libRegisterClient = (PtrFncRegisterClient) GetProcAddress(detail::hinstLib, "registerClient");
    libUnRegisterClient = (PtrFncUnRegisterClient) GetProcAddress(detail::hinstLib, "unRegisterClient");
    libGetPatients = (PtrFncGetPatients) GetProcAddress(detail::hinstLib, "getPat");
    libFreepatients = (PtrFncFreePatients) GetProcAddress(detail::hinstLib, "freePatients");
    libClearPatient = (PtrFncClearPatient) GetProcAddress(detail::hinstLib, "clearPatient");
  }
  return std::nullopt;
}

inline void freeLibrary() {
  if (detail::isVitejteLibLoaded) {
    detail::isVitejteLibLoaded = false;
    FreeLibrary(detail::hinstLib);
    libCreateCommunicationHandle = nullptr;
    libDeleteCommunicationHandle = nullptr;
    libCreateCallbackHandle = nullptr;
    libDeleteCallbackHandle = nullptr;
    libSetCallbackForComm = nullptr;
    libPatientChangedCallbackSet = nullptr;
    libErrorCallbackSet = nullptr;
    libStateChangeCallbackSet = nullptr;
    libFreeString = nullptr;
    libFreeStringArray = nullptr;
    libFindDevices = nullptr;
    libSetDevice = nullptr;
    libGetVitejteIp = nullptr;
    libGetVitejteID = nullptr;
    libGetInfo = nullptr;
    libStartPingTimer = nullptr;
    libStopPingTimer = nullptr;
    libRegisterClient = nullptr;
    libGetPatients = nullptr;
    libFreepatients = nullptr;
  }
}
}// namespace vitejte
#endif// LIB_TEST_LIBINTERFACE_H
