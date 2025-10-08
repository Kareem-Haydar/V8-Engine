#pragma once

#include <unordered_map>
#include <typeindex>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <memory>

#define V8_INVALID_ENTITY std::numeric_limits<V8_Entity>::max()

using V8_Entity = uint32_t;

struct V8_EntityRegistry {
  std::vector<V8_Entity> entities_;
  std::unordered_map<V8_Entity, std::unordered_map<std::type_index, std::shared_ptr<void>>> components_;

  V8_Entity CreateEntity() {
    V8_Entity entity = entities_.size();
    entities_.push_back(entity);
    return entity;
  }

  void RemoveEntity(V8_Entity entity) {
    entities_.erase(std::remove(entities_.begin(), entities_.end(), entity), entities_.end());
    components_.erase(entity);
  }

  template<typename T>
  void AddComponent(V8_Entity entity, std::shared_ptr<T> component) {
    std::type_index type = std::type_index(typeid(T));
    components_[entity][type] = component;
  }

  template<typename T>
  void AddComponent(V8_Entity entity) {
    std::type_index type = std::type_index(typeid(T));
    components_[entity][type] = std::make_shared<T>();
  }

  template<typename T>
  T* GetComponent(V8_Entity entity) {
    auto components = components_.find(entity);
    if (components == components_.end())
      return nullptr;
    auto component = components->second.find(std::type_index(typeid(T)));
    if (component == components->second.end())
      return nullptr;
    return static_cast<T*>(component->second.get());
  }
};
