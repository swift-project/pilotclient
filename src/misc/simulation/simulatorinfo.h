// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_SIMULATORINFO_H
#define SWIFT_MISC_SIMULATION_SIMULATORINFO_H

#include <QFlag>
#include <QFlags>
#include <QJsonObject>
#include <QList>
#include <QMetaType>
#include <QMultiMap>
#include <QSet>
#include <QStringList>

#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/statusmessage.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::Simulation, CSimulatorInfo)

namespace swift::misc::simulation
{
    //! Simple hardcoded info about the corresponding simulator.
    //!
    //! * in an ideal world this class would not exist, all would depend on flexible plugins \sa CSimulatorPluginInfo
    //! * in a real world the info is needed in a couple of places to specify the simulator
    //! ** when data from the swift datastore are read, the corresponding simulator is specified
    //! ** when model metadata are written to the swift datastore the DB simulator info needs to be provided
    //! ** when models are indexed from disk it does not know the corresponding driver
    //! ** also there is no strict dependency of some functions to the driver. I might not have the XP plugin installed,
    //!    but need to handle XP data from the swift datastore.
    //!
    //! If someone manages to remove this hardcoded simulator information and makes it entirely flexible
    //! based upon the plugin metadata feel free.
    class SWIFT_MISC_EXPORT CSimulatorInfo : public CValueObject<CSimulatorInfo>
    {
    public:
        //! Simulator
        enum SimulatorFlag
        {
            None = 0,
            FSX = 1 << 0,
            FS9 = 1 << 1,
            XPLANE = 1 << 2,
            P3D = 1 << 3,
            FG = 1 << 4,
            MSFS = 1 << 5,
            MSFS2024 = 1 << 6,
            FSX_P3D = FSX | P3D,
            AllFsFamily = FSX | FS9 | P3D | MSFS | MSFS2024,
            All = FSX | FS9 | XPLANE | P3D | FG | MSFS | MSFS2024
        };
        Q_DECLARE_FLAGS(Simulator, SimulatorFlag)

        //! Number of known individual simulators
        static constexpr int NumberOfSimulators = 7;

        //! Default constructor
        CSimulatorInfo();

        //! Constructor
        CSimulatorInfo(const QString &identifierString);

        //! Constructor
        CSimulatorInfo(const QStringList &simulators);

        //! Constructor
        CSimulatorInfo(Simulator s);

        //! Constructor
        CSimulatorInfo(int flagsAsInt);

        //! Constructor
        CSimulatorInfo(bool isFSX, bool isFS9, bool xp, bool isP3D, bool fg, bool msfs, bool msfs2024);

        //! Unspecified simulator
        bool isUnspecified() const;

        //! FSX?
        bool isFSX() const;

        //! FS9?
        bool isFS9() const;

        //! XPlane
        bool isXPlane() const;

        //! P3D?
        bool isP3D() const;

        //! FG?
        bool isFG() const;

        //! MSFS?
        bool isMSFS() const;

        //! MSFS2024?
        bool isMSFS2024() const;

        //! Any simulator?
        bool isAnySimulator() const;

        //! Single simulator selected
        bool isSingleSimulator() const;

        //! No simulator?
        bool isNoSimulator() const;

        //! Represents > 1 simulator
        bool isMultipleSimulators() const;

        //! Is all simulators?
        bool isAllSimulators() const;

        //! Microsoft Simulator?
        bool isMicrosoftSimulator() const;

        //! Microsoft Simulator or P3D?
        bool isMicrosoftOrPrepare3DSimulator() const;

        //! FSX family, i.e. FSX or P3D?
        bool isFsxP3DFamily() const;

        //! Number simulators selected
        int numberSimulators() const;

        //! Matches all simulators
        bool matchesAll(const CSimulatorInfo &otherInfo) const;

        //! Matches any simulator
        bool matchesAny(const CSimulatorInfo &otherInfo) const;

        //! Matches any simulator or None (NULL)
        //! \remark for cases where no specified sim. also matches
        bool matchesAnyOrNone(const CSimulatorInfo &otherInfo) const;

        //! Simulator
        Simulator getSimulator() const { return static_cast<Simulator>(m_simulator); }

        //! Simulator
        void setSimulator(Simulator s) { m_simulator = static_cast<int>(s); }

        //! Add simulator flags
        void addSimulator(Simulator s) { m_simulator |= static_cast<int>(s); }

        //! Add simulator
        void addSimulator(const CSimulatorInfo &simulatorInfo) { this->addSimulator(simulatorInfo.getSimulator()); }

