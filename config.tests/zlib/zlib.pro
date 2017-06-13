load(config_test_pre)

CONFIG -= qt
CONFIG += console

SOURCES = zlib.cpp
LIBS *= -lz
