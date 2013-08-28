/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include <QCoreApplication>

#include "samplesphysicalquantities.h"
#include "samplesaviation.h"
#include "blackmisc/blackmiscfreefunctions.h"

using namespace BlackMisc;
using namespace BlackMiscTest;

/*!
 * Sample tests
 */
int main(int argc, char *argv[])
{
    BlackMisc::initResources();
    QFile file(":/translations/blackmisc_i18n_de.qm");
    qDebug() << (file.exists() ? "Found translations in resources" : "No translations in resources");

    QTranslator translator;
    translator.load("blackmisc_i18n_de", ":/translations/");
    QCoreApplication a(argc, argv);

    qDebug() << "Use I18N version, y? n?";
    int yn = getchar();
    bool t = (yn == 'y' || yn == 'Y') ? a.installTranslator(&translator) : false;
    qDebug() << (t ? "Installed translator" : "No translator ");

    CSamplesPhysicalQuantities::samples();
    CSamplesAviation::samples();

    return a.exec();
}
