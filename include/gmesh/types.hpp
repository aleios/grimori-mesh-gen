#pragma once
#include <cstdint>
#include <vector>
#include <string>

namespace gmesh
{
    enum class Platform
        : std::uint8_t {
        Dreamcast,
        N3DS,
        PSP,
        PC
    };

    struct Vec4 {
        float x, y, z, w;
    };

    struct Vec3 {
        float x, y, z;
    };

    struct Vec2 {
        float x, y;
    };

    using IndexType = std::uint16_t;
    using PositionType = Vec3;
    using UVType = Vec2;
    using ColorType = std::uint32_t;

    using WeightType = float;
    using BoneIdType = std::uint8_t;

    using StripsType = std::uint16_t;

    using MaterialIndexType = std::uint8_t;

    using MaterialType = std::string;

    struct StripInfo {
        StripsType start;
        StripsType count;
    };

    struct Vertex {
        PositionType position;
        UVType uv;
        ColorType color;
        BoneIdType boneId[2];
        WeightType weight;
    };

    struct BoneGroup {
        std::vector<BoneIdType> boneIds;
        std::vector<IndexType> indices;
    };

    static constexpr MaterialIndexType InvalidMaterialIndex = ~0;
    struct SubmeshData {
        std::vector<Vertex> vertices{};
        std::vector<IndexType> indices{};
        std::vector<StripInfo> strips{};
        MaterialIndexType materialIndex{InvalidMaterialIndex}; //< Using it for dividing meshes up and for ident in the engine to retrieve the real material file.
        bool isSkinned{false};
    };

    static constexpr char magicNum[] = { 'A', 'E', 'M', 'F' };
    static constexpr uint16_t version = 1;
}