#!/bin/env python

# Copyright (C) 2017
# swift Project Community/Contributors
#
# This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
# directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
# including this file, may be copied, modified, propagated, or distributed except according to the terms
# contained in the LICENSE file.

import getopt
import multiprocessing
import os
import os.path as path
import platform
import requests
import subprocess
import sys
import datastore
import symbolstore
from lib.util import get_vs_env

if sys.version_info < (3, 0):
    import ConfigParser as configparser
else:
    import configparser


class Builder:

    def prepare(self):
        """
        Prepares the build environment, e.g. setup of environmental variables.
        Build processes will be called with the environment modified by this function.
        """
        print('Preparing environment ...')
        os.environ['PATH'] += os.pathsep + self._get_qt_binary_path()
        self._specific_prepare()

        print('Updating from datastore ...')
        host = 'https://datastore.swift-project.org'
        datastore_version = self.__config.get('General', 'datastore_version')
        source_path = self._get_swift_source_path()
        shared_path = os.path.abspath(os.path.join(source_path, 'resources', 'share'))
        datastore.update_shared(host, datastore_version, shared_path)

    def build(self, jobs, dev_build):
        """
        Run the build itself. Pass dev_build=True to enable a dev build
        """
        print('Running build ...')
        build_path = self._get_swift_build_path()
        if not os.path.isdir(build_path):
            os.makedirs(build_path)
        os.chdir(build_path)
        qmake_call = ['qmake']
        if dev_build:
            qmake_call += ['"BLACK_CONFIG+=SwiftDevBranch"']
        qmake_call += ['-r', os.pardir]
        subprocess.check_call(qmake_call, env=dict(os.environ))

        if not jobs:
            jobs = multiprocessing.cpu_count()
        jobs_arg = '-j{0}'.format(jobs)
        subprocess.check_call([self.make_cmd, jobs_arg], env=dict(os.environ))

    def checks(self):
        """
        Runs build checks.
        """
        print('Running checks ...')
        build_path = self._get_swift_build_path()
        os.chdir(build_path)
        if self._should_run_checks():
            subprocess.check_call([self.make_cmd, 'check'], env=dict(os.environ))
            pass

    def install(self):
        """
        Installs all products to the default path.
        """
        print('Running install ...')
        build_path = self._get_swift_build_path()
        os.chdir(build_path)
        if self._should_publish():
            subprocess.check_call([self.make_cmd, 'create_installer'], env=dict(os.environ))
            pass
        else:
            subprocess.check_call([self.make_cmd, 'install'], env=dict(os.environ))
            pass

    def publish(self):
        if self._should_publish():
            os_map = {'Linux': 'linux', 'Darwin': 'macos', 'Windows': 'windows'}
            extension_map = {'Linux': 'run', 'Darwin': 'dmg', 'Windows': 'exe'}
            version_segments = self.version.split('.')
            lastSegment = version_segments.pop()
            version_without_timestamp = '.'.join(version_segments)
            installer_name_old = '-'.join(['swiftinstaller', os_map[platform.system()], self.word_size, version_without_timestamp])
            installer_name_new = '.'.join([installer_name_old, lastSegment])
            installer_name_old = installer_name_old + '.' + extension_map[platform.system()]
            installer_name_new = installer_name_new + '.' + extension_map[platform.system()]
            build_path = os.path.abspath(path.join(self._get_swift_build_path(), installer_name_old))
            dest_path = os.path.abspath(path.join(self._get_swift_build_path(), os.pardir, installer_name_new))
            os.rename(build_path, dest_path)

    def package_xswiftbus(self):
        """
        Packages xswiftbus as 7z compressed archive into the swift source root.
        """
        print('Packaging xswiftbus ...')
        build_path = self._get_swift_build_path()
        os.chdir(build_path)
        os_map = {'Linux': 'linux', 'Darwin': 'macos', 'Windows': 'windows'}
        archive_name = '-'.join(['xswiftbus', os_map[platform.system()], self.word_size, self.version]) + '.7z'
        archive_path = path.abspath(path.join(os.pardir, archive_name))
        content_path = path.abspath(path.join(os.curdir, 'dist', 'xswiftbus'))
        subprocess.check_call(['7z', 'a', '-mx=9', archive_path, content_path], env=dict(os.environ))

    def symbols(self, upload_symbols):
        """
        Generates the binary symbols and archives them into a gzip archive, located in the swift source root.
        """
        if self._should_create_symbols():
            build_path = self._get_swift_build_path()
            os.chdir(build_path)
            print('Creating symbols')
            symbol_path = path.abspath(path.join(build_path, 'symbols'))
            binary_path = path.abspath(path.join(build_path, 'out'))
            symbolstore.Dumper.global_init()
            dumper = symbolstore.get_platform_specific_dumper(dump_syms=self.dump_syms, symbol_path=symbol_path)
            dumper.process(binary_path)
            dumper.finish()
            os_map = {'Linux': 'linux', 'Darwin': 'macos', 'Windows': 'windows'}
            tar_filename = '-'.join(['swiftsymbols', os_map[platform.system()], self.word_size, self.version]) + '.tar.gz'
            tar_path = path.abspath(path.join(self._get_swift_source_path(), tar_filename))
            dumper.pack(tar_path)
            if upload_symbols:
                self.__upload_symbol_files(symbol_path)

    def _get_swift_source_path(self):
        return self.__source_path

    def _get_swift_build_path(self):
        return self.__build_path

    def _specific_prepare(self):
        pass

    def _get_qmake_spec(self):
        raise NotImplementedError()

    def _get_make_cmd(self):
        raise NotImplementedError()

    def _get_qt_component(self):
        raise NotImplementedError()

    def _should_run_checks(self):
        return True

    def _should_publish(self):
        return True

    def _should_create_symbols(self):
        return True

    def _get_qtcreator_path(self):
        qtcreator_path = path.abspath(path.join(self.qt_path, 'Tools', 'QtCreator', 'bin'))
        return qtcreator_path

    def _get_externals_path(self):
        qmake_spec = self._get_qmake_spec()
        lib_path = 'lib' + self.word_size
        return path.abspath(path.join(self._get_swift_source_path(), 'externals', qmake_spec, lib_path))

    def _get_qt_binary_path(self):
        component = self._get_qt_component()
        if self.word_size == '64':
            component += '_64'
        else:
            # special case for MSVC 32 bit component (most likely all versions). Those don't have the 32 bit suffix
            if "msvc" not in component:
                component += '_32'
        qt_binary_path = path.abspath(path.join(self.qt_path, self.qt_version, '{0}'.format(component), 'bin'))
        return qt_binary_path

    def _get_config(self):
        return self.__config

    def __init__(self, config_file, word_size):
        self.__source_path = path.abspath(path.join(os.path.dirname(os.path.realpath(__file__)), os.pardir))
        self.__build_path = path.abspath(path.join(self.__source_path, 'build'))

        swift_project_file = path.join(self.__source_path, 'swift.pro')
        if not os.path.isfile(swift_project_file):
            raise RuntimeError('Cannot find swift.pro! Are we in the right directory?')

        self.word_size = word_size

        if not config_file:
            config_file = path.abspath(path.join(self._get_swift_source_path(), 'scripts', 'jenkins.cfg'))
        self.__config = configparser.SafeConfigParser()
        self.__config.read(config_file)
        self.qt_version = self.__config.get('General', 'qt_version')
        self.qt_path = path.abspath(self.__config.get(platform.system(), 'qt_path'))
        self.dump_syms = path.abspath(self.__config.get(platform.system(), 'dump_syms'))

        self.make_cmd = self._get_make_cmd()
        self.version = self.__get_swift_version()

    def __get_swift_version(self):
        version_major = '0'
        version_minor = '0'
        version_micro = '0'
        version_file = path.abspath(
            path.join(self._get_swift_source_path(), 'mkspecs', 'features', 'version')) + '.pri'
        f = open(version_file)
        line = f.readline()
        while line:
            # Remove all spaces
            line = line.strip().replace(' ', '')
            tokens = line.split('=')
            if not len(tokens) == 2:
                raise ValueError('version.pri has wrong format!')
            if tokens[0] == 'BLACK_VER_MAJ':
                version_major = tokens[1]
            elif tokens[0] == 'BLACK_VER_MIN':
                version_minor = tokens[1]
            elif tokens[0] == 'BLACK_VER_MIC':
                version_micro = tokens[1]
            else:
                pass
            line = f.readline()
        f.close()

        # Converted from swift's CBuildConfig::lastCommitTimestampAsVersionSegment
        last_commit_timestamp = int(self.__get_last_commit_timestamp())
        year_offset = 201000000000
        last_commit_timestamp = last_commit_timestamp - year_offset
        version = '.'.join([version_major, version_minor, version_micro, str(last_commit_timestamp)])
        return version

    def __get_last_commit_timestamp(self):
        out = subprocess.check_output(['git', 'log', '-1', '--date=format:"%Y%m%d%H%M"', '--pretty=format:%cd'])
        return out.decode("utf-8").strip('"')

    def __upload_symbol_files(self, symbol_path):
        print('Uploading symbols')
        url = 'http://crashreports.swift-project.org/symfiles'

        symbol_files = [os.path.join(root, name)
                        for root, dirs, files in os.walk(symbol_path)
                        for name in files
                        if name.endswith('.sym')]

        for symbol_file in symbol_files:
            print ('Uploading ' + symbol_file)
            files = [
                ('symfile', open(symbol_file, 'rb')),
            ]
            data = {'release': self.version}
            r = requests.post(url, files=files, data=data)
            r.raise_for_status()


