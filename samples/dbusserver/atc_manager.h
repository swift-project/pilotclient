/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ATCMANAGER_H
#define ATCMANAGER_H

#include <QObject>
#include <QStringList>

/*! 
 *  \brief     ATC Controller Managing class
 *  \details   This class is used hold a map of all controllers in the network and visible to the user
 *  \author    Roland Winklmeier
 *  \version   0.1
 *  \date      July 2013
 */

class CAtcManager : public QObject
{
    Q_OBJECT

    QStringList m_atcList; //!< List of all controllers with their callsigns

public:

	/*!
     * \brief Default constructor
	 * \param parent
     */
    CAtcManager(QObject *parent = 0);
	
	/*!
     * \brief Destructor
     */
    ~CAtcManager() {}

    /*!
     * \brief Returns a list of all controllers with their callsigns
     * \return atcList
     */
    QStringList atcList() const;

    /*!
     * \brief Add new ATC controller
     * \return controller
     */
    void addATC( const QString &controller );
    
signals:
    
public slots:
    
};

#endif // ATCMANAGER_H
