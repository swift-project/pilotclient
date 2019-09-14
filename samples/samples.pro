load(common_pre)

TEMPLATE = subdirs
CONFIG  += ordered

# SUBDIRS += samplecliclient
SUBDIRS += sampleblackmiscquantities
SUBDIRS += sampleblackmiscdbus
SUBDIRS += sampleblackmisc
SUBDIRS += sampleblackmiscsim
SUBDIRS += samplehotkey
SUBDIRS += sampleweatherdata
SUBDIRS += samplefsd
SUBDIRS += afvclient

samplecliclient.file = cliclient/samplecliclient.pro
sampleblackmiscquantities.file = blackmiscquantities/sampleblackmiscquantities.pro
sampleblackmiscdbus.file = blackmiscdbus/sampleblackmiscdbus.pro
sampleblackmisc.file = blackmisc/sampleblackmisc.pro
sampleblackmiscsim.file = blackmiscsim/sampleblackmiscsim.pro
samplehotkey.file = hotkey/samplehotkey.pro
sampleweatherdata.file = weatherdata/sampleweatherdata.pro
samplefsd.file = fsd/samplefsd.pro

load(common_post)
