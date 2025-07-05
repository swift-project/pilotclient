# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

from conan import ConanFile
from conan.tools.cmake import cmake_layout


class SwiftRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("opus/1.3.1")
        self.requires("libsodium/1.0.18")
        self.requires("libevent/2.1.12")
        if self.settings.os != "Linux":
                self.requires("dbus/1.15.8")

                # Transitive dependency of dbus
                self.requires("expat/2.7.1")

        self.requires("nlohmann_json/3.11.3")

        # External dependencies not managed via conan:
        # Qt
        # msgpack
        # qjsonwebtoken
        # simplecrypt
        # dbus (on linux only)

    def configure(self):
        self.options["libevent"].with_openssl = False
        self.options["libevent"].shared = True
        self.options["libsodium"].shared = True
        self.options["opus"].shared = True
        if self.settings.os != "Linux":
            self.options["dbus"].shared = True
            self.options["dbus"].message_bus = True

    def layout(self):
        cmake_layout(self)
