/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIM_FSCOMMON_AIRCRAFTCFGLIST_H
#define BLACKSIM_FSCOMMON_AIRCRAFTCFGLIST_H

#include "blackmisc/simulation/aircraftmodellist.h"
#include "aircraftcfgentries.h"
#include "blackmisc/sequence.h"
#include "blackmisc/collection.h"
#include <QDir>
#include <QVector>
#include <QDebug>
#include <QSettings>

namespace BlackSim
{
    namespace FsCommon
    {

        //! Utility, providing FS aircraft.cfg entries
        class CAircraftCfgEntriesList : public BlackMisc::CSequence<CAircraftCfgEntries>
        {

        public:

            //! Constructor
            CAircraftCfgEntriesList(const QString &rootDirectory = "") : m_rootDirectory(rootDirectory) {}

            //! Read all aircraft.cfg files starting from root directory
            int read()
            {
                if (this->m_readForDirectory) { return this->size(); }

                // not read so far, read it
                this->clear();
                this->m_readForDirectory = true;
                return this->read(this->m_rootDirectory, excludeDirectories());
            }

            //! Change the directory
            bool changeDirectory(const QString &directory)
            {
                if (this->m_rootDirectory != directory)
                {
                    this->m_rootDirectory = directory;
                    this->m_readForDirectory = false;
                }
                return (!directory.isEmpty() && this->existsDir(directory));
            }

            //! Virtual destructor
            virtual ~CAircraftCfgEntriesList() {}

            //! Does the directory exist?
            bool existsDir(const QString &directory = "") const;

            //! Has current directory been read?
            bool hasReadDirectory() const {  return this->m_readForDirectory; }

            //! Cancel read
            void cancelRead() { m_cancelRead = true; }

            //! Current root directory
            QString getRootDirectory() const {  return this->m_rootDirectory; }

            //! Contains model with title?
            bool containsModelWithTitle(const QString &title, Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive);

            //! All titles (aka model names)
            QStringList getTitles(bool sorted = false) const;

            //! As aircraft models
            BlackMisc::Simulation::CAircraftModelList toAircraftModelList() const;

            //! Ambiguous titles
            QStringList detectAmbiguousTitles() const;

            //! Find by title
            CAircraftCfgEntriesList findByTitle(const QString &title, Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive) const;

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! Do not include the following directories for FS
            static const QStringList &excludeDirectories()
            {
                static const QStringList exclude(
                {
                    "SimObjects/Animals",
                    "SimObjects/Misc",
                    "SimObjects/GroundVehicles",
                    "SimObjects/Boats"
                });
                return exclude;
            }

            //! Register metadata
            static void registerMetadata();

        private:
            QString m_rootDirectory;         //!< root directory reading aircraft.cfg files
            bool m_readForDirectory = false; //!< valid read for given directory
            bool m_cancelRead = false;

            //! Read all entries in one directory
            int read(const QString &directory, const QStringList &excludeDirectories = QStringList());

            //! Fix the content read
            static QString fixedStringContent(const QVariant &qv);

            //! Value from settings, fixed string
            static QString fixedStringContent(const QSettings &settings, const QString &key);
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackSim::FsCommon::CAircraftCfgEntriesList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackSim::FsCommon::CAircraftCfgEntries>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackSim::FsCommon::CAircraftCfgEntries>)

#endif // guard
