# FreeImage: origin and version

- **Original project:** FreeImage, including the FreeImagePlus C++ wrapper.
- **Imported version:** **3.18.0**, released **July 31, 2018**. The bundled Win32 and Win64 DLLs report version `3.18.0.0`.
- **Upstream website:** https://freeimage.sourceforge.io/
- **Official source/binary downloads:** https://freeimage.sourceforge.io/download.html
- **Original source repository:** https://sourceforge.net/p/freeimage/code/ (CVS hosted by SourceForge).

This directory is an imported Windows distribution snapshot, rather than a Git checkout. It contains the FreeImage DLLs, import libraries and headers under `Dist`, the FreeImagePlus wrapper under `Wrapper/FreeImagePlus`, and examples/documentation. The precise original archive filename was not recorded.

Local additions include RAD Studio build/deployment integration, such as `emb_deploy.bat`, `deploy.py`, and the wrapper's `emb` build scripts. Preserve these alongside the imported files when moving the package to `libs_cpp_forked`.

Original license notices are retained in `license-fi.txt`, `license-gplv2.txt`, `license-gplv3.txt`, and the source files.
