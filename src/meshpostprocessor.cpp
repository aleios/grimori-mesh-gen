#include "meshpostprocessor.hpp"

#include "meshoperations.hpp"

bool MeshPostprocessor::execute(std::vector<SubmeshData>& submeshes) {
    for (auto& submesh : submeshes) {
        std::vector<IndexType> indices;
        std::vector<StripInfo> strips;
        if (!stripify(indices, strips, submesh.indices, submesh.vertices)) {
            return false;
        }

        // Emit flat vertex array for dreamcast, no indirection via indices.
        if (m_opts.platform == Platform::Dreamcast) {
            submesh.vertices = flatten(submesh.vertices, indices);
        }

        flipUVs(submesh.vertices, m_opts.vflip, m_opts.hflip);

        submesh.indices = indices;
        submesh.strips = strips;
    }
    return true;
}
