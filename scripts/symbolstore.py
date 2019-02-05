#!/bin/env python
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This is a simplified version of Mozilla's symbolstore.py
# 
# Usage: create_symbolstore.py <params> <dump_syms path> <symbol store path>
#                                <debug info files or dirs>
#   Runs dump_syms on each debug info file specified on the command line,
#   then places the resulting symbol file in the proper directory
#   structure in the symbol store path.  Accepts multiple files
#   on the command line, so can be called as part of a pipe using
#   find <dir> | xargs symbolstore.pl <dump_syms> <storepath>
#   But really, you might just want to pass it <dir>.
#
#   Parameters accepted:
#     -c           : Copy debug info files to the same directory structure
#                    as sym files. On Windows, this will also copy
#                    binaries into the symbol store.
#     -a "<archs>" : Run dump_syms -a <arch> for each space separated
#                    cpu architecture in <archs> (only on OS X)

import sys
import os
import re
import shutil
import fnmatch
import subprocess
import time
import ctypes
import concurrent.futures
import multiprocessing
import platform
import tarfile

from optparse import OptionParser

# Utility functions

# A cache of files for which VCS info has already been determined. Used to
# prevent extra filesystem activity or process launching.
vcsFileInfoCache = {}


def get_platform_specific_dumper(**kwargs):
    """This function simply returns a instance of a subclass of Dumper
    that is appropriate for the current platform."""
    return {'Windows': DumperWin32,
            'Linux': DumperLinux,
            'Darwin': DumperMac}[platform.system()](**kwargs)


def start_job(dumper, lock, src_dir_repo_info, func_name, args):
    # Windows worker processes won't have run GlobalInit,
    # and due to a lack of fork(), won't inherit the class
    # variables from the parent, so set them here.
    Dumper.lock = lock
    Dumper.src_dir_repo_info = src_dir_repo_info
    return getattr(dumper, func_name)(*args)


class JobPool(object):
    jobs = {}
    executor = None

    @classmethod
    def init(cls, executor):
        cls.executor = executor

    @classmethod
    def shutdown(cls):
        cls.executor.shutdown()

    @classmethod
    def submit(cls, args, callback):
        cls.jobs[cls.executor.submit(start_job, *args)] = callback

    @classmethod
    def as_completed(cls):
        """Like concurrent.futures.as_completed, but allows adding new futures
        between generator steps. Iteration will end when the generator has
        yielded all completed futures and JobQueue.jobs is empty.
        Yields (future, callback) pairs.
        """
        while cls.jobs:
            completed, _ = concurrent.futures.wait(cls.jobs.keys(), return_when=concurrent.futures.FIRST_COMPLETED)
            for f in completed:
                callback = cls.jobs[f]
                del cls.jobs[f]
                yield f, callback


