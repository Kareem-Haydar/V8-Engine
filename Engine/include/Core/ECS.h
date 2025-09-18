#pragma once

#include <unordered_map>
#include <vector>
#include <typeindex>
#include <memory>
#include <cassert>

namespace Core {
  using Entity = uint32_t;

  class ECS {
    private:
      Entity nextEntity_ = 1;
      std::vector<Entity> entities_;
      std::unordered_map<std::type_index, std::unordered_map<Entity, std::shared_ptr<void>>> components_;

    public:
      Entity CreateEntity() {
        Entity id = nextEntity_++;
        entities_.push_back(id);
        return id;
      }

      void DestroyEntity(Entity id) {
        entities_.erase(std::remove(entities_.begin(), entities_.end(), id), entities_.end());
        for (auto& [type, compMap] : components_) {
          compMap.erase(id);
        }
      }

      template<typename T, typename... Args>
      void AddComponent(Entity id, Args&&... args) {
        auto comp = std::make_shared<T>(std::forward<Args>(args)...);
        components_[typeid(T)][id] = comp;
      }

      template<typename T>
      T* GetComponent(Entity id) {
        auto it = components_.find(typeid(T));
        if (it != components_.end()) {
          auto compIt = it->second.find(id);
          if (compIt != it->second.end()) {
            return static_cast<T*>(compIt->second.get());
          }
        }
        return nullptr;
      }

      template<typename T>
      bool HasComponent(Entity id) {
        auto it = components_.find(typeid(T));
        return it != components_.end() && it->second.count(id);
      }

      const std::vector<Entity>& GetEntities() const {
        return entities_;
      }
  };
}
