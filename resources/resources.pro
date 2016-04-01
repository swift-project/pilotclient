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
COPY_FILES += $$PWD/shared/bootstrap/*.*
COPY_FILES += $$PWD/shared/dbdata/*.*
COPY_FILES += $$PWD/shared/updateinfo/*.*

package_resources.path = $$PREFIX
package_resources.files += data
package_resources.files += shared
INSTALLS += package_resources

load(common_post)
