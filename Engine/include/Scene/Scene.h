#pragma once

#include <Scene/Types.h>
#include <Scene/Camera.h>
#include <Core/Entity.h>

struct V8_Scene {
  V8_Camera* cam;
  V8_EntityRegistry registry;

  V8_Scene() {}
};

struct V8_SceneManager {
  std::unordered_map<std::string, V8_Scene> scenes;

  void AddScene(const std::string& name) {
    scenes[name] = V8_Scene();
  }

  void BindCamera(const std::string& scene, V8_Camera* cam) {
    auto it = scenes.find(scene);
    if (it != scenes.end())
      it->second.cam = cam;
  }

  void UnbindCamera(const std::string& scene) {
    auto it = scenes.find(scene);
    if (it != scenes.end())
      it->second.cam = nullptr;
  }

  V8_Entity AddEntity(const std::string& scene) {
    if (scenes.find(scene) == scenes.end())
      return V8_INVALID_ENTITY;
    return scenes[scene].registry.CreateEntity();
  }

  template <typename T>
  void AddComponent(const std::string& scene, V8_Entity entity, std::shared_ptr<T> component) {
    if (scenes.find(scene) == scenes.end())
      return;

    scenes[scene].registry.AddComponent<T>(entity, component);
  }

  template <typename T>
  T* GetComponent(const std::string& scene, V8_Entity entity) {
    if (scenes.find(scene) == scenes.end())
      return nullptr;

    return scenes[scene].registry.GetComponent<T>(entity);
  }

  V8_Scene& GetScene(const std::string& name) {
    return scenes[name];
  }
};
