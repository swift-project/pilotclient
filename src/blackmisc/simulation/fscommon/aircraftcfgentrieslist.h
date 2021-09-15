/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_FSCOMMON_AIRCRAFTCFGLIST_H
#define BLACKMISC_SIMULATION_FSCOMMON_AIRCRAFTCFGLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/fscommon/aircraftcfgentries.h"
#include "blackmisc/simulation/simulatorinfo.h"

#include <QMetaType>
#include <QString>
#include <QStringList>
#include <QSet>
#include <Qt>
#include <tuple>

namespace BlackMisc::Simulation::FsCommon
{
    //! Utility, providing FS aircraft.cfg entries
    class BLACKMISC_EXPORT CAircraftCfgEntriesList :
        public CSequence<CAircraftCfgEntries>,
        public Mixin::MetaType<CAircraftCfgEntriesList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAircraftCfgEntriesList)
        using CSequence::CSequence;

        //! Contains model with title?
        bool containsModelWithTitle(const QString &title, Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive);

        //! All titles (aka model names)
        QStringList getTitles(bool sorted = false) const;

        //! Titles as set in upper case
        QSet<QString> getTitleSetUpperCase() const;

        //! All titles as string
        QString getTitlesAsString(bool sorted, const QString &separator = ", ") const;

        //! As aircraft models
        BlackMisc::Simulation::CAircraftModelList toAircraftModelList(bool ignoreDuplicatesAndEmptyModelStrings, CStatusMessageList &msgs) const;

        //! As aircraft models for simulator
        BlackMisc::Simulation::CAircraftModelList toAircraftModelList(const BlackMisc::Simulation::CSimulatorInfo &simInfo, bool ignoreDuplicatesAndEmptyModelStrings, CStatusMessageList &msgs) const;

        //! Ambiguous titles
        QStringList detectAmbiguousTitles() const;

        //! Find by title
        CAircraftCfgEntriesList findByTitle(const QString &title, Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive) const;

        //! Can check if a title (model string) is known
        bool containsTitle(const QString &title) const;

    private:
        //! Section within file
        enum FileSection
        {
            General,
            Fltsim,
            Unknown
        };
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::FsCommon::CAircraftCfgEntriesList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::FsCommon::CAircraftCfgEntries>)

#endif // guard
