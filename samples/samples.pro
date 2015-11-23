load(common_pre)

REQUIRES += contains(BLACK_CONFIG,Samples)

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += cliclient
SUBDIRS += blackmiscquantities
SUBDIRS += blackmiscdbus
SUBDIRS += blackmisc
SUBDIRS += blackmiscsim
SUBDIRS += hotkey

load(common_post)
