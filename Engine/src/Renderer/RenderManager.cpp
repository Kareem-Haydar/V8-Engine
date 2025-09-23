#include <Renderer/RenderManager.h>

uint32_t Renderer::RenderManager::CreateRenderer(const char* vertexShaderPath, const char* fragmentShaderPath, const RenderPassDescription& renderPassDesc, const Config& config) {
  uint32_t id = nextId_++;
  renderers_[id].Init(*context_, vertexShaderPath, fragmentShaderPath, renderPassDesc, config);
  return id;
}

Renderer::Renderer* Renderer::RenderManager::GetRenderer(uint32_t id) {
  if (renderers_.find(id) == renderers_.end())
    return nullptr;

  return &renderers_[id];
}

void Renderer::RenderManager::RemoveRenderer(uint32_t id) {
  renderers_.erase(id);
}

void Renderer::RenderManager::Render(uint32_t id) {
  if (context_->needsResize_) return;

  renderers_[id].Render();
}

void Renderer::RenderManager::RenderAll() {
  if (context_->needsResize_) return;

  for (auto& [id, renderer] : renderers_)
    renderer.Render();
}

void Renderer::RenderManager::HandleResize() {
  for (auto& [_, renderer] : renderers_)
    renderer.HandleResize();
}
