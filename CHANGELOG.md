GPUPixel Version: v1.3.0-beta
## What's New

1. 🔄 Replace the VNN face detection library with Mars-Face.

2. 📉 Further reduce the library size.

3. 🖥️ Use static linking for Mars-Face on Linux, macOS, and iOS.

4. 📱 Use dynamic linking for Mars-Face on Windows and Android.

5. 🐧 For Linux, the Mars-Face static library must be compiled using Debian 10.

---

📝 👀Both Linux and macOS platforms provide static libraries for all third-party dependencies, so if you want to experiment with WASM support, you can use these two platforms for compilation.