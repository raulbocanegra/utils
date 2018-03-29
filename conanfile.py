from conans import ConanFile, CMake

class UtilsConan(ConanFile):
    name = "Utils"
    version = "0.0.1"
    author = "Raul Bocanegra Algarra (raul.bocanegra.algarra@gmail.com.com)"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    default_options = "Boost:shared=True", "google-benchmark:shared=False"

    def requirements(self):
        self.requires("boost/1.66.0@conan/stable")
        self.requires("OpenSSL/1.1.0g@conan/stable")
        #self.requires("jsonformoderncpp/3.1.2@vthiery/stable")

    def build_requirements(self):
        self.build_requires("gtest/1.8.0@bincrafters/stable")
        self.build_requires("google-benchmark/1.3.0@rboc/testing")

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin") # From bin to bin
        self.copy("*.so", dst="bin", src="bin") # From bin to bin
        self.copy("*.dylib*", dst="bin", src="lib") # From lib to bin

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
