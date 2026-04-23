#include "resourceManager.h"

using namespace vax;

bool ResourceManager::cleanup() {
    if (!_bufferManager.fullCleanup()) return false;
    _logger.debug("ResourceManager cleanup complete");
    return true;
}