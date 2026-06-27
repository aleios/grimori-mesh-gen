#pragma once
#include "types.hpp"

namespace gmesh
{
    bool stripify(std::vector<IndexType>& indicesOut, std::vector<StripInfo>& stripsOut, const std::vector<IndexType>& indices, const std::vector<Vertex>& vertices);

    std::vector<Vertex> flatten(const std::vector<Vertex>& vertices, const std::vector<IndexType>& indices);

    std::vector<BoneGroup> partitionByBone(const std::vector<IndexType>& indices, const std::vector<Vertex>& vertices, uint8_t maxBones);

    void flipUVs(std::vector<Vertex>& vertices, bool vflip, bool hflip);
}