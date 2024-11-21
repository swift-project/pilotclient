#ifndef VATSIMAUTH_H
#define VATSIMAUTH_H

#if defined(_WIN32)
#  ifdef VATSIMAUTH_LIBRARY
#    define VATSIMAUTH_EXPORT __declspec(dllexport)
#  else
#    define VATSIMAUTH_EXPORT __declspec(dllimport)
#  endif
#else
#  define VATSIMAUTH_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <cstdint>

#ifdef __cplusplus
class vatsim_auth;
#else
typedef void *VatsimAuthentication;
#endif

VATSIMAUTH_EXPORT vatsim_auth *vatsim_auth_create(std::uint16_t clientId, const char *privateKey);
VATSIMAUTH_EXPORT void vatsim_auth_destroy(vatsim_auth *obj);

VATSIMAUTH_EXPORT std::uint16_t vatsim_auth_get_client_id(const vatsim_auth * obj);
VATSIMAUTH_EXPORT void vatsim_auth_set_initial_challenge(vatsim_auth *obj, const char *initialChallenge);
VATSIMAUTH_EXPORT void vatsim_auth_generate_response(vatsim_auth *obj, const char *challenge, char *response);
VATSIMAUTH_EXPORT void vatsim_auth_generate_challenge(const vatsim_auth *obj, char *challenge);
VATSIMAUTH_EXPORT void vatsim_get_system_unique_id(char *sysuid);

#ifdef __cplusplus
}
#endif

#endif // VATSIMAUTH_H
