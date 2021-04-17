/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/simulation/fscommon/aircraftcfgentries.h"
#include "blackmisc/simulation/fscommon/aircraftcfgparser.h"
#include "blackmisc/simulation/fscommon/fsdirectories.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/worker.h"
#include "blackmisc/stringutils.h"
#include "blackconfig/buildconfig.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QFlags>
#include <QIODevice>
#include <QList>
#include <QMetaType>
#include <QSettings>
#include <QTextStream>
#include <Qt>
#include <QtGlobal>
#include <atomic>
#include <tuple>
#include <QStringView>

using namespace BlackConfig;
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
            // response for async. loading
            using LoaderResponse = std::tuple<CAircraftCfgEntriesList, CAircraftModelList, CStatusMessageList>;

            CAircraftCfgParser::CAircraftCfgParser(const CSimulatorInfo &simInfo, QObject *parent) : IAircraftModelLoader(simInfo, parent)
            { }

            CAircraftCfgParser *CAircraftCfgParser::createModelLoader(const CSimulatorInfo &simInfo, QObject *parent)
            {
                return new CAircraftCfgParser(simInfo, parent);
            }

            CAircraftCfgParser::~CAircraftCfgParser()
            {
                // that should be safe as long as the worker uses deleteLater (which it does)
                if (m_parserWorker) { m_parserWorker->waitForFinished(); }
            }

            void CAircraftCfgParser::startLoadingFromDisk(LoadMode mode, const ModelConsolidationCallback &modelConsolidation, const QStringList &modelDirectories)
            {
                static const CStatusMessage statusLoadingOk(this, CStatusMessage::SeverityInfo, u"Aircraft config parser loaded data");
                static const CStatusMessage statusLoadingError(this, CStatusMessage::SeverityError, u"Aircraft config parser did NOT load data");

                const CSimulatorInfo simulator = this->getSimulator();
                const QStringList modelDirs = this->getInitializedModelDirectories(modelDirectories, simulator);
                const QStringList excludedDirectoryPatterns(m_settings.getModelExcludeDirectoryPatternsOrDefault(simulator)); // copy

                if (mode.testFlag(LoadInBackground))
                {
                    if (m_parserWorker && !m_parserWorker->isFinished()) { return; }
                    emit this->diskLoadingStarted(simulator, mode);
                    m_parserWorker = CWorker::fromTask(this, "CAircraftCfgParser::startLoadingFromDisk",
                                                       [this, modelDirs, excludedDirectoryPatterns, simulator, modelConsolidation]()
                    {
                        CStatusMessageList msgs;
                        const CAircraftCfgEntriesList aircraftCfgEntriesList = this->performParsing(modelDirs, excludedDirectoryPatterns, msgs);
                        CAircraftModelList models;
                        if (msgs.isSuccess())
                        {
                            models = aircraftCfgEntriesList.toAircraftModelList(simulator, true, msgs);
                            if (modelConsolidation) { modelConsolidation(models, true); }
                        }
                        return std::make_tuple(aircraftCfgEntriesList, models, msgs);
                    });
                    m_parserWorker->thenWithResult<LoaderResponse>(this, [this, simulator](const LoaderResponse & tuple)
                    {
                        m_loadingMessages = std::get<2>(tuple);
                        if (m_loadingMessages.isSuccess())
                        {
                            m_parsedCfgEntriesList = std::get<0>(tuple);
                            const CAircraftModelList models(std::get<1>(tuple));
                            const bool hasData = !models.isEmpty();
                            if (hasData)
                            {
                                this->setModelsForSimulator(models, this->getSimulator());
                            }
                            // currently I treat no data as error
                            m_loadingMessages.push_front(hasData ? statusLoadingOk : statusLoadingError);
                        }
                        m_loadingMessages.freezeOrder();
                        emit this->loadingFinished(m_loadingMessages, simulator, ParsedData);
                    });
                }
                else if (mode == LoadDirectly)
                {
                    emit this->diskLoadingStarted(simulator, mode);

                    CStatusMessageList msgs;
                    m_parsedCfgEntriesList = this->performParsing(modelDirs, excludedDirectoryPatterns, msgs);
                    const CAircraftModelList models(m_parsedCfgEntriesList.toAircraftModelList(simulator, true, msgs));
                    m_loadingMessages = msgs;
                    m_loadingMessages.freezeOrder();
                    const bool hasData = !models.isEmpty();
                    if (hasData)
                    {
                        this->setCachedModels(models, this->getSimulator());
                    }
                    // currently I treat no data as error
                    emit this->loadingFinished(hasData ? statusLoadingOk : statusLoadingError, simulator, ParsedData);
                }
            }

            bool CAircraftCfgParser::isLoadingFinished() const
            {
                return !m_parserWorker || m_parserWorker->isFinished();
            }

            CAircraftCfgEntriesList CAircraftCfgParser::performParsing(const QStringList &directories, const QStringList &excludeDirectories, CStatusMessageList &messages)
            {
                CAircraftCfgEntriesList entries;
                for (const QString &dir : directories)
                {
                    entries.push_back(this->performParsing(dir, excludeDirectories, messages));
                }
                return entries;
            }

            CAircraftCfgEntriesList CAircraftCfgParser::performParsing(const QString &directory, const QStringList &excludeDirectories, CStatusMessageList &messages)
            {
                //
                // function has to be threadsafe
                //

                if (m_cancelLoading) { return CAircraftCfgEntriesList(); }

                // excluded?
                if (CFileUtils::isExcludedDirectory(directory, excludeDirectories) || isExcludedSubDirectory(directory))
                {
                    const CStatusMessage m = CStatusMessage(this).info(u"Skipping directory '%1' (excluded)") << directory;
                    messages.push_back(m);
                    return CAircraftCfgEntriesList();
                }

                // set directory with name filters, get aircraft.cfg and sub directories
                static const QString NoNameFilter;
                QDir dir(directory, NoNameFilter, QDir::Name, QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot);
                dir.setNameFilters(fileNameFilters());
                if (!dir.exists())
                {
                    return CAircraftCfgEntriesList(); // can happen if there are shortcuts or linked dirs not available
                }

                const QString currentDir = dir.absolutePath();
                CAircraftCfgEntriesList result;
                emit this->loadingProgress(this->getSimulator(), QStringLiteral("Parsing '%1'").arg(currentDir), -1);

                // Dirs last is crucial, since I will break recursion on "aircraft.cfg" level
                // with T514 this behaviour has been changed
                const QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot, QDir::DirsLast);

                // the sim.cfg/aircraft.cfg file should have an *.air file sibling
                // if not we assume these files can be ignored
                const QDir dirForAir(directory, CFsDirectories::airFileFilter(), QDir::Name, QDir::Files | QDir::NoDotAndDotDot);
                const int airFilesCount = dirForAir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::DirsLast).size();
                const bool hasAirFiles =  airFilesCount > 0;

                if (CBuildConfig::buildWordSize() != 32 && !hasAirFiles)
                {
                    const CStatusMessage m = CStatusMessage(this).warning(u"No \"air\" files in '%1'") << currentDir;
                    messages.push_back(m);
                }

                for (const auto &fileInfo : files)
                {
                    if (m_cancelLoading) { return CAircraftCfgEntriesList(); }
                    if (fileInfo.isDir())
                    {
                        const QString nextDir = fileInfo.absoluteFilePath();
                        if (currentDir.startsWith(nextDir, Qt::CaseInsensitive)) { continue; } // do not go up
                        if (dir == currentDir) { continue; } // do not recursively call same directory

                        const CAircraftCfgEntriesList subList(performParsing(nextDir, excludeDirectories, messages));
                        if (messages.isSuccess())
                        {
                            result.push_back(subList);
                        }
                        else
                        {
                            const CStatusMessage m = CStatusMessage(this).warning(u"Parsing failed for '%1'") << nextDir;
                            messages.push_back(m);
                        }
                    }
                    else
                    {
                        // Enforce air files only for 64 bit P3D
                        if (CBuildConfig::buildWordSize() != 32 && !hasAirFiles) { continue; }

                        // due to the filter we expect only "aircraft.cfg"/"sim.cfg" here
                        // remark: in a 1st version I have used QSettings to parse to file as ini file
                        // unfortunately some files are malformed which could end up in wrong data

                        const QString fileName = fileInfo.absoluteFilePath(); // full path and name
                        bool fileOk = false;
                        CStatusMessageList fileMsgs;
                        const CAircraftCfgEntriesList fileResults = CAircraftCfgParser::performParsingOfSingleFile(fileName, fileOk, fileMsgs);
                        if (!fileOk)
                        {
                            const CStatusMessage m = CStatusMessage(this).warning(u"Parsing of '%1' failed") << fileName;
                            messages.push_back(fileMsgs);
                            continue;
                        }

                        result.push_back(fileResults);

                        // With T514 we do not skip not anymore
                        // return result; // do not go any deeper in file tree, we found aircraft.cfg
                    }
                }

                // all files finished,
                // normally reached when no aircraft.cfg is found
                return result;
            }

            CAircraftCfgEntriesList CAircraftCfgParser::performParsingOfSingleFile(const QString &fileName, bool &ok, CStatusMessageList &msgs)
            {
                // due to the filter we expect only "aircraft.cfg" files here
                // remark: in a 1st version I have used QSettings to parse to file as ini file
                // unfortunately some files are malformed which could end up in wrong data

                ok = false;
                const QString fnFixed = CFileUtils::fixWindowsUncPath(fileName);
                QFile file(fnFixed); // includes path
                if (!file.open(QFile::ReadOnly | QFile::Text))
                {
                    const CStatusMessage m = CStatusMessage(static_cast<CAircraftCfgParser *>(nullptr)).warning(u"Unable to read file '%1'") << fnFixed;
                    msgs.push_back(m);
                    return CAircraftCfgEntriesList();
                }

                QTextStream in(&file);
                QList<CAircraftCfgEntries> tempEntries;

                // parse through the file
                QString atcType;
                QString atcModel;
                QString fltSection("[FLTSIM.0]");
                static const QString fltSectionStr("[FLTSIM.%1]");

                int fltsimCounter = 0;
                FileSection currentSection = Unknown;
                const bool isRotorcraftPath = fileName.contains("rotorcraft", Qt::CaseInsensitive);

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
                            fltSection = fltSectionStr.arg(++fltsimCounter);
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
                                const QString c = getFixedIniLineContent(lineFixed);
                                if (lineFixed.startsWith("atc_type", Qt::CaseInsensitive))
                                {
                                    atcType = c;
                                }
                                /*else if (lineFixed.startsWith("atc_model", Qt::CaseInsensitive))
                                {
                                    atcModel = c;
                                }*/
                                else if (lineFixed.startsWith("icao_type_designator", Qt::CaseInsensitive))
                                {
                                    atcModel = c;
                                }
                            }
                        }
                        break;
                    case Fltsim:
                        {
                            if (lineFixed.startsWith("//")) { break; }
                            CAircraftCfgEntries &e = tempEntries[tempEntries.size() - 1];
                            if (lineFixed.startsWith("atc_", Qt::CaseInsensitive))
                            {
                                if (lineFixed.startsWith("atc_parking_codes", Qt::CaseInsensitive))
                                {
                                    e.setAtcParkingCode(getFixedIniLineContent(lineFixed));
                                }
                                else if (lineFixed.startsWith("atc_airline", Qt::CaseInsensitive))
                                {
                                    e.setAtcAirline(getFixedIniLineContent(lineFixed));
                                }
                                else if (lineFixed.startsWith("atc_id_color", Qt::CaseInsensitive))
                                {
                                    e.setAtcIdColor(getFixedIniLineContent(lineFixed));
                                }
                            }
                            else if (lineFixed.startsWith("ui_", Qt::CaseInsensitive))
                            {
                                if (lineFixed.startsWith("ui_manufacturer", Qt::CaseInsensitive))
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
                                else if (lineFixed.startsWith("ui_variation", Qt::CaseInsensitive))
                                {
                                    e.setUiVariation(getFixedIniLineContent(lineFixed));
                                }
                            }
                            else if (lineFixed.startsWith("description", Qt::CaseInsensitive))
                            {
                                e.setDescription(getFixedIniLineContent(lineFixed));
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
                const QFileInfo fileInfo(fnFixed);
                QDateTime fileTimestamp(fileInfo.lastModified());
                if (!fileTimestamp.isValid() || fileInfo.birthTime() > fileTimestamp)
                {
                    fileTimestamp = fileInfo.birthTime();
                }
                Q_ASSERT_X(fileTimestamp.isValid(), Q_FUNC_INFO, "Missing file timestamp");

                CAircraftCfgEntriesList result;
                for (const CAircraftCfgEntries &e : std::as_const(tempEntries))
                {
                    if (e.getTitle().isEmpty())
                    {
                        const CStatusMessage m = CStatusMessage(static_cast<CAircraftCfgParser *>(nullptr)).info(u"FS model in %1, index %2 has no title") << fileName << e.getIndex();
                        msgs.push_back(m);
                        continue;
                    }
                    CAircraftCfgEntries newEntries(e);
                    newEntries.setAtcModel(atcModel);
                    newEntries.setAtcType(atcType);
                    newEntries.setUtcTimestamp(fileTimestamp);
                    result.push_back(newEntries);
                }
                ok = true;
                return result; // do not go any deeper in file tree, we found aircraft.cfg
            }

            QString CAircraftCfgParser::fixedStringContent(const QSettings &settings, const QString &key)
            {
                return fixedStringContent(settings.value(key));
            }

            QString CAircraftCfgParser::fixedStringContent(const QVariant &qv)
            {
                if (qv.isNull() || !qv.isValid())
                {
                    return {}; // normal when there is no settings value
                }
                else if (static_cast<QMetaType::Type>(qv.type()) == QMetaType::QStringList)
                {
                    const QStringList l = qv.toStringList();
                    return l.join(",").trimmed();
                }
                else if (static_cast<QMetaType::Type>(qv.type()) == QMetaType::QString)
                {
                    return qv.toString().trimmed();
                }
                Q_ASSERT(false);
                return {};
            }

            QString CAircraftCfgParser::getFixedIniLineContent(const QString &line)
            {
                if (line.isEmpty()) { return {}; }

                // Remove inline comments starting with ;
                const int indexComment = line.indexOf(';');
                QString content = line.leftRef(indexComment - 1).trimmed().toString();

                const int index = line.indexOf('=');
                if (index < 0) { return {}; }
                if (line.length() < index + 1) { return {}; }

                content = content.midRef(index + 1).trimmed().toString();

                // fix "" strings, some are malformed and just contain " at beginning, not at the end
                if (hasBalancedQuotes(content, '"'))
                {
                    // seems to be OK
                    // ex: title=B767-300ER - Condor "Retro Jet"

                    if (content.size() > 2 && content.startsWith('"') && content.endsWith('"'))
                    {
                        // completly in quotes, example title="B767-300ER - Condor Retro Jet"
                        // we assume the quotes shall be removed
                        content.remove(0, 1);
                        content.chop(1);
                    }
                }
                else
                {
                    // UNBALANCED

                    // could be OK, example title=B767-300ER - Condor Retro Jet"
                    // if (content.endsWith('"'))   { content.remove(content.size() - 1, 1); }

                    // Unlikely, title="B767-300ER - Condor "Retro Jet
                    if (content.startsWith('"')) { content.remove(0, 1); }
                }

                // fix C style linebreaks
                content.replace("\\n", " ");
                content.replace("\\t", " ");
                return content;
            }

            const QStringList &CAircraftCfgParser::fileNameFilters()
            {
                if (CBuildConfig::buildWordSize() == 32)
                {
                    static const QStringList f({ "aircraft.cfg" });
                    return f;
                }
                else
                {
                    static const QStringList f({ "aircraft.cfg", "sim.cfg" });
                    return f;
                }
            }

            bool CAircraftCfgParser::isExcludedSubDirectory(const QString &checkDirectory)
            {
                if (checkDirectory.isEmpty()) { return false; }
                const QString dir = CFileUtils::lastPathSegment(checkDirectory).toLower();
                if (dir == u"texture" || dir.startsWith("texture.")) { return true; }
                if (dir == u"sound"   || dir == "soundai") { return true; }
                if (dir == u"panel")  { return true; }
                if (dir == u"model")  { return true; }
                return false;
            }
        } // ns
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Simulation::FsCommon::LoaderResponse)
