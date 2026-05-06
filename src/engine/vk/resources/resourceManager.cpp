#include "resourceManager.h"

using namespace vax;

void ResourceManager::cleanup() {
    _bufferManager.fullCleanup();
    _meshManager.fullCleanup();
    _textureManager.fullCleanup();
    _logger.debug("ResourceManager cleanup complete");
}