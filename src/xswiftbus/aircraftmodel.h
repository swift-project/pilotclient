/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIM_XSWIFTBUS_AIRCRAFTMODEL_H
#define BLACKSIM_XSWIFTBUS_AIRCRAFTMODEL_H

#include <string>

namespace XSwiftBus
{

    //! Simplified implementation of \sa BlackMisc::Simulation::CDistributor
    class CDistributor
    {
    public:
        //! Default constructor
        CDistributor() = default;

        //! Constructor
        CDistributor(const std::string &description) : m_description(description) {}

        //! \copydoc BlackMisc::Simulation::CDistributor::hasDescription
        bool hasDescription() const { return !m_description.empty(); }

        //! \copydoc BlackMisc::Simulation::CDistributor::getDescription
        std::string getDescription() const { return m_description; }

    private:
        std::string m_description;
    };

    //! Simplified implementation of \sa BlackMisc::Simulation::CAircraftModel
    class CAircraftModel
    {
    public:
        CAircraftModel() = default;

        //! \copydoc BlackMisc::Simulation::CAircraftModel::hasDescription
        bool hasDescription() const { return !m_description.empty(); }

        //! \copydoc BlackMisc::Simulation::CAircraftModel::hasAircraftDesignator
        bool hasAircraftDesignator() const { return !m_icao.empty(); }

        //! \copydoc BlackMisc::Simulation::CAircraftModel::getName
        std::string getName() const { return m_name; }

        //! \copydoc BlackMisc::Simulation::CAircraftModel::getDistributor
        CDistributor getDistributor() const { return m_distributor; }

        //! \copydoc BlackMisc::Simulation::CAircraftModel::getAircraftIcaoCodeDesignator
        std::string getAircraftIcaoCodeDesignator() const { return m_icao; }

        //! \copydoc BlackMisc::Simulation::CAircraftModel::getModelString
        std::string getModelString() const { return m_modelString; }

        //! \copydoc BlackMisc::Simulation::CAircraftModel::setAircraftIcaoCode
        void setAircraftIcaoCode(const std::string &icao) { m_icao = icao; }

        //! \copydoc BlackMisc::Simulation::CAircraftModel::setDescription
        void setDescription(const std::string &description) { m_description = description; }

        //! \copydoc BlackMisc::Simulation::CAircraftModel::setName
        void setName(const std::string &name) { m_name = name; }

        //! \copydoc BlackMisc::Simulation::CAircraftModel::setDistributor
        void setDistributor(const CDistributor &distributor) { m_distributor = distributor; }

        //! \copydoc BlackMisc::Simulation::CAircraftModel::setModelString
        void setModelString(const std::string &modelString) { m_modelString = modelString; }

    private:
        std::string m_name;
        std::string m_icao;
        std::string m_description;
        CDistributor m_distributor;
        std::string m_modelString;
    };

}

#endif // guard
