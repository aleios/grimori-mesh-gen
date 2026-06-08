# Grimori Mesh Gen

Domain specific mesh format for Grimori.

Probably not very useful for anything else really.

Outputs a glTF file as flat vertex and index data along with strip information.

Materials are not derived from the glTF file, only the name is taken and used as an index 
for the actual generated material at load time.

Static mesh only at the moment. Plans to export skeleton, animations, and bone group data for platforms with limited matrix palettes.

Current targets:
* Dreamcast
* 3DS
* PSP

Alignment and vertex data order handled by the engine so not present here.

Maybe some parts are useful to somebody.
