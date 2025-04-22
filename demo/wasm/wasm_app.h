#ifndef WASM_APP_H_
#define WASM_APP_H_

#include <stdint.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define WASM_EXPORT EMSCRIPTEN_KEEPALIVE
#else
#define WASM_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

WASM_EXPORT int Init(const char* resource_path);

WASM_EXPORT void Destroy();

WASM_EXPORT void SetBeautyParams(float smoothing, float whitening);

WASM_EXPORT void ProcessImage(const uint8_t* rgba_data, int width, int height);

#ifdef __cplusplus
}
#endif

#endif  // WASM_APP_H_
