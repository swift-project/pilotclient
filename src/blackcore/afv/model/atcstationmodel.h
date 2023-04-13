/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKCORE_AFV_MODEL_ATCSTATIONMODEL_H
#define BLACKCORE_AFV_MODEL_ATCSTATIONMODEL_H

#include "blackcore/afv/dto.h"
#include "blackcore/blackcoreexport.h"

#include <QtGlobal>
#include <QAbstractListModel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QObject>

namespace BlackCore::Afv::Model
{
    //! Sample ATC station
    class BLACKCORE_EXPORT CSampleAtcStation
    {
    public:
        //! Ctor
        CSampleAtcStation() {}

        //! Ctor
        CSampleAtcStation(const QString &callsign, const BlackCore::Afv::TransceiverDto &transceiver);

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
} // ns

#endif // guard
