/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmiscfreefunctions.h"
#include "math/math.h"
#include "geo/geo.h"
#include "audio/audio.h"
#include "input/input.h"
#include "propertyindexlist.h"
#include "propertyindexvariantmap.h"
#include "namevariantpairlist.h"
#include "variantlist.h"
#include "variantmap.h"
#include "valuecache.h"
#include "rgbcolor.h"
#include "countrylist.h"
#include "statusmessagelist.h"
#include "pixmap.h"
#include "iconlist.h"
#include "identifierlist.h"
#include "logpattern.h"
#include <QtNetwork/QHostInfo>
#include <QProcessEnvironment>
#include <QSysInfo>
#include <QProcess>
#include <QBuffer>

void BlackMisc::initResources()
{
    initBlackMiscResources();
}

uint BlackMisc::calculateHash(const QList<uint> &values, const char *className)
{
    // http://stackoverflow.com/questions/113511/hash-code-implementation/113600#113600
    if (values.isEmpty()) return 0;
    uint hash = values.first();
    for (int i = 1; i < values.size(); i++)
    {
        hash = 37 * hash + values.at(i);
    }

    // same values, but different class?
    if (className)
    {
        hash = 37 * hash + qHash(QString(className));
    }
    return hash;
}

uint BlackMisc::calculateHash(const QList<int> &values, const char *className)
{
    QList<uint> list;
    uint s = 0;
    foreach(int i, values)
    {

        if (i >= 0)
        {
            list.append(static_cast<uint>(i));
        }
        else
        {
            list.append(static_cast<uint>(i));
            list.append(s++);
        }
    }
    return calculateHash(list, className);
}

const QString &BlackMisc::localHostName()
{
    static const QString hostName = QHostInfo::localHostName();
    return hostName;
}

const QString &BlackMisc::localHostNameEnvVariable()
{
    static const QString hostName = QProcessEnvironment::systemEnvironment().value("COMPUTERNAME", QProcessEnvironment::systemEnvironment().value("HOSTNAME"));
    return hostName;
}

bool BlackMisc::Audio::startWindowsMixer()
{
    QStringList parameterlist;
    return QProcess::startDetached("SndVol.exe", parameterlist);
}
