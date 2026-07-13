from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.build import check_min_cppstd, can_run


class Dstu4145Conan(ConanFile):
    name = "dstu4145"
    version = "0.1.0"
    license = "GPL-3.0-only"
    url = "https://github.com/shamray/dstu4145"
    description = "DSTU 4145-2002 Ukrainian elliptic curve digital signature standard"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    exports_sources = "CMakeLists.txt", "cmake/*", "src/*", "test/*"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        self.requires("openssl/[>=3 <4]")

    def build_requirements(self):
        self.test_requires("gtest/[>=1.11.0]")

    def validate(self):
        check_min_cppstd(self, 17)

    def generate(self):
        CMakeDeps(self).generate()
        CMakeToolchain(self).generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        if can_run(self):
            cmake.test()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "dstu4145")
        self.cpp_info.set_property("cmake_target_name", "dstu4145::dstu4145")
        self.cpp_info.libs = ["dstu4145"]
