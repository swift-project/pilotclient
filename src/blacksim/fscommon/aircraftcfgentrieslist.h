/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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

        //! \brief Utility, providing FSX aircraft.cfg entries
        class CAircraftCfgEntriesList : public BlackMisc::CSequence<CAircraftCfgEntries>
        {
        private:
            QString m_rootDirectory; //!< root directory reading aircraft.cfg files
            bool m_readForDirectory; //!< valid read for given directory

            //! \brief Read all entries in one directory
            qint32 read(const QString &directory);

        public:

            //! \brief Constructor
            CAircraftCfgEntriesList(const QString &rootDirectory = "") : m_rootDirectory(rootDirectory), m_readForDirectory(false) {}

            //! \brief Read all aircraft.cfg files starting from root directory
            qint32 read()
            {
                if (this->m_readForDirectory) return this->size();

                // not read so far, read it
                this->clear();
                this->m_readForDirectory = true;
                return this->read(this->m_rootDirectory);
            }

            //! \brief Change the directory
            bool changeDirectory(const QString &directory)
            {
                if (this->m_rootDirectory != directory)
                {
                    this->m_rootDirectory = directory;
                    this->m_readForDirectory = false;
                }
                return !directory.isEmpty() && this->existsDir(directory);
            }

            //! \brief Virtual destructor
            virtual ~CAircraftCfgEntriesList() {}

            //! \brief Does the directory exist?
            bool existsDir(const QString &directory = "") const;

            //! \brief Has current directory been read?
            bool hasReadDirectory() const {  return this->m_readForDirectory; }

            //! \brief Current root directory
            QString getRootDirectory() const {  return this->m_rootDirectory; }

            //! \brief Unknown entries
            static const CAircraftCfgEntries &UnknownCfgEntries()
            {
                static CAircraftCfgEntries entries;
                return entries;
            }

            //! \brief Register metadata
            static void registerMetadata();
        };
    }
}

Q_DECLARE_METATYPE(BlackSim::FsCommon::CAircraftCfgEntriesList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackSim::FsCommon::CAircraftCfgEntries>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackSim::FsCommon::CAircraftCfgEntries>)

#endif // guard
