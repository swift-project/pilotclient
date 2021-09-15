/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKCORE_AFV_AFVMAPREADER_H
#define BLACKCORE_AFV_AFVMAPREADER_H

#include "atcstationmodel.h"
#include "blackcore/blackcoreexport.h"

#include <QObject>
#include <QTimer>

namespace BlackCore::Afv::Model
{
    //! Map reader
    class BLACKCORE_EXPORT CAfvMapReader : public QObject
    {
        Q_OBJECT

        //! Map reader properties
        //! @{
        Q_PROPERTY(CSampleAtcStationModel *atcStationModel READ getAtcStationModel CONSTANT)
        //! @}

    public:
        //! Ctor
        CAfvMapReader(QObject *parent = nullptr);

        //! Own callsign
        Q_INVOKABLE void setOwnCallsign(const QString &callsign) { m_callsign = callsign; }

        //! Update ATC stations in model
        void updateFromMap();

        //! ATC model
        CSampleAtcStationModel *getAtcStationModel() { return m_model; }

    private:
        CSampleAtcStationModel *m_model = nullptr;
        QTimer *m_timer = nullptr;
        QString m_callsign;
    };
} // ns

#endif // guard
