#include "gmesh/meshreader.hpp"
#include <cstring>
#include <fstream>
#include <sstream>

using namespace gmesh;

namespace {

    bool read_vec3(std::istream& is, Vec3& v) {
        is.read(reinterpret_cast<char*>(&v.x), sizeof(float));
        is.read(reinterpret_cast<char*>(&v.y), sizeof(float));
        is.read(reinterpret_cast<char*>(&v.z), sizeof(float));
        return is.good();
    }

    bool read_vec2(std::istream& is, Vec2& v) {
        is.read(reinterpret_cast<char*>(&v.x), sizeof(float));
        is.read(reinterpret_cast<char*>(&v.y), sizeof(float));
        return is.good();
    }

    bool read_str(std::istream& is, std::string& str) {
        uint16_t len = 0;
        is.read(reinterpret_cast<char*>(&len), sizeof(uint16_t));
        if (!is.good()) return false;
        str.resize(len);
        is.read(str.data(), len);
        return is.good();
    }

    template<typename T>
    bool read_val(std::istream& is, T& value) {
        is.read(reinterpret_cast<char*>(&value), sizeof(T));
        return is.good();
    }

    template<typename T>
    bool read_val(std::istream& is, T& value, std::streamsize size) {
        is.read(reinterpret_cast<char*>(&value), size);
        return is.good();
    }
}

bool MeshReader::read(std::string filename) {
    std::ifstream fs(filename, std::ios::binary);

    if (!fs.is_open())
        return false;

    return readFromStream(fs);
}

bool MeshReader::read(const std::vector<uint8_t>& buffer) {
    std::string str(buffer.begin(), buffer.end());
    std::istringstream iss(str, std::ios::binary);

    return readFromStream(iss);
}

bool MeshReader::readFromStream(std::istream& is) {

    m_materials.clear();
    m_submeshes.clear();

    // Header
    char magic[4]{};
    is.read(magic, sizeof(uint8_t) * 4);
    if (!is.good() || std::memcmp(magic, magicNum, 4) != 0)
        return false;

    uint16_t fileVersion = 0;
    if (!read_val(is, fileVersion))
        return false;
    if (fileVersion != version)
        return false;

    if (!read_val(is, m_platform, sizeof(uint8_t)))
        return false;

    uint8_t numMaterials = 0;
    uint8_t numSubmeshes = 0;
    if (!read_val(is, numMaterials))
        return false;
    if (!read_val(is, numSubmeshes))
        return false;

    // Materials
    m_materials.resize(numMaterials);
    for (auto& material : m_materials) {
        if (!read_str(is, material))
            return false;
    }

    // Submeshes
    m_submeshes.resize(numSubmeshes);
    for (auto& submesh : m_submeshes) {

        // Mesh type
        uint8_t type = 0;
        if (!read_val(is, type))
            return false;
        submesh.isSkinned = (type != 0);

        // Material index
        if (!read_val(is, submesh.materialIndex))
            return false;

        // Vertices
        uint16_t numVertices = 0;
        if (!read_val(is, numVertices))
            return false;

        submesh.vertices.resize(numVertices);
        for (auto& [position, uv, color, boneId, weight] : submesh.vertices) {
            if (!read_vec3(is, position))
                return false;
            if (!read_vec2(is, uv))
                return false;
            if (!read_val(is, color))
                return false;

            if (submesh.isSkinned) {
                if (!read_val(is, boneId[0]))
                    return false;
                if (!read_val(is, boneId[1]))
                    return false;
                if (!read_val(is, weight))
                    return false;
            }
        }

        // Indices
        if (m_platform != Platform::Dreamcast) {
            uint16_t numIndices = 0;
            if (!read_val(is, numIndices))
                return false;

            submesh.indices.resize(numIndices);
            for (auto& index : submesh.indices) {
                if (!read_val(is, index))
                    return false;
            }
        }

        // Bone groups
        if (m_platform != Platform::Dreamcast && submesh.isSkinned) {
            // TODO: Need to read partitioned meshes once writing is implemented.
        }

        // Strips
        uint16_t numStrips = 0;
        if (!read_val(is, numStrips))
            return false;

        submesh.strips.resize(numStrips);
        for (auto& [start, count] : submesh.strips) {
            if (!read_val(is, start))
                return false;
            if (!read_val(is, count))
                return false;
        }
    }

    return true;
}
