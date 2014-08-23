/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

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

        //! Find by ICAO code, empty fields are treated as wildcards
        BlackMisc::Network::CAircraftMappingList findByIcaoWildcard(const BlackMisc::Aviation::CAircraftIcao &icao) const;

        //! Find by ICAO code, empty fields are treated literally
        BlackMisc::Network::CAircraftMappingList findByIcaoExact(const BlackMisc::Aviation::CAircraftIcao &icao) const;

    protected:
        BlackMisc::Network::CAircraftMappingList m_mappings; //!< Mappings
    };
}

#endif // guard
