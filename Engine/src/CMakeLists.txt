set(SRC 
  Renderer/Config.cpp
  Renderer/RenderPass.cpp
  #Renderer/Pipeline.cpp
  Renderer/Semaphore.cpp
  Renderer/Fence.cpp
  Renderer/CommandBuffer.cpp
  Renderer/Renderer.cpp
  Renderer/RenderManager.cpp
  Core/Logger.cpp
  Core/Config.cpp
  Core/Context.cpp
)

add_library(V8-lib SHARED ${SRC})

target_include_directories(V8-lib PUBLIC ${CMAKE_SOURCE_DIR}/Engine/include)

find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(fmt REQUIRED)

target_link_libraries(V8-lib PUBLIC SDL2::SDL2 Vulkan::Vulkan fmt::fmt)
