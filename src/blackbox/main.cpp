//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "blackbox.h"
#include <QApplication>
#include "blackmisc/context.h"
#include "blackmisc/debug.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	BlackMisc::CApplicationContext myBlackApp;
	BlackMisc::IContext::getInstance().getDebug()->create();
	

    BlackBox w;
    w.show();
    
    return a.exec();
}
