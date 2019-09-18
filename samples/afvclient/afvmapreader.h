/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKSAMPLE_AFVMAPREADER_H
#define BLACKSAMPLE_AFVMAPREADER_H

#include "models/atcstationmodel.h"

#include <QObject>
#include <QTimer>

//! Map reader
class AFVMapReader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(CSampleAtcStationModel* atcStationModel READ getAtcStationModel CONSTANT)

public:
    //! Ctor
    AFVMapReader(QObject *parent = nullptr);

    Q_INVOKABLE void setOwnCallsign(const QString &callsign) { m_callsign = callsign; }

    void updateFromMap();

    CSampleAtcStationModel *getAtcStationModel() { return m_model; }

private:
    CSampleAtcStationModel *m_model = nullptr;
    QTimer *m_timer = nullptr;
    QString m_callsign;
};

#endif // guard
