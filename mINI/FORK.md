# Maintained mINI fork

This is the Facta Vera-maintained version of mINI. Its sources are tracked directly by the parent `libs_cpp_forked` repository, rather than by a nested repository or submodule.

## Origin

- Upstream project: https://github.com/pulzed/mINI
- Imported baseline: `d1502838ac8a925cdce63c4e0f6fa0ae03cb3281` (2019-11-29, "Add copy test").
- Upstream version: 0.9.7.
- Original author: Danijel Durakovic (pulzed).
- Original license and copyright notices remain in `LICENSE` and the source.

The upstream commit history is retained as an ancestor of this repository's import commit. The former nested Git metadata is archived locally beneath the parent repository's `.git/migration-backups/`; that archive is not needed by a fresh clone. Future changes belong in `libs_cpp_forked` and its own origin remote. Upstream improvements can be reviewed and adapted individually.

## Local implementation

The maintained version adds a Unicode/BOM and line-storage layer, UTF-16/32 conversion, filesystem paths, encoding/newline properties, selectable case-sensitive and case-insensitive types, and RAD Studio library/test projects. It retains much of the original parser and formatting-preserving writer logic.

It is a compiled library, with eight headers and three implementation files under `src/mINI`. Consumers include `mINI/ini.h`, select a structure type, and instantiate the matching file type:

```cpp
using INIStructure = mINI::INIStructureCI;
using INIFile = mINI::INIFile<INIStructure>;
```

Case-insensitive structures fold ASCII letters only. Other bytes, including UTF-8 sequences, are compared exactly; Unicode case folding is not performed. Hashing and equality use the same locale-independent rule, and the first inserted spelling is retained.

The original `README.md` remains as upstream documentation; its header-only installation and fixed `mINI::INIStructure` examples do not describe the maintained API.

## Building

The library requires C++17 filesystem support and Boost.Locale conversion headers. The implementation files are `ini_common.cpp`, `ini_parser.cpp`, and `unicode_utils.cpp`. Add this project's `src` directory to the compiler include path and link the resulting mINI library.

`mINI.cbproj` builds the library with RAD Studio; `tests/mini_tests.groupproj` contains the IDE test projects. The test-project paths are relative to this checkout, and their library search path includes `$(BDSCOMMONDIR)\lib\$(Platform)\$(Config)`, matching the library project's output convention. Global RAD Studio include/library settings in consuming projects must also point to the new location when they previously referenced `libs_cpp/mINI`.

The eight automated test programs are `testcasesens`, `testcopy`, `testgenerate`, `testhuge`, `testread`, `testutf8`, `testwrite`, and `testregressions`. `testudl.cpp` is an older manual sample with machine-specific paths and an obsolete include, not part of the verified automated suite. The original `tests/build.*` scripts target the original header-only layout and are retained as historical documentation.

CMake 3.18 or newer provides a command-line build and runs each test in its own working directory. From this directory:

```sh
cmake -S . -B build -DMINI_BOOST_INCLUDE_DIR="path/to/boost"
cmake --build build
ctest --test-dir build --output-on-failure
```

Select a generator and compiler appropriate to your installation, for example `-G Ninja -DCMAKE_CXX_COMPILER=bcc64x`. For a multi-configuration generator, supply `--config Debug` to the build and `-C Debug` to CTest. CMake consumers can use `add_subdirectory()` and link the `mINI` target. It supplies the include paths, C++17 requirement, and definitions to disable implicit mINI/Boost.Locale linking; the UTF conversion routines used here require only Boost headers. RAD Studio's existing implicit mINI linking remains enabled outside this build unless `MINI_NO_AUTOLINK` is defined.

## Correctness improvements

- Generation honors `file_props` encoding and newline settings, creates empty files, reports open/close errors, and opens the destination directly rather than deleting it first.
- BOM probing handles short and BOM-only files; reads reject incomplete UTF-16/32 units and short stream reads. Typed buffers hold UTF-16/32 code units.
- Each read detects LF/CRLF independently. UTF conversion leaves the process locale unchanged, and moving line storage transfers its vector instead of copying it.
- Active includes match the `mINI` directory's spelling, and case-insensitive maps use portable hashing and equality with matching behavior for embedded NUL and non-ASCII bytes.

Regression coverage includes byte fixtures for both endian variants of UTF-16/32, Unicode generation/lazy writes, BOM-only files, newline detection, invalid output paths, short reads, locale preservation, and map lookup edge cases.

## Historical local snapshot

`src/mini__original_more_or_less` is an inactive intermediate copy retained for reference. It is already modified and is not the upstream baseline. Active library builds use `src/mINI`.
