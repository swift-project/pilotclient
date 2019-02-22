/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
