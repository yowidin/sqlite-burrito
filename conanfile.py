from conan import ConanFile
from conan.tools.files import copy, rmdir, load
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.build import check_min_cppstd, can_run
from conan.tools.env import VirtualRunEnv, Environment

import os
import re


class Recipe(ConanFile):
    name = "sqlite-burrito"
    description = "SQLite3 C++ wrapper"
    license = "MIT"
    url = "https://github.com/yowidin/sqlite-burrito"
    homepage = "https://github.com/yowidin/sqlite-burrito"
    package_type = "library"
    generators = 'CMakeToolchain', 'CMakeDeps', 'VirtualRunEnv'

    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
    }

    exports_sources = '*', '!.git/*', '!build/*', '!cmake-build-*'

    def validate(self):
        check_min_cppstd(self, "17")

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def set_version(self):
        if self.version:
            return

        cmake_contents = load(self, os.path.join(self.recipe_folder, 'CMakeLists.txt'))

        regex = r"^set\(SB_VERSION\s*(.*?)\)\s*$"
        m = re.search(regex, cmake_contents, re.MULTILINE)
        if not m:
            raise RuntimeError('Error extracting library version')

        self.version = m.group(1)

    def layout(self):
        cmake_layout(self, src_folder='.')

    def requirements(self):
        self.requires("sqlite3/3.49.1", transitive_headers=True, transitive_libs=True)
        self.test_requires("catch2/3.8.1")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

        if can_run(self):
            # All of this could be replaced by `cmake.ctest()` but currently on Windows this results in our own
            # library path not being added to the PATH variable and this not being found in shared builds.
            env = VirtualRunEnv(self)
            with env.vars().apply():
                if self.settings.os == "Windows":
                    lib_dir = os.path.join(self.build_folder, str(self.settings.build_type))
                    env_vars = Environment()
                    env_vars.prepend_path("PATH", lib_dir)

                    with env_vars.vars(self).apply():
                        cmake.ctest()
                else:
                    cmake.ctest()

    def package(self):
        copy(self, "LICENSE*", src=self.source_folder, dst=os.path.join(self.package_folder, "licenses"))
        cmake = CMake(self)
        cmake.install()
        rmdir(self, os.path.join(self.package_folder, "lib", "cmake"))

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "SQLiteBurrito")
        self.cpp_info.set_property("cmake_target_name", "SQLiteBurrito::library")
        self.cpp_info.libs = ["SQLiteBurrito"]
        self.cpp_info.requires = ["sqlite3::sqlite3"]
