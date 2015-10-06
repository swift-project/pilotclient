win32-msvc*:DEFINES *= _SCL_SECURE_NO_WARNINGS
# win32-msvc*:QMAKE_CXXFLAGS *=

# exclude Qt lib warnings
# win32-g++: QMAKE_CXXFLAGS +=  $$join(QMAKE_INCDIR_QT, " -isystem", "-isystem")

# swift standard warnings
win32-msvc*:QMAKE_CXXFLAGS_WARN_ON *= /wd4351 /wd4661

# elevated warnings
# win32-msvc*:QMAKE_CXXFLAGS_WARN_ON *= /Wall /wd4640 /wd4619 /wd4350 /wd4351 /wd4946 /wd4510 /wd4820 /wd4571 /wd4625 /wd4626 /wd4127
