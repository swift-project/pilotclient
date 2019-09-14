#ifndef AFVMAPREADER_H
#define AFVMAPREADER_H

#include "models/atcstationmodel.h"

#include <QObject>
#include <QTimer>

class AFVMapReader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AtcStationModel* atcStationModel READ getAtcStationModel CONSTANT)
public:
    AFVMapReader(QObject *parent = nullptr);

    Q_INVOKABLE void setOwnCallsign(const QString &callsign) { m_callsign = callsign; }

    void updateFromMap();

    AtcStationModel *getAtcStationModel() { return model; }

private:
    AtcStationModel *model = nullptr;
    QTimer *timer = nullptr;
    QString m_callsign;
};

#endif // AFVMAPREADER_H
