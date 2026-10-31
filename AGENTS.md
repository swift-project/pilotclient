<!--
 SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
 SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1
-->

# Agent guide

Instructions for AI coding assistants working in this repository.

## Orientation

- Read [docs/repo-map.md](docs/repo-map.md) first. It describes the top-level
  folders, the `src/` modules and the test layout.
- The project overview and licensing are in [README.md](README.md).

## Conventions

- C++20 with Qt 6. Match the surrounding code's style and naming.
- Formatting follows [.clang-format](.clang-format); static analysis rules are
  in [.clang-tidy](.clang-tidy).
- New source files go into the `CMakeLists.txt` of their folder.
- Source files carry an SPDX header. [REUSE.toml](REUSE.toml) covers files
  without one.
- Pull requests target the `main` branch.
