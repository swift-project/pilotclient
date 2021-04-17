msvc:DEFINES *= _SCL_SECURE_NO_WARNINGS

# swift standard warnings
msvc:QMAKE_CXXFLAGS_WARN_ON *= /wd4351 /wd4661
msvc:QMAKE_CXXFLAGS_WARN_ON -= -w44996
clang_cl:QMAKE_CXXFLAGS_WARN_ON *= /W4 -Wno-unknown-pragmas -Wno-undefined-inline -Wno-self-assign-overloaded
gcc:QMAKE_CXXFLAGS_WARN_ON *= -Woverloaded-virtual
gcc:QMAKE_CXXFLAGS_USE_PRECOMPILE = -Winvalid-pch $$QMAKE_CXXFLAGS_USE_PRECOMPILE
gcc|llvm:QMAKE_CXXFLAGS_WARN_ON *= -Wzero-as-null-pointer-constant
gcc:QMAKE_CXXFLAGS_WARN_ON += -isystem $$[QT_INSTALL_HEADERS]/QtCore
llvm:QMAKE_CXXFLAGS_WARN_ON *= -Wno-system-headers
gcc:!clang:QMAKE_CXXFLAGS_WARN_ON *= -Wsuggest-override

# elevated warnings
swiftConfig(allowNoisyWarnings) {
    llvm {
        QMAKE_CXXFLAGS_WARN_ON -= -Wno-self-assign-overloaded
        QMAKE_CXXFLAGS_WARN_ON *= -Weverything $$clangArg(--system-header-prefix=$$[QT_INSTALL_HEADERS])
        QMAKE_CXXFLAGS_WARN_ON += -Wno-c++98-compat-pedantic -Wno-class-varargs -Wno-covered-switch-default
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
else {
    DEFINES *= QT_NO_DEPRECATED_WARNINGS
    mingw:QMAKE_CXXFLAGS_WARN_ON += -Wno-maybe-uninitialized
}

# clazy - Qt-aware linter
equals(QMAKE_CXX, clazy)|equals(QMAKE_CXX, clazy-cl) {
    CLAZY_WARNINGS *= level3 no-reserve-candidates qrequiredresult-candidates
    CLAZY_WARNINGS *= isempty-vs-count raw-environment-function tr-non-literal

    # TODO: gradually fix issues so we can re-enable some of these warnings
    CLAZY_WARNINGS *= no-qstring-allocations no-copyable-polymorphic no-function-args-by-value
    CLAZY_WARNINGS *= no-inefficient-qlist-soft no-ctor-missing-parent-argument

    QMAKE_CXXFLAGS_WARN_ON += -Xclang -plugin-arg-clang-lazy -Xclang $$join(CLAZY_WARNINGS, ",")
}
