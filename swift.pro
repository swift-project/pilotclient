include (config.pri)
include (build.pri)

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += src/blackmisc

contains(BLACK_CONFIG, BlackSound) {
    SUBDIRS += src/blacksound
}

contains(BLACK_CONFIG, BlackInput) {
    SUBDIRS += src/blackinput
}

contains(BLACK_CONFIG, BlackCore) {
    SUBDIRS += src/blackcore
    SUBDIRS += src/swiftcore/swiftcore.pro
}

contains(BLACK_CONFIG, BlackGui) {
    SUBDIRS += src/blackgui
    SUBDIRS += src/swiftgui_standard/swiftgui_standard.pro

    contains(BLACK_CONFIG, BlackCore) {
        SUBDIRS += src/swiftcorectrl/swiftcorectrl.pro
    }
}

win32 {

    if (contains(BLACK_CONFIG, FSX) | contains(BLACK_CONFIG, FS9)) {
        SUBDIRS += src/plugins/simulator/fscommon/plugin_fscommon.pro
    }

    contains(BLACK_CONFIG, FSX) {
        SUBDIRS += src/plugins/simulator/fsx/plugin_fsx.pro
    }
    contains(BLACK_CONFIG, FS9) {
        SUBDIRS += src/plugins/simulator/fs9/plugin_fs9.pro
    }
}

contains(BLACK_CONFIG, XPlane) {
    SUBDIRS += src/xbus/xbus.pro
    SUBDIRS += src/plugins/simulator/xplane/plugin_xplane.pro
}

contains(BLACK_CONFIG, Samples) {
    SUBDIRS += samples/cli_client/sample_cli_client.pro
    SUBDIRS += samples/blackmiscgeo/sample_geo.pro
    SUBDIRS += samples/blackmiscquantities/sample_quantities_aviation.pro
    SUBDIRS += samples/blackmiscdbus/sample_blackmisc_dbus.pro
    SUBDIRS += samples/blackmisc/sample_blackmisc.pro
    SUBDIRS += samples/voiceclient/sample_voice_client.pro
    SUBDIRS += samples/blackmiscsim/sample_blackmisc_sim.pro
    SUBDIRS += samples/hotkey/sample_hotkey.pro
}

contains(BLACK_CONFIG, Unittests) {
    SUBDIRS += tests/blackmisc/test_blackmisc.pro
    SUBDIRS += tests/blackcore/test_blackcore.pro
}

contains(BLACK_CONFIG, Doxygen) {
    SUBDIRS += docs/doxygen.pro
}
