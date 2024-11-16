// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_CORE_AFV_MODEL_ATCSTATIONMODEL_H
#define SWIFT_CORE_AFV_MODEL_ATCSTATIONMODEL_H

#include <QAbstractListModel>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QtGlobal>

#include "core/afv/dto.h"
#include "core/swiftcoreexport.h"

namespace swift::core::afv::model
{
    //! Sample ATC station
    class SWIFT_CORE_EXPORT CSampleAtcStation
    {
    public:
        //! Ctor
        CSampleAtcStation() {}

        //! Ctor
        CSampleAtcStation(const QString &callsign, const swift::core::afv::TransceiverDto &transceiver);

        //! @{
        //! Getter
        const QString &callsign() const { return m_callsign; }
        QString formattedFrequency() const;
        double latitude() const;
        double longitude() const;
        double radioDistanceM() const;
        quint32 frequency() const;
        //! @}

    private:
        QString m_callsign;
        TransceiverDto m_transceiver;
    };

    inline bool operator==(const CSampleAtcStation &lhs, const CSampleAtcStation &rhs)
    {
        return lhs.callsign() == rhs.callsign() &&
               qFuzzyCompare(lhs.latitude(), rhs.latitude()) &&
               qFuzzyCompare(lhs.longitude(), rhs.longitude());
    }

    //! Sample list model
    class CSampleAtcStationModel : public QAbstractListModel
    {
        Q_OBJECT

    public:
        //! Roles for model
        enum AtcStationRoles
        {
            CallsignRole = Qt::UserRole + 1,
            LatitudeRole,
            LongitudeRole,
            RadioDistanceRole,
            FrequencyRole,
            FrequencyKhzRole
        };

        //! Ctor
        CSampleAtcStationModel(QObject *parent = nullptr);

        //! Dtor
        virtual ~CSampleAtcStationModel() override;

        //! Update the stations
        void updateAtcStations(const QVector<CSampleAtcStation> &atcStations);

        //! copydoc QAbstractListModel::rowCount
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;

        //! copydoc QAbstractListModel::data
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    protected:
        //! copydoc QAbstractListModel::roleNames
        QHash<int, QByteArray> roleNames() const override;

    private:
        void addStation(const CSampleAtcStation &atcStation);
        void removeStationAtPosition(int i);

        QList<CSampleAtcStation> m_atcStations;
    };
} // namespace swift::core::afv::model

#endif // guard
