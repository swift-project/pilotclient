from conan import ConanFile
from conan.tools.cmake import cmake_layout


class SwiftRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("opus/1.3.1")

    def layout(self):
        cmake_layout(self)