class Dumper:
    """This class can dump symbols from a file with debug info, and
    store the output in a flat directory structure.
    Requires a path to a dump_syms binary--|dump_syms| and a directory
    to store symbols in--|symbol_path|.

    You don't want to use this directly if you intend to process files.
    Instead, call GetPlatformSpecificDumper to get an instance of a
    subclass.

    Processing is performed asynchronously via worker processes; in
    order to wait for processing to finish and cleanup correctly, you
    must call Finish after all ProcessFiles calls have been made.
    You must also call Dumper.GlobalInit before creating or using any
    instances."""

    srcdirRepoInfo = None
    lock = None

    def __init__(self, dump_syms, symbol_path,
                 archs=None,
                 srcdirs=None,
                 exclude=None):
        # popen likes absolute paths, at least on windows
        if srcdirs is None:
            srcdirs = []
        if exclude is None:
            exclude = []
        self.dump_syms = os.path.abspath(dump_syms)
        self.symbol_path = symbol_path
        if archs is None:
            # makes the loop logic simpler
            self.archs = ['']
        else:
            self.archs = ['-a %s' % a for a in archs.split()]
        self.srcdirs = [os.path.normpath(a) for a in srcdirs]
        self.exclude = exclude[:]

        # book-keeping to keep track of the cleanup work per file tuple
        self.files_record = {}

    @classmethod
    def global_init(cls, executor=concurrent.futures.ProcessPoolExecutor):
        """Initialize the class globals for the multiprocessing setup; must
        be called before any Dumper instances are created and used. Test cases
        may pass in a different executor to use, usually
        concurrent.futures.ThreadPoolExecutor."""
        num_cpus = multiprocessing.cpu_count()
        if num_cpus is None:
            # assume a dual core machine if we can't find out for some reason
            # probably better on single core anyway due to I/O constraints
            num_cpus = 2

        # have to create any locks etc before the pool
        manager = multiprocessing.Manager()
        cls.lock = manager.RLock()
        cls.srcdirRepoInfo = manager.dict()
        JobPool.init(executor(max_workers=num_cpus))

    @staticmethod
    def output(destination, output_str):
        """Writes |output_str| to |destination|, holding |lock|;
        terminates with a newline."""
        with Dumper.lock:
            destination.write(output_str + "\n")
            destination.flush()

    def output_pid(self, dest, output_str):
        """Debugging output; prepends the pid to the string."""
        self.output(dest, "%d: %s" % (os.getpid(), output_str))

    # subclasses override this
    def should_process(self, file_name):
        return not any(fnmatch.fnmatch(os.path.basename(file_name), exclude) for exclude in self.exclude)

    # and can override this
    def should_skip_dir(self, directory):
        return False

    @staticmethod
    def run_file_command(file_name):
        """Utility function, returns the output of file(1)"""
        try:
            # we use -L to read the targets of symlinks,
            # and -b to print just the content, not the file_name
            return os.popen("file -Lb " + file_name).read()
        except OSError:
            return ""

    # This is a no-op except on Win32
    def fix_filename_case(self, file_name):
        return file_name

    def finish(self, stop_pool=True):
        """Process all pending jobs and any jobs their callbacks submit.
        By default, will shutdown the executor, but for testcases that
        need multiple runs, pass stop_pool = False."""
        for job, callback in JobPool.as_completed():
            try:
                res = job.result()
            except Exception as e:
                self.output(sys.stderr, 'Job raised exception: %s' % e)
                continue
            callback(res)
        if stop_pool:
            JobPool.shutdown()

    def pack(self, tar_path=None):
        if tar_path is None:
            tar_path = os.path.normpath(os.path.join(self.symbol_path, ".."))
            tar_path = os.path.join(tar_path, 'symbols.tar.gz')
        tar = tarfile.open(tar_path, "w:gz")
        for dirname, subdirs, files in os.walk(self.symbol_path):
            for filename in files:
                tar.add(os.path.join(dirname, filename), filename)
        tar.close()

    def process(self, *args):
        """Process files recursively in args."""
        # We collect all files to process first then sort by size to schedule
        # larger files first because larger files tend to take longer and we
        # don't like long pole stragglers.
        files = set()
        for arg in args:
            for f in self.get_files_to_process(arg):
                files.add(f)

        for f in sorted(files, key=os.path.getsize, reverse=True):
            self.process_files((f,))

    def get_files_to_process(self, file_or_dir):
        """Generate the files to process from an input."""
        if os.path.isdir(file_or_dir) and not self.should_skip_dir(file_or_dir):
            for f in self.get_files_to_process_in_dir(file_or_dir):
                yield f
        elif os.path.isfile(file_or_dir):
            yield file_or_dir

    def get_files_to_process_in_dir(self, path):
        """Generate the files to process in a directory.

        Valid files are are determined by calling ShouldProcess.
        """
        for root, dirs, files in os.walk(path):
            for d in dirs[:]:
                if self.should_skip_dir(d):
                    dirs.remove(d)
            for f in files:
                full_path = os.path.join(root, f)
                if self.should_process(full_path):
                    yield full_path

    def submit_job(self, func_name, args, callback):
        """Submits a job to the pool of workers"""
        JobPool.submit((self, Dumper.lock, Dumper.srcdirRepoInfo, func_name, args), callback)

    def process_files_finished(self, res):
        """Callback from multiprocessing when process_files_work finishes;
        run the cleanup work, if any"""
        # only run the cleanup function once per tuple of files
        self.files_record[res['files']] += 1
        if self.files_record[res['files']] == len(self.archs):
            del self.files_record[res['files']]
            if res['after']:
                res['after'](res['status'], res['after_arg'])

    def process_files(self, files, after=None, after_arg=None):
        """Dump symbols from these files into a symbol file, stored
        |symbol_path|; processing is performed asynchronously, and Finish must be
        called to wait for it complete and cleanup.
        All files after the first are fallbacks in case the first file does not process
        successfully; if it does, no other files will be touched."""
        self.output_pid(sys.stderr, "Submitting jobs for files: %s" % str(files))

        for arch_num, arch in enumerate(self.archs):
            self.files_record[files] = 0  # record that we submitted jobs for this tuple of files
            self.submit_job('process_files_work', args=(files, arch, after, after_arg),
                            callback=self.process_files_finished)

    def dump_syms_cmdline(self, file_name, arch, files):
        """Get the commandline used to invoke dump_syms."""
        # The Mac dumper overrides this.
        return [self.dump_syms, file_name]

    def process_files_work(self, file_names, arch, after, after_arg):
        t_start = time.time()
        self.output_pid(sys.stderr, "Worker processing files: %s" % (file_names,))

        # our result is a status, a cleanup function, an argument to that function, and the tuple of files
        # we were called on
        result = {'status': False, 'after': after, 'after_arg': after_arg, 'files': file_names}

        source_file_stream = ''
        for file_name in file_names:
            # files is a tuple of files, containing fallbacks in case the first file doesn't process successfully
            try:
                cmd = self.dump_syms_cmdline(file_name, arch, file_names)
                self.output_pid(sys.stderr, ' '.join(cmd))
                proc = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                                        stderr=open(os.devnull, 'wb'))
                module_line = proc.stdout.readline().decode("utf-8")
                if module_line.startswith("MODULE"):
                    # MODULE os cpu guid debug_file
                    (guid, debug_file) = (module_line.split())[3:5]
                    # strip off .pdb extensions, and append .sym
                    sym_file = re.sub("\.pdb$", "", debug_file) + ".sym"
                    full_path = os.path.normpath(os.path.join(self.symbol_path,
                                                              sym_file))
                    try:
                        os.makedirs(os.path.dirname(full_path))
                    except OSError:  # already exists
                        pass
                    f = open(full_path, "w")
                    f.write(module_line)
                    # now process the rest of the output
                    for line_as_bytes in proc.stdout:
                        line = line_as_bytes.decode("utf-8")
                        if line.startswith("FILE"):
                            # FILE index filename
                            (x, index, filename) = line.rstrip().split(None, 2)
                            filename = os.path.normpath(self.fix_filename_case(filename))
                            f.write("FILE %s %s\n" % (index, filename))
                        elif line.startswith("INFO CODE_ID "):
                            # INFO CODE_ID code_id code_file
                            # This gives some info we can use to
                            # store binaries in the symbol store.
                            f.write(line)
                        else:
                            # pass through all other lines unchanged
                            f.write(line)
                            # we want to return true only if at least one line is not a MODULE or FILE line
                            result['status'] = True
                    f.close()
                    proc.wait()
                    # we output relative paths so callers can get a list of what
                    # was generated
                    self.output(sys.stdout, sym_file)
            except StopIteration:
                pass
            except Exception as e:
                self.output(sys.stderr, "Unexpected error: %s" % (str(e),))
                raise
            if result['status']:
                # we only need 1 file to work
                break

        elapsed = time.time() - t_start
        self.output_pid(sys.stderr, 'Worker finished processing %s in %.2fs' %
                        (file_names, elapsed))
        return result


