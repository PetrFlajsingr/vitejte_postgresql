//
// Created by Petr on 01.04.2020.
//

#ifndef LIB_TEST_VITEJTELIBFNCTYPES_H
#define LIB_TEST_VITEJTELIBFNCTYPES_H

namespace vitejte {
#pragma pack(push, 1)
struct RawPatient {
  int id;
  double registrationTime;
  double clearanceTime;
  const char *titlePre;
  const char *name;
  const char *lastName;
  const char *titlePost;
  const char *rc;
  const char *zp;
  const char *duvod;
  int idFronty;
  int state;
  const char *language;
  int cardType;
};
#pragma pack(pop)

using CommHandle = void *;
using CallbackHandle = void *;

// handle management
using FncCreateCommunicationHandle = __stdcall CommHandle();
using FncDeleteCommunicationHandle = __stdcall int(CommHandle);
using FncCreateCallbackHandle = __stdcall CallbackHandle();
using FncDeleteCallbackHandle = __stdcall int(CallbackHandle);
using PtrFncCreateCommunicationHandle = FncCreateCommunicationHandle *;
using PtrFncDeleteCommunicationHandle = FncDeleteCommunicationHandle *;
using PtrFncCreateCallbackHandle = FncCreateCallbackHandle *;
using PtrFncDeleteCallbackHandle = FncDeleteCallbackHandle *;
// string memory management
using FncFreeString = __stdcall void(char *&);
using FncFreeStringArray = __stdcall void(char **&, int);
using PtrFncFreeString = FncFreeString *;
using PtrFncFreeStringArray = FncFreeStringArray *;

using FncFindDevices = __stdcall int(CommHandle, char **&, int &);
using FncSetDevice = __stdcall int(CommHandle, char *);
using FncGetVitejteIP = __stdcall int(CommHandle, char *&);
using FncGetVitejteID = __stdcall int(CommHandle, char *&);
using FncGetInfo = __stdcall int(CommHandle, char *&);
using FncStartPingTimer = __stdcall int(CommHandle);
using FncStopPingTimer = __stdcall int(CommHandle);
using FncRegisterClient = __stdcall int(CommHandle, int, char *);
using FncUnRegisterClient = __stdcall int(CommHandle);
using FncGetPatients = __stdcall int(CommHandle, RawPatient **&, int &count);
using FncFreePatients = __stdcall int(RawPatient **, int count);
using FncClearPatient = __stdcall int(CommHandle, int);

using PtrFncFindDevices = FncFindDevices *;
using PtrFncSetDevice = FncSetDevice *;
using PtrFncGetVitejteIP = FncGetVitejteIP *;
using PtrFncGetVitejteID = FncGetVitejteID *;
using PtrFncGetInfo = FncGetInfo *;
using PtrFncStartPingTimer = FncStartPingTimer *;
using PtrFncStopPingTimer = FncStopPingTimer *;
using PtrFncRegisterClient = FncRegisterClient *;
using PtrFncUnRegisterClient = FncUnRegisterClient *;
using PtrFncGetPatients = FncGetPatients *;
using PtrFncFreePatients = FncFreePatients *;
using PtrFncClearPatient = FncClearPatient *;

/// callbacks
using FncSetCallbackForComm = __stdcall int(CommHandle, CallbackHandle);
using PtrFncSetCallbackForComm = FncSetCallbackForComm *;
using FncPatientChangedCallback = __stdcall void(void *);
using PtrFncPatientChangedCallback = FncPatientChangedCallback *;
using FncPatientChangedCallbackSet = __stdcall int(CallbackHandle, PtrFncPatientChangedCallback, void *);
using PtrFncPatientChangedCallbackSet = FncPatientChangedCallbackSet *;

using FncErrorCallback = __stdcall void(void *, const char *);
using PtrFncErrorCallback = FncErrorCallback *;
using FncErrorCallbackSet = __stdcall int(CallbackHandle, PtrFncErrorCallback, void *);
using PtrFncErrorCallbackSet = FncErrorCallbackSet *;

using FncStateChangeCallback = __stdcall void(void *, int, double);
using PtrFncStateChangeCallback = FncStateChangeCallback *;
using FncStateChangeCallbackSet = __stdcall int(CallbackHandle, PtrFncStateChangeCallback, void *);
using PtrFncStateChangeCallbackSet = FncStateChangeCallbackSet *;
}// namespace vitejte

#endif// LIB_TEST_VITEJTELIBFNCTYPES_H
