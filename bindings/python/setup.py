from setuptools import setup, Extension
from pybind11.setup_helpers import Pybind11Extension, build_ext
from pybind11 import get_cmake_dir
import pybind11
import os
import subprocess

# Get ICU library information using pkg-config
def get_icu_config():
    try:
        # Get ICU include directories
        icu_cflags = subprocess.check_output(['pkg-config', '--cflags', 'icu-uc', 'icu-i18n'], 
                                           universal_newlines=True).strip().split()
        # Get ICU library directories and libraries
        icu_libs = subprocess.check_output(['pkg-config', '--libs', 'icu-uc', 'icu-i18n'], 
                                         universal_newlines=True).strip().split()
        
        include_dirs = []
        library_dirs = []
        libraries = []
        
        for flag in icu_cflags:
            if flag.startswith('-I'):
                include_dirs.append(flag[2:])
        
        for flag in icu_libs:
            if flag.startswith('-L'):
                library_dirs.append(flag[2:])
            elif flag.startswith('-l'):
                libraries.append(flag[2:])
        
        return include_dirs, library_dirs, libraries
    except (subprocess.CalledProcessError, FileNotFoundError):
        # Fallback to common paths
        return ['/usr/include', '/usr/local/include'], ['/usr/lib', '/usr/local/lib'], ['icuuc', 'icui18n']

# Get the directory containing this script
script_dir = os.path.dirname(os.path.abspath(__file__))
project_root = os.path.join(script_dir, '..', '..')

# Get ICU configuration
icu_include_dirs, icu_library_dirs, icu_libraries = get_icu_config()

# Define the extension module
ext_modules = [
    Pybind11Extension(
        "unicode_confusables_py",
        [
            "unicode_confusables_py.cpp",
            "../../src/unicode_confusables.cpp",
            "../../src/unicode_confusables_data.cpp",
        ],
        include_dirs=[
            # Path to pybind11 headers
            pybind11.get_include(),
            # Path to project headers
            "../../include",
        ] + icu_include_dirs,
        library_dirs=icu_library_dirs,
        libraries=icu_libraries,
        language='c++',
        cxx_std=17,
    ),
]

setup(
    name="unicode_confusables",
    version="1.0.0",
    author="Unicode Confusables Library",
    author_email="",
    description="Python bindings for Unicode Confusables detection and normalization",
    long_description="Python bindings for Unicode Confusables detection and normalization",
    long_description_content_type="text/markdown",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    packages=["unicode_confusables"],
    zip_safe=False,
    python_requires=">=3.6",
    install_requires=["pybind11"],
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: C++",
        "Topic :: Text Processing :: Linguistic",
        "Topic :: Security",
    ],
    license="MIT",
)
