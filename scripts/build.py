#!/bin/env python

# Copyright (C) 2017
# swift Project Community/Contributors
#
# This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
# directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
# including this file, may be copied, modified, propagated, or distributed except according to the terms
# contained in the LICENSE file.

from datetime import date
import getopt
import json
import multiprocessing
import os
import os.path as path
import platform
import requests
import subprocess
import sys
import datastore
import tarfile
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

    def build(self, jobs, qmake_args, dev_build, eolInMonth):
        """
        Run the build itself. Pass dev_build=True to enable a dev build
        """
        print('Running build ...')
        build_path = self._get_swift_build_path()
        if not os.path.isdir(build_path):
            os.makedirs(build_path)
        os.chdir(build_path)

        qmake_call = ['qmake'] + qmake_args
        if dev_build:
            qmake_call += ['SWIFT_CONFIG.devBranch=true']

        if eolInMonth > 0:
            eolYear = date.today().year
            eolMonth = date.today().month + eolInMonth - 1
            eolYear = eolYear + ( eolMonth / 12 )
            eolMonth = eolMonth % 12 + 1
            eolDate = date(int(eolYear), int(eolMonth), 1)
            print('Setting EOL date to ' + eolDate.strftime('%Y%m%d'))
            qmake_call += ['SWIFT_CONFIG.endOfLife=' + eolDate.strftime('%Y%m%d')]

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
        # Do not even generate symbols if they aren't used. They got so big now, that we cannot afford to archive them in Jenkins
        if not upload_symbols:
            return
        
        if self._should_create_symbols():
            build_path = self._get_swift_build_path()
            os.chdir(build_path)
            print('Creating symbols')
            binary_path = path.abspath(path.join(build_path, 'out'))

            os_map = {'Linux': 'linux', 'Darwin': 'macos', 'Windows': 'windows'}
            tar_filename = '-'.join(
                ['swiftsymbols', os_map[platform.system()], self.word_size, self.version]) + '.tar.gz'
            tar_path = path.abspath(path.join(self._get_swift_source_path(), tar_filename))
            tar = tarfile.open(tar_path, "w:gz")

            ignore_list = ['sample', 'test', 'win', 'liblin.so', 'libmac.dylib']
            for root, dirs, files in os.walk(binary_path):
                if platform.system() == 'Windows':
                    for f in files:
                        if f.endswith('.pdb') and not f.startswith(tuple(ignore_list)):
                            symbol_path = path.abspath(path.join(root, f))
                            print('Adding ' + symbol_path)
                            tar.add(symbol_path, f)
                            if self.word_size == '64':
                                # Add also *.exe/*.dll with the same name if existing
                                exe_path = symbol_path.replace('.pdb', '.exe')
                                if os.path.isfile(exe_path):
                                    print('Adding ' + exe_path)
                                    tar.add(exe_path, f.replace('.pdb', '.exe'))
                                dll_path = symbol_path.replace('.pdb', '.dll')
                                if os.path.isfile(dll_path):
                                    print('Adding ' + dll_path)
                                    tar.add(dll_path, f.replace('.pdb', '.dll'))
                elif platform.system() == 'Darwin':
                    for d in dirs:
                        if d.endswith('.dSYM') and not d.startswith(tuple(ignore_list)):
                            symbol_path = path.abspath(path.join(root, d))
                            print('Adding ' + symbol_path)
                            tar.add(symbol_path, d)
                elif platform.system() == 'Linux':
                    for f in files:
                        if f.endswith('.debug') and not f.startswith(tuple(ignore_list)):
                            symbol_path = path.abspath(path.join(root, f))
                            print('Adding ' + symbol_path)
                            tar.add(symbol_path, f)
            tar.close()
            self.__upload_symbol_files(tar_path)

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
            config_file = path.abspath(path.join(self._get_swift_source_path(), 'scripts', 'build.cfg'))
        self.__config = configparser.ConfigParser()
        self.__config.read(config_file)
        self.qt_version = self.__config.get('General', 'qt_version')
        self.qt_path = path.abspath(self.__config.get(platform.system(), 'qt_path'))
        self.dump_syms = path.abspath(self.__config.get(platform.system(), 'dump_syms'))

        self.make_cmd = self._get_make_cmd()
        self.version = self.__get_swift_version()

    def __get_config_file(self):
        return path.abspath(path.join(self._get_swift_source_path(), 'default')) + '.json';

    def __get_swift_version(self):
        return self.__get_swift_version_base() + '.' + str(self.__get_rev_count())

    def __get_swift_version_base(self):
        f = open(self.__get_config_file())
        config_json = json.load(f)
        f.close()
        version_major = config_json['version']['major']
        version_minor = config_json['version']['minor']
        version_micro = config_json['version']['micro']
        return '.'.join([str(version_major), str(version_minor), str(version_micro)])

    def __get_rev_count(self):
        this_version = self.__get_swift_version_base()
        config_log = subprocess.check_output(['git', 'log', '--format=%H', self.__get_config_file()])
        for sha in config_log.decode("utf-8").split():
            json_data = subprocess.check_output(['git', 'show', sha + ':default.json'])
            config_json = json.loads(json_data.decode("utf-8"))
            version_major = config_json['version']['major']
            version_minor = config_json['version']['minor']
            version_micro = config_json['version']['micro']
            if this_version == '.'.join([str(version_major), str(version_minor), str(version_micro)]):
                base_commit = sha
            else:
                break
        count = subprocess.check_output(['git', 'rev-list', '--count', 'HEAD', '^' + base_commit])
        return int(count.decode("utf-8"))

    def __upload_symbol_files(self, symbols_package):
        print('Uploading symbols')
        url = 'https://swift-project.sp.backtrace.io:6098/post'
        token = '44166372034af002070a1f0ae3c8dfd8632e0210b36577c5269626fe5b87b2f6'

        data = open(symbols_package, 'rb').read()
        params = (
            ('format', 'symbols'),
            ('token', token),
        )
        r = requests.post(url, params=params, data=data)
        r.raise_for_status()


