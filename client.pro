TEMPLATE = subdirs

CONFIG += ordered

WITH_BLACKMISC = ON

WITH_BLACKCORE = ON

WITH_BLACKD = ON

WITH_BLACKBOX = ON

WITH_SAMPLES = ON

#WITH_DRIVER_FSX = ON

#WITH_DRIVER_FS9 = ON

#WITH_DRIVER_XPLANE = ON


equals(WITH_BLACKMISC, ON) {
    SUBDIRS += src/blackmisc
}

equals(WITH_BLACKCORE, ON) {
    SUBDIRS += src/blackcore
}

equals(WITH_BLACKD, ON) {
    SUBDIRS += src/blackd
}

equals(WITH_BLACKBOX, ON) {
    SUBDIRS += src/blackbox
}

equals(WITH_DRIVER_FSX, ON) {
    SUBDIRS += src/driver/fsx
}

equals(WITH_DRIVER_FS9, ON) {
    SUBDIRS += src/driver/fs9
}

equals(WITH_DRIVER_XPLANE, ON) {
    SUBDIRS += src/driver/xplane
}

equals(WITH_SAMPLES, ON) {
    SUBDIRS += samples/com_client
	SUBDIRS += samples/com_server
	SUBDIRS += samples/config
	SUBDIRS += samples/Geodetic2Ecef
	SUBDIRS += samples/interpolator
	SUBDIRS += samples/Logging
}

SUBDIRS +=



