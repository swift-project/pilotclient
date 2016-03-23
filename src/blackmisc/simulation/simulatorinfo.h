/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_SIMULATORINFO_H
#define BLACKMISC_SIMULATION_SIMULATORINFO_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"

namespace BlackMisc
{
    namespace Simulation
    {
        //! Simple hardcoded info about the corresponding simulator.
        //! * in an ideal world this class would not exist, all would depend on flexible plugins \sa CSimulatorPluginInfo
        //! * in a real world the info is needed in a couple of places to specify the simulator
        //! ** when data from the swift data store a read, the corresponding simulator is specified
        //! ** when model metadata are written to the swift data store the DB simulator info needs to be provided
        //! ** when models are indexed from disk it does not know the corresponding driver
        //! ** also there is no strict dependency of some functions to the driver. I might not have the XP plugin installed,
        //!    but need to handle XP data from the swift data store
        //! If someone manages to remove this hardocded simulator information and makes it entirely flexible
        //! based upon the plugin metadata feel free.
        class BLACKMISC_EXPORT CSimulatorInfo : public BlackMisc::CValueObject<CSimulatorInfo>
        {
        public:
            //! Simulator
            enum SimulatorFlag
            {
                None    = 0,
                FSX     = 1 << 0,
                FS9     = 1 << 1,
                XPLANE  = 1 << 2,
                P3D     = 1 << 3,
                FSX_P3D = FSX | P3D,
                AllMS   = FSX | FS9 | P3D,
                All     = FSX | FS9 | XPLANE | P3D
            };
            Q_DECLARE_FLAGS(Simulator, SimulatorFlag)

            //! Default constructor
            CSimulatorInfo();

            //! Constructor
            CSimulatorInfo(const QString &identifierString);

            //! Constructor
            CSimulatorInfo(Simulator s);

            //! Constructor
            CSimulatorInfo(int flagsAsInt);

            //! Constructor
            CSimulatorInfo(bool fsx, bool fs9, bool xp, bool p3d);

            //! Unspecified simulator
            bool isUnspecified() const;

            //! FSX?
            bool fsx() const;

            //! FS9?
            bool fs9() const;

            //! XPlane
            bool xplane() const;

            //! P3D?
            bool p3d() const;

            //! Any simulator?
            bool isAnySimulator() const;

            //! Single simulator selected
            bool isSingleSimulator() const;

            //! No simulator?
            bool isNoSimulator() const;

            //! Is all simulators?
            bool isAllSimulators() const;

            //! Number simulators selected
            int numberSimulators() const;

            //! Matches all simulators
            bool matchesAll(const CSimulatorInfo &otherInfo) const;

            //! Matches any simulator
            bool matchesAny(const CSimulatorInfo &otherInfo) const;

            //! Simulator
            Simulator getSimulator() const { return static_cast<Simulator>(m_simulator); }

            //! Simulator
            void setSimulator(Simulator s) { m_simulator = static_cast<int>(s); }

            //! All simulators
            void setAllSimulators() { setSimulator(All); }

            //! Compare for index
            int comparePropertyByIndex(const CSimulatorInfo &compareValue, const CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! Add simulator
            void add(const CSimulatorInfo &other);

            //! Bool flags to enum
            static Simulator boolToFlag(bool fsx, bool fs9, bool xp, bool p3d);

            //! Identifer, as provided by plugin
            static Simulator identifierToFlag(const QString &identifier);

            //! All simulators
            static const CSimulatorInfo &allSimulators();

            //! Locally installed simulators
            static const CSimulatorInfo getLocallyInstalledSimulators();

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CSimulatorInfo)
            int m_simulator = static_cast<int>(None);
        };
    } // ns
} // ns

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Simulation::CSimulatorInfo, (
                                   attr(o.m_simulator)
                               ))
Q_DECLARE_METATYPE(BlackMisc::Simulation::CSimulatorInfo)
Q_DECLARE_METATYPE(BlackMisc::Simulation::CSimulatorInfo::SimulatorFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackMisc::Simulation::CSimulatorInfo::Simulator)

#endif // guard
