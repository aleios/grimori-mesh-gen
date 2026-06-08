#include "meshwriter.hpp"
#include "meshparser.hpp"
#include <fstream>

namespace {

    bool write_vec3(std::ofstream& fs, const Vec3& v) {
        fs.write(reinterpret_cast<const char*>(&v.x), sizeof(float));
        fs.write(reinterpret_cast<const char*>(&v.y), sizeof(float));
        fs.write(reinterpret_cast<const char*>(&v.z), sizeof(float));
        return fs.good();
    }
    bool write_vec2(std::ofstream& fs, const Vec2& v) {
        fs.write(reinterpret_cast<const char*>(&v.x), sizeof(float));
        fs.write(reinterpret_cast<const char*>(&v.y), sizeof(float));
        return fs.good();
    }
    bool write_str(std::ofstream& fs, const std::string& str) {
        const uint16_t len = str.size();
        fs.write(reinterpret_cast<const char*>(&len), sizeof(uint16_t));
        fs.write(str.c_str(), len);
        return fs.good();
    }

    template<typename T>
    bool write_val(std::ofstream& fs, const T& value) {
        fs.write(reinterpret_cast<const char*>(&value), sizeof(T));
        return fs.good();
    }

    template<typename T>
    bool write_val(std::ofstream& fs, const T& value, std::streamsize size) {
        fs.write(reinterpret_cast<const char*>(&value), size);
        return fs.good();
    }
}

bool MeshWriter::write(const std::string& filePath, const MeshParser& parser) {

    std::ofstream fs(filePath, std::ios::binary);

    if (!fs.is_open())
        return false;

    // Header: magicnum, version, platform id
    write_val(fs, magicNum, sizeof(uint8_t)*4);
    write_val(fs, version);
    write_val(fs, m_platform, sizeof(uint8_t));

    const auto numMaterials = parser.getMaterialsCount();
    const auto numSubmeshes = parser.getSubmeshCount();
    write_val(fs, numMaterials);
    write_val(fs, numSubmeshes);

    // Materials
    for (const auto& material : parser.getMaterials()) {
        if (!write_str(fs, material)) {
            return false;
        }
    }

    // Submeshes
    for (const auto& submeshes = parser.getSubmeshes(); const auto& submesh : submeshes) {

        // Mesh type
        uint8_t type = submesh.isSkinned ? 1 : 0;
        write_val(fs, type);

        // Material index
        write_val(fs, submesh.materialIndex);

        // Vertices
        // Dreamcast: Straight stream with no indices
        uint16_t numVertices = submesh.vertices.size();
        write_val(fs, numVertices);
        for (const auto& [position, uv, color, boneId, weight] : submesh.vertices) {
            write_vec3(fs, position);
            write_vec2(fs, uv);
            write_val(fs, color);

            if (submesh.isSkinned) {
                write_val(fs, boneId[0]);
                write_val(fs, boneId[1]);
                write_val(fs, weight); // Reconstructed via 1.0f - w0;
            }
        }

        // Indices
        if (m_platform != Platform::Dreamcast) {
            uint16_t numIndices = submesh.indices.size();
            write_val(fs, numIndices);
            for (const auto& index : submesh.indices) {
                write_val(fs, index);
            }
        }

        // Bone groups
        if (m_platform != Platform::Dreamcast && submesh.isSkinned) {
            // TODO: Need to partition the meshes first.
        }

        // Strips
        uint16_t numStrips = submesh.strips.size();
        write_val(fs, numStrips);
        for (const auto& [start, count] : submesh.strips) {
            write_val(fs, start);
            write_val(fs, count);
        }
    }

    return true;
}
