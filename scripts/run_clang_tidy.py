#!/bin/env python

# SPDX-FileCopyrightText: Copyright (C) 2025 swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

"""This script runs clang-tidy only on files that have changed compared to the latest origin/main branch."""

import argparse

import utils
import os
import subprocess

def run_clang_tidy(build_path: str):
    src_path = utils.get_swift_source_path()
    os.chdir(src_path)

    result = subprocess.run(
        ['git', 'diff', '--name-only', 'origin/main...HEAD'],
        check=True,
        stdout=subprocess.PIPE,
        text=True
    )

    files = [line for line in result.stdout.splitlines() if (line.endswith('.cpp') or line.endswith('.h')) and not line.startswith('tests')]
    nproc = os.cpu_count()
    subprocess.run([
        'xargs',
        '-P', str(nproc),
        '-n', '1',
        '-r',
        'clang-tidy',
        '-p', build_path,
        '--warnings-as-errors', '*'
    ], input='\n'.join(files), text=True, check=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(prog="swift clang-tidy helper")
    parser.add_argument("--build-path", required=True, help='Path to build folder')
    args = parser.parse_args()

    run_clang_tidy(args.build_path)
