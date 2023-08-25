#!/usr/bin/env python

# SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

from __future__ import print_function

import itertools
import subprocess
import sys
import os


def validate_pair(ob):
    if not (len(ob) == 2):
        print("Unexpected result:", ob, file=sys.stderr)
        return False
    else:
        return True


def consume(it):
    try:
        while True:
            next(it)
    except StopIteration:
        pass


def get_environment_from_batch_command(env_cmd, initial=None):
    """
    Take a command (either a single command or list of arguments)
    and return the environment created after running that command.
    Note that if the command must be a batch file or .cmd file, or the
    changes to the environment will not be captured.

    If initial is supplied, it is used as the initial environment passed
    to the child process.
    """
    if not isinstance(env_cmd, (list, tuple)):
        env_cmd = [env_cmd]
    # Construct the command that will alter the environment.
    env_cmd = subprocess.list2cmdline(env_cmd)
    # Create a tag so we can tell in the output when the proc is done.
    tag = b'END OF BATCH COMMAND'
    # Construct a cmd.exe command to do accomplish this.
    cmd = 'cmd.exe /s /c "{env_cmd} && echo "{tag}" && set"'.format(**vars())
    # Launch the process.
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, env=initial)
    # Parse the output sent to stdout.
    lines = proc.stdout
    # Consume whatever output occurs until the tag is reached.
    consume(itertools.takewhile(lambda l: tag not in l, lines))
    # Define a way to handle each KEY=VALUE line.
    handle_line = lambda l: l.decode('utf-8').rstrip().split('=', 1)
    # Parse key/values into pairs.
    pairs = map(handle_line, lines)
    # Make sure the pairs are valid.
    valid_pairs = filter(validate_pair, pairs)
    # Construct a dictionary of the pairs.
    result = dict(valid_pairs)
    # Let the process finish.
    proc.communicate()
    return result


def get_vs_env(arch):
    """
    Returns the env object for VS building environment.

    The arch has to be one of "x86", "amd64", "arm", "x86_amd64", "x86_arm", "amd64_x86",
    "amd64_arm", i.e. the args passed to vcvarsall.bat.
    """

    # https://github.com/Microsoft/vswhere/wiki
    component = 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64'
    vswhere = os.environ['ProgramFiles(x86)'] + "\\Microsoft Visual Studio\\Installer\\vswhere.exe"
    vswhereCmd = [vswhere, '-latest', '-products', '*', '-requires', component, '-property', 'installationPath']
    vspath = subprocess.check_output(vswhereCmd).decode("utf-8").strip()

    vsvarsall = vspath + "\\VC\\Auxiliary\\Build\\vcvarsall.bat"
    return get_environment_from_batch_command([vsvarsall, arch])
