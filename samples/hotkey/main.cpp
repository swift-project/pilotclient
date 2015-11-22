/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackgui/components/settingshotkeycomponent.h"
#include "blackmisc/registermetadata.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    BlackMisc::registerMetadata();
    BlackGui::Components::CSettingsHotkeyComponent w;
    w.show();

    return a.exec();
}
