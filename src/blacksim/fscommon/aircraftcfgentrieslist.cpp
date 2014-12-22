/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftcfgentrieslist.h"
#include "blackmisc/predicates.h"
#include "blackmisc/logmessage.h"
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

using namespace BlackMisc;
using namespace BlackMisc::Network;

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
            if (d.isEmpty()) { return false; }
            QDir dir(d);
            //! \todo not available network dir can make this hang here
            return dir.exists();
        }

        /*
         * Model for title
         */
        bool CAircraftCfgEntriesList::containsModelWithTitle(const QString &title, Qt::CaseSensitivity caseSensitivity)
        {
            if (title.isEmpty()) { return false; }
            return this->containsBy(
                       [ = ](const CAircraftCfgEntries & entries) -> bool { return title.compare(entries.getTitle(), caseSensitivity) == 0; }
                   );
        }

        /*
         * Double titles
         */
        QStringList CAircraftCfgEntriesList::detectAmbiguousTitles() const
        {
            QStringList titles = this->getTitles(true);
            QStringList ambiguousTitles;
            QString last;
            for (const QString &title : titles)
            {
                if (title.isEmpty()) { continue; }
                if (title.compare(last, Qt::CaseInsensitive) == 0)
                {
                    if (!ambiguousTitles.contains(title, Qt::CaseInsensitive))
                    {
                        ambiguousTitles.append(title);
                    }
                }
                last = title;
            }
            return ambiguousTitles;
        }

        /*
         * All titles
         */
        QStringList CAircraftCfgEntriesList::getTitles(bool sorted) const
        {
            QStringList titles = this->transform(Predicates::MemberTransform(&CAircraftCfgEntries::getTitle));
            if (sorted) { titles.sort(Qt::CaseInsensitive); }
            return titles;
        }

        /*
         * As model list
         */
        CAircraftModelList CAircraftCfgEntriesList::toAircraftModelList() const
        {
            CAircraftModelList ml;
            for (auto it = this->begin() ; it != this->end(); ++it)
            {
                ml.push_back(it->toAircraftModel());
            }
            return ml;
        }

        /*
         * Models for title
         */
        CAircraftCfgEntriesList CAircraftCfgEntriesList::findByTitle(const QString &title, Qt::CaseSensitivity caseSensitivity) const
        {
            return this->findBy([ = ](const CAircraftCfgEntries & entries) -> bool
            { return title.compare(entries.getTitle(), caseSensitivity) == 0; });
        }

        /*
         * Read all entrities in given directory
         */
        int CAircraftCfgEntriesList::read(const QString &directory)
        {
            if (m_cancelRead) { return -1; }

            // set directory with name filters, get aircraft.cfg and sub directories
            QDir dir(directory, "aircraft.cfg", QDir::Name, QDir::Files | QDir::AllDirs);
            if (!dir.exists()) return 0; // can happen if there are shortcuts or linked dirs not available

            int counter = 0;
            QString currentDir = dir.absolutePath();

            // Dirs last is crucial,since I will break recursion on "aircraft.cfg" level
            QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::AllDirs, QDir::DirsLast);

            for (const QFileInfo &file : files)
            {
                if (m_cancelRead) { return -1; }
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

                    // from the general section
                    const QString atcType = aircraftCfg.value("atc_type").toString();
                    const QString atcModel = aircraftCfg.value("atc_model").toString();

                    int index = 0;
                    while (index >= 0)
                    {
                        if (m_cancelRead) { return -1; }
                        QString group = QString("fltsim.%1").arg(index);
                        aircraftCfg.beginGroup(group);

                        // does group exist?
                        if (aircraftCfg.contains("title"))
                        {
                            QString title = fixedStringContent(aircraftCfg, "title");
                            if (!title.isEmpty())
                            {
                                CAircraftCfgEntries entry(path, index, title, atcType, atcModel, "", "");
                                entry.setAtcParkingCode(fixedStringContent(aircraftCfg, "atc_parking_codes"));
                                entry.setDescription(fixedStringContent(aircraftCfg, "description"));
                                entry.setUiManufacturer(fixedStringContent(aircraftCfg, "ui_manufacturer"));
                                entry.setUiType(fixedStringContent(aircraftCfg, "ui_type"));
                                this->push_back(entry);
                            }
                            else
                            {
                                CLogMessage(this).info("FSX model in %1, index %2 has no title") << path << index;
                            }
                            ++index;
                            ++counter;
                        }
                        else
                        {
                            // marks end of the "fltsim.x" groups
                            index = -1;
                        }
                        aircraftCfg.endGroup();
                    }
                    break;
                }
            }
            return counter;
        }

        QString CAircraftCfgEntriesList::fixedStringContent(const QSettings &settings, const QString &key)
        {
            return fixedStringContent(settings.value(key));
        }

        QString CAircraftCfgEntriesList::fixedStringContent(const QVariant &qv)
        {
            if (qv.isNull() || !qv.isValid())
            {
                return ""; // normal when there is no settings value
            }
            else if (qv.type() == QMetaType::QStringList)
            {
                QStringList l = qv.toStringList();
                return l.join(",").trimmed();
            }
            else if (qv.type() == QMetaType::QString)
            {
                return qv.toString().trimmed();
            }
            Q_ASSERT(false);
            return "";
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
