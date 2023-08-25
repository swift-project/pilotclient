# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

load(config_test_pre)

CONFIG -= qt
CONFIG += console

SOURCES = libpng.cpp
LIBS *= -lpng -lz
