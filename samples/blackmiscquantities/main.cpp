/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "samplesphysicalquantities.h"
#include "samplesaviation.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QTextStream>

using namespace BlackMisc;
using namespace BlackMiscTest;

//! Samples
int main(int argc, char *argv[])
{
    QTextStream out(stdout, QIODevice::WriteOnly);

    BlackMisc::initResources();
    QFile file(":blackmisc/translations/blackmisc_i18n_de.qm");
    out << (file.exists() ? "Found translations in resources" : "No translations in resources") << endl;
    QTranslator translator;
    bool t = translator.load("blackmisc_i18n_de", ":blackmisc/translations/");
    out << (t ? "Translator loaded" : "Translator not loaded") << endl;


    QCoreApplication a(argc, argv);
    out << "Use I18N version, y? n?";
    out.flush();
    int yn = getchar();
    t = (yn == 'y' || yn == 'Y');
    t =  t ? a.installTranslator(&translator) : false;
    out << (t ? "Installed translator" : "No translator ");

    CSamplesPhysicalQuantities::samples(out);
    CSamplesAviation::samples(out);

    return 0;
}
