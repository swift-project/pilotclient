TEMPLATE = subdirs

CONFIG += ordered

include (externals.pri)

WITH_BLACKMISC = ON
WITH_BLACKCORE = ON
WITH_BLACKGUI = ON
WITH_BLACKSIM = ON
WITH_BLACKSOUND = ON
WITH_SAMPLES = ON
WITH_UNITTESTS = ON

#WITH_DOXYGEN = ON

equals(WITH_BLACKMISC, ON) {
    SUBDIRS += src/blackmisc
#    SUBDIRS += src/blackmisc_cpp2xml
}

equals(WITH_BLACKSOUND, ON) {
    SUBDIRS += src/blacksound
}

equals(WITH_BLACKCORE, ON) {
    SUBDIRS += src/blackcore
}

equals(WITH_BLACKGUI, ON) {
    SUBDIRS += src/blackgui
}

equals(WITH_BLACKSIM, ON) {
    SUBDIRS += src/blacksim
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
    SUBDIRS += samples/blacksim/sample_blacksim.pro
    SUBDIRS += samples/hotkey/sample_hotkey.pro
}

equals(WITH_UNITTESTS, ON) {
    SUBDIRS += tests/blackmisc/test_blackmisc.pro
    SUBDIRS += tests/blackcore/test_blackcore.pro
}

equals(WITH_DOXYGEN, ON) {
    SUBDIRS += docs/doxygen.pro
}
