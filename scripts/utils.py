# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1


from os import path, pardir
import json
import subprocess
import argparse


def get_swift_source_path() -> str:
    return path.abspath(path.join(path.dirname(path.realpath(__file__)), pardir))


def get_swift_version() -> str:
    """
    :return: Full version number (for example "0.12.123")
    """
    return get_swift_version_base() + '.' + str(get_rev_count())


def get_swift_version_base() -> str:
    """
    :return: Base version number without revision (for example "0.12")
    """
    f = open(__get_version_file())
    config_json = json.load(f)
    f.close()
    version_major = config_json['version']['major']
    version_minor = config_json['version']['minor']
    return '.'.join([str(version_major), str(version_minor)])


def get_rev_count() -> int:
    """
    :return: Number of commits since the current major and minor version was set in version.json
    """
    this_version = get_swift_version_base()
    config_log = subprocess.check_output(['git', 'log', '--format=%H', __get_version_file()])
    for sha in config_log.decode('utf-8').split():
        json_data = subprocess.check_output(['git', 'show', sha + ':version.json'])
        config_json = json.loads(json_data.decode('utf-8'))
        version_major = config_json['version']['major']
        version_minor = config_json['version']['minor']
        if this_version == '.'.join([str(version_major), str(version_minor)]):
            base_commit = sha
        else:
            break
    count = subprocess.check_output(['git', 'rev-list', '--count', 'HEAD', '^' + base_commit])
    return int(count.decode('utf-8'))


def __get_version_file() -> str:
    """
    :return: Path to the version.json
    """
    return path.abspath(path.join(get_swift_source_path(), 'version')) + '.json'


if __name__ == '__main__':
    parser = argparse.ArgumentParser('swift utils')
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('-v', '--version', action='store_true', help='Print the swift version')
    group.add_argument('--rev', action='store_true', help='Print the revision number (aka the last number of the '
                                                          'version')
    group.add_argument('--base', action='store_true',
                       help='Print the base version number (aka the first two numbers of '
                            'the version')

    args = parser.parse_args()

    if args.version:
        print(get_swift_version())
    elif args.rev:
        print(get_rev_count())
    elif args.base:
        print(get_swift_version_base())