class MSVCBuilder(Builder):

    def _specific_prepare(self):
        os.environ['PATH'] += os.pathsep + self._get_externals_path()
        os.environ['PATH'] += os.pathsep + 'C:/Program Files/7-Zip'
        if self.word_size == '32':
            vs_env = get_vs_env('VS2017', 'x86')
        else:
            vs_env = get_vs_env('VS2017', 'amd64')

        os.environ.update(vs_env)

    def _get_qmake_spec(self):
        return 'win32-msvc2015'

    def _get_make_cmd(self):
        return path.abspath(path.join(self._get_qtcreator_path(), 'jom.exe'))

    def _get_qt_component(self):
        if self.word_size == '32':
            return 'msvc2015'
        elif self.word_size == '64':
            return 'msvc2017'
        else:
            raise RuntimeError('Illegal word size!')

    def __init__(self, config_file, word_size):
        Builder.__init__(self, config_file, word_size)


class MinGWBuilder(Builder):

    def _specific_prepare(self):
        os.environ['PATH'] += os.pathsep + self._get_externals_path()
        gcc_path = path.abspath(self._get_config().get(platform.system(), 'mingw_path'))
        os.environ['PATH'] += os.pathsep + gcc_path
        os.environ['PATH'] += os.pathsep + path.abspath(path.join('c:', os.sep, 'Program Files', '7-Zip'))

    def _get_qmake_spec(self):
        return 'win32-g++'

    def _get_make_cmd(self):
        return 'mingw32-make'

    def _get_qt_component(self):
        return 'mingw53'

    def _should_run_checks(self):
        return False

    def _should_publish(self):
        return True

    def _should_create_symbols(self):
        return False

    def __init__(self, config_file, word_size):
        Builder.__init__(self, config_file, word_size)


