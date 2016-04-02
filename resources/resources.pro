load(common_pre)

TEMPLATE = lib
CONFIG += staticlib
CONFIG -= qt

OTHER_FILES += data/images/flags/*.png
OTHER_FILES += data/images/airlines/*.png
OTHER_FILES += shared//bootstrap/*.*
OTHER_FILES += shared/dbdata/*.*
OTHER_FILES += shared/updateinfo/*.*

COPY_FILES += $$PWD/data/images/flags/*.png
COPY_FILES += $$PWD/data/images/airlines/*.png
COPY_FILES += $$PWD/data/shared/bootstrap/*.*
COPY_FILES += $$PWD/data/shared/dbdata/*.*
COPY_FILES += $$PWD/data/shared/updateinfo/*.*

package_resources.path = $$PREFIX
package_resources.files += data
INSTALLS += package_resources

load(common_post)
