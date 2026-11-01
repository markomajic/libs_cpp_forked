# Boost compression filters for C++Builder Win64x

Build both static and DLL versions of the Boost.Iostreams bzip2, zlib and gzip implementations with RAD Studio 13's `bcc64x.exe`, Embarcadero's patched CMake 4.1+, and Ninja. Existing bundled compression sources are retained.

From a RAD Studio / Facta Vera command prompt:

```bat
build.bat /64x /D
build.bat /64x /R
```

Each command builds **both linkage types** for the selected configuration. `build.bat` refreshes CMake's configuration and cleans that configuration before building. `make.bat` accepts the same arguments and builds incrementally. Both work from any current directory and return a nonzero exit code if configuration, compilation, linking, or archive verification fails. These scripts target Modern Win64x; they do not build legacy Win64 or Win32 libraries.

`fv_vars.bat` is called when `BDS` is not set. Set `BDSBOOSTROOT` to the Boost header directory and `BDSCOMMONDIR` to the common output directory when using another environment setup. Use Embarcadero's patched CMake, not a generic build without `set_embt_target()` support.

## Output directories

Paths below are under `%BDSCOMMONDIR%`:

| Artifact | Debug | Release |
|---|---|---|
| Static archives (`.a`) | `lib\Win64x\Debug` | `lib\Win64x\Release` |
| DLL import archives (`.dll.a`) | `lib\Win64x\Debug` | `lib\Win64x\Release` |
| Runtime DLLs (`.dll`) | `DLL\Win64x\Debug` | `DLL\Win64x\Release` |

For Boost 1.89 / Clang 20, the file-name stems are:

| Library | Debug stem | Release stem |
|---|---|---|
| bzip2 | `libboost_bzip2-mgw20-mt-d-x64-1_89` | `libboost_bzip2-mgw20-mt-x64-1_89` |
| zlib and gzip | `libboost_zlib-mgw20-mt-d-x64-1_89` | `libboost_zlib-mgw20-mt-x64-1_89` |

Append `.a`, `.dll.a`, or `.dll` to the stem for the corresponding artifact. There are six output files per configuration, plus DLL debug-symbol files.

Compiler major and Boost library version come from the selected toolchain/headers. Debug names contain `-d`; static and DLL versions both use the dynamic C++ runtime. Static and DLL wrappers are compiled separately so their export/import declarations are correct.

## Consuming the libraries

Application library search paths should contain `$(BDSCOMMONDIR)\lib\$(Platform)\$(Config)`. Retire obsolete copies of the same archives from earlier search directories so they cannot override the configuration-specific outputs.

Static linkage is Boost's default. For DLL linkage, compile consuming code with `BOOST_IOSTREAMS_DYN_LINK` and deploy the matching runtime DLLs beside the executable or make their configuration-specific directory available through the runtime search path. Apply the define to any of your own static libraries that compile the compression calls, then rebuild those libraries too. Do not define `BOOST_IOSTREAMS_SOURCE` in consumers; the Boost wrapper implementation files define it themselves.

The CMake targets are `libboost_bzip2` and `libboost_zlib` for static linkage, and `libboost_bzip2_dll` and `libboost_zlib_dll` for DLL linkage. The DLL targets propagate `BOOST_IOSTREAMS_DYN_LINK` to CMake consumers. Building both variants does not change any application's linkage settings.

## Verification

Source files are enumerated explicitly. The build checks each static/import archive with `llvm-nm` and fails if it defines a program entry point such as `main` or `WinMain`. bzip2's `mk251.c` is a separate test-data generator and must never be included in the library.

Optional end-to-end tests, after configuring either build directory:

```bat
cmake -S . -B build\Win64x\Debug -DBOOST_FILTERS_BUILD_TESTS=ON
cmake --build build\Win64x\Debug
ctest --test-dir build\Win64x\Debug --output-on-failure
```

CTest runs two executables: one linked statically and one linked through the DLL import archives. Both compress and restore binary data through Boost's bzip2, zlib and gzip filters, checking exact restored bytes. The DLL test's search path includes the new DLL output directory. Repeat with Release to check that configuration too.
