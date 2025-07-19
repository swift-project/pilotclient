<!--
 SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
 SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1
-->

About
=====

_swift_ is a pilot client for virtual air traffic simulation networks,
such as VATSIM, supporting the following simulators and operating systems:

- Prepar3D (Windows)
- Flight Simulator X (Windows)
- Flight Simulator 2004 (Windows)
- Flight Simulator 2020 (Windows)
- X-Plane 11 & 12 (Windows, Linux, macOS)
- FlightGear (Windows, Linux, macOS)

Downloads
=========

[![Beta version](https://img.shields.io/github/v/release/swift-project/pilotclient?color=blue&label=beta)][1]
[![Alpha version](https://img.shields.io/github/v/release/swift-project/pilotclient?include_prereleases&label=alpha)][2]

[1]: https://github.com/swift-project/pilotclient/releases/latest
[2]: https://github.com/swift-project/pilotclient/releases

Resources
=========

[:globe_with_meridians: Website](https://swift-project.org/)

[:green_book: User guide](https://swift-project.org/documentation/)

[:wave: Discord](https://discord.gg/R7Atd9A)

Contributing
============

_swift_ is written in modern C++17 using the Qt framework.


We are open to any contribution! More information can be found [here](https://swift-project.org/home/getting_involved/).


[![Development version](https://img.shields.io/badge/version-0.15-blue)](.)
[![Build status](https://img.shields.io/github/actions/workflow/status/swift-project/pilotclient/build.yml?branch=main)][3]

[3]: https://github.com/swift-project/pilotclient/actions

Pull requests should be made against the `main` branch.

[:blue_book: Developer guide](https://github.com/swift-project/pilotclient/wiki)

[:orange_book: API documentation](https://apidocs.swift-project.org/)

Licensing
=========
swift is dual-licensed under the GPL-3.0-or-later or the swift pilot client license (a modified GPL-3.0-or-later license).
Official releases are published under the swift pilot client license.
This dual licensing is necessary because swift may, depending on the build settings, link to proprietary software that is not compatible with the GPL-3.0-or-later.
For example, this includes vatsimauth or simulator-specific connectors.
