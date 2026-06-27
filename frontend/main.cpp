#include <iostream>
#include <argparse/argparse.hpp>

#include "gmesh/types.hpp"
#include "gmesh/gltfparser.hpp"
#include "gmesh/meshpostprocessor.hpp"
#include "gmesh/meshwriter.hpp"

using namespace gmesh;

int main(int argc, char** argv) {

    bool vflip, hflip, verbose;
    argparse::ArgumentParser program("mesh-gen");

    program.add_argument("input_filename").required();
    program.add_argument("output_filename").required();

    program.add_argument("-v", "--verbose").nargs(0).help("Enable verbose output").store_into(verbose);
    program.add_argument("-p", "--platform").help("Specify the target platform (dc, 3ds, psp)").required().choices(
        "dc", "3ds", "psp"
    );
    program.add_argument("--vflip-uv").nargs(0).help("Flip UV coordinates vertically").store_into(vflip);
    program.add_argument("--hflip-uv").nargs(0).help("Flip UV coordinates horizontally").store_into(hflip);

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
    } else if (program.get("platform") == "pc") {
        currentPlatform = Platform::PC;
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
    MeshPostprocessor postprocess{
        { currentPlatform, hflip, vflip }
    };
    if (!postprocess.execute(parser.getSubmeshes())) {
        return 1;
    }

    // Write out data.
    MeshWriter writer{currentPlatform};
    if (!writer.write(outputFilename, parser)) {
        std::cerr << "Failed to write output file" << std::endl;
        return 1;
    }

    return 0;
}