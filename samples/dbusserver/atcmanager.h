#ifndef ATCMANAGER_H
#define ATCMANAGER_H

#include <QObject>
#include <QStringList>


class CAtcManager : public QObject
{
    Q_OBJECT

    QStringList m_atcList; //!< List of all controllers with their callsigns

public:
    CAtcManager(QObject *parent = 0);
    ~CAtcManager() {}

    /*!
     * \brief Returns a list of all controllers with their callsigns
     * \return atcList
     */
    QStringList atcList() const;
    
signals:
    
public slots:
    
};

#endif // ATCMANAGER_H
