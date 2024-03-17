# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

execute_process(COMMAND qmake6 -query QT_INSTALL_LIBS OUTPUT_VARIABLE QT_INSTALL_LIBS)

string(STRIP ${QT_INSTALL_LIBS} QT_INSTALL_LIBS)

function(copy_framework name)
    # MacOS workaround using rsync. Otherwise all headers are also copied.
    execute_process(COMMAND rsync -avzl --exclude "Headers*" --exclude "*debug*" ${QT_INSTALL_LIBS}/${name}.framework/ lib/${name}.framework/ WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX})
endfunction()

copy_framework(QtConcurrent)
copy_framework(QtCore)
copy_framework(QtCore5Compat)
copy_framework(QtDBus)
copy_framework(QtGui)
copy_framework(QtMultimedia)
copy_framework(QtNetwork)
copy_framework(QtOpenGL)
copy_framework(QtQml)
copy_framework(QtPrintSupport)
copy_framework(QtSvg)
copy_framework(QtWidgets)
copy_framework(QtXml)

