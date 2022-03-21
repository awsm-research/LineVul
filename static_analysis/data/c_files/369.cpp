ImageBitmapFactories::ImageBitmapLoader::ImageBitmapLoader(
ImageBitmapFactories& factory,
base::Optional<IntRect> crop_rect,
ScriptState* script_state,
const ImageBitmapOptions* options)
    : loader_(
FileReaderLoader::Create(FileReaderLoader::kReadAsArrayBuffer, this)),
factory_(&factory),
resolver_(ScriptPromiseResolver::Create(script_state)),
crop_rect_(crop_rect),
options_(options) {}
