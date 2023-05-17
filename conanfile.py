from conan import ConanFile
from conan.tools.files import copy, rmdir
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.build import check_min_cppstd
import os


class Recipe(ConanFile):
    name = "sqlite-burrito"
    description = "SQLite3 C++ wrapper"
    license = "MIT"
    url = "https://github.com/yowidin/sqlite-burrito"
    homepage = "https://github.com/yowidin/sqlite-burrito"
    package_type = "library"
    version = "0.2.0"

    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared": [True, False],
        "fPIC": [True, False]
    }
    default_options = {
        "shared": False,
        "fPIC": True
    }

    exports_sources = '*', '!.git/*', '!build/*', '!cmake-build-*'

    @property
    def _minimum_cpp_standard(self):
        return 17

    def validate(self):
        if self.settings.compiler.cppstd:
            check_min_cppstd(self, self._minimum_cpp_standard)

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self, src_folder='.')

    def requirements(self):
        self.requires("sqlite3/3.41.2", transitive_headers=True, transitive_libs=True)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        copy(self, "LICENSE*", src=self.source_folder, dst=os.path.join(self.package_folder, "licenses"))
        cmake = CMake(self)
        cmake.install()
        rmdir(self, os.path.join(self.package_folder, "lib", "cmake"))

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "SQLiteBurrito")
        self.cpp_info.set_property("cmake_target_name", "SQLiteBurrito::library")
        self.cpp_info.libs = ["SQLiteBurrito"]
