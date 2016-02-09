/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSAMPLE_WEATHERDATA_LINEREADER_H
#define BLACKSAMPLE_WEATHERDATA_LINEREADER_H

#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"
#include <QThread>
#include <QString>

/*!
 * Class reading line input and triggering commands
 */
class CLineReader : public QThread
{
    Q_OBJECT

public:
    //! Constructor
    CLineReader(QObject *parent = nullptr) : QThread(parent) {}

protected:
    //! \copydoc QThread::run
    void run();

signals:
    //! User is asking for weather data
    void weatherDataRequest(const BlackMisc::Geo::CLatitude &lat, const BlackMisc::Geo::CLongitude &lon);

    //! User is asking to quit
    void quit();
};

#endif // guard
