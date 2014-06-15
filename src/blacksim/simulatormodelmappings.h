/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSIM_MODELMAPPINGS_H
#define BLACKSIM_MODELMAPPINGS_H

#include "blackmisc/nwaircraftmappinglist.h"
#include <QObject>

namespace BlackSim
{
    /*!
     * \brief Model mappings
     */
    class ISimulatorModelMappings : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        ISimulatorModelMappings(QObject *parent = nullptr);

        //! Destructor
        virtual ~ISimulatorModelMappings() {}

        //! Load data
        virtual bool load() = 0;

        //! Empty
        bool isEmpty() const;

        //! Size
        int size() const;

        //! Get list
        const BlackMisc::Network::CAircraftMappingList &getMappingList() const;

        //! Find by ICAO code
        BlackMisc::Network::CAircraftMappingList findByIcao(const BlackMisc::Aviation::CAircraftIcao &icao, bool emptyMeansWildCard = true) const;

    protected:
        BlackMisc::Network::CAircraftMappingList m_mappings; //!< Mappings
    };
}

#endif // guard
