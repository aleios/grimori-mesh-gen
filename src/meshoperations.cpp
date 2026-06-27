#include "gmesh/meshoperations.hpp"
#include <algorithm>
#include <map>
#include <meshoptimizer.h>


namespace gmesh
{
    //constexpr uint32_t RestartStripSentinel = 0xFFFFFFFF;
    constexpr uint16_t RestartStripSentinel = 0xFFFF;
    bool stripify(std::vector<IndexType>& indicesOut, std::vector<StripInfo>& stripsOut, const std::vector<IndexType>& indices, const std::vector<Vertex>& vertices) {

        std::vector<IndexType> optimized_indices(indices.size());
        meshopt_optimizeVertexCache(optimized_indices.data(), indices.data(), indices.size(), vertices.size());

        const auto bound = meshopt_stripifyBound(optimized_indices.size());
        std::vector<IndexType> raw_strip(bound);

        const auto stripSize = meshopt_stripify(raw_strip.data(),
                    optimized_indices.data(), optimized_indices.size(),
                    vertices.size(), RestartStripSentinel);
        raw_strip.resize(stripSize);

        if (raw_strip.size() > std::numeric_limits<IndexType>::max()) {
            return false;
        }

        indicesOut.clear();
        stripsOut.clear();

        IndexType subStripStart = 0;
        bool isInStrip = false;

        for (const auto idx : raw_strip) {

            if (idx == RestartStripSentinel) {
                if (isInStrip && indicesOut.size() > subStripStart) {
                    stripsOut.emplace_back(
                        subStripStart,
                        static_cast<IndexType>(indicesOut.size()) - subStripStart
                    );
                }
                isInStrip = false;
                continue;
            }

            if (!isInStrip) {
                subStripStart = static_cast<IndexType>(indicesOut.size());
                isInStrip = true;
            }

            indicesOut.push_back(idx);
        }

        // Handle the remaining strip
        if (isInStrip && indicesOut.size() > subStripStart) {
            stripsOut.emplace_back(
                subStripStart,
                static_cast<IndexType>(indicesOut.size()) - subStripStart
            );
        }

        return true;
    }

    std::vector<Vertex> flatten(const std::vector<Vertex>& vertices, const std::vector<IndexType>& indices) {
        std::vector<Vertex> outVertices;
        outVertices.reserve(indices.size());
        for (auto& i : indices) {
            outVertices.push_back(vertices[i]);
        }
        return outVertices;
    }

    std::vector<BoneGroup> partitionByBone(const std::vector<IndexType>& indices, const std::vector<Vertex>& vertices, uint8_t maxBones) {

        std::map<std::vector<BoneIdType>, size_t> paletteToGroup;
        std::vector<BoneGroup> groups;

        // Go over each triangle in the mesh
        for (size_t i = 0; i + 2 < indices.size(); i += 3) {
            const auto& v0 = vertices[indices[i+0]];
            const auto& v1 = vertices[indices[i+1]];
            const auto& v2 = vertices[indices[i+2]];


        }
        return groups;
    }

    void flipUVs(std::vector<Vertex>& vertices, bool vflip, bool hflip) {
        if (!vflip && !hflip) {
            return;
        }
        std::ranges::for_each(vertices, [=](auto& v) {
            v.uv = { hflip ? 1.0f - v.uv.x : v.uv.x, vflip ? 1.0f - v.uv.y : v.uv.y };
        });
    }
}