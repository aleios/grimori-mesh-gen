#include "gmesh/gltfparser.hpp"
#include <filesystem>
#include <iostream>
#include <numeric>
#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>

using namespace gmesh;

namespace
{
    void process_mesh(const fastgltf::Asset& asset, const fastgltf::Mesh& mesh, std::vector<SubmeshData>& submeshes) {

        for (auto& prim : mesh.primitives) {
            SubmeshData data{};

            // Material index validation
            if (prim.materialIndex.has_value()) {
                const auto idxVal = prim.materialIndex.value();
                if (idxVal > std::numeric_limits<MaterialIndexType>::max()) {
                    data.materialIndex = InvalidMaterialIndex;
                    std::cout << "Warning: Material index out of range, setting to InvalidMaterialIndex" << std::endl;
                } else {
                    data.materialIndex = static_cast<MaterialIndexType>(idxVal);
                }
            } else {
                data.materialIndex = InvalidMaterialIndex;
            }

            //
            // Load positions
            //
            auto* posIter = prim.findAttribute("POSITION");
            if (posIter == prim.attributes.end()) {
                //TODO: Probably want to exit instead of continue?
                std::cout << "Warning: Missing POSITION attribute for primitive, skipping submesh" << std::endl;
                continue;
            }
            auto& posAccessor = asset.accessors[posIter->accessorIndex];
            const auto vertexCount = posAccessor.count; // Count of total vertices equivalent to count of positions since requires one.
            data.vertices.resize(vertexCount);
            fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                asset, posAccessor,
                [&](fastgltf::math::fvec3 pos, size_t i) {
                    data.vertices[i].position = { pos.x(), pos.y(), pos.z() };
                });

            //
            // Load texcoords
            //
            auto* uvIter = prim.findAttribute("TEXCOORD_0");
            if (uvIter != prim.attributes.end()) {
                auto& uvAccessor = asset.accessors[uvIter->accessorIndex];
                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(
                    asset, uvAccessor,
                    [&](fastgltf::math::fvec2 uv, size_t i) {
                        data.vertices[i].uv = { uv.x(), uv.y() };
                    });
            }

            //
            // Load colours
            //
            auto* colIter = prim.findAttribute("COLOR_0");
            if (colIter != prim.attributes.end()) {
                auto& colAccessor = asset.accessors[colIter->accessorIndex];
                if (colAccessor.type == fastgltf::AccessorType::Vec4) {
                    fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec4>(
                        asset, colAccessor,
                        [&](fastgltf::math::fvec4 col, size_t i) {
                            const auto r = static_cast<uint8_t>(col.x() * 255.f);
                            const auto g = static_cast<uint8_t>(col.y() * 255.f);
                            const auto b = static_cast<uint8_t>(col.z() * 255.f);
                            const auto a = static_cast<uint8_t>(col.w() * 255.f);
                            data.vertices[i].color = (a << 24) | (b << 16) | (g << 8) | r;
                                //(a << 24) | (b << 16) | (g << 8) | r;
                        });
                } else if (colAccessor.type == fastgltf::AccessorType::Vec3) { // No alpha channel
                    fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                        asset, colAccessor,
                        [&](fastgltf::math::fvec3 col, size_t i) {
                            const auto r = static_cast<uint8_t>(col.x() * 255.f);
                            const auto g = static_cast<uint8_t>(col.y() * 255.f);
                            const auto b = static_cast<uint8_t>(col.z() * 255.f);
                            const auto a = static_cast<uint8_t>(255);
                            data.vertices[i].color = (a << 24) | (b << 16) | (g << 8) | r;
                        });
                }
            } else {
                for (auto& v : data.vertices) {
                    v.color = 0xFFFFFFFF;
                }
            }

            //
            // Index buffer
            //
            // TODO: Check if index buffer will overflow.
            if (prim.indicesAccessor.has_value()) {
                auto& idxAccessor = asset.accessors[prim.indicesAccessor.value()];
                data.indices.resize(idxAccessor.count);

                fastgltf::iterateAccessorWithIndex<uint32_t>(
                    asset, idxAccessor,
                    [&](uint32_t idx, size_t i) {
                        data.indices[i] = idx;
                    });
            } else { // Generate flat index buffer
                data.indices.resize(vertexCount);
                std::iota(data.indices.begin(), data.indices.end(), 0u);
            }

            submeshes.push_back(std::move(data));
        }
    }
}

bool GltfParser::parse(const std::string& filePath) {

    std::filesystem::path path(filePath);
    fastgltf::Parser parser;

    auto data = fastgltf::GltfDataBuffer::FromPath(path);
    if (data.error() != fastgltf::Error::None) {
        std::cerr << "Failed to load glTF file: " << filePath << std::endl;
        return false;
    }

    auto asset = parser.loadGltfBinary(data.get(), path.parent_path(), fastgltf::Options::None);
    if (asset.error() != fastgltf::Error::None) {
        std::cerr << "Failed to parse glTF file: " << filePath << std::endl;
        return false;
    }

    const auto& assetVal = asset.get();

    // Store material names for retrieval by the engine cache later.
    // Note: Not suing the properties out of the gltf. Doesn't have depth and alpha props/etc as far as can tell.
    for (auto& material : assetVal.materials) {
        m_materials.emplace_back(material.name.c_str());
    }

    int meshIdx = 0;
    for (auto& mesh : assetVal.meshes) {
        try {
            process_mesh(assetVal, mesh, m_submeshes);
        } catch (const std::exception& e) {
            std::cerr << "Failed to process mesh at index " << meshIdx << ": " << e.what() << std::endl;
            return false;
        }
        ++meshIdx;
    }

    return true;
}
