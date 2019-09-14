#ifndef ATCSTATIONMODEL_H
#define ATCSTATIONMODEL_H

#include "dto.h"
#include <QtGlobal>
#include <QAbstractListModel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class AtcStation
{
public:
    AtcStation() {}
    AtcStation(const QString &callsign, const TransceiverDto &transceiver);

    QString callsign() const;
    double latitude() const;
    double longitude() const;
    quint32 frequency() const;

    QString formattedFrequency() const;

    double radioDistanceM() const;

private:
    QString m_callsign;
    TransceiverDto m_transceiver;
};

inline bool operator==(const AtcStation& lhs, const AtcStation& rhs)
{
    return lhs.callsign() == rhs.callsign() &&
            qFuzzyCompare(lhs.latitude(), rhs.latitude()) &&
            qFuzzyCompare(lhs.longitude(), rhs.longitude());
}


class AtcStationModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum AtcStationRoles {
        CallsignRole = Qt::UserRole + 1,
        LatitudeRole,
        LongitudeRole,
        RadioDistanceRole,
        FrequencyRole,
        FrequencyKhzRole
    };

    AtcStationModel(QObject *parent = nullptr);
    virtual ~AtcStationModel();

    void updateAtcStations(const QVector<AtcStation> &atcStations);

    int rowCount(const QModelIndex & parent = QModelIndex()) const override;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

protected:
    QHash<int, QByteArray> roleNames() const override;
private:
    void addStation(const AtcStation &atcStation);
    void removeStationAtPosition(int i);

    QList<AtcStation> m_atcStations;
};

#endif // ATCSTATIONMODEL_H
