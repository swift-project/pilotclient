# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

from conan import ConanFile
from conan.tools.cmake import cmake_layout


class SwiftRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("opus/1.3.1")
        self.requires("libevent/2.1.12")

    def configure(self):
        self.options["libevent"].with_openssl = False

    def layout(self):
        cmake_layout(self)
