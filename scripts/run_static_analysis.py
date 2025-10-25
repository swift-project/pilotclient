#!/bin/env python

# SPDX-FileCopyrightText: Copyright (C) 2025 swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

"""This script runs static analysis tools only on files that have changed compared to the latest origin/main branch."""

import argparse
import json
import os
import subprocess
from subprocess import CalledProcessError

import utils

# Currently we are not checking all directories as they might need to run compilation first for Qt UIC
CHECK_DIRECTORIES = [
    "src/core",
    "src/misc",
    "src/input",
    "src/sound",
]


def _get_all_files(build_path: str) -> list[str]:
    """Get all files inside the compile commands."""
    src_path = utils.get_swift_source_path()
    os.chdir(src_path)

    with open(os.path.join(build_path, "compile_commands.json"), 'r') as f:
        commands = json.load(f)
    commands = set([os.path.relpath(entry["file"], utils.get_swift_source_path()) for entry in commands])

    commands = [command for command in commands if
                not command.startswith("third_party") and not command.startswith(build_path)]
    return commands


def _get_all_files_ci(build_path: str) -> list[str]:
    """Get all files in the compile commands and which will be checked in CI runs."""
    src_path = utils.get_swift_source_path()
    os.chdir(src_path)

    with open(os.path.join(build_path, "compile_commands.json"), 'r') as f:
        commands = json.load(f)
    commands = set([os.path.relpath(entry["file"], utils.get_swift_source_path()) for entry in commands])

    commands = [command for command in commands if command.startswith(tuple(CHECK_DIRECTORIES))]
    return commands


def _get_changed_files_ci(build_path: str) -> set[str]:
    """Get all files in the compile commands which has changed since the last main commit and are in the included directories."""
    src_path = utils.get_swift_source_path()
    os.chdir(src_path)

    result = subprocess.run(
        ['git', 'diff', '--name-only', 'origin/main'],
        check=True,
        stdout=subprocess.PIPE,
        text=True
    )
    with open(os.path.join(build_path, "compile_commands.json"), 'r') as f:
        commands = json.load(f)
    commands = set([os.path.relpath(entry["file"], utils.get_swift_source_path()) for entry in commands])

    files = set([line for line in result.stdout.splitlines() if
                 (line.endswith('.cpp') or line.endswith('.h')) and line.startswith(tuple(CHECK_DIRECTORIES))])
    return files & commands


def _has_changed_files() -> bool:
    src_path = utils.get_swift_source_path()
    os.chdir(src_path)

    result = subprocess.run(
        ['git', 'diff', '--name-only', 'origin/main'],
        check=True,
        stdout=subprocess.PIPE,
        text=True
    )

    return len([line for line in result.stdout.splitlines() if
                (line.endswith('.cpp') or line.endswith('.h')) and line.startswith(tuple(CHECK_DIRECTORIES))]) > 0


def run_clang_tidy(build_path: str, changed_source_files: set[str]):
    print(f"Run clang-tidy on files: {changed_source_files}")
    nproc = 10
    try:
        subprocess.run([
            'xargs',
            '-P', str(nproc),
            '-n', '1',
            '-r',
            'clang-tidy',
            '-p', build_path,
            '--warnings-as-errors', '*',
            '--quiet',
            '--header-filter', f'{utils.get_swift_source_path()}/src/',
        ], input='\n'.join(changed_source_files), text=True, check=True)
    except CalledProcessError:
        print("Clang-tidy finished with errors")
        exit(1)


def run_clazy(build_path: str, changed_source_files: set[str]):
    print(f"Run clazy on files: {changed_source_files}")
    nproc = os.cpu_count()
    try:
        subprocess.run([
            'clazy-standalone',
            '-p', build_path,
            "-extra-arg", "-Werror",
            "-extra-arg", "-Wno-unnecessary-virtual-specifier",
            '--header-filter', '(config|core|gui|input|misc|plugins|sound|swiftcore|swiftdata|swiftguistandard|swiftlauncher|xswiftbus)/',
            *changed_source_files
        ], text=True, check=True)
    except CalledProcessError:
        print("Clazy finished with errors")
        exit(1)


def main():
    parser = argparse.ArgumentParser(prog="swift clang-tidy helper")
    parser.add_argument("--build-path", help='Path to build folder')

    check_mode = parser.add_mutually_exclusive_group(required=True)
    check_mode.add_argument("--all-files", action="store_true",
                            help="Run check on all files in the compile commands")
    check_mode.add_argument("--all-files-ci", action="store_true",
                            help="Run check on all files in the compile commands and which will be checked in CI runs.")
    check_mode.add_argument("--changed-files-ci", action="store_true",
                            help="Run check on all files in the compile commands which are changed since the last main commit and are in the included directories (no Qt UIC files).")
    check_mode.add_argument("--check-changed-files", action="store_true",
                        help="Check if files have changed for evaluation. Program exits with 0 if no files changed; with 1 otherwise")

    parser.add_argument("--clang-tidy", action="store_true",
                        help="Run clang-tidy checks")
    parser.add_argument("--clazy", action="store_true",
                        help="Run clazy checks")
    args = parser.parse_args()

    if args.all_files:
        source_files = _get_all_files(args.build_path)
    elif args.all_files_ci:
        source_files = _get_all_files_ci(args.build_path)
    elif args.changed_files_ci:
        source_files = _get_changed_files_ci(args.build_path)
    else:
        exit(1 if _has_changed_files() else 0)

    if args.clang_tidy:
        run_clang_tidy(args.build_path, source_files)
    elif args.clazy:
        run_clazy(args.build_path, source_files)


if __name__ == '__main__':
    main()
