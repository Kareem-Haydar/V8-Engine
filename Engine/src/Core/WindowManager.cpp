#include <Core/WindowManager.h>
#include <Core/Logger.h>

Core::WindowManager Core::windowManager = Core::WindowManager();

uint32_t Core::WindowManager::CreateWindow(const char* title, int width, int height, bool resizable) {
  uint32_t id = nextId_;

  windows_[id].Init(width, height, title, resizable);

  nextId_++;

  return id;
}
