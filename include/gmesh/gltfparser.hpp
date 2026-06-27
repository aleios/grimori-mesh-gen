#pragma once
#include "meshparser.hpp"

namespace gmesh
{
    class GltfParser
        : public MeshParser {
    public:
        bool parse(const std::string& filePath) override;
    };
}