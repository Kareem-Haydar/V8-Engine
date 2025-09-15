#pragma once

#include <cstdint>
#include <string>
#include <vector>

#ifdef _WIN32
  #define PLATFORM "Win32"
#endif

#ifdef __APPLE__
  #define PLATFORM "macOS"
#endif

#ifdef __linux__
  #define PLATFORM "Linux"
#endif

#define CHOOSE_BEST_DEVICE -1

namespace Core {
  struct Config {
    std::string appName;
    uint32_t appVersion;
    std::string engineName;
    uint32_t engineVersion;
    uint32_t apiVersion;
    bool enableValidationLayers;
    std::vector<const char*> validationLayers;
    std::vector<const char*> deviceExtensions;
    int physicalDeviceIndex = CHOOSE_BEST_DEVICE;
  };

  extern Config defaultConfig;
}
