#!/bin/env python

# SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

import getopt
import os
import os.path as path
import platform
import requests
import subprocess
import sys
import datastore
import tarfile
from lib.util import get_vs_env
import utils


class Builder:

    def prepare(self):
        """
        Prepares the build environment, e.g. setup of environmental variables.
        Build processes will be called with the environment modified by this function.
        """
        print('Preparing environment ...')
        self._specific_prepare()

        print('Updating from datastore ...')
        host = 'https://datastore.swift-project.org'
        datastore_version = '0.7.0'
        source_path = utils.get_swift_source_path()
        shared_path = os.path.abspath(os.path.join(source_path, 'resources', 'share'))
        datastore.update_shared(host, datastore_version, shared_path)

    def build(self, cmake_args):
        """
        Run the build itself
        """
        print('Running build ...')
        build_path = self._get_swift_build_path()
        if not os.path.isdir(build_path):
            os.makedirs(build_path)
        os.chdir(build_path)

        use_crashpad = "ON" if platform.system() == 'Darwin' or platform.system() == 'Windows' else "OFF"

        cmake_call = ['cmake',
                      '..',
                      '-G {}'.format(self._get_generator()),
                      '-DCMAKE_BUILD_TYPE=RelWithDebInfo',
                      '-DSWIFT_MINIFY_DEBUG_SYMBOLS=ON',
                      '-DCMAKE_INSTALL_PREFIX=../dist',
                      '-DSWIFT_USE_CRASHPAD={}'.format(use_crashpad)] + cmake_args
        subprocess.check_call(cmake_call, env=dict(os.environ))

        # Workaround while using Make for macOS to pass number of jobs
        if self.__class__.__name__ == 'MacOSBuilder':
            subprocess.check_call(["cmake", "--build", ".", "-j3"], env=dict(os.environ))
        elif self.__class__.__name__ == 'MSVCBuilder':
            # it seems that ninja does not automatically spawn the correct number of jobs on Windows
            subprocess.check_call(["cmake", "--build", ".", "-j4"], env=dict(os.environ))
        else:
            subprocess.check_call(["cmake", "--build", "."], env=dict(os.environ))

    def checks(self):
        """
        Runs build checks.
        """
        print('Running checks ...')
        build_path = self._get_swift_build_path()
        os.chdir(build_path)
        if self._should_run_checks():
            subprocess.check_call(["ctest", "--verbose"], env=dict(os.environ))
            pass

    def create_installer(self):
        bitrock_builder_bin = os.environ["BITROCK_BUILDER"]
        os.chdir(utils.get_swift_source_path())
        os_map = {'Linux': 'linux', 'Darwin': 'macos', 'Windows': 'windows'}
        installer_platform_map = {'Linux': 'linux-x{}'.format(self.word_size), 'Darwin': 'osx', 'Windows': 'windows'}
        extension_map = {'Linux': 'run', 'Darwin': 'app', 'Windows': 'exe'}
        extension = extension_map[platform.system()]
        os_name = os_map[platform.system()]
        version_full = utils.get_swift_version_base()
        version_rev = utils.get_rev_count()
        windows64 = 1 if os_name == 'windows' and int(self.word_size) == 64 else 0
        installer_platform = installer_platform_map[platform.system()]

        subprocess.check_call([os.path.expanduser(bitrock_builder_bin),
                                "quickbuild",
                                "installer/installbuilder/project.xml",
                                "{}".format(installer_platform),
                                "--verbose",
                                "--license",
                                "~/license.xml",
                                "--setvars",
                                "project.outputDirectory=build",
                                "project.installerFilename=swiftinstaller-{}-{}-{}.{}".format(os_name, self.word_size, version_full, extension),
                                "project.version={}".format(version_full),
                                "versionFull={}.{}".format(version_full, version_rev),
                                "project.windows64bitMode={}".format(windows64),
                                "project.enableDebugger=0",
                                "architecture={}".format(self.word_size)
                                ], env=dict(os.environ))

    def install(self):
        """
        Installs all products to the default path.
        """
        print('Running install ...')
        build_path = self._get_swift_build_path()
        os.chdir(build_path)
        subprocess.check_call(["cmake", "--install", "."], env=dict(os.environ))

        if self._should_publish():
            self._strip_debug()
            self.create_installer()

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
        content_path = path.abspath(path.join(utils.get_swift_source_path(), 'dist', 'xswiftbus'))
        subprocess.check_call(['7z', 'a', '-mx=9', archive_path, content_path, "-xr!*.debug", "-xr!*.dSYM"], env=dict(os.environ))

    def symbols(self, upload_symbols):
        """
        Generates the binary symbols and archives them into a gzip archive, located in the swift source root.
        """
        # Do not even generate symbols if they aren't used. They got so big now, that we cannot afford to archive them.
        if not upload_symbols:
            return

        if self._should_create_symbols():
            build_path = self._get_swift_build_path()
            os.chdir(build_path)
            print('Creating symbols')

            # Debug symbols for Windows are in the build folder
            # and for OSX and Linux in dist/install folder (after splitting)
            if platform.system() == 'Windows':
                binary_path = path.abspath(path.join(build_path, 'out'))
            else:
                binary_path = path.abspath(path.join(utils.get_swift_source_path(), 'dist'))

            os_map = {'Linux': 'linux', 'Darwin': 'macos', 'Windows': 'windows'}
            tar_filename = '-'.join(
                ['swiftsymbols', os_map[platform.system()], self.word_size, self.version]) + '.tar.gz'
            tar_path = path.abspath(path.join(utils.get_swift_source_path(), tar_filename))
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

    def bundle_csl2xsb(self):
        pass

    def _get_swift_build_path(self) -> str:
        return self.__build_path

    def _specific_prepare(self):
        pass

    def _get_platform_name(self) -> str:
        raise NotImplementedError()

    def _get_generator(self) -> str:
        raise NotImplementedError()

    def _should_run_checks(self) -> bool:
        return True

    def _should_publish(self) -> bool:
        return True

    def _should_create_symbols(self) -> bool:
        return True

    def _get_externals_path(self) -> str:
        return path.abspath(path.join(utils.get_swift_source_path(), 'externals', self._get_platform_name(), self.word_size, 'lib'))

    def _strip_debug(self):
        raise NotImplementedError()

    def __init__(self, word_size):
        self.__build_path = path.abspath(path.join(utils.get_swift_source_path(), 'build'))

        files = os.listdir(utils.get_swift_source_path())
        for swift_dir in ['src', 'installer', 'third_party']:
            if swift_dir not in files:
                raise RuntimeError('Cannot find {} folder! Are we in the right directory?'.format(swift_dir))

        self.word_size = word_size
        self.version = utils.get_swift_version()



    def __upload_symbol_files(self, symbols_package):
        print('Uploading symbols')
        url = 'https://swift-project.sp.backtrace.io:6098/post'
        token = os.environ['BACKTRACE_SYMBOL_TOKEN']

        data = open(symbols_package, 'rb').read()
        params = (
            ('format', 'symbols'),
            ('token', token),
            ('tag', self.version),
        )
        r = requests.post(url, params=params, data=data)
        r.raise_for_status()


