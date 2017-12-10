/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include "testicon.h"
#include "blackmisc/iconlist.h"
#include <QString>
#include <QTest>

using namespace BlackMisc;

namespace BlackMiscTest
{
    void CTestIcon::iconIndex()
    {
        const int max = static_cast<int>(CIcons::IconIsGenerated);
        QVERIFY2(CIconList::allIcons().size() == max, "Icon list size");

        int c = 0;
        for (const CIcon &icon : CIconList::allIcons())
        {
            CIcons::IconIndex ii = icon.getIndex();
            const int i = static_cast<int>(ii);
            if (i != c)
            {
                static const QString verifyMsg("Wrong index %1/%2 for icon %3");
                qDebug() << verifyMsg.arg(i).arg(c).arg(icon.getDescriptiveText());
                QVERIFY2(i == c, "Wrong icon index");
            }
            c++;
        }
    }
} //namespace

//! \endcond
