# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

load(common_pre)

TEMPLATE = lib
CONFIG += staticlib
CONFIG -= qt

OTHER_FILES += $$files($$PWD/share/images/flags/*.png)
OTHER_FILES += $$files($$PWD/share/images/airlines/*.png)
OTHER_FILES += $$files($$PWD/share/html/*.*)
OTHER_FILES += $$files($$PWD/share/legal/*.*)
OTHER_FILES += $$files($$PWD/share/test/*.*)
OTHER_FILES += $$files($$PWD/share/matchingscript/*.*)
OTHER_FILES += $$files($$PWD/share/simulator/swiftTerrainProbe/*.*)
OTHER_FILES += $$files($$PWD/share/simulator/swiftTerrainProbe/model/*.*)
OTHER_FILES += $$files($$PWD/share/simulator/swiftTerrainProbe/texture/*.*)
OTHER_FILES += $$files($$PWD/share/simulator/models/*.*)
OTHER_FILES += $$files($$PWD/shared/bootstrap/*.*)
OTHER_FILES += $$files($$PWD/shared/dbdata/*.*)
OTHER_FILES += $$files($$PWD/shared/updateinfo/*.*)

COPY_FILES += $$files($$PWD/share/images/flags/*.png)
COPY_FILES += $$files($$PWD/share/images/airlines/*.png)
COPY_FILES += $$files($$PWD/share/html/*.*)
COPY_FILES += $$files($$PWD/share/legal/*.*)
COPY_FILES += $$files($$PWD/share/test/*.*)
COPY_FILES += $$files($$PWD/share/matchingscript/*.*)
COPY_FILES += $$files($$PWD/share/misc/*.*)
# COPY_FILES += $$files($$PWD/share/simulator/*.*)
COPY_FILES += $$files($$PWD/share/simulator/swiftTerrainProbe/*.*)
COPY_FILES += $$files($$PWD/share/simulator/swiftTerrainProbe/model/*.*)
COPY_FILES += $$files($$PWD/share/simulator/swiftTerrainProbe/texture/*.*)
COPY_FILES += $$files($$PWD/share/simulator/models/*.*)
COPY_FILES += $$files($$PWD/share/shared/bootstrap/*.*)
COPY_FILES += $$files($$PWD/share/shared/dbdata/*.*)
COPY_FILES += $$files($$PWD/share/shared/updateinfo/*.*)

package_resources.path = $$PREFIX
package_resources.files += share
INSTALLS += package_resources

load(common_post)
