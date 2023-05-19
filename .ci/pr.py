#!/usr/bin/env python3

import os
import sys
from subprocess import run
from argparse import ArgumentParser


def do_run(*args):
    print(f"--- running: {' '.join(*args)} ---")
    sys.stdout.flush()
    run(*args, check=True)


def main():
    parser = ArgumentParser('Project builder')
    parser.add_argument('preset', help='Build preset')
    parser.add_argument('shared', help='Controls whether a shared library should be built')

    args = parser.parse_args()

    preset = args.preset
    preset_name = f'conan-{preset.lower()}'
    shared = args.shared

    source_dir = os.getcwd()
    build_dir = os.path.join(source_dir, 'build', args.preset)
    toolchain_path = os.path.join(build_dir, 'generators', 'conan_toolchain.cmake')

    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    do_run(['conan', 'install', '-b', 'missing', '-s', 'compiler.cppstd=17', '-s',
            f'build_type={preset}', '-c:h', 'tools.cmake.cmake_layout:build_folder_vars=["settings.build_type"]',
            source_dir])

    do_run(['cmake', '--preset', preset_name, '-DBUILD_TESTING=ON', f'-DBUILD_SHARED_LIBS={shared}',
            f'-DCMAKE_RUNTIME_OUTPUT_DIRECTORY={build_dir}'])

    do_run(['cmake', '--build', '--preset', preset_name,  '--config', preset])

    do_run(['ctest', '--test-dir', build_dir, '-C', preset, '--output-on-failure'])


if __name__ == "__main__":
    main()
