#pragma once
#include "types.hpp"

bool stripify(std::vector<IndexType>& indicesOut, std::vector<StripInfo>& stripsOut, const std::vector<IndexType>& indices, const std::vector<Vertex>& vertices);

void flatten(std::vector<Vertex>& outVertices, const std::vector<Vertex>& vertices, const std::vector<IndexType>& indices);

std::vector<BoneGroup> partitionByBone(const std::vector<IndexType>& indices, const std::vector<Vertex>& vertices, uint8_t maxBones);