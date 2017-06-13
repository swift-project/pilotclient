load(config_test_pre)

CONFIG -= qt
CONFIG += console

SOURCES = libpng.cpp
LIBS *= -lpng -lz
