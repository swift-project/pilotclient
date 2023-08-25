// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef BLACKCORE_AFV_AFVMAPREADER_H
#define BLACKCORE_AFV_AFVMAPREADER_H

#include "blackcore/afv/model/atcstationmodel.h"
#include "blackcore/blackcoreexport.h"

#include <QObject>
#include <QTimer>

namespace BlackCore::Afv::Model
{
    //! Map reader
    class BLACKCORE_EXPORT CAfvMapReader : public QObject
    {
        Q_OBJECT

        //! @{
        //! Map reader properties
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
