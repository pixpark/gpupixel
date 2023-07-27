---
outline: deep
---

# Docs
<br>
<br>

::: code-group
```js [index.ts]
Under construction Test
```
:::

因跨平台开发，安卓WebRTC之前在Ubuntu上编译，太麻烦，修改为Mac编译，针对M1，x86修改类似

### 代码准备

两份代码

```bash
fetch webrtc_android
fetch webrtc_ios

```

### Android目录

拷贝 `webrtc_android/third_party` 目录下的 android 开头的目录覆盖掉`webrtc_ios/third_party`

### R8

`webrtc_android/third_party/r8` -&gt; `webrtc_ios/third_party/r8`

###  NDK

下载mac版 ndk，替换 `webrtc_ios/third_party/android_ndk`, 注意保留里面的 `android_ndk` 目录中的 `BUILD.gn`

### JDK

手动下载 JDK, 需要11 或者以后的版本, 替换到 third\_party/jdk/current 目录下

###  LLVM

拷贝Mac版  
`android_ndk/toolchains/llvm/prebuilt/darwin-x86_64/lib64/clang/12.0.9/lib/linux`  
到  
`third_party/llvm-build/Release+Asserts/lib/clang/15.0.0/lib`

###  脚本修改

修改

```cpp
if (host_cpu == "x64") {
    android_host_arch = "x86_64"
} else if (host_cpu == "x86") {
    android_host_arch = "x86"
} else if (host_cpu == "arm64") {
    android_host_arch = "x86_64"
} else {
    assert(false, "Need Android toolchain support for your build CPU arch.")
}
```

## TODO
- [x] 123
- [ ] sdfsd
- [x] sdfjlksdjf
 

## Code Group
::: code-group

```js [config.js]
/**
 * @type {import('vitepress').UserConfig}
 */
const config = {
  // ...
}

export default config
```

```ts [config.ts]
import type { UserConfig } from 'vitepress'

const config: UserConfig = {
  // ...
}

export default config
```

:::


修改`third_party/ijar/BUILD.gn` 添加 `|| is_apple` 判断

```make
if (is_linux || is_chromeos || is_apple) {
  config("ijar_compiler_flags") {
    if (is_clang) {
      cflags = [

```

从 llvm 官网下载mac arm 15版本, 拷贝

```bash
llvm-nm
llvm-strip
llvm-objcopy
```

到

```bash
third_party/llvm-build/Release+Asserts/bin

```

若报错`CalledProcessError`  
注掉 `build/android/gyp/util/build_utils.py`

```shell
raise CalledProcessError(cwd, args, MSG.format(stream_string))

```

### 其他错误

到这因环境配置导致的编译错误已经很少了，如报错，具体问题具体分析，整体看修改的东西不是太多。

###  编译

```bash
gn gen out/armv8 --args='target_os="android" target_cpu="arm64" is_component_build=false is_debug=false rtc_enable_protobuf=false rtc_include_tests=false rtc_build_examples=false   rtc_enable_sctp=false rtc_enable_libevent=false rtc_build_tools=false disable_android_lint=false use_errorprone_java_compiler=false use_custom_libcxx=false android32_ndk_api_level=18'

```

### 疑问

NDK中有clang, llvm中有clang，究竟用哪个编译，两者关系是什么？

###  参考

1. <https://blog.csdn.net/liuwenchang1234/article/details/107559530>
2. <https://www.jianshu.com/p/a65545d37022>