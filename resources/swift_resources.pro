load(common_pre)

TEMPLATE = lib
CONFIG += staticlib
CONFIG -= qt

OTHER_FILES += data/images/flags/*.png
OTHER_FILES += data/images/airlines/*.png
OTHER_FILES += swiftDB/*.*
OTHER_FILES += bootstrap/0.6/productive/*.*
OTHER_FILES += bootstrap/0.6/development/*.*
OTHER_FILES += local.env.template/*.*
OTHER_FILES += local.env.template/bootstrap/*.*
OTHER_FILES += local.env.template/bootstrap/0.6/*.*

COPY_FILES += $$PWD/data/images/flags/*.png
COPY_FILES += $$PWD/data/images/airlines/*.png
COPY_FILES += $$PWD/swiftDB/*.*
COPY_FILES += $$PWD/bootstrap/0.6/productive/*.*
COPY_FILES += $$PWD/bootstrap/0.6/development/*.*
COPY_FILES += $$PWD/local.env.template/*.*
COPY_FILES += $$PWD/local.env.template/bootstrap/*.*
COPY_FILES += $$PWD/local.env.template/bootstrap/0.6/*.*

load(common_post)
