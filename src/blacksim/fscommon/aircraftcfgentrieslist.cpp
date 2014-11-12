/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftcfgentrieslist.h"

namespace BlackSim
{
    namespace FsCommon
    {

        /*
         * Does the directory exist?
         */
        bool CAircraftCfgEntriesList::existsDir(const QString &directory) const
        {
            QString d = directory.isEmpty() ? this->m_rootDirectory : directory;
            if (d.isEmpty()) return false;
            QDir dir(d);
            return (dir.exists());
        }

        /*
         * Model for title
         */
        bool CAircraftCfgEntriesList::containsModeWithTitle(const QString &title, Qt::CaseSensitivity caseSensitivity)
        {
            return this->containsBy([ = ](const CAircraftCfgEntries & entries) -> bool
            { return title.compare(entries.getTitle(), caseSensitivity) == 0; });
        }

        /*
         * Read all entrities in given directory
         */
        qint32 CAircraftCfgEntriesList::read(const QString &directory)
        {
            // set directory with name filters, get aircraft.cfg and sub directories
            QDir dir(directory, "aircraft.cfg", QDir::Name, QDir::Files | QDir::AllDirs);
            if (!dir.exists()) return 0; // can happen if there are shortcuts or linked dirs not available

            qint32 counter = 0;
            QString currentDir = dir.absolutePath();

            // Dirs last is crucial,since I will break recursion on "aircraft.cfg" level
            QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::AllDirs, QDir::DirsLast);

            foreach(QFileInfo file, files)
            {
                if (file.isDir())
                {
                    QString nextDir = file.absoluteFilePath();
                    if (currentDir.startsWith(nextDir, Qt::CaseInsensitive)) continue; // do not go up
                    if (dir == currentDir) continue; // do not recursively call same directory
                    counter += CAircraftCfgEntriesList::read(nextDir);
                }
                else
                {
                    // due to the filter we expect only "aircraft.cfg" here
                    QString path = file.absoluteFilePath();

                    // I abuse the QSettings as ini-file reader
                    QSettings aircraftCfg(path, QSettings::IniFormat);
                    const QString atcType = aircraftCfg.value("atc_type").toString();
                    const QString atcModel = aircraftCfg.value("atc_model").toString();

                    int index = 0;
                    while (index >= 0)
                    {
                        QString group = QString("fltsim.%1").arg(index);
                        aircraftCfg.beginGroup(group);
                        if (aircraftCfg.contains("title"))
                        {
                            CAircraftCfgEntries entry(path, index, "", atcType, atcModel, "");
                            entry.setTitle(aircraftCfg.value("title").toString());
                            entry.setAtcParkingCode(aircraftCfg.value("atc_parking_codes").toString());
                            this->push_back(entry);
                            aircraftCfg.endGroup();
                            ++index;
                            ++counter;
                        }
                        else
                        {
                            index = -1;
                        }
                    }
                    break;
                }
            }
            return counter;
        }

        /*
         * Register metadata
         */
        void CAircraftCfgEntriesList::registerMetadata()
        {
            qRegisterMetaType<BlackMisc::CSequence<CAircraftCfgEntries>>();
            qDBusRegisterMetaType<BlackMisc::CSequence<CAircraftCfgEntries>>();
            qRegisterMetaType<BlackMisc::CCollection<CAircraftCfgEntries>>();
            qDBusRegisterMetaType<BlackMisc::CCollection<CAircraftCfgEntries>>();
            qRegisterMetaType<CAircraftCfgEntriesList>();
            qDBusRegisterMetaType<CAircraftCfgEntriesList>();
            BlackMisc::registerMetaValueType<CAircraftCfgEntriesList>();
        }

    } // namespace
} // namespace