class MSVCBuilder(Builder):

    def _specific_prepare(self):
        os.environ['PATH'] += os.pathsep + self._get_externals_path() #TODO still needed? we copy externals anyway
        os.environ['PATH'] += os.pathsep + 'C:/Program Files/7-Zip'
        if self.word_size == '32':
            vs_env = get_vs_env('x86')
        else:
            vs_env = get_vs_env('amd64')

        os.environ.update(vs_env)

        # On Windows, the default Qt logger doesn't write to stderr, but uses
        # the Win32 API OutputDebugString instead, which Jenkins can't see.
        # This environment variable forces it to use stderr. It also forces
        # QPlainTestLogger::outputMessage to print to stdout.
        os.environ['QT_FORCE_STDERR_LOGGING'] = '1'

    def _get_qmake_spec(self):
        return 'win32-msvc'

    def _get_make_cmd(self):
        return "C:/ProgramData/chocolatey/lib/jom/tools/jom.exe"

    def _get_qt_component(self):
        return 'msvc2017'

    def __init__(self, config_file, word_size):
        Builder.__init__(self, config_file, word_size)


class MinGWBuilder(Builder):

    def _specific_prepare(self):
        os.environ['PATH'] += os.pathsep + self._get_externals_path()
        gcc_path = path.abspath(self._get_config().get(platform.system(), 'mingw_path'))
        os.environ['PATH'] += os.pathsep + gcc_path
        os.environ['PATH'] += os.pathsep + path.abspath(path.join('c:', os.sep, 'Program Files', '7-Zip'))

        # See comment in MSVCBuilder.
        os.environ['QT_FORCE_STDERR_LOGGING'] = '1'

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
    print('build.py -c <config file> -w <32|64> -t <' + compiler_help + '> [-d] [-e <end of life in month>] [-q <extra qmake argument>]')


# Entry point if called as a standalone program
def main(argv):
    config_file = ''
    word_size = ''
    tool_chain = ''
    dev_build = False
    jobs = None
    upload_symbols = False
    eolInMonth = 0
    qmake_args = []

    try:
        opts, args = getopt.getopt(argv, 'hc:w:t:j:due:q:', ['config=', 'wordsize=', 'toolchain=', 'jobs=', 'dev', 'upload', 'eol', 'qmake-arg='])
    except getopt.GetoptError:
        print_help()
        sys.exit(2)

    if len(opts) < 2:
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
        elif opt in ('-e', '--eol'):
            eolInMonth = int(arg)
        elif opt in ('-q', '--qmake-arg'):
            qmake_args += [arg]

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
    builder.build(jobs, qmake_args, dev_build, eolInMonth)
    builder.checks()
    builder.install()
    builder.publish()
    builder.package_xswiftbus()
    builder.symbols(upload_symbols)


# run main if run directly
if __name__ == "__main__":
    main(sys.argv[1:])
