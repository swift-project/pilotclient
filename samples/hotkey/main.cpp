// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
