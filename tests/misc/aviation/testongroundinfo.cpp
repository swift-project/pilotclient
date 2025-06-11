// SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testmisc
#include <QTest>

#include "test.h"

#include "misc/aviation/ongroundinfo.h"

using namespace swift::misc::aviation;

namespace MiscTest
{
    //! Aviation classes basic tests
    class CMiscOnGroundInfo : public QObject
    {
        Q_OBJECT
    private slots:
        void createDefault();
        void createFromGndFactorNotOnGround();
        void createFromGndFactorOnGround();
        void getGroundFactorUnknown();
        void getGroundFactorOnGround();
        void getGroundFactorNotOnGround();
    };

    void CMiscOnGroundInfo::createDefault()
    {
        COnGroundInfo info;
        QVERIFY2(info.getGroundFactor() == -1, "Wrong ground factor");
        QVERIFY2(info.getGroundDetails() == COnGroundInfo::NotSetGroundDetails, "Wrong ground details");
        QVERIFY2(info.getOnGround() == COnGroundInfo::OnGroundSituationUnknown, "Wrong on ground situation");
    }

    void CMiscOnGroundInfo::createFromGndFactorNotOnGround()
    {
        COnGroundInfo info(0.5);
        QCOMPARE(info.getGroundFactor(), 0.5);
        QVERIFY2(info.getGroundDetails() == COnGroundInfo::OnGroundByInterpolation, "Wrong ground details");
        QVERIFY2(info.getOnGround() == COnGroundInfo::NotOnGround, "Wrong on ground situation");
    }

    void CMiscOnGroundInfo::createFromGndFactorOnGround()
    {
        COnGroundInfo info(0.96);
        QCOMPARE(info.getGroundFactor(), 0.96);
        QVERIFY2(info.getGroundDetails() == COnGroundInfo::OnGroundByInterpolation, "Wrong ground details");
        QVERIFY2(info.getOnGround() == COnGroundInfo::OnGround, "Wrong on ground situation");
    }

    void CMiscOnGroundInfo::getGroundFactorUnknown()
    {
        COnGroundInfo info(COnGroundInfo::OnGroundSituationUnknown, COnGroundInfo::NotSetGroundDetails);
        QVERIFY2(info.getGroundFactor() == -1, "Wrong ground factor");
        QVERIFY2(info.getGroundDetails() == COnGroundInfo::NotSetGroundDetails, "Wrong ground details");
        QVERIFY2(info.getOnGround() == COnGroundInfo::OnGroundSituationUnknown, "Wrong on ground situation");
    }

    void CMiscOnGroundInfo::getGroundFactorOnGround()
    {
        COnGroundInfo info(COnGroundInfo::OnGround, COnGroundInfo::InFromNetwork);
        QVERIFY2(info.getGroundFactor() == 1, "Wrong ground factor");
        QVERIFY2(info.getGroundDetails() == COnGroundInfo::InFromNetwork, "Wrong ground details");
        QVERIFY2(info.getOnGround() == COnGroundInfo::OnGround, "Wrong on ground situation");
    }

    void CMiscOnGroundInfo::getGroundFactorNotOnGround()
    {
        COnGroundInfo info(COnGroundInfo::NotOnGround, COnGroundInfo::InFromNetwork);
        QVERIFY2(info.getGroundFactor() == 0, "Wrong ground factor");
        QVERIFY2(info.getGroundDetails() == COnGroundInfo::InFromNetwork, "Wrong ground details");
        QVERIFY2(info.getOnGround() == COnGroundInfo::NotOnGround, "Wrong on ground situation");
    }

} // namespace MiscTest

//! main
SWIFTTEST_APPLESS_MAIN(MiscTest::CMiscOnGroundInfo);

#include "testongroundinfo.moc"

//! \endcond
