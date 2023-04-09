#!/bin/env python

# Copyright (C) 2023
# swift Project Community/Contributors
#
# This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
# directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
# including this file, may be copied, modified, propagated, or distributed except according to the terms
# contained in the LICENSE file.

import os
import subprocess


def should_file_formatted(file) -> bool:
    return file.endswith(".cpp") or file.endswith(".h")


def get_formattable_files(folder_name):
    excluded_dirs = ["LegacyData", "xplanemp2", "thirdparty", "external", "dsp", "g2clib"]
    all_files = []
    for root, dirs, files in os.walk(os.path.join(os.getcwd(), folder_name)):
        files_in_folder = [os.path.join(root, file)
                           for file in files if should_file_formatted(file)]
        all_files.extend(files_in_folder)
        for dir in excluded_dirs:
            if dir in dirs:
                dirs.remove(dir)

    return all_files


def is_swift_dir():
    cwd = os.getcwd()
    return os.path.isdir(os.path.join(cwd, "src")) and os.path.isdir(os.path.join(cwd, "resources"))


if not is_swift_dir():
    print("Script must be called from top level swift directory!")
    exit(1)


all_files = []

all_files.extend(get_formattable_files("samples"))
all_files.extend(get_formattable_files("tests"))
all_files.extend(get_formattable_files("src"))

all_files_string = " ".join(all_files)

exec = subprocess.run(["xargs", "clang-format", "--Werror", "--style=file:.clang-format",
                      "--dry-run"], input=all_files_string, text=True)
exit(exec.returncode)
