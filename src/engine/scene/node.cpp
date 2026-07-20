#include "node.h"
#include "sceneNode.h"
#include "shaderSharedUtils.h"
#include "transform.h"

using namespace vax::engine;
using namespace vax;
using namespace vax::math;

void Node::draw(const DrawContext& drawContext) {
    if (_type == NodeType::SCENE) {
        auto scene = static_cast<SceneNode*>(this);
        scene->resetDrawingRangeIndex();
    }
    for (size_t i = 0; i < _instancesCount; ++i) {
        TransformMatrixHandle worldHandle;
        auto& instanceInfo = _instanceInfos[i];
        worldHandle.updateModelMatrix(
            instanceInfo._parentTransformMatrices.getModelMatrix() * instanceInfo._transformHandle.getModelMatrix()
        );
        bool isSelected = instanceInfo.isSelected || _isSelected;

        InstanceData instanceData = {
            .model = worldHandle.getModelMatrix(),
            .normalMatrix = worldHandle.getNormalMatrix(),
            .packedColor = instanceInfo.selectionPackedColor,
            .flags = static_cast<uint32_t>(
                isSelected ? InstanceFlags::IsInstanceSelected : InstanceFlags::InstanceFlagsNone
            ),
            .instanceId = _nodeId == NO_ID ? NO_ID : static_cast<uint32_t>(_nodeId + i),
        };
        switch (_type) {
        case NodeType::SCENE: {
            auto scene = static_cast<SceneNode*>(this);
            scene->updateInstanceData(drawContext, instanceData, i);
        } break;
        default:
            break;
        }

        if (instanceInfo.isChildrenTransformDirty) {
            for (auto& child : _children) {
                // for now child instances should have at least the same count as the parent instances
                // TODO: maybe fix this
                const size_t chunkSize = child->_instancesCount / _instancesCount;
                for (size_t j = 0; j < chunkSize; ++j) {
                    auto& childTransformInfo = child->_instanceInfos[i * chunkSize + j];
                    childTransformInfo._parentTransformMatrices.updateModelMatrix(
                        worldHandle.getModelMatrix() *
                        childTransformInfo._originalParentRelativeTransform.getModelMatrix()
                    );
                    childTransformInfo.isChildrenTransformDirty = true;
                }
            }
        }
        _instanceInfos[i].isChildrenTransformDirty = false;
    }
    switch (_type) {
    case NodeType::SCENE: {
        auto scene = static_cast<SceneNode*>(this);
        scene->drawModels(drawContext);
    } break;
    case NodeType::CAMERA:
        break;
    }
    for (auto& child : _children) {
        child->draw(drawContext);
    }
}

void Node::insertChild(std::unique_ptr<Node> child) {
    _children.push_back(std::move(child));
    for (size_t i = 0; i < _instancesCount; ++i) {
        _instanceInfos[i].isChildrenTransformDirty = true;
    }
}

Node* Node::getChild(const std::string& name, int depth) {
    if (name == _name) {
        return this;
    }
    if (depth == 0) {
        return nullptr;
    }
    for (auto& child : _children) {
        auto result = child->getChild(name, depth == -1 ? depth : depth - 1);
        if (result) {
            return result;
        }
    }
    return nullptr;
}

void Node::setNodeId(uint32_t nodeId, bool propagate) {
    _nodeId = nodeId;
    if (propagate) {
        for (auto& child : _children) {
            child->setNodeId(nodeId, true);
        }
    }
}

void Node::setIsSelected(bool isSelected, bool propagate) {
    _isSelected = isSelected;
    if (propagate) {
        for (auto& child : _children) {
            child->setIsSelected(isSelected, true);
        }
    }
}

void Node::selectInstance(uint32_t instanceIndex) { _instanceInfos[instanceIndex].isSelected = true; }

void Node::setSelectionColor(uint32_t instanceIndex, Color color) {
    _instanceInfos[instanceIndex].selectionPackedColor = packRGBA(color);
}

void Node::setNodeSelectionColor(Color color, bool propagate) {
    for (size_t i = 0; i < _instancesCount; ++i) {
        _instanceInfos[i].selectionPackedColor = packRGBA(color);
    }
    if (propagate) {
        for (auto& child : _children) {
            child->setNodeSelectionColor(color, true);
        }
    }
}

void Node::unselectInstance(uint32_t instanceIndex) { _instanceInfos[instanceIndex].isSelected = false; }

void Node::unselectAllInstances() {
    for (size_t i = 0; i < _instancesCount; ++i) {
        _instanceInfos[i].isSelected = false;
    }
}