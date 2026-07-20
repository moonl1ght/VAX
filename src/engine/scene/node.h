#pragma once

#include "drawContext.h"
#include "transform.h"
#include <colorPalette.h>
#include <memory>
#include <unordered_map>
#include <variant>

namespace vax::engine {

enum class NodeType {
    SCENE,
    CAMERA,
};

class Node {
  public:
    using MetadataValue = std::variant<std::string, float, int, bool>;

    struct InstanceInfo final {
        vax::math::Transform _originalParentRelativeTransform;
        vax::math::TransformMatrixHandle _parentTransformMatrices;
        vax::math::TransformHandle _transformHandle;
        uint32_t selectionPackedColor = 0;
        bool isChildrenTransformDirty = false;
        bool isSelected = false;
    };

    explicit Node(NodeType type, std::string name, uint32_t instancesCount, bool isRoot)
        : _type(type)
        , _name(std::move(name))
        , _instancesCount(instancesCount)
        , _isRoot(isRoot) {
        _instanceInfos.reserve(instancesCount);
    }

    virtual ~Node() = default;

    Node(const Node& other) = delete;
    Node& operator=(const Node& other) = delete;

    Node(Node&& other) noexcept = default;
    Node& operator=(Node&& other) noexcept = default;

    NodeType getType() const { return _type; }

    void draw(const DrawContext& drawContext);

    bool isRoot() const { return _isRoot; };

    const std::string& name() const { return _name; };

    /// Accumulates transform
    template <typename T> void updateTransform(const T& updater, uint32_t instanceIndex = 0) {
        if (instanceIndex >= _instancesCount) {
            return;
        }
        updater(_instanceInfos[instanceIndex]._transformHandle);
        _instanceInfos[instanceIndex].isChildrenTransformDirty = true;
    }

    template <typename T> void updateTransforms(const T& updater) {
        for (uint32_t i = 0; i < _instancesCount; ++i) {
            updater(i, _instanceInfos[i]._transformHandle);
            _instanceInfos[i].isChildrenTransformDirty = true;
        }
    }

    const vax::math::Transform& getTransform(uint32_t instanceIndex = 0) const {
        return _instanceInfos[instanceIndex]._transformHandle.getTransform();
    }

    uint32_t nodeId() const { return _nodeId; }

    void setNodeId(uint32_t nodeId, bool propagate = true);

    bool isSelected() const { return _isSelected; }

    void setIsSelected(bool isSelected, bool propagate = true);

    /// @param depth - depth of the child, if -1 search from all children with max depth,
    ///                 if 0 will compare only with node itself.
    Node* getChild(const std::string& name, int depth = -1);

    void insertChild(std::unique_ptr<Node> child);

    const std::vector<std::unique_ptr<Node>>& children() const { return _children; }

    uint32_t instancesCount() const { return _instancesCount; }

    void setMetadata(const std::string& key, const MetadataValue& value) { _metadata[key] = value; }

    const std::optional<MetadataValue> getMetadata(const std::string& key) const {
        if (auto it = _metadata.find(key); it != _metadata.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    void selectInstance(uint32_t instanceIndex);

    void unselectInstance(uint32_t instanceIndex);

    void unselectAllInstances();

    void setNodeSelectionColor(Color color, bool propagate = true);

    void setSelectionColor(uint32_t instanceIndex, Color color);

  protected:
    std::string _name;
    std::vector<InstanceInfo> _instanceInfos;
    std::vector<std::unique_ptr<Node>> _children;
    std::unordered_map<std::string, MetadataValue> _metadata;
    NodeType _type;
    uint32_t _nodeId = NO_ID;
    uint32_t _instancesCount = 1;
    bool _isRoot = false;
    bool _isSelected = false;
};
} // namespace vax::engine