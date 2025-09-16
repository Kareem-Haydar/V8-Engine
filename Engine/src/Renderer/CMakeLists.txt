set(SRC 
  Config.cpp
  Context.cpp
  Surface.cpp
  Device.cpp
  Swapchain.cpp
  RenderPass.cpp
  Pipeline.cpp
  Semaphore.cpp
  Fence.cpp
)

add_library(Renderer SHARED ${SRC})

target_include_directories(Renderer PUBLIC ${CMAKE_SOURCE_DIR}/Engine/include)

find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)

target_link_directories(Renderer PRIVATE ${CMAKE_SOURCE_DIR}/build/lib)
target_link_libraries(Renderer PUBLIC SDL2::SDL2 Vulkan::Vulkan Core)
