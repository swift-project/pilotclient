/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/afv/model/atcstationmodel.h"
#include <QtMath>

using namespace BlackCore::Afv;

namespace BlackCore::Afv::Model
{
    CSampleAtcStation::CSampleAtcStation(const QString &callsign, const TransceiverDto &transceiver) : m_callsign(callsign),
                                                                                                       m_transceiver(transceiver)
    {}

    double CSampleAtcStation::latitude() const
    {
        return m_transceiver.LatDeg;
    }

    double CSampleAtcStation::longitude() const
    {
        return m_transceiver.LonDeg;
    }

    quint32 CSampleAtcStation::frequency() const
    {
        return m_transceiver.frequencyHz;
    }

    QString CSampleAtcStation::formattedFrequency() const
    {
        return QString::number(m_transceiver.frequencyHz / 1000000.0, 'f', 3);
    }

    double CSampleAtcStation::radioDistanceM() const
    {
        const double sqrtAltM = qSqrt(m_transceiver.HeightMslM);
        const double radioFactor = 4193.18014745372;
        return radioFactor * sqrtAltM;
    }

    CSampleAtcStationModel::CSampleAtcStationModel(QObject *parent) : QAbstractListModel(parent)
    {}

    CSampleAtcStationModel::~CSampleAtcStationModel() {}

    void CSampleAtcStationModel::updateAtcStations(const QVector<CSampleAtcStation> &atcStations)
    {
        // Add stations which didn't exist yet
        for (const auto &station : atcStations)
        {
            if (!m_atcStations.contains(station)) { addStation(station); }
        }

        // Remove all stations which are no longer there
        for (int i = m_atcStations.size() - 1; i >= 0; i--)
        {
            CSampleAtcStation &station = m_atcStations[i];
            if (!m_atcStations.contains(station))
            {
                removeStationAtPosition(i);
            }
        }
    }

    void CSampleAtcStationModel::addStation(const CSampleAtcStation &atcStation)
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        m_atcStations << atcStation;
        endInsertRows();
    }

    void CSampleAtcStationModel::removeStationAtPosition(int i)
    {
        beginRemoveRows(QModelIndex(), i, i);
        m_atcStations.removeAt(i);
        endRemoveRows();
    }

    int CSampleAtcStationModel::rowCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent)
        return m_atcStations.count();
    }

    QVariant CSampleAtcStationModel::data(const QModelIndex &index, int role) const
    {
        if (index.row() < 0 || index.row() >= m_atcStations.count())
            return QVariant();

        const CSampleAtcStation &atcStation = m_atcStations[index.row()];
        if (role == CallsignRole) return atcStation.callsign();
        if (role == LatitudeRole) return atcStation.latitude();
        if (role == LongitudeRole) return atcStation.longitude();
        if (role == RadioDistanceRole) return atcStation.radioDistanceM();
        if (role == FrequencyRole) return atcStation.formattedFrequency();
        if (role == FrequencyKhzRole) return atcStation.frequency() / 1000;
        return QVariant();
    }

    QHash<int, QByteArray> CSampleAtcStationModel::roleNames() const
    {
        QHash<int, QByteArray> roles;
        roles[CallsignRole] = "callsign";
        roles[LatitudeRole] = "latitude";
        roles[LongitudeRole] = "longitude";
        roles[RadioDistanceRole] = "radioDistanceM";
        roles[FrequencyRole] = "frequencyAsString";
        roles[FrequencyKhzRole] = "frequencyKhz";
        return roles;
    }
}
