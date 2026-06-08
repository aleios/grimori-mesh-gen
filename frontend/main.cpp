#include <iostream>
#include <argparse/argparse.hpp>

#include "../include/types.hpp"
#include "gltfparser.hpp"
#include "meshwriter.hpp"
#include "../include/meshoperations.hpp"

int main(int argc, char** argv) {

    argparse::ArgumentParser program("mesh-gen");

    program.add_argument("input_filename").required();
    program.add_argument("output_filename").required();

    program.add_argument("-v", "--verbose").nargs(0).help("Enable verbose output").default_value(false);
    program.add_argument("-p", "--platform").help("Specify the target platform (dc, 3ds, psp)").required().choices(
        "dc", "3ds", "psp"
    );

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& e) {
        std::cout << program << std::endl;
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    auto inputFilename = program.get("input_filename");
    auto outputFilename = program.get("output_filename");

    // Select platform for build.
    std::string platform_str = program.get("platform");
    std::ranges::transform(platform_str, platform_str.begin(), [](unsigned char c) {
        return std::tolower(c);
    });

    auto currentPlatform = Platform::Dreamcast;
    if (program.get("platform") == "3ds") {
        currentPlatform = Platform::N3DS;
    } else if (program.get("platform") == "psp") {
        currentPlatform = Platform::PSP;
    }

    // Parse the input file
    GltfParser parser{};
    if (!parser.parse(inputFilename)) {
        std::cerr << "Failed to parse input file" << std::endl;
        return 1;
    }

    if (parser.getSubmeshCount() == 0) {
        std::cerr << "No mesh data found in input file" << std::endl;
        return 1;
    }

    // Process into strips
    // note; overwriting the original parser data.
    for (auto& submeshes = parser.getSubmeshes(); auto& submesh : submeshes) {
        std::vector<IndexType> indices;
        std::vector<StripInfo> strips;
        if (!stripify(indices, strips, submesh.indices, submesh.vertices)) {
            return 1;
        }

        if (currentPlatform == Platform::Dreamcast) {
            std::vector<Vertex> newVerts;
            flatten(newVerts, submesh.vertices, indices);
            submesh.vertices = newVerts; // TODO: bruh nah
        }

        submesh.indices = indices;
        submesh.strips = strips;
    }

    // Write out data.
    MeshWriter writer{currentPlatform};
    if (!writer.write(outputFilename, parser)) {
        std::cerr << "Failed to write output file" << std::endl;
        return 1;
    }

    return 0;
}