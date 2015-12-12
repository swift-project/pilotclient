load(common_pre)

REQUIRES += contains(BLACK_CONFIG,Samples)

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += samplecliclient
SUBDIRS += sampleblackmiscquantities
SUBDIRS += sampleblackmiscdbus
SUBDIRS += sampleblackmisc
SUBDIRS += sampleblackmiscsim
SUBDIRS += samplehotkey

samplecliclient.file = cliclient/samplecliclient.pro
sampleblackmiscquantities.file = blackmiscquantities/sampleblackmiscquantities.pro
sampleblackmiscdbus.file = blackmiscdbus/sampleblackmiscdbus.pro
sampleblackmisc.file = blackmisc/sampleblackmisc.pro
sampleblackmiscsim.file = blackmiscsim/sampleblackmiscsim.pro
samplehotkey.file = hotkey/samplehotkey.pro

load(common_post)
