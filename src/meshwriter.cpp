#include "meshwriter.hpp"
#include "meshparser.hpp"
#include <fstream>
#include <sstream>

using namespace gmesh;

namespace {

    bool write_vec3(std::ostream& fs, const Vec3& v) {
        fs.write(reinterpret_cast<const char*>(&v.x), sizeof(float));
        fs.write(reinterpret_cast<const char*>(&v.y), sizeof(float));
        fs.write(reinterpret_cast<const char*>(&v.z), sizeof(float));
        return fs.good();
    }
    bool write_vec2(std::ostream& fs, const Vec2& v) {
        fs.write(reinterpret_cast<const char*>(&v.x), sizeof(float));
        fs.write(reinterpret_cast<const char*>(&v.y), sizeof(float));
        return fs.good();
    }
    bool write_str(std::ostream& fs, const std::string& str) {
        const uint16_t len = str.size();
        fs.write(reinterpret_cast<const char*>(&len), sizeof(uint16_t));
        fs.write(str.c_str(), len);
        return fs.good();
    }

    template<typename T>
    bool write_val(std::ostream& fs, const T& value) {
        fs.write(reinterpret_cast<const char*>(&value), sizeof(T));
        return fs.good();
    }

    template<typename T>
    bool write_val(std::ostream& fs, const T& value, std::streamsize size) {
        fs.write(reinterpret_cast<const char*>(&value), size);
        return fs.good();
    }
}

bool MeshWriter::write(const std::string& filePath, const MeshParser& parser) {

    std::ofstream fs(filePath, std::ios::binary);

    if (!fs.is_open())
        return false;

    return writeToStream(fs, parser);
}

bool MeshWriter::write(std::vector<uint8_t>& buffer, const MeshParser& parser) {
    std::ostringstream oss{std::ios::binary};

    if (!writeToStream(oss, parser)) {
        return false;
    }

    const auto& str = oss.str();
    buffer.assign(str.begin(), str.end());
    return true;
}

bool MeshWriter::writeToStream(std::ostream& os, const MeshParser& parser) {

    // Header: magicnum, version, platform id
    write_val(os, magicNum, sizeof(uint8_t)*4);
    write_val(os, version);
    write_val(os, m_platform, sizeof(uint8_t));

    const auto numMaterials = parser.getMaterialsCount();
    const auto numSubmeshes = parser.getSubmeshCount();
    write_val(os, numMaterials);
    write_val(os, numSubmeshes);

    // Materials
    for (const auto& material : parser.getMaterials()) {
        if (!write_str(os, material)) {
            return false;
        }
    }

    // Submeshes
    for (const auto& submeshes = parser.getSubmeshes(); const auto& submesh : submeshes) {

        // Mesh type
        uint8_t type = submesh.isSkinned ? 1 : 0;
        write_val(os, type);

        // Material index
        write_val(os, submesh.materialIndex);

        // Vertices
        // Dreamcast: Straight stream with no indices
        uint16_t numVertices = submesh.vertices.size();
        write_val(os, numVertices);
        for (const auto& [position, uv, color, boneId, weight] : submesh.vertices) {
            write_vec3(os, position);
            write_vec2(os, uv);
            write_val(os, color);

            if (submesh.isSkinned) {
                write_val(os, boneId[0]);
                write_val(os, boneId[1]);
                write_val(os, weight); // Reconstructed via 1.0f - w0;
            }
        }

        // Indices
        if (m_platform != Platform::Dreamcast) {
            uint16_t numIndices = submesh.indices.size();
            write_val(os, numIndices);
            for (const auto& index : submesh.indices) {
                write_val(os, index);
            }
        }

        // Bone groups
        if (m_platform != Platform::Dreamcast && submesh.isSkinned) {
            // TODO: Need to partition the meshes first.
        }

        // Strips
        uint16_t numStrips = submesh.strips.size();
        write_val(os, numStrips);
        for (const auto& [start, count] : submesh.strips) {
            write_val(os, start);
            write_val(os, count);
        }
    }

    return true;
}
