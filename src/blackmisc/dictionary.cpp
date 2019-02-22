/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/dictionary.h"

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
