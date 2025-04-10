#include "gpupixel/gpupixel.h"
#include "utils/util.h"

namespace gpupixel {

std::string GPUPixel::GetResourcePath(const std::string& name) {
  return Util::getResourcePath(name);
}

void GPUPixel::SetResourceRoot(const std::string& root) {
  Util::setResourceRoot(root);
}

}  // namespace gpupixel
