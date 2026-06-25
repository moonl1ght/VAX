#include "modelsController.h"

using namespace vax::objects;

bool ModelsController::canAddDrawableInstance() const { return _drawableInstancesCount < _maxDrawableInstances; }

bool ModelsController::addDrawableInstance(uint32_t instanceCount) {
    if (!canAddDrawableInstance()) {
        return false;
    }
    _drawableInstancesCount += instanceCount;
    return true;
}

bool ModelsController::removeDrawableInstance(uint32_t instanceCount) {
    if (_drawableInstancesCount < instanceCount) {
        return false;
    }
    _drawableInstancesCount -= instanceCount;
    return true;
}

uint32_t ModelsController::drawableInstancesCount() const { return _drawableInstancesCount; }

uint32_t ModelsController::maxDrawableInstances() const { return _maxDrawableInstances; }

void ModelsController::resetDrawableInstancesCounter() { _drawableInstancesCount = 0; }