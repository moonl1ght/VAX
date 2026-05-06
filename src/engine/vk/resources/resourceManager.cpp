#include "resourceManager.h"

using namespace vax;

void ResourceManager::cleanup() {
    _bufferManager.fullCleanup();
    _meshManager.fullCleanup();
    _textureManager.fullCleanup();
    _materialManager.cleanup();
    _logger.debug("ResourceManager cleanup complete");
}

bool ResourceManager::setup() {
    if (!_materialManager.setup()) return false;
    return true;
}