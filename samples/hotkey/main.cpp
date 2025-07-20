// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplehotkey

#include <QApplication>

#include "gui/components/settingshotkeycomponent.h"
#include "gui/guiapplication.h"
#include "misc/directoryutils.h"

using namespace swift::misc;
using namespace swift::gui;

//! main
int main(int argc, char *argv[])
{
    QApplication qa(argc, argv);
    CGuiApplication a("samplehotkey", swift::misc::CApplicationInfo::Sample, QPixmap());
    swift::gui::components::CSettingsHotkeyComponent w;
    w.show();
    return a.exec();
}
