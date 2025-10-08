#include <Renderer/RenderManager.h>

void V8_RenderManager::CreateRenderer(const std::string& name, const char* vertexShaderPath, const char* fragmentShaderPath, const V8_RenderPassDescription& renderPassDesc, const V8_RenderConfig& config) {
  renderers_[name].Init(*context_, vertexShaderPath, fragmentShaderPath, renderPassDesc, config);
}

V8_Renderer* V8_RenderManager::GetRenderer(const std::string& name) {
  if (renderers_.find(name) == renderers_.end())
    return nullptr;

  return &renderers_[name];
}

void V8_RenderManager::RemoveRenderer(const std::string& name) {
  renderers_.erase(name);
}

void V8_RenderManager::Render(const std::string& id) {
  if (context_->needsResize_) return;

  renderers_[id].Render();
}

void V8_RenderManager::RenderAll() {
  if (context_->needsResize_) return;

  for (auto& [id, renderer] : renderers_)
    renderer.Render();
}

void V8_RenderManager::HandleResize() {
  for (auto& [_, renderer] : renderers_)
    renderer.HandleResize();
}
