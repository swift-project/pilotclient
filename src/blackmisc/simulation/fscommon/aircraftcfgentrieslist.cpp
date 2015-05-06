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
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Simulation
    {

        namespace FsCommon
        {
            int CAircraftCfgEntriesList::read()
            {
                if (this->m_readForDirectory) { return this->size(); }

                // not read so far, read it
                this->clear();
                this->m_readForDirectory = true;
                return this->read(this->m_rootDirectory, excludeDirectories());
            }

            bool CAircraftCfgEntriesList::changeDirectory(const QString &directory)
            {
                if (this->m_rootDirectory != directory)
                {
                    this->m_rootDirectory = directory;
                    this->m_readForDirectory = false;
                }
                return (!directory.isEmpty() && this->existsDir(directory));
            }

            bool CAircraftCfgEntriesList::existsDir(const QString &directory) const
            {
                QString d = directory.isEmpty() ? this->m_rootDirectory : directory;
                if (d.isEmpty()) { return false; }
                QDir dir(d);
                //! \todo unavailable network dir can make this hang here
                return dir.exists();
            }

            bool CAircraftCfgEntriesList::containsModelWithTitle(const QString &title, Qt::CaseSensitivity caseSensitivity)
            {
                if (title.isEmpty()) { return false; }
                return this->containsBy(
                           [ = ](const CAircraftCfgEntries & entries) -> bool { return title.compare(entries.getTitle(), caseSensitivity) == 0; }
                       );
            }

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

            QStringList CAircraftCfgEntriesList::getTitles(bool sorted) const
            {
                QStringList titles = this->transform(Predicates::MemberTransform(&CAircraftCfgEntries::getTitle));
                if (sorted) { titles.sort(Qt::CaseInsensitive); }
                return titles;
            }

            CAircraftModelList CAircraftCfgEntriesList::toAircraftModelList() const
            {
                CAircraftModelList ml;
                for (auto it = this->begin() ; it != this->end(); ++it)
                {
                    ml.push_back(it->toAircraftModel());
                }
                return ml;
            }

            CAircraftCfgEntriesList CAircraftCfgEntriesList::findByTitle(const QString &title, Qt::CaseSensitivity caseSensitivity) const
            {
                return this->findBy([ = ](const CAircraftCfgEntries & entries) -> bool
                { return title.compare(entries.getTitle(), caseSensitivity) == 0; });
            }

            const QStringList &CAircraftCfgEntriesList::excludeDirectories()
            {
                static const QStringList exclude
                {
                    // "SimObjects/Misc",
                    "SimObjects/Animals",
                    "SimObjects/GroundVehicles",
                    "SimObjects/Boats"
                };
                return exclude;
            }

            int CAircraftCfgEntriesList::read(const QString &directory, const QStringList &excludeDirectories)
            {
                if (m_cancelRead) { return -1; }

                // excluded?
                for (const QString &excludeDir : excludeDirectories)
                {
                    if (directory.contains(excludeDir, Qt::CaseInsensitive))
                    {
                        CLogMessage(this).debug() << "Skipping directory " << directory;
                        return 0;
                    }
                }

                // set directory with name filters, get aircraft.cfg and sub directories
                QDir dir(directory, "aircraft.cfg", QDir::Name, QDir::Files | QDir::AllDirs);


                if (!dir.exists()) return 0; // can happen if there are shortcuts or linked dirs not available

                int counter = 0;
                QString currentDir = dir.absolutePath();

                // Dirs last is crucial, since I will break recursion on ".cfg" level
                // once I have found .cfg, I do not go any deeper
                QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::AllDirs, QDir::DirsLast);
                for (const QFileInfo &file : files)
                {
                    if (m_cancelRead) { return -1; }
                    if (file.isDir())
                    {
                        QString nextDir = file.absoluteFilePath();
                        if (currentDir.startsWith(nextDir, Qt::CaseInsensitive)) { continue; } // do not go up
                        if (dir == currentDir) { continue; } // do not recursively call same directory
                        counter += CAircraftCfgEntriesList::read(nextDir, excludeDirectories);
                    }
                    else
                    {
                        // due to the filter we expect only ".cfg" here
                        // remark: in a 1st version I have used QSettings to parse to file as ini file
                        // unfortunately some files are malformed which could end up in wrong data
                        QString fileName = file.absoluteFilePath();
                        QFile file(fileName);
                        if (!file.open(QFile::ReadOnly | QFile::Text))
                        {
                            CLogMessage(this).warning("Unable to read file %1") << fileName;
                            continue;
                        }
                        QTextStream in(&file);
                        QList<CAircraftCfgEntries> tempEntries;

                        // parse through the file
                        QString atcType;
                        QString atcModel;
                        QString fltSection("[FLTSIM.0]");
                        int fltsimCounter = 0;
                        FileSection currentSection = Unknown;
                        bool isRotorcraftPath = fileName.toLower().contains("rotorcraft");

                        while (!in.atEnd())
                        {
                            const QString lineFixed(in.readLine().trimmed());
                            if (lineFixed.isEmpty()) { continue; }
                            if (lineFixed.startsWith("["))
                            {
                                if (lineFixed.startsWith("[GENERAL]", Qt::CaseInsensitive)) { currentSection = General; continue; }
                                if (lineFixed.startsWith(fltSection, Qt::CaseInsensitive))
                                {
                                    CAircraftCfgEntries e(fileName, fltsimCounter);
                                    if (isRotorcraftPath)
                                    {
                                        e.setRotorcraft(true);
                                    }
                                    tempEntries.append(e);
                                    currentSection = Fltsim;
                                    fltSection = QString("[FLTSIM.%1]").arg(++fltsimCounter);
                                    continue;
                                }
                                currentSection = Unknown;
                                continue;
                            }
                            switch (currentSection)
                            {
                            case General:
                                {
                                    if (lineFixed.startsWith("//")) { break; }
                                    if (atcType.isEmpty() || atcModel.isEmpty())
                                    {
                                        QString c = getFixedIniLineContent(lineFixed);
                                        if (lineFixed.startsWith("atc_type", Qt::CaseInsensitive)) { atcType = c; }
                                        else if (lineFixed.startsWith("atc_model", Qt::CaseInsensitive)) { atcModel = c; }
                                    }
                                }
                                break;
                            case Fltsim:
                                {
                                    if (lineFixed.startsWith("//")) { break; }
                                    CAircraftCfgEntries &e = tempEntries[tempEntries.size() - 1];
                                    if (lineFixed.startsWith("atc_parking_codes", Qt::CaseInsensitive))
                                    {
                                        e.setAtcParkingCode(getFixedIniLineContent(lineFixed));
                                    }
                                    else if (lineFixed.startsWith("description", Qt::CaseInsensitive))
                                    {
                                        e.setDescription(getFixedIniLineContent(lineFixed));
                                    }
                                    else if (lineFixed.startsWith("ui_manufacturer", Qt::CaseInsensitive))
                                    {
                                        e.setUiManufacturer(getFixedIniLineContent(lineFixed));
                                    }
                                    else if (lineFixed.startsWith("ui_typerole", Qt::CaseInsensitive))
                                    {
                                        bool r = getFixedIniLineContent(lineFixed).toLower().contains("rotor");
                                        e.setRotorcraft(r);
                                    }
                                    else if (lineFixed.startsWith("ui_type", Qt::CaseInsensitive))
                                    {
                                        e.setUiType(getFixedIniLineContent(lineFixed));
                                    }
                                    else if (lineFixed.startsWith("texture", Qt::CaseInsensitive))
                                    {
                                        e.setTexture(getFixedIniLineContent(lineFixed));
                                    }
                                    else if (lineFixed.startsWith("title", Qt::CaseInsensitive))
                                    {
                                        e.setTitle(getFixedIniLineContent(lineFixed));
                                    }
                                }
                                break;
                            default:
                            case Unknown: break;
                            }

                        } // all lines
                        file.close();

                        // store all entries
                        for (const CAircraftCfgEntries &e : tempEntries)
                        {
                            if (e.getTitle().isEmpty())
                            {
                                CLogMessage(this).info("FSX model in %1, index %2 has no title") << fileName << e.getIndex();
                                continue;
                            }
                            CAircraftCfgEntries newEntries(e);
                            newEntries.setAtcModel(atcModel);
                            newEntries.setAtcType(atcType);
                            this->push_back(newEntries);
                            counter++;
                        }
                        return counter; // do not go any deeper in file tree, we found aircraft.cfg

                    } // file, no directory
                } // files
                return counter;
            }

            QString CAircraftCfgEntriesList::getFixedIniLineContent(const QString &line)
            {
                if (line.isEmpty()) { return ""; }
                int index = line.indexOf('=');
                if (index < 0) { return ""; }
                if (line.length() < index  + 1) { return ""; }

                QString content(line.mid(index + 1).trimmed());

                // fix "" strings, some are malformed and just contain " at beginning, end
                if (content.endsWith('"')) { content.remove(content.size() - 1 , 1); }
                if (content.startsWith('"')) { content.remove(0 , 1); }

                // fix C style linebreaks
                content.replace("\\n", " ");
                content.replace("\\t", " ");

                // return
                return content;
            }

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
} // namespace
