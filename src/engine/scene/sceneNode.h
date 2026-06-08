#pragma once

#include "drawableModel.h"
#include <vector>

namespace vax::objects {
class SceneNode final {
  public:
    std::vector<SceneNode> children;
    std::vector<DrawableModel> drawableModels;

    explicit SceneNode(std::string name, bool isRoot = false)
        : _name(std::move(name))
        , _isRoot(isRoot) {};
    ~SceneNode() = default;

    SceneNode(const SceneNode& other) = default;
    SceneNode& operator=(const SceneNode& other) = default;

    SceneNode(SceneNode&& other) noexcept = default;
    SceneNode& operator=(SceneNode&& other) = default;

    bool isRoot() const { return _isRoot; };
    const std::string& name() const { return _name; };
    bool hasDrawableModels() const { return !drawableModels.empty(); };

  private:
    std::string _name;
    bool _isRoot = false;
};
} // namespace vax::objects