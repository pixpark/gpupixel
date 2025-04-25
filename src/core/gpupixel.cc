#include "gpupixel/gpupixel.h"
#include "utils/util.h"

namespace gpupixel {

void GPUPixel::SetResourcePath(const std::string& path) {
  Util::SetResourcePath(fs::path(path));
}
}  // namespace gpupixel
