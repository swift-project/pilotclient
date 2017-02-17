msvc:DEFINES *= _SCL_SECURE_NO_WARNINGS

# swift standard warnings
msvc:QMAKE_CXXFLAGS_WARN_ON *= /wd4351 /wd4661
clang_cl:QMAKE_CXXFLAGS_WARN_ON *= -Wall -Wextra -Wno-unknown-pragmas
gcc:QMAKE_CXXFLAGS_WARN_ON *= -Woverloaded-virtual

# elevated warnings
contains(BLACK_CONFIG, AllowNoisyWarnings) {
    clang {
        QMAKE_CXXFLAGS_WARN_ON *= -Weverything --system-header-prefix=$$[QT_INSTALL_HEADERS]
        QMAKE_CXXFLAGS_WARN_ON += -Wno-system-headers -Wno-c++98-compat-pedantic -Wno-class-varargs -Wno-covered-switch-default
        QMAKE_CXXFLAGS_WARN_ON += -Wno-documentation -Wno-documentation-unknown-command -Wno-double-promotion -Wno-exit-time-destructors
        QMAKE_CXXFLAGS_WARN_ON += -Wno-gnu -Wno-missing-prototypes -Wno-newline-eof -Wno-padded -Wno-undefined-reinterpret-cast
        QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-macros -Wno-used-but-marked-unused -Wno-weak-template-vtables
    }
    else:msvc {
        QMAKE_CXXFLAGS_WARN_ON *= /Wall
        QMAKE_CXXFLAGS_WARN_ON += /wd4640 /wd4619 /wd4350 /wd4351 /wd4946 /wd4510 /wd4820 /wd4571 /wd4625 /wd4626 /wd4127

        DEFINES *= QT_CC_WARNINGS
        QMAKE_CXXFLAGS_WARN_ON *= /wd4251 /wd4275 /wd4514 /wd4800 /wd4097 /wd4706 /wd4710 /wd4530
    }
}

# gcc 5 can warn about missing override keyword
gcc {
    GCC_VERSION = $$system($$QMAKE_CXX -dumpversion)
    GCC_MAJOR = $$section(GCC_VERSION, ., 0, 0)
    greaterThan(GCC_MAJOR, 4) {
        QMAKE_CXXFLAGS_WARN_ON *= -Wsuggest-override
    }
}
