#include "atcstationmodel.h"
#include "dto.h"
#include <QtMath>

AtcStation::AtcStation(const QString &callsign, const TransceiverDto &transceiver) :
    m_callsign(callsign),
    m_transceiver(transceiver)
{ }

double AtcStation::latitude() const
{
    return m_transceiver.LatDeg;
}

double AtcStation::longitude() const
{
    return m_transceiver.LonDeg;
}

quint32 AtcStation::frequency() const
{
    return m_transceiver.frequency;
}


QString AtcStation::formattedFrequency() const
{
    return QString::number(m_transceiver.frequency / 1000000.0, 'f', 3);
}

double AtcStation::radioDistanceM() const
{
    double sqrtAltM = qSqrt(m_transceiver.HeightMslM);
    const double radioFactor = 4193.18014745372;

    return radioFactor * sqrtAltM;
}

QString AtcStation::callsign() const
{
    return m_callsign;
}

AtcStationModel::AtcStationModel(QObject *parent) :
    QAbstractListModel(parent)
{

}

AtcStationModel::~AtcStationModel() {}

void AtcStationModel::updateAtcStations(const QVector<AtcStation> &atcStations)
{
    // Add stations which didn't exist yet
    for (const auto &station : atcStations)
    {
        if (! m_atcStations.contains(station)) { addStation(station); }
    }

    // Remove all stations which are no longer there
    for (int i = m_atcStations.size() - 1; i >= 0; i--)
    {
        AtcStation &station = m_atcStations[i];
        if (! m_atcStations.contains(station))
        {
            removeStationAtPosition(i);
        }
    }
}

void AtcStationModel::addStation(const AtcStation &atcStation)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_atcStations << atcStation;
    endInsertRows();
}

void AtcStationModel::removeStationAtPosition(int i)
{
    beginRemoveRows(QModelIndex(), i, i);
    m_atcStations.removeAt(i);
    endRemoveRows();
}

int AtcStationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_atcStations.count();
}

QVariant AtcStationModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_atcStations.count())
        return QVariant();

    const AtcStation &atcStation = m_atcStations[index.row()];
    if (role == CallsignRole)
        return atcStation.callsign();
    else if (role == LatitudeRole)
        return atcStation.latitude();
    else if (role == LongitudeRole)
        return atcStation.longitude();
    else if (role == RadioDistanceRole)
        return atcStation.radioDistanceM();
    else if (role == FrequencyRole)
        return atcStation.formattedFrequency();
    else if (role == FrequencyKhzRole)
        return atcStation.frequency() / 1000;
    return QVariant();
}

QHash<int, QByteArray> AtcStationModel::roleNames() const
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
