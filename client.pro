TEMPLATE = subdirs

CONFIG += ordered

include (externals.pri)

WITH_BLACKMISC = ON
WITH_BLACKCORE = ON
WITH_BLACKGUI = ON
WITH_SAMPLES = ON
WITH_UNITTESTS = ON

#WITH_DRIVER_FSX = ON
#WITH_DRIVER_FS9 = ON
#WITH_DRIVER_XPLANE = ON
#WITH_DOXYGEN = ON

equals(WITH_BLACKMISC, ON) {
    SUBDIRS += src/blackmisc
    SUBDIRS += src/blackmisc_cpp2xml
}

equals(WITH_BLACKCORE, ON) {
    SUBDIRS += src/blackcore
}

equals(WITH_BLACKGUI, ON) {
    SUBDIRS += src/blackgui
}

equals(WITH_DRIVER_FSX, ON) {
    SUBDIRS += src/driver/fsx/driver_fsx.pro
}

equals(WITH_DRIVER_FS9, ON) {
    SUBDIRS += src/driver/fs9/driver_fs9.pro
}

equals(WITH_DRIVER_XPLANE, ON) {
    SUBDIRS += src/driver/xplane/driver_xplane.pro
}

equals(WITH_SAMPLES, ON) {
    SUBDIRS += samples/cli_client/sample_cli_client.pro
    SUBDIRS += samples/interpolator/sample_interpolator.pro
    SUBDIRS += samples/plugin/sample_plugin.pro
    SUBDIRS += samples/pluginmgr/sample_pluginmgr.pro
    SUBDIRS += samples/blackmiscvectorgeo/sample_vector_geo.pro
    SUBDIRS += samples/blackmiscquantities/sample_quantities_aviation.pro
    SUBDIRS += samples/blackmiscdbus/sample_blackmisc_dbus.pro
    SUBDIRS += samples/blackgui/sample_blackgui.pro
    SUBDIRS += samples/blackcore/sample_blackcore.pro
    SUBDIRS += samples/blackmisc/sample_blackmisc.pro
    SUBDIRS += samples/voiceclient/sample_voice_client.pro
}

equals(WITH_UNITTESTS, ON) {
    SUBDIRS += tests/blackmisc/test_blackmisc.pro
    SUBDIRS += tests/blackcore/test_blackcore.pro
}

equals(WITH_DOXYGEN, ON) {
    SUBDIRS += docs/doxygen.pro
}