class MSVCBuilder(Builder):

    def _specific_prepare(self):
        if self.word_size == '32':
            vs_env = get_vs_env('amd64_x86')
        else:
            vs_env = get_vs_env('amd64')

        os.environ.update(vs_env)

        # On Windows, the default Qt logger doesn't write to stderr, but uses
        # the Win32 API OutputDebugString instead.
        # This environment variable forces it to use stderr. It also forces
        # QPlainTestLogger::outputMessage to print to stdout.
        os.environ['QT_FORCE_STDERR_LOGGING'] = '1'

    def _get_platform_name(self) -> str:
        return 'win32-msvc'

    def _get_generator(self) -> str:
        return "Ninja"

    def _strip_debug(self):
        pass

    def bundle_csl2xsb(self):
        os.chdir(utils.get_swift_source_path())
        subprocess.check_call(["pyinstaller", "-y", "--distpath", "dist/share",
                                "--workpath", os.environ["TEMP"], "scripts/csl2xsb/CSL2XSB.py"])

    def __init__(self, word_size):
        Builder.__init__(self, word_size)


class LinuxBuilder(Builder):

    def _specific_prepare(self):
        pass

    def _get_platform_name(self) -> str:
        return 'linux-g++'

    def _get_generator(self) -> str:
        return 'Ninja'

    def _strip_debug(self):
        files = [
            "bin/swiftcore",
            "bin/swiftdata",
            "bin/swiftguistd",
            "bin/swiftlauncher",
            "lib/libcore.so",
            "lib/libgui.so",
            "lib/libinput.so",
            "lib/libmisc.so",
            "lib/libplugincommon.so",
            "lib/libsound.so",
            "bin/plugins/simulator/libsimulatoremulated.so",
            "bin/plugins/simulator/libsimulatoremulatedconfig.so",
            "bin/plugins/simulator/libsimulatorflightgear.so",
            "bin/plugins/simulator/libsimulatorflightgearconfig.so",
            "bin/plugins/simulator/libsimulatorxplane.so",
            "bin/plugins/simulator/libsimulatorxplaneconfig.so",
            "bin/plugins/weatherdata/libweatherdatagfs.so",
            "xswiftbus/64/lin.xpl",
        ]
        dist_path = path.join(utils.get_swift_source_path(), "dist")
        for file in files:
            subprocess.check_call(["objcopy", "--only-keep-debug",
                                   path.join(dist_path, file),
                                   path.join(dist_path, file + ".debug")], env=dict(os.environ))
            subprocess.check_call(["objcopy", "--strip-debug", path.join(dist_path, file)], env=dict(os.environ))

    def __init__(self, word_size):
        Builder.__init__(self, word_size)


