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

#include "testflightplan.h"
#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/aviation/selcal.h"
#include "blackmisc/network/voicecapabilities.h"
#include <QTest>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;

namespace BlackMiscTest
{
    void CTestFlightPlan::flightPlanRemarks()
    {
        QString remarks;
        CFlightPlanRemarks fpRemarks(remarks);
        QVERIFY2(fpRemarks.isEmpty(), "Expect NULL flight plan remarks");

        remarks = QStringLiteral("RT/KESTREL OPR/MYTRAVEL REG/G-DAJC SEL/FP-ES PER/C NAV/RNP10");
        fpRemarks = CFlightPlanRemarks(remarks);
        QVERIFY2(fpRemarks.getSelcalCode().getCode() == "FPES" , "Wrong SELCAL code");
        QVERIFY2(fpRemarks.getFlightOperator() == "MYTRAVEL" , "Wrong flight operator");
        QVERIFY2(fpRemarks.getRegistration().asString() == "GDAJC" , "Wrong registration");

        remarks = QStringLiteral("OPR/UAL CALLSIGN/UNITED");
        fpRemarks = CFlightPlanRemarks(remarks);
        QVERIFY2(fpRemarks.getAirlineIcao().getDesignator() == "UAL" , "Wrong airline, expect UAL");
        QVERIFY2(fpRemarks.getFlightOperator().isEmpty() , "Expect to operator, should be in airline");
        QVERIFY2(fpRemarks.getRegistration().isEmpty(), "Expect no registration");
        QVERIFY2(fpRemarks.getRadioTelephony() == "UNITED" , "Expect telephony");

        remarks = QStringLiteral("/v/FPL-VIR9-IS-A346/DEP/S-EGLL/ARR/KJFK/REG/G-VGAS/TCAS RVR/200 OPR/VIRGIN AIRLINES");
        fpRemarks = CFlightPlanRemarks(remarks);
        QVERIFY2(fpRemarks.getRegistration().asString() == "GVGAS" , "Wrong registration");
        QVERIFY2(fpRemarks.getFlightOperator() == "VIRGIN AIRLINES", "Wrong operator");
        QVERIFY2(fpRemarks.getVoiceCapabilities().getCapabilities() == CVoiceCapabilities::Voice, "Wrong airline, expect UAL");

        remarks = QStringLiteral("/v/FPL-VIR9-IS-A346/ OPR/VIRGIN AIRLINES DEP/S-EGLL/ARR/KJFK/REG/G-VGAS/TCAS RVR/200");
        fpRemarks = CFlightPlanRemarks(remarks);
        QVERIFY2(fpRemarks.getRegistration().asString() == "GVGAS" , "Wrong registration");
        QVERIFY2(fpRemarks.getFlightOperator() == "VIRGIN AIRLINES", "Wrong operator");
        QVERIFY2(fpRemarks.getVoiceCapabilities().getCapabilities() == CVoiceCapabilities::Voice, "Wrong airline, expect UAL");
    }
} // ns

//! \endcond