# Platform-specific subclasses.  For the most part, these just have
# logic to determine what files to extract symbols from.


class DumperWin32(Dumper):
    fixedFilenameCaseCache = {}

    def should_process(self, file_name):
        """This function will allow processing of pdb files that have dll
        or exe files with the same base name next to them."""
        if not Dumper.should_process(self, file_name):
            return False
        if file_name.endswith(".pdb"):
            (path, ext) = os.path.splitext(file_name)
            if os.path.isfile(path + ".exe") or os.path.isfile(path + ".dll"):
                return True
        return False

    def fix_filename_case(self, file_name):
        """Recent versions of Visual C++ put filenames into
        PDB files as all lowercase.  If the file_name exists
        on the local filesystem, fix it."""

        # Use a cached version if we have one.
        if file_name in self.fixedFilenameCaseCache:
            return self.fixedFilenameCaseCache[file_name]

        result = file_name

        ctypes.windll.kernel32.SetErrorMode(ctypes.c_uint(1))
        if not isinstance(file_name, str):
            file_name = file_name.decode(sys.getfilesystemencoding())
        handle = ctypes.windll.kernel32.CreateFileW(file_name,
                                                    # GENERIC_READ
                                                    0x80000000,
                                                    # FILE_SHARE_READ
                                                    1,
                                                    None,
                                                    # OPEN_EXISTING
                                                    3,
                                                    0,
                                                    None)
        if handle != -1:
            size = ctypes.windll.kernel32.GetFinalPathNameByHandleW(handle,
                                                                    None,
                                                                    0,
                                                                    0)
            buf = ctypes.create_unicode_buffer(size)
            if ctypes.windll.kernel32.GetFinalPathNameByHandleW(handle,
                                                                buf,
                                                                size,
                                                                0) > 0:
                # The return value of GetFinalPathNameByHandleW uses the
                # '\\?\' prefix.
                result = buf.value.encode(sys.getfilesystemencoding())[4:]
            ctypes.windll.kernel32.CloseHandle(handle)

        # Cache the corrected version to avoid future filesystem hits.
        self.fixedFilenameCaseCache[file_name] = result
        return result


