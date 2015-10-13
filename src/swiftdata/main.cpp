/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "swiftdata.h"
#include "blackmisc/icons.h"
#include "blackmisc/worker.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/project.h"
#include "blackmisc/loghandler.h"
#include "blackmisc/filelogger.h"
#include "blackgui/guiutility.h"
#include "blackgui/stylesheetutility.h"

#include <QApplication>
#include <QMessageBox>

using namespace BlackMisc;
using namespace BlackCore;
using namespace BlackGui;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CGuiUtility::initSwiftGuiApplication(a, "swiftdata", CIcons::swiftDatabase24());

    CSwiftData w;
    w.show();
    int r = a.exec();
    return r;
}
