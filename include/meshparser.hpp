#pragma once
#include <string>
#include <vector>
#include "types.hpp"

class MeshParser {
public:
    virtual ~MeshParser() = default;
    virtual bool parse(const std::string& filePath) = 0;

    [[nodiscard]] const std::vector<SubmeshData>& getSubmeshes() const { return m_submeshes; }
    [[nodiscard]] std::vector<SubmeshData>& getSubmeshes() { return m_submeshes; }
    [[nodiscard]] std::uint8_t getSubmeshCount() const { return static_cast<std::uint8_t>(m_submeshes.size()); }

    [[nodiscard]] const std::vector<MaterialType>& getMaterials() const { return m_materials; }
    [[nodiscard]] std::vector<MaterialType>& getMaterials() { return m_materials; }
    [[nodiscard]] std::uint8_t getMaterialsCount() const { return static_cast<std::uint8_t>(m_materials.size()); }

protected:
    std::vector<MaterialType> m_materials;
    std::vector<SubmeshData> m_submeshes;
};