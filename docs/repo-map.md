<!--
 SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
 SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1
-->

Repository map
==============

A tool-independent overview of this repository: what lives where and how the
pieces relate. It is meant for new contributors and for any editor, IDE or
AI assistant that needs orientation. Keep it short and update it when a
top-level folder or a module's role changes.

Overview
--------

_swift_ is a C++20 / Qt 6 pilot client for virtual air traffic networks such
as VATSIM. The code builds several executables (the main GUI, a core-only
application, a data tool, a launcher) on top of shared libraries, plus
simulator plugins and an X-Plane bridge.

Top-level layout
----------------

| Path               | Contents                                                              |
|--------------------|-----------------------------------------------------------------------|
| `src/`             | All application and library source code (see below)                   |
| `tests/`           | Qt Test based unit and GUI tests, mirroring the `src/` modules        |
| `samples/`         | Small example programs that exercise individual libraries             |
| `third_party/`     | Vendored third-party code (DSP, crypto, JSON web token, msgpack)      |
| `resources/`       | Shared runtime resources (`resources/share`)                          |
| `cmake/`           | CMake modules and helper scripts                                      |
| `ci/`              | Build profiles per platform (Linux, macOS, macOS ARM, Windows)        |
| `scripts/`         | Python helpers: build, clang-format, static analysis, data store      |
| `installer/`       | Installer definitions and images                                      |
| `docs/`            | Doxygen configuration, main page and documentation assets             |
| `LICENSES/`        | License texts referenced by `REUSE.toml`                              |
| `.github/`         | GitHub Actions workflows (PR checks, merge queue, release, docs)      |

Build entry points: `CMakeLists.txt` and `CMakePresets.json` at the root,
`conanfile.py` for dependencies, and `version.json` for the version number.

Source modules (`src/`)
-----------------------

| Module          | Role                                                                  |
|-----------------|-----------------------------------------------------------------------|
| `misc/`         | Shared foundation: utilities, containers, aviation types, settings, crash handling, audio helpers |
| `core/`         | Core logic: application context, data model and database, network (FSD), audio/AFV, airspace, aircraft matching, input handling |
| `config/`       | Build configuration generation (`buildconfig.*` and `.in` templates)  |
| `gui/`          | Qt widgets and dialogs shared by the GUI applications (`components/`, `editors/`, `filters/`) |
| `input/`        | Keyboard, joystick and hotkey input per platform (`linux/`, `macos/`, `win/`) |
| `sound/`        | Audio: codecs, DSP, WAV handling, tone and SELCAL playback            |
| `plugins/`      | Simulator plugin implementations (`simulator/`)                       |
| `swiftguistandard/` | Main GUI application (executable)                                 |
| `swiftcore/`    | Core-only application (executable)                                    |
| `swiftdata/`    | Data management application (executable)                              |
| `swiftlauncher/`| Launcher application (executable)                                     |
| `xswiftbus/`    | X-Plane plugin bridge, built only with `SWIFT_BUILD_XSWIFTBUS`        |

Tests (`tests/`)
----------------

`tests/core`, `tests/gui` and `tests/misc` mirror the `src/` modules.
`tests/simpluginfsxp3d` is built only when a Prepar3D or MSFS plugin is
enabled. Tests use Qt Test and share the `tests_test` interface library
defined in `tests/test.h`.

Samples (`samples/`)
--------------------

Standalone programs such as `afvclient`, `fsd`, `hotkey`, `misc`,
`miscdbus`, `miscquantities` and `miscsim`. They are useful as minimal
examples of a single library and are not part of the shipped application.

Conventions
-----------

- Each folder with sources has its own `CMakeLists.txt`; add new sources there.
- Source files carry an SPDX header; `REUSE.toml` covers files without one.
- Formatting follows `.clang-format`; static analysis uses `.clang-tidy`.
- Pull requests target `main`.
