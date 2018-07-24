/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/aircraftmodelutils.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/simulation/xplane/aircraftmodelloaderxplane.h"
#include "blackmisc/simulation/xplane/xplaneutil.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/worker.h"

#include <string.h>
#include <QChar>
#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QFlags>
#include <QIODevice>
#include <QList>
#include <QMap>
#include <QRegularExpression>
#include <QTextStream>
#include <QStringBuilder>
#include <algorithm>
#include <functional>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Simulation
    {
        namespace XPlane
        {
            //! Normalizes CSL model "designators" e.g. __XPFW_Jets:A320_a:A320_a_Austrian_Airlines.obj
            static void normalizePath(QString &path)
            {
                for (auto &e : path)
                {
                    if (e == '/' || e == ':' || e == '\\')
                    {
                        e = '/';
                    }
                }
            }

            //! Create a unique string to identify a model
            static QString stringForFlyableModel(const CAircraftModel &model, const QFileInfo &acfFile)
            {
                if (model.getDistributor().hasDescription())
                {
                    if (!model.getName().isEmpty())
                    {
                        if (model.getName().contains(model.getDistributor().getDescription()))
                        {
                            return model.getName();
                        }
                        else
                        {
                            return model.getDistributor().getDescription() % ' ' % model.getName();
                        }
                    }
                    else if (model.hasAircraftDesignator())
                    {
                        return model.getDistributor().getDescription() % ' ' % model.getAircraftIcaoCodeDesignator();
                    }
                }
                return acfFile.dir().dirName() % ' ' % acfFile.baseName();
            }

            //! Create a description string for a model that doesn't already have one
            static QString descriptionForFlyableModel(const CAircraftModel &model)
            {
                if (!model.getName().isEmpty())
                {
                    if (model.getDistributor().hasDescription() && !model.getName().contains(model.getDistributor().getDescription()))
                    {
                        return QStringLiteral("[ACF] ") % model.getName() % QStringLiteral(" by ") % model.getDistributor().getDescription();
                    }
                    else
                    {
                        return QStringLiteral("[ACF] ") % model.getName();
                    }
                }
                else if (model.hasAircraftDesignator())
                {
                    if (model.getDistributor().hasDescription())
                    {
                        return QStringLiteral("[ACF] ") % model.getAircraftIcaoCodeDesignator() % QStringLiteral(" by ") % model.getDistributor().getDescription();
                    }
                    else
                    {
                        return QStringLiteral("[ACF] ") % model.getAircraftIcaoCodeDesignator();
                    }
                }
                return QStringLiteral("[ACF]");
            }

            CAircraftModelLoaderXPlane::CAircraftModelLoaderXPlane(QObject *parent) : IAircraftModelLoader(CSimulatorInfo::xplane(), parent)
            { }

            CAircraftModelLoaderXPlane::~CAircraftModelLoaderXPlane()
            {
                // that should be safe as long as the worker uses deleteLater (which it does)
                if (m_parserWorker) { m_parserWorker->waitForFinished(); }
            }

            void CAircraftModelLoaderXPlane::startLoadingFromDisk(LoadMode mode, const ModelConsolidationCallback &modelConsolidation, const QStringList &modelDirectories)
            {
                const CSimulatorInfo simulator = CSimulatorInfo::xplane();
                const QStringList modelDirs = this->getInitializedModelDirectories(modelDirectories, simulator);
                const QStringList excludedDirectoryPatterns(m_settings.getModelExcludeDirectoryPatternsOrDefault(simulator)); // copy

                if (modelDirs.isEmpty())
                {
                    this->clearCachedModels(CSimulatorInfo::xplane());
                    emit this->loadingFinished(CStatusMessage(this, CStatusMessage::SeverityError, "XPlane model directories '%1' are empty") << modelDirectories.join(", "), simulator, ParsedData);
                    return;
                }

                if (mode.testFlag(LoadInBackground))
                {
                    if (m_parserWorker && !m_parserWorker->isFinished()) { return; }
                    m_parserWorker = BlackMisc::CWorker::fromTask(this, "CAircraftModelLoaderXPlane::performParsing",
                                     [this, modelDirs, excludedDirectoryPatterns, modelConsolidation]()
                    {
                        //! \todo KB/MS 2017-09 not high prio, but still needed: according to meet XP needs to support multiple directories
                        //! \todo KB with T118 now model directories are passed (changed signatures) but the code below needs to support multiple dirs
                        const QString modelDirectory = modelDirs.front();
                        auto models = performParsing(modelDirectory, excludedDirectoryPatterns);
                        if (modelConsolidation) { modelConsolidation(models, true); }
                        return models;
                    });
                    m_parserWorker->thenWithResult<CAircraftModelList>(this, [ = ](const auto & models)
                    {
                        this->updateInstalledModels(models);
                        emit this->loadingFinished(m_loadingMessages, simulator, ParsedData);
                    });
                }
                else if (mode.testFlag(LoadDirectly))
                {
                    CAircraftModelList models(performParsing(this->getFirstModelDirectoryOrDefault(), excludedDirectoryPatterns));
                    updateInstalledModels(models);
                }
            }

            bool CAircraftModelLoaderXPlane::isLoadingFinished() const
            {
                return !m_parserWorker || m_parserWorker->isFinished();
            }

            void CAircraftModelLoaderXPlane::updateInstalledModels(const CAircraftModelList &models)
            {
                this->setModelsForSimulator(models, CSimulatorInfo::xplane());
                emit this->loadingFinished(CStatusMessage(this, CStatusMessage::SeverityInfo, "XPlane updated '%1' models") << models.size(), CSimulatorInfo::xplane(), ParsedData);
            }

            QString CAircraftModelLoaderXPlane::CSLPlane::getModelName() const
            {
                QString modelName =
                    dirNames.join(' ') %
                    QLatin1Char(' ') % objectName %
                    QLatin1Char(' ') % textureName;
                return std::move(modelName).trimmed();
            }

            CAircraftModelList CAircraftModelLoaderXPlane::performParsing(const QString &rootDirectory, const QStringList &excludeDirectories)
            {
                CAircraftModelList allModels;
                allModels.push_back(parseCslPackages(CXPlaneUtil::xswiftbusLegacyDir(rootDirectory), excludeDirectories));
                allModels.push_back(parseCslPackages(rootDirectory, excludeDirectories));
                allModels.push_back(parseFlyableAirplanes(rootDirectory, excludeDirectories));
                return allModels;
            }

            //! Add model only if there no other model with the same model string
            void addUniqueModel(const CAircraftModel &model, CAircraftModelList &models)
            {
                if (models.containsModelString(model.getModelString()))
                {
                    CLogMessage(static_cast<CAircraftModelLoaderXPlane *>(nullptr)).warning("XPlane model '%1' exists already! Potential model string conflict! Ignoring it.") << model.getModelString();
                }
                models.push_back(model);
            }

            CAircraftModelList CAircraftModelLoaderXPlane::parseFlyableAirplanes(const QString &rootDirectory, const QStringList &excludeDirectories)
            {
                Q_UNUSED(excludeDirectories);
                if (rootDirectory.isEmpty()) { return {}; }

                QDir searchPath(rootDirectory, fileFilterFlyable());
                QDirIterator aircraftIt(searchPath, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

                CAircraftModelList installedModels;
                while (aircraftIt.hasNext())
                {
                    aircraftIt.next();
                    if (CFileUtils::isExcludedDirectory(aircraftIt.fileInfo(), excludeDirectories, Qt::CaseInsensitive)) { continue; }

                    CAircraftModel model = extractAcfProperties(aircraftIt.filePath(), aircraftIt.fileInfo());
                    model.setModelType(CAircraftModel::TypeOwnSimulatorModel);
                    model.setSimulator(CSimulatorInfo::xplane());
                    model.setFileName(aircraftIt.filePath());
                    const QDateTime lastModifiedTs(aircraftIt.fileInfo().lastModified());
                    model.setUtcTimestamp(lastModifiedTs);
                    model.setFileTimestamp(lastModifiedTs);
                    model.setModelMode(CAircraftModel::Exclude);
                    addUniqueModel(model, installedModels);

                    const QString baseModelString = model.getModelString();
                    QDirIterator liveryIt(aircraftIt.fileInfo().canonicalPath() + "/liveries", QDir::Dirs | QDir::NoDotAndDotDot);
                    while (liveryIt.hasNext())
                    {
                        liveryIt.next();
                        model.setModelString(baseModelString % ' ' % liveryIt.fileName());
                        addUniqueModel(model, installedModels);
                    }
                }
                return installedModels;
            }

            BlackMisc::Simulation::CAircraftModel CAircraftModelLoaderXPlane::extractAcfProperties(const QString &filePath, const QFileInfo &fileInfo)
            {
                BlackMisc::Simulation::CAircraftModel model;
                QFile file(filePath);
                if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { return model; }

                QTextStream ts(&file);
                if (ts.readLine() == "I" && ts.readLine().contains("version") && ts.readLine() == "ACF")
                {
                    while (!ts.atEnd())
                    {
                        const QString line = ts.readLine();
                        QVector<QStringRef> tokens = line.splitRef(' ', QString::SkipEmptyParts);
                        if (tokens.size() < 3 || tokens.at(0) != QLatin1String("P")) { continue; }
                        if (tokens.at(1) == QLatin1String("acf/_ICAO"))
                        {
                            const CAircraftIcaoCode icao(tokens.at(2).toString());
                            model.setAircraftIcaoCode(icao);
                        }
                        else if (tokens.at(1) == QLatin1String("acf/_descrip"))
                        {
                            const QString desc(line.mid(tokens.at(2).position()));
                            model.setDescription("[ACF] " % desc);
                        }
                        else if (tokens.at(1) == QLatin1String("acf/_name"))
                        {
                            const QString name(line.mid(tokens.at(2).position()));
                            model.setName(name);
                        }
                        else if (tokens.at(1) == QLatin1String("acf/_studio"))
                        {
                            const CDistributor dist({}, line.mid(tokens.at(2).position()), {}, {}, CSimulatorInfo::XPLANE);
                            model.setDistributor(dist);
                        }
                        else if (tokens.at(1) == QLatin1String("acf/_author"))
                        {
                            if (model.getDistributor().hasDescription()) { continue; }
                            thread_local const QRegularExpression end("\\W\\s", QRegularExpression::UseUnicodePropertiesOption);
                            QString author = line.mid(tokens.at(2).position());
                            author = author.left(author.indexOf(end)).trimmed();
                            if (author.isEmpty()) { continue; }
                            const CDistributor dist({}, author, {}, {}, CSimulatorInfo::XPLANE);
                            model.setDistributor(dist);
                        }
                    }
                }
                file.close();

                model.setModelString(stringForFlyableModel(model, fileInfo));
                if (!model.hasDescription()) { model.setDescription(descriptionForFlyableModel(model)); }
                return model;
            }

            CAircraftModelList CAircraftModelLoaderXPlane::parseCslPackages(const QString &rootDirectory, const QStringList &excludeDirectories)
            {
                Q_UNUSED(excludeDirectories);
                if (rootDirectory.isEmpty()) { return {}; }

                m_cslPackages.clear();

                QDir searchPath(rootDirectory, fileFilterCsl());
                QDirIterator it(searchPath, QDirIterator::Subdirectories);
                while (it.hasNext())
                {
                    QString packageFile = it.next();
                    if (CFileUtils::isExcludedDirectory(it.filePath(), excludeDirectories)) { continue; }

                    const QString packageFilePath = it.fileInfo().absolutePath();
                    QFile file(packageFile);
                    file.open(QIODevice::ReadOnly);
                    QString content;

                    QTextStream ts(&file);
                    content.append(ts.readAll());
                    file.close();

                    const auto package = parsePackageHeader(packageFilePath, content);
                    if (package.hasValidHeader()) m_cslPackages.push_back(package);
                }

                CAircraftModelList installedModels;

                // Now we do a full run
                for (auto &package : m_cslPackages)
                {
                    QString packageFile(package.path);
                    packageFile += "/xsb_aircraft.txt";
                    QFile file(packageFile);
                    file.open(QIODevice::ReadOnly);
                    QString content;

                    QTextStream ts(&file);
                    content.append(ts.readAll());
                    file.close();
                    parseFullPackage(content, package);

                    for (const auto &plane : as_const(package.planes))
                    {
                        if (installedModels.containsModelString(plane.getModelName()))
                        {
                            const CStatusMessage msg = CStatusMessage(this).warning("XPlane model '%1' exists already! Potential model string conflict! Ignoring it.") << plane.getModelName();
                            m_loadingMessages.push_back(msg);
                            continue;
                        }

                        CAircraftModel model(plane.getModelName(), CAircraftModel::TypeOwnSimulatorModel);
                        model.setFileName(plane.filePath);
                        model.setUtcTimestamp(QFileInfo(plane.filePath).lastModified());

                        CAircraftIcaoCode icao(plane.icao);
                        model.setAircraftIcaoCode(icao);

                        CLivery livery;
                        livery.setCombinedCode(plane.livery);
                        CAirlineIcaoCode airline(plane.airline);
                        livery.setAirlineIcaoCode(airline);
                        model.setLivery(livery);

                        CDistributor distributor(package.name);
                        model.setDistributor(distributor);

                        model.setSimulator(CSimulatorInfo::xplane());
                        model.setDescription("[CSL]");
                        installedModels.push_back(model);
                    }
                }
                return installedModels;
            }

            bool CAircraftModelLoaderXPlane::doPackageSub(QString &ioPath)
            {
                for (auto i = m_cslPackages.cbegin(); i != m_cslPackages.cend(); ++i)
                {
                    if (strncmp(qPrintable(i->name), qPrintable(ioPath), i->name.size()) == 0)
                    {
                        ioPath.remove(0, i->name.size());
                        ioPath.insert(0, i->path);
                        return true;
                    }
                }
                return false;
            }

            bool CAircraftModelLoaderXPlane::parseExportCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum)
            {
                if (tokens.size() != 2)
                {
                    CLogMessage(this).warning("XPlane '%1' - '%2': EXPORT_NAME command requires 1 argument.") << path << lineNum;
                    return false;
                }

                auto p = std::find_if(m_cslPackages.cbegin(), m_cslPackages.cend(), [&tokens](const CSLPackage & p) { return p.name == tokens[1]; });
                if (p == m_cslPackages.cend())
                {
                    package.path = path;
                    package.name = tokens[1];
                    return true;
                }
                else
                {
                    CLogMessage(this).warning("XPlane package name '%1' already in use by '%2' reqested by use by '%3'") << tokens[1] << p->path << path;
                    return false;
                }
            }

            bool CAircraftModelLoaderXPlane::parseDependencyCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum)
            {
                Q_UNUSED(package);
                if (tokens.size() != 2)
                {
                    CLogMessage(this).warning("'%1' - '%2': DEPENDENCY command requires 1 argument.") << path << lineNum;
                    return false;
                }

                if (std::count_if(m_cslPackages.cbegin(), m_cslPackages.cend(), [&tokens](const CSLPackage & p) { return p.name == tokens[1]; }) == 0)
                {
                    CLogMessage(this).warning("XPlane required package %1 not found. Aborting processing of this package.") << tokens[1];
                    return false;
                }

                return true;
            }

            //! Reads the next line from stream ignoring empty ones.
            //! Returns a null QString if stream is at the end.
            QString readLineFrom(QTextStream &stream)
            {
                QString line;
                do
                {
                    line = stream.readLine();
                }
                while (line.isEmpty() && !stream.atEnd());
                return line;
            }

            bool CAircraftModelLoaderXPlane::parseObjectCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum)
            {
                if (tokens.size() != 2)
                {
                    CLogMessage(this).warning("XPlane '%1' - '%2': OBJECT command requires 1 argument.") << path << lineNum;
                    return false;
                }

                QString relativePath(tokens[1]);
                normalizePath(relativePath);
                QString fullPath(relativePath);
                if (!doPackageSub(fullPath))
                {
                    CLogMessage(this).warning("XPlane '%1' - '%2: package not found.") << path << lineNum;
                    return false;
                }

                // Get obj header
                QFile objFile(fullPath);
                if (!objFile.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    CLogMessage(this).warning("XPlane object '%1' does not exist.") << fullPath;
                    return false;
                }
                QTextStream ts(&objFile);

                // First line is about line endings. We don't need it.
                readLineFrom(ts);

                // Version number.
                QString versionLine = readLineFrom(ts);
                if (versionLine.isNull()) { return false; }
                QString version = splitStringRefs(versionLine, [](QChar c) { return c.isSpace(); }).value(0).toString();

                // For version 7, there is another line 'obj'
                if (version == "700") { readLineFrom(ts); }

                // Texture
                QString textureLine = readLineFrom(ts);
                if (textureLine.isNull()) { return false; }
                QString texture = splitStringRefs(textureLine, [](QChar c) { return c.isSpace(); }).value(0).toString();

                objFile.close();

                package.planes.push_back(CSLPlane());
                QFileInfo fileInfo(fullPath);

                QStringList dirNames;
                dirNames.append(relativePath.split('/', QString::SkipEmptyParts));
                // Replace the first one being the package name with the package root dir
                QString packageRootDir = package.path.mid(package.path.lastIndexOf('/') + 1);
                dirNames.replace(0, packageRootDir);
                // Remove the last one being the obj itself
                dirNames.removeLast();

                package.planes.back().dirNames = dirNames;
                package.planes.back().objectName = fileInfo.completeBaseName();
                package.planes.back().textureName = texture;
                package.planes.back().filePath = fullPath;
                return true;
            }

            bool CAircraftModelLoaderXPlane::parseTextureCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum)
            {
                if (tokens.size() != 2)
                {
                    CLogMessage(this).warning("XPlane '%1' - '%2': TEXTURE command requires 1 argument.") << path << lineNum;
                    return false;
                }

                // Load regular texture
                QString relativeTexPath = tokens[1];
                normalizePath(relativeTexPath);
                QString absoluteTexPath(relativeTexPath);

                if (!doPackageSub(absoluteTexPath))
                {
                    CLogMessage(this).warning("XPlane '%1' - '%2': package not found.") << path << lineNum;
                    return false;
                }

                QFileInfo fileInfo(absoluteTexPath);
                if (!fileInfo.exists())
                {
                    CLogMessage(this).warning("XPlane texture '%1' does not exist.") << absoluteTexPath;
                    return false;
                }

                package.planes.back().textureName = fileInfo.completeBaseName();
                return true;
            }

            bool CAircraftModelLoaderXPlane::parseAircraftCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum)
            {
                Q_UNUSED(package)
                // AIRCAFT <min> <max> <path>
                if (tokens.size() != 4)
                {
                    CLogMessage(this).warning("XPlane '%1' - '%2': AIRCRAFT command requires 3 arguments.") << path << lineNum;
                }

                // Flyable aircrafts are parsed by a different method. We don't know any aircraft files in CSL packages.
                // If there is one, implement this method here.
                CLogMessage(this).warning("Not implemented yet.");
                return true;
            }

            bool CAircraftModelLoaderXPlane::parseObj8AircraftCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum)
            {
                // OBJ8_AIRCRAFT <path>
                if (tokens.size() != 2)
                {
                    CLogMessage(this).warning("XPlane '%1' - '%2': OBJ8_AIRCARFT command requires 1 argument.") << path << lineNum;
                }

                package.planes.push_back(CSLPlane());
                // Package name and object name uniquely identify an OBJ8 aircraft.
                // File path just points to the package root.
                return true;
            }

            bool CAircraftModelLoaderXPlane::parseObj8Command(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum)
            {
                // OBJ8 <group> <animate YES|NO> <filename> {<texture filename> {<lit texture filename>}}
                if (tokens.size() < 4 || tokens.size() > 6)
                {
                    CLogMessage(this).warning("XPlane '%1' - '%2': OBJ8 command takes 3-5 arguments.") << path << lineNum;
                }

                if (tokens[1] != "SOLID") { return true; }

                QString relativePath(tokens[3]);
                normalizePath(relativePath);
                QString fullPath(relativePath);
                if (!doPackageSub(fullPath))
                {
                    CLogMessage(this).warning("XPlane '%1' - '%2': package not found.") << path << lineNum;
                    return false;
                }

                QStringList dirNames;
                dirNames.append(relativePath.split('/', QString::SkipEmptyParts));
                // Replace the first one being the package name with the package root dir
                QString packageRootDir = package.path.mid(package.path.lastIndexOf('/') + 1);
                dirNames.replace(0, packageRootDir);
                // Remove the last one being the obj itself
                dirNames.removeLast();

                QFileInfo fileInfo(fullPath);
                package.planes.back().dirNames = dirNames;
                package.planes.back().objectName = fileInfo.completeBaseName();
                package.planes.back().filePath = fullPath;

                if (tokens.size() >= 5)
                {
                    // Load regular texture
                    QString relativeTexPath = dirNames.join('/') + '/' + tokens[4];
                    normalizePath(relativeTexPath);
                    QString absoluteTexPath(relativeTexPath);

                    if (!doPackageSub(absoluteTexPath))
                    {
                        CLogMessage(this).warning("XPlane '%1' - '%2': package not found.") << path << lineNum;
                        return false;
                    }

                    QFileInfo fileInfo(absoluteTexPath);
                    if (!fileInfo.exists())
                    {
                        CLogMessage(this).warning("XPlane texture '%1' does not exist.") << absoluteTexPath;
                        return false;
                    }

                    package.planes.back().textureName = fileInfo.completeBaseName();
                }
                return true;
            }

            bool CAircraftModelLoaderXPlane::parseHasGearCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum)
            {
                Q_UNUSED(tokens)
                Q_UNUSED(package)
                Q_UNUSED(path)
                Q_UNUSED(lineNum)
                return true;
            }

            bool CAircraftModelLoaderXPlane::parseIcaoCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum)
            {
                // ICAO <code>
                if (tokens.size() != 2)
                {
                    CLogMessage(this).warning("XPlane '%1' - '%2': ICAO command requires 1 argument.") << path << lineNum;
                    return false;
                }

                QString icao = tokens[1];
                package.planes.back().icao = icao;
                return true;
            }

            bool CAircraftModelLoaderXPlane::parseAirlineCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum)
            {
                // AIRLINE <code> <airline>
                if (tokens.size() != 3)
                {
                    CLogMessage(this).warning("XPlane '%1' - '%2': AIRLINE command requires 2 arguments.") << path << lineNum;
                    return false;
                }

                QString icao = tokens[1];
                package.planes.back().icao = icao;
                QString airline = tokens[2];
                package.planes.back().airline = airline;
                return true;
            }

            bool CAircraftModelLoaderXPlane::parseLiveryCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum)
            {
                // LIVERY <code> <airline> <livery>
                if (tokens.size() != 4)
                {
                    CLogMessage(this).warning("XPlane '%1' - '%2': LIVERY command requires 3 arguments.") << path << lineNum;
                    return false;
                }

                QString icao = tokens[1];
                package.planes.back().icao = icao;
                QString airline = tokens[2];
                package.planes.back().airline = airline;
                QString livery = tokens[3];
                package.planes.back().livery = livery;
                return true;
            }

            bool CAircraftModelLoaderXPlane::parseDummyCommand(const QStringList & /* tokens */, CSLPackage & /* package */, const QString & /* path */, int /*lineNum*/)
            {
                return true;
            }

            CAircraftModelLoaderXPlane::CSLPackage CAircraftModelLoaderXPlane::parsePackageHeader(const QString &path, const QString &content)
            {
                using command = std::function<bool(const QStringList &, CSLPackage &, const QString &, int)>;
                using namespace std::placeholders;

                const QMap<QString, command> commands
                {
                    { "EXPORT_NAME", std::bind(&CAircraftModelLoaderXPlane::parseExportCommand, this, _1, _2, _3, _4) }
                };

                CSLPackage package;
                int lineNum = 0;

                QString localCopy(content);
                QTextStream in(&localCopy);
                while (!in.atEnd())
                {
                    ++lineNum;
                    QString line = in.readLine();
                    auto tokens = splitString(line, [](QChar c) { return c.isSpace(); });
                    if (!tokens.empty())
                    {
                        auto it = commands.find(tokens[0]);
                        if (it != commands.end())
                        {
                            bool result = it.value()(tokens, package, path, lineNum);
                            // Stop loop once we found EXPORT command
                            if (result) break;
                        }
                    }
                }
                return package;
            }

            void CAircraftModelLoaderXPlane::parseFullPackage(const QString &content, CSLPackage &package)
            {
                using command = std::function<bool(const QStringList &, CSLPackage &, const QString &, int)>;
                using namespace std::placeholders;

                const QMap<QString, command> commands
                {
                    { "EXPORT_NAME", std::bind(&CAircraftModelLoaderXPlane::parseDummyCommand, this, _1, _2, _3, _4) },
                    { "DEPENDENCY", std::bind(&CAircraftModelLoaderXPlane::parseDependencyCommand, this, _1, _2, _3, _4) },
                    { "OBJECT", std::bind(&CAircraftModelLoaderXPlane::parseObjectCommand, this, _1, _2, _3, _4) },
                    { "TEXTURE", std::bind(&CAircraftModelLoaderXPlane::parseTextureCommand, this, _1, _2, _3, _4) },
                    { "AIRCRAFT", std::bind(&CAircraftModelLoaderXPlane::parseAircraftCommand, this, _1, _2, _3, _4) },
                    { "OBJ8_AIRCRAFT", std::bind(&CAircraftModelLoaderXPlane::parseObj8AircraftCommand, this, _1, _2, _3, _4) },
                    { "OBJ8", std::bind(&CAircraftModelLoaderXPlane::parseObj8Command, this, _1, _2, _3, _4) },
                    { "HASGEAR", std::bind(&CAircraftModelLoaderXPlane::parseHasGearCommand, this, _1, _2, _3, _4) },
                    { "ICAO", std::bind(&CAircraftModelLoaderXPlane::parseIcaoCommand, this, _1, _2, _3, _4) },
                    { "AIRLINE", std::bind(&CAircraftModelLoaderXPlane::parseAirlineCommand, this, _1, _2, _3, _4) },
                    { "LIVERY", std::bind(&CAircraftModelLoaderXPlane::parseLiveryCommand, this, _1, _2, _3, _4) },
                    { "VERT_OFFSET", std::bind(&CAircraftModelLoaderXPlane::parseDummyCommand, this, _1, _2, _3, _4) },
                };

                int lineNum = 0;

                QString localCopy(content);
                QTextStream in(&localCopy);

                while (!in.atEnd())
                {
                    ++lineNum;
                    QString line = in.readLine();
                    if (line.isEmpty() || line[0] == '#') continue;
                    auto tokens = splitString(line, [](QChar c) { return c.isSpace(); });
                    if (!tokens.empty())
                    {
                        auto it = commands.find(tokens[0]);
                        if (it != commands.end())
                        {
                            bool result = it.value()(tokens, package, package.path, lineNum);
                            if (!result)
                            {
                                const CStatusMessage m = CStatusMessage(this).warning("XPlane ignoring CSL package '%1'") << package.name;
                                m_loadingMessages.push_back(m);
                                break;
                            }
                        }
                        else
                        {
                            const CStatusMessage m = CStatusMessage(this).warning("XPlane unrecognized command '%1' in '%2'") << tokens[0] << package.name;
                            m_loadingMessages.push_back(m);
                            break;
                        }
                    }
                }
            }

            const QString &CAircraftModelLoaderXPlane::fileFilterFlyable()
            {
                static const QString f("*.acf");
                return f;
            }

            const QString &CAircraftModelLoaderXPlane::fileFilterCsl()
            {
                static const QString f("xsb_aircraft.txt");
                return f;
            }

        } // namespace
    } // namespace
} // namespace
