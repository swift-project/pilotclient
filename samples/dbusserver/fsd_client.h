/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FSDCLIENT_H
#define FSDCLIENT_H

#include <QObject>

/*! 
 *  \brief     FSD client
 *  \details   This class implements methods to connect to a FSD server
 *  \author    Roland Winklmeier
 *  \version   0.1
 *  \date      July 2013
 */

class CFsdClient : public QObject
{
    Q_OBJECT
public:

	/*!
     * \brief Constructor
     * \param parent
     */
    CFsdClient(QObject *parent = 0);

	/*!
     * \brief Starts connection to FSD server
     * \param host
     */
    void connectTo(const QString &host);
    
signals:
    
public slots:
    
};

#endif // FSDCLIENT_H
