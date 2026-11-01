# libs_cpp_forked

This repository preserves C++ library sources and custom library packages that we maintain independently of their original distribution. It provides a versioned backup and a structured way to recover the exact source, local modifications, and build setup used by Facta Vera projects.

It complements `libs_cpp`, where libraries generally remain separate upstream-based repositories that can continue to receive upstream updates through their existing Git workflow.

## Criteria for adding a project

A project belongs here when one or more of the following apply:

- **Substantial local divergence:** implementation, API, or architectural changes mean that updating from upstream requires reviewing and adapting individual changes rather than routinely pulling or merging releases.
- **Imported source without an upstream Git workflow:** the code was obtained as an archive, vendor snapshot, article attachment, or other source package, and its exact maintained version needs to be preserved in our own version control.
- **A custom maintained package:** selected sources, local integration code, and build scripts form a package that must be maintained and restored together, rather than as an ordinary checkout of one upstream project.

The decision is based on how we intend to maintain the project. A library does not need to be a substantial rewrite to qualify as an imported source package.

## Projects that remain in libs_cpp

Keep a project in `libs_cpp` when it remains primarily an upstream checkout and its local changes can be maintained as a small patch set or dedicated branch. Examples include:

- Unmodified upstream libraries.
- Minor compiler or standard-library compatibility fixes.
- RAD Studio project files, build scripts, or deployment collateral added around otherwise unchanged upstream sources.
- Small API extensions that can still be maintained alongside normal upstream updates.

Local changes in those repositories should still be committed and backed up through the appropriate personal fork or remote. Their exclusion from this collection does not mean that the changes are disposable.

## Preserving each project's context

Each included project should retain its original attribution and license notices, and document its source, imported version or baseline commit where known, local changes, and build prerequisites. Upstream fixes may still be incorporated selectively as part of maintaining our version.

Track source, useful tests, and the build or deployment files needed to reproduce the maintained package. Exclude generated binaries, object files, build caches, and personal IDE session state.

## Included projects

- [mINI](mINI/FORK.md): independently maintained INI library with Unicode support and a compiled, templated API; derived from pulzed/mINI 0.9.7.
