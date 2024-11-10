// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testmisc

#include "misc/iconlist.h"
#include "test.h"
#include <QString>
#include <QTest>

using namespace swift::misc;

namespace MiscTest
{
    //! Testing icons
    class CTestIcon : public QObject
    {
        Q_OBJECT

    private slots:
        //! Icon order/index
        void iconIndex();
    };

    void CTestIcon::iconIndex()
    {
        const int max = static_cast<int>(CIcons::IconIsGenerated);
        QVERIFY2(CIcon::allIcons().size() == max, "Icon list size");

        int c = 0;
        for (const CIcon &icon : CIcon::allIcons())
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
} // namespace

//! main
BLACKTEST_MAIN(MiscTest::CTestIcon);

#include "testicon.moc"

//! \endcond
