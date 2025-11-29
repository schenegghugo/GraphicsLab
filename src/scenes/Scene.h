#pragma once
#include <string>

class Scene {
public:
    virtual ~Scene() = default;
    virtual void OnAttach() = 0;
    virtual void OnUpdate(float deltaTime) = 0;
    virtual void OnRender() = 0;
    virtual std::string GetName() const = 0;
};
