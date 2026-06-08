#include "loaderDescriptor.h"

using namespace vax::objects;

LoaderDescriptor::ModelExtension LoaderDescriptor::getModelExtension() {
    auto dot = path.rfind('.');
    if (dot == std::string::npos)
        return ModelExtension::UNKNOWN;

    std::string ext = path.substr(dot + 1);
    for (auto& c : ext)
        c = std::tolower(c);

    if (ext == "glb")
        return ModelExtension::GLB;
    if (ext == "urdf")
        return ModelExtension::URDF;
    return ModelExtension::UNKNOWN;
}