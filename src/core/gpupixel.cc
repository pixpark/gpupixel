#include "gpupixel/gpupixel.h"
#include "utils/util.h"

namespace gpupixel {

std::string GPUPixel::GetResourcePath(const std::string& name) {
  return Util::GetResourcePath(name);
}

void GPUPixel::SetResourceRoot(const std::string& root) {
  Util::SetResourceRoot(root);
}
}  // namespace gpupixel
