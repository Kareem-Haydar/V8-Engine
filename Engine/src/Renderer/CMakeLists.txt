set(SRC 
  Config.cpp
  Context.cpp
  Device.cpp
  Surface.cpp
  Swapchain.cpp
  RenderPass.cpp
  Pipeline.cpp
  Semaphore.cpp
  Fence.cpp
  CommandPool.cpp
  CommandBuffer.cpp
  RenderManager.cpp
  Renderer.cpp
  WindowManager.cpp
)

add_library(Renderer SHARED ${SRC})

target_include_directories(Renderer PUBLIC ${CMAKE_SOURCE_DIR}/Engine/include)

find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)

target_link_directories(Renderer PRIVATE ${CMAKE_SOURCE_DIR}/build/lib)
target_link_libraries(Renderer PUBLIC SDL2::SDL2 Vulkan::Vulkan Core)