        //! All simulators
        void setAllSimulators() { setSimulator(All); }

        //! \copydoc mixin::String::toQString
        int comparePropertyByIndex(CPropertyIndexRef index, const CSimulatorInfo &compareValue) const;

        //! \copydoc mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::mixin::Icon::toIcon()
        CIcons::IconIndex toIcon() const;

        //! Add simulator
        CSimulatorInfo add(const CSimulatorInfo &other);

        //! As a set of single simulator info objects
        QSet<CSimulatorInfo> asSingleSimulatorSet() const;

        //! All simulators selected become unselected and vice versa
        void invertSimulators();

        //! Validate simulators for an aircraft model
        CStatusMessage validateSimulatorsForModel() const;

        //! Bool flags to enum
        static Simulator boolToFlag(bool isFSX, bool isFS9, bool xp, bool isP3D, bool fg, bool msfs, bool msfs2024);

        //! Identifer, as provided by plugin
        static Simulator identifierToSimulator(const QString &identifier);

        //! All simulators
        static const CSimulatorInfo &allSimulators();

        //! All simulator strings
        static const QStringList &allSimulatorStrings();

        //! All simulators as set
        static const QSet<CSimulatorInfo> &allSimulatorsSet();

        //! All simulators of the FS family (P3D FSX, FS9)
        static const CSimulatorInfo &allFsFamilySimulators();

        //! Locally installed simulators
        static CSimulatorInfo getLocallyInstalledSimulators();

        //! Guess a default simulator based on installation
        static const CSimulatorInfo &guessDefaultSimulator();

        //! From database JSON
        static CSimulatorInfo fromDatabaseJson(const QJsonObject &json, const QString &prefix);

        //! @{
        //! Const simulator info objects
        static const CSimulatorInfo &fg()
        {
            static const CSimulatorInfo s(FG);
            return s;
        }
        static const CSimulatorInfo &fsx()
        {
            static const CSimulatorInfo s(FSX);
            return s;
        }
        static const CSimulatorInfo &p3d()
        {
            static const CSimulatorInfo s(P3D);
            return s;
        }
        static const CSimulatorInfo &fs9()
        {
            static const CSimulatorInfo s(FS9);
            return s;
        }
        static const CSimulatorInfo &xplane()
        {
            static const CSimulatorInfo s(XPLANE);
            return s;
        }
        static const CSimulatorInfo &msfs()
        {
            static const CSimulatorInfo s(MSFS);
            return s;
        }
        static const CSimulatorInfo &msfs2024()
        {
            static const CSimulatorInfo s(MSFS2024);
            return s;
        }
        //! @}

    private:
        int m_simulator = static_cast<int>(None);

        SWIFT_METACLASS(
            CSimulatorInfo,
            SWIFT_METAMEMBER(simulator));
    };

    //! Count per simulator, small utility class allows to retrieve values as per simulator
    class SWIFT_MISC_EXPORT CCountPerSimulator
    {
    public:
        //! Constructor
        CCountPerSimulator();

        //! Object count for given simulator
        int getCount(const CSimulatorInfo &simulator) const;

        //! Unknown count
        int getCountForUnknownSimulators() const;

        //! P3D, FSX, MSFS or FS9
        int getCountForFsFamilySimulators() const;

        //! P3D, MSFS or FSX
        int getCountForFsxFamilySimulators() const;

        //! Set count
        void setCount(int count, const CSimulatorInfo &simulator);

        //! Increase all simulators given here
        void increaseSimulatorCounts(const CSimulatorInfo &simulator);

        //! Maximum
        int getMaximum() const;

        //! Minimum
        int getMinimum() const;

        //! Number of simulators with count > 0
        int simulatorsRepresented() const;

        //! Sorted (ascending) per simulator
        QMultiMap<int, CSimulatorInfo> countPerSimulator() const;

        //! As QString
        QString toQString() const;

    private:
        QList<int> m_counts;
        static int internalIndex(const CSimulatorInfo &simulator);
        static CSimulatorInfo simulator(int internalIndex);
    };
} // namespace swift::misc::simulation

Q_DECLARE_METATYPE(swift::misc::simulation::CSimulatorInfo)
Q_DECLARE_METATYPE(swift::misc::simulation::CSimulatorInfo::SimulatorFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(swift::misc::simulation::CSimulatorInfo::Simulator)

#endif // SWIFT_MISC_SIMULATION_SIMULATORINFO_H
