/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blacksim/blacksimfreefunctions.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "samplesfscommon.h"
#include "samplesfsx.h"
#include "samplesmodelmapping.h"

#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>

/*!
 * Samples
 */
int main(int argc, char *argv[])
{
    QTextStream streamIn(stdin);
    BlackMisc::registerMetadata();
    BlackSim::registerMetadata();

    qDebug() << "Run samples:";
    qDebug() << "1 .. FS common";
    qDebug() << "2 .. FSX";
    qDebug() << "3 .. Mappings";
    qDebug() << "x .. exit";
    QString i = streamIn.readLine().toLower().trimmed();

    if (i.startsWith("1"))
    {
        BlackSimTest::CSamplesFsCommon::samples();
    }
    else if (i.startsWith("2"))
    {
        BlackSimTest::CSamplesFsx::samples();
    }
    else if (i.startsWith("3"))
    {
        BlackSimTest::CSamplesModelMapping::samples();
    }
    else if (i.startsWith("x"))
    {
        return 0;
    }

    QCoreApplication a(argc, argv);
    return a.exec();
}
