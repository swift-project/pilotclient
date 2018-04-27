/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISCTEST_TESTAIRCRAFTSITUATION_H
#define BLACKMISCTEST_TESTAIRCRAFTSITUATION_H

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include "blackmisc/aviation/aircraftsituationlist.h"
#include <QObject>

namespace BlackMiscTest
{
    /*!
     * CAircraftSituation and CAircraftSituationChange tests
     */
    class CTestAircraftSituation : public QObject
    {
        Q_OBJECT

    public:
        //! Standard test case constructor
        explicit CTestAircraftSituation(QObject *parent = nullptr);

    private slots:
        //! All GND flags
        void allGndFlagsAndTakeOff() const;

        //! All not GND flags
        void allNotGndFlagsAndTouchdown() const;

        //! Ascending aircraft
        void ascending();

        //! Descending aircraft
        void descending();

        //! Rotating up aircraft
        void rotateUp();

        //! Test sort order
        void sortOrder() const;

    private:
        //! Test situations (ascending)
        static BlackMisc::Aviation::CAircraftSituationList testSituations();

        //! Set descending altitudes
        static BlackMisc::Aviation::CAircraftSituationList testSetDescendingAltitudes(const BlackMisc::Aviation::CAircraftSituationList &situations);

        //! Set descending altitudes
        static BlackMisc::Aviation::CAircraftSituationList testSetRotateUpPitch(const BlackMisc::Aviation::CAircraftSituationList &situations);
    };
} // namespace

//! \endcond

#endif // guard
