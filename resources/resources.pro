load(common_pre)

TEMPLATE = lib
CONFIG += staticlib
CONFIG -= qt

OTHER_FILES += share/images/flags/*.png
OTHER_FILES += share/images/airlines/*.png
OTHER_FILES += share/html/*.*
OTHER_FILES += share/legal/*.*
OTHER_FILES += share/test/*.*
# OTHER_FILES += share/simulator/*.*
OTHER_FILES += share/simulator/swiftTerrainProbe/*.*
OTHER_FILES += share/simulator/swiftTerrainProbe/model/*.*
OTHER_FILES += share/simulator/swiftTerrainProbe/texture/*.*
OTHER_FILES += shared/bootstrap/*.*
OTHER_FILES += shared/dbdata/*.*
OTHER_FILES += shared/updateinfo/*.*

COPY_FILES += $$PWD/share/images/flags/*.png
COPY_FILES += $$PWD/share/images/airlines/*.png
COPY_FILES += $$PWD/share/html/*.*
COPY_FILES += $$PWD/share/legal/*.*
COPY_FILES += $$PWD/share/test/*.*
# COPY_FILES += $$PWD/share/simulator/*.*
COPY_FILES += $$PWD/share/simulator/swiftTerrainProbe/*.*
COPY_FILES += $$PWD/share/simulator/swiftTerrainProbe/model/*.*
COPY_FILES += $$PWD/share/simulator/swiftTerrainProbe/texture/*.*
COPY_FILES += $$PWD/share/shared/bootstrap/*.*
COPY_FILES += $$PWD/share/shared/dbdata/*.*
COPY_FILES += $$PWD/share/shared/updateinfo/*.*

package_resources.path = $$PREFIX
package_resources.files += share
INSTALLS += package_resources

load(common_post)