class MacOSBuilder(Builder):

    def _specific_prepare(self):
        pass

    def _get_platform_name(self) -> str:
        return 'macx-clang'

    def _get_generator(self) -> str:
        return 'Unix Makefiles'

    def _should_create_symbols(self) -> bool:
        return True

    def _strip_debug(self):
        bundle_files = [
            "bin/swiftcore.app/Contents/MacOS/swiftcore",
            "bin/swiftdata.app/Contents/MacOS/swiftdata",
            "bin/swiftguistd.app/Contents/MacOS/swiftguistd",
            "bin/swiftlauncher.app/Contents/MacOS/swiftlauncher",
        ]

        files = [
            "lib/libcore.dylib",
            "lib/libgui.dylib",
            "lib/libinput.dylib",
            "lib/libmisc.dylib",
            "lib/libplugincommon.dylib",
            "lib/libsound.dylib",
            "bin/plugins/simulator/libsimulatoremulated.dylib",
            "bin/plugins/simulator/libsimulatoremulatedconfig.dylib",
            "bin/plugins/simulator/libsimulatorflightgear.dylib",
            "bin/plugins/simulator/libsimulatorflightgearconfig.dylib",
            "bin/plugins/simulator/libsimulatorxplane.dylib",
            "bin/plugins/simulator/libsimulatorxplaneconfig.dylib",
            "bin/plugins/weatherdata/libweatherdatagfs.dylib",
            "xswiftbus/64/mac.xpl",
        ]
        dist_path = path.join(utils.get_swift_source_path(), "dist")

        # Put output of bundle files directly in bin folder
        for file in bundle_files:
            subprocess.check_call(["dsymutil", path.join(dist_path, file), "-o", path.join(dist_path, "bin", file.split("/")[-1] + ".dSYM")], env=dict(os.environ))

        for file in files:
            subprocess.check_call(["dsymutil", path.join(dist_path, file)], env=dict(os.environ))

    def __init__(self, word_size):
        Builder.__init__(self, word_size)


def print_help():
    supported_compilers = {'Linux': ['gcc'],
                           'Darwin': ['clang'],
                           'Windows': ['msvc']
                           }
    compiler_help = '|'.join(supported_compilers[platform.system()])
    print('build.py -w <32|64> -t <' + compiler_help + '> [-c <extra CMake argument>]')


# Entry point if called as a standalone program
def main(argv):
    word_size = ''
    tool_chain = ''
    upload_symbols = False
    cmake_args = []

    try:
        opts, args = getopt.getopt(argv, 'hw:t:uc', ['wordsize=', 'toolchain=', 'upload', 'cmake-arg='])
    except getopt.GetoptError:
        print_help()
        sys.exit(2)

    if len(opts) == 0:
        print_help()
        sys.exit(2)

    for opt, arg in opts:
        if opt == '-h':
            print_help()
            sys.exit()
        elif opt in ('-w', '--wordsize'):
            word_size = arg
        elif opt in ('-t', '--toolchain'):
            tool_chain = arg
        elif opt in ('-u', '--upload'):
            upload_symbols = True
        elif opt in ('-c', '--cmake-arg'):
            cmake_args += [arg]

    if word_size not in ['32', '64']:
        print('Unsupported word size. Choose 32 or 64')
        sys.exit(2)

    builders = {'Linux': {
                    'gcc': LinuxBuilder},
                'Darwin': {
                    'clang': MacOSBuilder},
                'Windows': {
                    'msvc': MSVCBuilder,
                }
    }

    if tool_chain not in builders[platform.system()]:
        print('Unknown or unsupported tool chain!')
        sys.exit(2)

    builder = builders[platform.system()][tool_chain](word_size)

    builder.prepare()
    builder.build(cmake_args)
    builder.bundle_csl2xsb()
    builder.checks()
    builder.install()
    builder.publish()
    if word_size == '64':
        builder.package_xswiftbus()
    builder.symbols(upload_symbols)


# run main if run directly
if __name__ == "__main__":
    main(sys.argv[1:])
