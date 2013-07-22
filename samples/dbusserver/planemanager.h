#ifndef PLANEMANAGER_H
#define PLANEMANAGER_H

#include <QObject>
#include <QStringList>

class CPlaneManager : public QObject
{
    Q_OBJECT

    QStringList m_pilotsList; //!< List of all pilots with their callsigns

public:
    CPlaneManager(QObject *parent = 0);
    ~CPlaneManager();

    /*!
     * \brief Returns a list of all pilots with their callsigns
     * \return pilotList
     */
    QStringList pilotList() const;

signals:

public slots:
};

#endif // PLANEMANAGER_H
