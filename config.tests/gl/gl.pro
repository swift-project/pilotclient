# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

load(config_test_pre)

CONFIG -= qt
CONFIG += console

SOURCES = gl.cpp

win32 {
    DEFINES += Q_OS_WIN
    LIBS *= -lopengl32
}
linux: LIBS *= -lGL
macx {
    DEFINES += Q_OS_MAC
    LIBS += -framework OpenGL
}
