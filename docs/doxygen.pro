# Copyright (C) 2013 VATSIM community / contributors
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/
#
# Black magic to get qmake to generate a build target to process the file
# Doxyfile.cmake.in and then invoke doxygen.

TEMPLATE = lib
CONFIG += staticlib
CONFIG -= qt

COPY_FILES += qconfigure.pl
CONFIGURE_IN += Doxyfile.cmake.in
DOXYFILE = Doxyfile

defineReplace(qconfigureReplace) {
	file = $$1
	file = $$basename(file)
	file = $$replace(file, .cmake.in, )
	return($$file)
}

win32:	copy.commands = copy ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
else:	copy.commands =   cp ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy.CONFIG = no_link
copy.input = COPY_FILES
copy.output = ${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}
copy.name = COPY
QMAKE_EXTRA_COMPILERS += copy

qconfigure.commands = perl qconfigure.pl ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT} \
	CMAKE_CURRENT_BINARY_DIR=$$OUT_PWD \
	CMAKE_CURRENT_SOURCE_DIR=$$PWD
qconfigure.CONFIG = no_link
qconfigure.depends = qconfigure.pl
qconfigure.input = CONFIGURE_IN
qconfigure.name = QCONFIGURE
qconfigure.output_function = qconfigureReplace
QMAKE_EXTRA_COMPILERS += qconfigure

DOXY_INPUT = .
doxy.commands = doxygen $$DOXYFILE
doxy.CONFIG = no_link
doxy.depends = $$DOXYFILE
doxy.input = DOXY_INPUT
doxy.name = DOXY
doxy.output = .nothing
QMAKE_EXTRA_COMPILERS += doxy