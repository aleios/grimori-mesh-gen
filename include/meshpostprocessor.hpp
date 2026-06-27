#pragma once
#include "types.hpp"

class MeshPostprocessor {
public:
    struct Options {
        Platform platform;
        bool hflip;
        bool vflip;
    };

    explicit MeshPostprocessor(Options opts)
        : m_opts{opts}
    {}
    bool execute(std::vector<SubmeshData>& submeshes);
private:
    Options m_opts;
};