class DumperLinux(Dumper):
    objcopy = os.environ['OBJCOPY'] if 'OBJCOPY' in os.environ else 'objcopy'

    def should_process(self, file_name):
        """This function will allow processing of files that are
        executable, or end with the .so extension, and additionally
        file(1) reports as being ELF files.  It expects to find the file
        command in PATH."""
        if not Dumper.should_process(self, file_name):
            return False
        if file_name.endswith(".so") or os.access(file_name, os.X_OK):
            if not os.path.islink(file_name) and self.run_file_command(file_name).startswith("ELF"):
                return True
        return False


def after_mac(status, dsymbundle):
    """Cleanup function to run on Macs after we process the file(s)."""
    shutil.rmtree(dsymbundle)


class DumperMac(Dumper):
    def should_process(self, file_name):
        """This function will allow processing of files that are
        executable, or end with the .dylib extension, and additionally
        file(1) reports as being Mach-O files.  It expects to find the file
        command in PATH."""
        if not Dumper.should_process(self, file_name):
            return False
        if file_name.endswith(".dylib") or os.access(file_name, os.X_OK):
            if not os.path.islink(file_name) and self.run_file_command(file_name).startswith("Mach-O"):
                return True
        return False

    def should_skip_dir(self, directory):
        """We create .dSYM bundles on the fly, but if someone runs
        buildsymbols twice, we should skip any bundles we created
        previously, otherwise we'll recurse into them and try to 
        dump the inner bits again."""
        if directory.endswith(".dSYM"):
            return True
        return False

    def process_files(self, files, after=None, after_arg=None):
        # also note, files must be len 1 here, since we're the only ones
        # that ever add more than one file to the list
        file_name = files[0]
        self.output_pid(sys.stderr, "Submitting job for Mac pre-processing on file: %s" % file_name)
        self.submit_job('process_files_work_mac', args=(file_name,), callback=self.process_files_mac_finished)

    def process_files_mac_finished(self, result):
        if result['status']:
            # kick off new jobs per-arch with our new list of files
            Dumper.process_files(self, result['files'], after=after_mac, after_arg=result['files'][0])

    def dump_syms_cmdline(self, file_name, arch, files):
        """Get the commandline used to invoke dump_syms."""
        # dump_syms wants the path to the original binary and the .dSYM
        # in order to dump all the symbols.
        if len(files) == 2 and file_name == files[0] and file_name.endswith('.dSYM'):
            # This is the .dSYM bundle.
            return [self.dump_syms] + arch.split() + ['-g', file_name, files[1]]
        return Dumper.dump_syms_cmdline(self, file_name, arch, files)

    def process_files_work_mac(self, file_name):
        """dump_syms on Mac needs to be run on a dSYM bundle produced
        by dsymutil(1), so run dsymutil here and pass the bundle name
        down to the superclass method instead."""
        t_start = time.time()
        self.output_pid(sys.stderr, "Worker running Mac pre-processing on file: %s" % (file_name,))

        # our return is a status and a tuple of files to dump symbols for
        # the extra files are fallbacks; as soon as one is dumped successfully, we stop
        result = {'status': False, 'files': None, 'file_key': file_name}
        dsymbundle = file_name + ".dSYM"
        if os.path.exists(dsymbundle):
            shutil.rmtree(dsymbundle)
        #dsymutil = buildconfig.substs['DSYMUTIL']
        dsymutil = 'dsymutil'
        # dsymutil takes --arch=foo instead of -a foo like everything else
        try:
            cmd = ([dsymutil] +
                   [a.replace('-a ', '--arch=') for a in self.archs if a] +
                   [file_name])
            self.output_pid(sys.stderr, ' '.join(cmd))
            subprocess.check_call(cmd, stdout=open(os.devnull, 'w'))
        except subprocess.CalledProcessError as e:
            self.output_pid(sys.stderr, 'Error running dsymutil: %s' % str(e))

        if not os.path.exists(dsymbundle):
            # dsymutil won't produce a .dSYM for files without symbols
            self.output_pid(sys.stderr, "No symbols found in file: %s" % (file_name,))
            result['status'] = False
            result['files'] = (file_name,)
            return result

        result['status'] = True
        result['files'] = (dsymbundle, file_name)
        elapsed = time.time() - t_start
        self.output_pid(sys.stderr, 'Worker finished processing %s in %.2fs' %
                        (file_name, elapsed))
        return result


# Entry point if called as a standalone program
def main():
    parser = OptionParser(usage="usage: %prog [options] <dump_syms binary> <symbol store path> <debug info files>")
    parser.add_option("-a", "--archs",
                      action="store", dest="archs",
                      help="Run dump_syms -a <arch> for each space separated cpu architecture in ARCHS (only on OS X)")
    parser.add_option("-x", "--exclude",
                      action="append", dest="exclude", default=[], metavar="PATTERN",
                      help="Skip processing files matching PATTERN.")
    (options, args) = parser.parse_args()

    if len(args) < 3:
        parser.error("not enough arguments")
        exit(1)

    dumper = get_platform_specific_dumper(dump_syms=args[0],
                                          symbol_path=args[1],
                                          archs=options.archs,
                                          exclude=options.exclude)

    dumper.process(*args[2:])
    dumper.finish()
    dumper.pack()


# run main if run directly
if __name__ == "__main__":
    # set up the multiprocessing infrastructure before we start;
    # note that this needs to be in the __main__ guard, or else Windows will choke
    Dumper.global_init()

    main()
