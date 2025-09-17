set(SRC 
  Logger.cpp
  Config.cpp
)

add_library(Core SHARED ${SRC})

target_include_directories(Core PUBLIC ${CMAKE_SOURCE_DIR}/Engine/include)

find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(fmt REQUIRED)

target_link_libraries(Core PUBLIC SDL2::SDL2 Vulkan::Vulkan fmt::fmt)
