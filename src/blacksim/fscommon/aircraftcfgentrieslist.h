/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIM_FSCOMMON_AIRCRAFTCFG_H
#define BLACKSIM_FSCOMMON_AIRCRAFTCFG_H

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

        //! Utility, providing FSX aircraft.cfg entries
        class CAircraftCfgEntriesList : public BlackMisc::CSequence<CAircraftCfgEntries>
        {
        private:
            QString m_rootDirectory; //!< root directory reading aircraft.cfg files
            bool m_readForDirectory; //!< valid read for given directory

            //! Read all entries in one directory
            qint32 read(const QString &directory);

        public:

            //! Constructor
            CAircraftCfgEntriesList(const QString &rootDirectory = "") : m_rootDirectory(rootDirectory), m_readForDirectory(false) {}

            //! Read all aircraft.cfg files starting from root directory
            qint32 read()
            {
                if (this->m_readForDirectory) return this->size();

                // not read so far, read it
                this->clear();
                this->m_readForDirectory = true;
                return this->read(this->m_rootDirectory);
            }

            //! Change the directory
            bool changeDirectory(const QString &directory)
            {
                if (this->m_rootDirectory != directory)
                {
                    this->m_rootDirectory = directory;
                    this->m_readForDirectory = false;
                }
                return !directory.isEmpty() && this->existsDir(directory);
            }

            //! Virtual destructor
            virtual ~CAircraftCfgEntriesList() {}

            //! Does the directory exist?
            bool existsDir(const QString &directory = "") const;

            //! Has current directory been read?
            bool hasReadDirectory() const {  return this->m_readForDirectory; }

            //! Current root directory
            QString getRootDirectory() const {  return this->m_rootDirectory; }

            //! Contains model with title?
            bool containsModeWithTitle(const QString &title, Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive);

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::fromQVariant
            virtual void fromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! Unknown entries
            static const CAircraftCfgEntries &UnknownCfgEntries()
            {
                static CAircraftCfgEntries entries;
                return entries;
            }

            //! Register metadata
            static void registerMetadata();
        };
    }
}

Q_DECLARE_METATYPE(BlackSim::FsCommon::CAircraftCfgEntriesList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackSim::FsCommon::CAircraftCfgEntries>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackSim::FsCommon::CAircraftCfgEntries>)

#endif // guard
