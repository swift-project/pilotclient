// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef BLACKSAMPLE_WEATHERDATA_LINEREADER_H
#define BLACKSAMPLE_WEATHERDATA_LINEREADER_H

//! \file
//! \ingroup sampleweatherdata

#include "blackmisc/geo/coordinategeodetic.h"

#include <QObject>
#include <QThread>

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
    virtual void run() override;

signals:
    //! User is asking for weather data
    void weatherDataRequest(const BlackMisc::Geo::CCoordinateGeodetic &position);

    //! User is asking to quit
    void wantsToQuit();
};

#endif // guard
