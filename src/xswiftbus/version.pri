# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

# this file is included by swift.pro in the project root

XSWIFTBUS_DEPENDENTS = $$SourceRoot/src/xswiftbus \
    $$SourceRoot/src/blackmisc/simulation/xplane/qtfreeutils.* \
    $$SourceRoot/src/blackmisc/simulation/settings/xswiftbussettingsqtfree.*

XSWIFTBUS_COMMIT = $$system(git log -n 1 --format=%h -- $$XSWIFTBUS_DEPENDENTS)

# make the variable available in other files without needing to be included
cache(XSWIFTBUS_COMMIT)
