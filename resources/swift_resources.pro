load(common_pre)

TEMPLATE = lib
CONFIG += staticlib
CONFIG -= qt

OTHER_FILES += data/images/flags/*.png
OTHER_FILES += data/images/airlines/*.png
OTHER_FILES += swiftDB/*.*

COPY_FILES += $$PWD/data/images/flags/*.png
COPY_FILES += $$PWD/data/images/airlines/*.png
COPY_FILES += $$PWD/swiftDB/*.*

load(common_post)
