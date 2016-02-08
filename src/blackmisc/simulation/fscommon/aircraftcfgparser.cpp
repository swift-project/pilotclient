/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftcfgparser.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/simulation/fscommon/fscommonutil.h"
#include "blackmisc/predicates.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/makeunique.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Simulation
    {
        namespace FsCommon
        {
            CAircraftCfgParser::CAircraftCfgParser() { }

            CAircraftCfgParser::CAircraftCfgParser(const CSimulatorInfo &simInfo, const QString &rootDirectory, const QStringList &exludes) :
                IAircraftModelLoader(simInfo),
                m_rootDirectory(rootDirectory),
                m_excludedDirectories(exludes)
            { }

            std::unique_ptr<CAircraftCfgParser> CAircraftCfgParser::createModelLoader(const CSimulatorInfo &simInfo)
            {
                if (simInfo.fsx())
                {
                    return make_unique<CAircraftCfgParser>(
                               CSimulatorInfo(CSimulatorInfo::FSX),
                               CFsCommonUtil::fsxSimObjectsDir(),
                               CFsCommonUtil::fsxSimObjectsExcludeDirectories());
                }
                else if (simInfo.fs9())
                {
                    return make_unique<CAircraftCfgParser>(
                               CSimulatorInfo(CSimulatorInfo::FS9),
                               CFsCommonUtil::fs9AircraftDir(),
                               CFsCommonUtil::fs9AircraftObjectsExcludeDirectories());
                }
                else if (simInfo.p3d())
                {
                    return make_unique<CAircraftCfgParser>(
                               CSimulatorInfo(CSimulatorInfo::P3D),
                               CFsCommonUtil::p3dSimObjectsDir(),
                               CFsCommonUtil::p3dSimObjectsExcludeDirectories());
                }
                Q_ASSERT_X(false, Q_FUNC_INFO, "Illegal simulator info");
                return {};
            }

            CAircraftCfgParser::~CAircraftCfgParser()
            {
                // that should be safe as long as the worker uses deleteLater (which it does)
                if (this->m_parserWorker) { this->m_parserWorker->waitForFinished(); }
            }

            bool CAircraftCfgParser::changeRootDirectory(const QString &directory)
            {
                if (m_rootDirectory == directory) { return false; }
                if (directory.isEmpty() || !existsDir(directory)) { return false; }

                m_rootDirectory = directory;
                return true;
            }

            CPixmap CAircraftCfgParser::iconForModel(const QString &modelString, CStatusMessage &statusMessage) const
            {
                static const CPixmap empty {};
                if (modelString.isEmpty()) { return empty; }
                CAircraftCfgEntriesList cfgEntries = this->getAircraftCfgEntriesList().findByTitle(modelString);
                if (cfgEntries.isEmpty())
                {
                    statusMessage = CStatusMessage(CStatusMessage::SeverityWarning, QString("No .cfg entry for '%1'").arg(modelString));
                    return empty;
                }

                // normally we should have only one entry
                if (cfgEntries.size() > 1)
                {
                    statusMessage = CStatusMessage(CStatusMessage::SeverityWarning, QString("Multiple FSX .cfg entries for '%1'").arg(modelString));
                }

                // use first with icon
                for (const CAircraftCfgEntries &entry : cfgEntries)
                {
                    const QString thumbnail = entry.getThumbnailFileName();
                    if (thumbnail.isEmpty()) { continue; }
                    QPixmap pm;
                    if (pm.load(thumbnail)) { return CPixmap(pm); }
                }
                return empty;
            }

            void CAircraftCfgParser::startLoading(LoadMode mode)
            {
                if (mode == ModeBackground)
                {
                    if (m_parserWorker && !m_parserWorker->isFinished()) { return; }
                    auto rootDirectory = m_rootDirectory;
                    auto excludedDirectories = m_excludedDirectories;
                    m_parserWorker = BlackMisc::CWorker::fromTask(this, "CAircraftCfgParser::changeDirectory",
                                     [this, rootDirectory, excludedDirectories]()
                    {
                        bool ok;
                        auto aircraftCfgEntriesList = this->performParsing(rootDirectory, excludedDirectories, &ok);
                        return std::make_pair(aircraftCfgEntriesList, ok);
                    });
                    m_parserWorker->thenWithResult<std::pair<CAircraftCfgEntriesList, bool>>(this, [this](const std::pair<CAircraftCfgEntriesList, bool> &pair)
                    {
                        if (pair.second) { this->updateCfgEntriesList(pair.first); }
                    });
                }
                else if (mode == ModeBlocking)
                {
                    bool ok;
                    m_parsedCfgEntriesList = performParsing(m_rootDirectory, m_excludedDirectories, &ok);
                    emit loadingFinished(ok);
                }
            }

            bool CAircraftCfgParser::isLoadingFinished() const
            {
                return !m_parserWorker || m_parserWorker->isFinished();
            }

            CAircraftModelList CAircraftCfgParser::getAircraftModels() const
            {
                return getAircraftCfgEntriesList().toAircraftModelList(this->m_simulatorInfo);
            }


            void CAircraftCfgParser::updateCfgEntriesList(const CAircraftCfgEntriesList &cfgEntriesList)
            {
                m_parsedCfgEntriesList = cfgEntriesList;
                emit loadingFinished(true);
            }

            CAircraftCfgEntriesList CAircraftCfgParser::performParsing(const QString &directory, const QStringList &excludeDirectories, bool *ok)
            {
                //
                // function has to be thread safe
                //

                *ok = false;
                if (m_cancelLoading) { return CAircraftCfgEntriesList(); }

                // excluded?
                for (const auto &excludeDir : excludeDirectories)
                {
                    if (m_cancelLoading) { return CAircraftCfgEntriesList(); }
                    if (directory.contains(excludeDir, Qt::CaseInsensitive))
                    {
                        CLogMessage(this).debug() << "Skipping directory " << directory;
                        *ok = true;
                        return CAircraftCfgEntriesList();
                    }
                }

                // set directory with name filters, get aircraft.cfg and sub directories
                QDir dir(directory, "aircraft.cfg", QDir::Name, QDir::Files | QDir::AllDirs);
                if (!dir.exists())
                {
                    *ok = true;
                    return CAircraftCfgEntriesList(); // can happen if there are shortcuts or linked dirs not available
                }

                QString currentDir = dir.absolutePath();
                CAircraftCfgEntriesList result;

                // Dirs last is crucial,since I will break recursion on "aircraft.cfg" level
                QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot, QDir::DirsLast);
                for (const auto &file : files)
                {
                    if (m_cancelLoading) { return CAircraftCfgEntriesList(); }
                    if (file.isDir())
                    {
                        QString nextDir = file.absoluteFilePath();
                        if (currentDir.startsWith(nextDir, Qt::CaseInsensitive)) { continue; } // do not go up
                        if (dir == currentDir) { continue; } // do not recursively call same directory

                        bool dirOk;
                        const CAircraftCfgEntriesList subList(performParsing(nextDir, excludeDirectories, &dirOk));
                        if (dirOk)
                        {
                            result.push_back(subList);
                        }
                        else
                        {
                            CLogMessage(this).warning("Parsing failed for %1") << nextDir;
                        }
                    }
                    else
                    {
                        // due to the filter we expect only "aircraft.cfg" here
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
                                    else if (lineFixed.startsWith("atc_airline", Qt::CaseInsensitive))
                                    {
                                        e.setAtcAirline(getFixedIniLineContent(lineFixed));
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
                                    else if (lineFixed.startsWith("createdBy", Qt::CaseInsensitive))
                                    {
                                        e.setCreatedBy(getFixedIniLineContent(lineFixed));
                                    }
                                    else if (lineFixed.startsWith("sim", Qt::CaseInsensitive))
                                    {
                                        e.setSimName(getFixedIniLineContent(lineFixed));
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
                                CLogMessage(this).info("FS model in %1, index %2 has no title") << fileName << e.getIndex();
                                continue;
                            }
                            CAircraftCfgEntries newEntries(e);
                            newEntries.setAtcModel(atcModel);
                            newEntries.setAtcType(atcType);
                            result.push_back(newEntries);
                        }
                        *ok = true;
                        return result; // do not go any deeper in file tree, we found aircraft.cfg
                    }
                }

                // all files finished,
                // normally reached when no aircraft.cfg is found
                *ok = true;
                return result;
            }

            bool CAircraftCfgParser::existsDir(const QString &directory) const
            {
                if (directory.isEmpty()) { return false; }
                QDir dir(directory);
                //! \todo not available network dir can make this hang here
                return dir.exists();
            }

            QString CAircraftCfgParser::fixedStringContent(const QSettings &settings, const QString &key)
            {
                return fixedStringContent(settings.value(key));
            }

            QString CAircraftCfgParser::fixedStringContent(const QVariant &qv)
            {
                if (qv.isNull() || !qv.isValid())
                {
                    return ""; // normal when there is no settings value
                }
                else if (static_cast<QMetaType::Type>(qv.type()) == QMetaType::QStringList)
                {
                    QStringList l = qv.toStringList();
                    return l.join(",").trimmed();
                }
                else if (static_cast<QMetaType::Type>(qv.type()) == QMetaType::QString)
                {
                    return qv.toString().trimmed();
                }
                Q_ASSERT(false);
                return "";
            }

            QString CAircraftCfgParser::getFixedIniLineContent(const QString &line)
            {
                if (line.isEmpty()) { return ""; }
                int index = line.indexOf('=');
                if (index < 0) { return ""; }
                if (line.length() < index  + 1) { return ""; }

                QString content(line.mid(index + 1).trimmed());

                // fix "" strings, some are malformed and just contain " at beginning, not at the end
                if (content.endsWith('"')) { content.remove(content.size() - 1 , 1); }
                if (content.startsWith('"')) { content.remove(0 , 1); }

                // fix C style linebreaks
                content.replace("\\n", " ");
                content.replace("\\t", " ");
                return content;
            }

        } // namespace
    } // namespace
} // namespace
