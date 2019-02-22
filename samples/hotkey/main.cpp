/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file
//! \ingroup samplehotkey

#include "blackmisc/directoryutils.h"
#include "blackgui/components/settingshotkeycomponent.h"
#include "blackgui/guiapplication.h"

#include <QApplication>

using namespace BlackMisc;
using namespace BlackGui;

//! main
int main(int argc, char *argv[])
{
    CGuiApplication::highDpiScreenSupport();
    QApplication qa(argc, argv);
    CGuiApplication a("samplehotkey", BlackMisc::CApplicationInfo::Sample, QPixmap());
    BlackGui::Components::CSettingsHotkeyComponent w;
    w.show();
    return a.exec();
}
