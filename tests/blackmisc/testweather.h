/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISCTEST_TESTWEATHER_H
#define BLACKMISCTEST_TESTWEATHER_H

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include <QObject>

namespace BlackMiscTest
{
    //! Aviation classes basic tests
    class CTestWeather : public QObject
    {
        Q_OBJECT

    public:
        //! Standard test case constructor
        explicit CTestWeather(QObject *parent = nullptr);

    private slots:
        //! Testing CCloudLayer/CCloudLayerList
        void cloudLayer();

        //! Testing CTemperatureLayer/CTemperatureLayerList
        void temperatureLayer();

        //! Testing CVisibilityLayer/CVisibilityLayerList
        void visibilityLayer();

        //! Testing CWindLayer/CWindLayerList
        void windLayer();

        //! Testing METAR decoder
        void metarDecoder();
    };
} // namespace

//! \endcond

#endif // guard
