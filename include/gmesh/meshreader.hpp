#pragma once
#include "types.hpp"

namespace gmesh
{
    class MeshReader {
    public:
        bool read(std::string filename);
        bool read(const std::vector<uint8_t>& buffer);

        [[nodiscard]] Platform platform() const { return m_platform; }
        [[nodiscard]] const std::vector<std::string>& materials() const { return m_materials; }
        [[nodiscard]] const std::vector<SubmeshData>& submeshes() const { return m_submeshes; }
    private:
        Platform m_platform{Platform::Dreamcast};
        std::vector<std::string> m_materials;
        std::vector<SubmeshData> m_submeshes;

        bool readFromStream(std::istream& is);
    };
}
