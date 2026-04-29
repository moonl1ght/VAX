#include "submesh.h"

using namespace vax::objects;
using namespace vax;

void Submesh::debugPrint(const utils::Logger& logger) const {
    logger.debug(
        "Submesh: indexCount=",
        indexCount,
        ", firstIndex=",
        firstIndex,
        ", vertexOffset=",
        vertexOffset,
        ", materialIndex=",
        materialIndex
    );
}