set(SRC 
  Config.cpp
  Context.cpp
  Surface.cpp
  Device.cpp
)

add_library(VulkanRenderer SHARED ${SRC})

target_include_directories(VulkanRenderer PUBLIC ${CMAKE_SOURCE_DIR}/Engine/include)

find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)

target_link_directories(VulkanRenderer PRIVATE ${CMAKE_SOURCE_DIR}/build/lib)
target_link_libraries(VulkanRenderer PUBLIC SDL2::SDL2 Vulkan::Vulkan Core)
