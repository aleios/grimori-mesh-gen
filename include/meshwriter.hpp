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
    bool write(std::vector<uint8_t>& buffer, const MeshParser& parser);

private:
    Platform m_platform;

    bool writeToStream(std::ostream& os, const MeshParser& parser);
};
