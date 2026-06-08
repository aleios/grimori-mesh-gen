#pragma once
#include <string>
#include "types.hpp"

class MeshParser;
class MeshWriter {
public:
    explicit MeshWriter(Platform platform)
        : m_platform(platform) {
    }
    bool write(const std::string& filePath, const MeshParser& parser);

private:
    Platform m_platform;
};
