#pragma once
#include "meshparser.hpp"

class GltfParser
    : public MeshParser {
public:
    bool parse(const std::string& filePath) override;
};