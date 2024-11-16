// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_FSCOMMON_AIRCRAFTCFGLIST_H
#define SWIFT_MISC_SIMULATION_FSCOMMON_AIRCRAFTCFGLIST_H

#include <tuple>

#include <QMetaType>
#include <QSet>
#include <QString>
#include <QStringList>
#include <Qt>

#include "misc/collection.h"
#include "misc/sequence.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/fscommon/aircraftcfgentries.h"
#include "misc/simulation/simulatorinfo.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::simulation::fscommon, CAircraftCfgEntries, CAircraftCfgEntriesList)

namespace swift::misc::simulation::fscommon
{
    //! Utility, providing FS aircraft.cfg entries
    class SWIFT_MISC_EXPORT CAircraftCfgEntriesList :
        public CSequence<CAircraftCfgEntries>,
        public mixin::MetaType<CAircraftCfgEntriesList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CAircraftCfgEntriesList)
        using CSequence::CSequence;

        //! Default constructor.
        CAircraftCfgEntriesList();

        //! Construct from a base class object.
        CAircraftCfgEntriesList(const CSequence<CAircraftCfgEntries> &other);

        //! Contains model with title?
        bool containsModelWithTitle(const QString &title, Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive);

        //! All titles (aka model names)
        QStringList getTitles(bool sorted = false) const;

        //! Titles as set in upper case
        QSet<QString> getTitleSetUpperCase() const;

        //! All titles as string
        QString getTitlesAsString(bool sorted, const QString &separator = ", ") const;

        //! As aircraft models
        swift::misc::simulation::CAircraftModelList toAircraftModelList(bool ignoreDuplicatesAndEmptyModelStrings, CStatusMessageList &msgs) const;

        //! As aircraft models for simulator
        swift::misc::simulation::CAircraftModelList toAircraftModelList(const swift::misc::simulation::CSimulatorInfo &simInfo, bool ignoreDuplicatesAndEmptyModelStrings, CStatusMessageList &msgs) const;

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
} // namespace swift::misc::simulation::fscommon

Q_DECLARE_METATYPE(swift::misc::simulation::fscommon::CAircraftCfgEntriesList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::simulation::fscommon::CAircraftCfgEntries>)

#endif // guard