class LinuxBuilder(Builder):

    def _specific_prepare(self):
        os.environ['LD_LIBRARY_PATH'] = path.abspath(path.join(self._get_swift_build_path(), 'build', 'out', 'release', 'lib'))
        os.environ['LD_LIBRARY_PATH'] += os.pathsep + path.abspath(path.join(self._get_swift_source_path(), 'lib'))
        os.environ['LD_LIBRARY_PATH'] += os.pathsep + self._get_externals_path()

    def _get_qmake_spec(self):
        return 'linux-g++'

    def _get_make_cmd(self):
        return 'make'

    def _get_qt_component(self):
        return 'gcc'

    def __init__(self, config_file, word_size):
        Builder.__init__(self, config_file, word_size)


class MacOSBuilder(Builder):

    def _specific_prepare(self):
        os.environ['LD_LIBRARY_PATH'] = path.abspath(path.join(self._get_swift_build_path(), 'build', 'out', 'release', 'lib'))
        os.environ['LD_LIBRARY_PATH'] += os.pathsep + self._get_externals_path()

    def _get_qmake_spec(self):
        return 'macx-clang'

    def _get_make_cmd(self):
        return 'make'

    def _get_qt_component(self):
        return 'clang'

    def _should_create_symbols(self):
        return True

    def __init__(self, config_file, word_size):
        Builder.__init__(self, config_file, word_size)


def print_help():
    supported_compilers = {'Linux': ['gcc'],
                           'Darwin': ['clang'],
                           'Windows': ['msvc', 'mingw']
                           }
    compiler_help = '|'.join(supported_compilers[platform.system()])
    print('jenkins.py -c <config file> -w <32|64> -t <' + compiler_help + '> [-d]')


# Entry point if called as a standalone program
def main(argv):
    config_file = ''
    word_size = ''
    tool_chain = ''
    dev_build = False
    jobs = None
    upload_symbols = False

    try:
        opts, args = getopt.getopt(argv, 'hc:w:t:j:du', ['config=', 'wordsize=', 'toolchain=', 'jobs=', 'dev', 'upload'])
    except getopt.GetoptError:
        print_help()
        sys.exit(2)

    if len(opts) < 2 or len(opts) > 5:
        print_help()
        sys.exit(2)

    for opt, arg in opts:
        if opt == '-h':
            print_help()
            sys.exit()
        elif opt in ('-c', '--config'):
            config_file = path.abspath(arg)
            if not os.path.exists(config_file):
                print('Specified config file does not exist')
                sys.exit(2)
        elif opt in ('-w', '--wordsize'):
            word_size = arg
        elif opt in ('-t', '--toolchain'):
            tool_chain = arg
        elif opt in ('-j', '--jobs'):
            jobs = arg
        elif opt in ('-d', '--dev'):
            dev_build = True
        elif opt in ('-u', '--upload'):
            upload_symbols = True

    if word_size not in ['32', '64']:
        print('Unsupported word size. Choose 32 or 64')
        sys.exit(2)

    builders = {'Linux': {
                    'gcc': LinuxBuilder},
                'Darwin': {
                    'clang': MacOSBuilder},
                'Windows': {
                    'msvc': MSVCBuilder,
                    'mingw': MinGWBuilder
                }
    }

    if tool_chain not in builders[platform.system()]:
        print('Unknown or unsupported tool chain!')
        sys.exit(2)

    builder = builders[platform.system()][tool_chain](config_file, word_size)

    builder.prepare()
    builder.build(jobs, dev_build)
    builder.checks()
    builder.install()
    builder.publish()
    builder.package_xswiftbus()
    builder.symbols(upload_symbols)


# run main if run directly
if __name__ == "__main__":
    main(sys.argv[1:])
