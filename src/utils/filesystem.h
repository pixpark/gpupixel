#if _MSVC_LANG >= 201703L || __cplusplus >= 201703L && defined(__has_include)
// ^ Supports MSVC prior to 15.7 without setting /Zc:__cplusplus to fix
// __cplusplus _MSVC_LANG works regardless. But without the switch, the compiler
// always reported 199711L:
// https://blogs.msdn.microsoft.com/vcblog/2018/04/09/msvc-now-correctly-reports-__cplusplus/
#if __has_include( \
    <filesystem>)  // Two stage __has_include needed for MSVC 2015 and per https://gcc.gnu.org/onlinedocs/cpp/_005f_005fhas_005finclude.html
#define GPUPIXEL_USE_STD_FS

// Old Apple OSs don't support std::filesystem, though the header is available
// at compile time. In particular, std::filesystem is unavailable before
// macOS 10.15, iOS/tvOS 13.0, and watchOS 6.0.
#ifdef __APPLE__
#include <Availability.h>
// Note: This intentionally uses std::filesystem on any new Apple OS, like
// visionOS released after std::filesystem, where std::filesystem is always
// available. (All other __<platform>_VERSION_MIN_REQUIREDs will be undefined
// and thus 0.)
#if __MAC_OS_X_VERSION_MIN_REQUIRED &&                                       \
        __MAC_OS_X_VERSION_MIN_REQUIRED < 101500 ||                          \
    __IPHONE_OS_VERSION_MIN_REQUIRED &&                                      \
        __IPHONE_OS_VERSION_MIN_REQUIRED < 130000 ||                         \
    __TV_OS_VERSION_MIN_REQUIRED && __TV_OS_VERSION_MIN_REQUIRED < 130000 || \
    __WATCH_OS_VERSION_MAX_ALLOWED && __WATCH_OS_VERSION_MAX_ALLOWED < 60000
#undef GPUPIXEL_USE_STD_FS
#endif
#endif
#endif
#endif

#ifdef GPUPIXEL_USE_STD_FS
#include <filesystem>
namespace fs {
using namespace std::filesystem;
using ifstream = std::ifstream;
using ofstream = std::ofstream;
using fstream = std::fstream;
}  // namespace fs
#else
#include "ghc/filesystem.hpp"
namespace fs {
using namespace ghc::filesystem;
using ifstream = ghc::filesystem::ifstream;
using ofstream = ghc::filesystem::ofstream;
using fstream = ghc::filesystem::fstream;
}  // namespace fs
#endif
