/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/aircraftmodelutils.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/simulation/xplane/aircraftmodelloaderxplane.h"
#include "blackmisc/simulation/xplane/xplaneutil.h"
#include "blackmisc/simulation/xplane/qtfreeutils.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/worker.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/verify.h"
#include "blackconfig/buildconfig.h"

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

using namespace BlackConfig;
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

            //! Create a description string for a model that doesn't already have one
            static QString descriptionForFlyableModel(const CAircraftModel &model)
            {
                if (!model.getName().isEmpty())
                {
                    if (model.getDistributor().hasDescription() && !model.getName().contains(model.getDistributor().getDescription()))
                    {
                        return u"[ACF] " % model.getName() % u" by " % model.getDistributor().getDescription();
                    }
                    else
                    {
                        return u"[ACF] " % model.getName();
                    }
                }
                else if (model.hasAircraftDesignator())
                {
                    if (model.getDistributor().hasDescription())
                    {
                        return u"[ACF] " % model.getAircraftIcaoCodeDesignator() % u" by " % model.getDistributor().getDescription();
                    }
                    else
                    {
                        return u"[ACF] " % model.getAircraftIcaoCodeDesignator();
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
                    emit this->loadingFinished(CStatusMessage(this, CStatusMessage::SeverityError, u"XPlane model directories '%1' are empty") << modelDirectories.join(", "), simulator, ParsedData);
                    return;
                }

                if (mode.testFlag(LoadInBackground))
                {
                    if (m_parserWorker && !m_parserWorker->isFinished()) { return; }
                    emit this->diskLoadingStarted(simulator, mode);

                    m_parserWorker = CWorker::fromTask(this, "CAircraftModelLoaderXPlane::performParsing",
                                                       [this, modelDirs, excludedDirectoryPatterns, modelConsolidation]()
                    {
                        auto models = this->performParsing(modelDirs, excludedDirectoryPatterns);
                        if (modelConsolidation) { modelConsolidation(models, true); }
                        return models;
                    });
                    m_parserWorker->thenWithResult<CAircraftModelList>(this, [ = ](const auto & models)
                    {
                        this->updateInstalledModels(models);
                        m_loadingMessages.freezeOrder();
                        emit this->loadingFinished(m_loadingMessages, simulator, ParsedData);
                    });
                }
                else if (mode.testFlag(LoadDirectly))
                {
                    emit this->diskLoadingStarted(simulator, mode);
                    CAircraftModelList models(this->performParsing(modelDirs, excludedDirectoryPatterns));
                    this->updateInstalledModels(models);
                }
            }

            bool CAircraftModelLoaderXPlane::isLoadingFinished() const
            {
                return !m_parserWorker || m_parserWorker->isFinished();
            }

            void CAircraftModelLoaderXPlane::updateInstalledModels(const CAircraftModelList &models)
            {
                this->setModelsForSimulator(models, CSimulatorInfo::xplane());
                const CStatusMessage m = CStatusMessage(this, CStatusMessage::SeverityInfo, u"XPlane updated '%1' models") << models.size();
                m_loadingMessages.push_back(m);
            }

            QString CAircraftModelLoaderXPlane::CSLPlane::getModelName() const
            {
                QString modelName =
                    dirNames.join(' ') %
                    u' ' % objectName;
                if (objectVersion == OBJ7) { modelName += u' ' % textureName; }
                return std::move(modelName).trimmed();
            }

            CAircraftModelList CAircraftModelLoaderXPlane::performParsing(const QStringList &rootDirectories, const QStringList &excludeDirectories)
            {
                CAircraftModelList allModels;
                for (const QString &rootDirectory : rootDirectories)
                {
                    allModels.push_back(parseCslPackages(rootDirectory, excludeDirectories));
                    allModels.push_back(parseFlyableAirplanes(rootDirectory, excludeDirectories));
                }
                return allModels;
            }

            //! Add model only if there no other model with the same model string
            void CAircraftModelLoaderXPlane::addUniqueModel(const CAircraftModel &model, CAircraftModelList &models)
            {
                if (models.containsModelString(model.getModelString()))
                {
                    const CStatusMessage m = CStatusMessage(this).warning(u"XPlane model '%1' exists already! Potential model string conflict! Ignoring it.") << model.getModelString();
                    m_loadingMessages.push_back(m);
                }
                models.push_back(model);
            }

            CAircraftModelList CAircraftModelLoaderXPlane::parseFlyableAirplanes(const QString &rootDirectory, const QStringList &excludeDirectories)
            {
                Q_UNUSED(excludeDirectories)
                if (rootDirectory.isEmpty()) { return {}; }

                QDir searchPath(rootDirectory, fileFilterFlyable());
                QDirIterator aircraftIt(searchPath, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

                emit loadingProgress(this->getSimulator(), QStringLiteral("Parsing flyable airplanes in '%1'").arg(rootDirectory), -1);

                CAircraftModelList installedModels;
                while (aircraftIt.hasNext())
                {
                    aircraftIt.next();
                    if (CFileUtils::isExcludedDirectory(aircraftIt.fileInfo(), excludeDirectories, Qt::CaseInsensitive)) { continue; }

                    using namespace BlackMisc::Simulation::XPlane::QtFreeUtils;
                    AcfProperties acfProperties = extractAcfProperties(aircraftIt.filePath().toStdString());

                    const CDistributor dist({}, QString::fromStdString(acfProperties.author), {}, {}, CSimulatorInfo::XPLANE);
                    CAircraftModel model;
                    model.setAircraftIcaoCode(QString::fromStdString(acfProperties.aircraftIcaoCode));
                    model.setDescription(QString::fromStdString(acfProperties.modelDescription));
                    model.setName(QString::fromStdString(acfProperties.modelName));
                    model.setDistributor(dist);
                    model.setModelString(QString::fromStdString(acfProperties.modelString));
                    if (!model.hasDescription()) { model.setDescription(descriptionForFlyableModel(model)); }
                    model.setModelType(CAircraftModel::TypeOwnSimulatorModel);
                    model.setSimulator(CSimulatorInfo::xplane());
                    model.setFileDetailsAndTimestamp(aircraftIt.fileInfo());
                    model.setModelMode(CAircraftModel::Exclude);
                    addUniqueModel(model, installedModels);

                    const QString baseModelString = model.getModelString();
                    QDirIterator liveryIt(CFileUtils::appendFilePaths(aircraftIt.fileInfo().canonicalPath(), QStringLiteral("liveries")), QDir::Dirs | QDir::NoDotAndDotDot);
                    emit this->loadingProgress(this->getSimulator(), QStringLiteral("Parsing flyable liveries in '%1'").arg(aircraftIt.fileInfo().canonicalPath()), -1);
                    while (liveryIt.hasNext())
                    {
                        liveryIt.next();
                        model.setModelString(baseModelString % u' ' % liveryIt.fileName());
                        addUniqueModel(model, installedModels);
                    }
                }
                return installedModels;
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
                    const QString packageFile = CFileUtils::appendFilePaths(package.path, QStringLiteral("xsb_aircraft.txt"));
                    emit this->loadingProgress(this->getSimulator(), QStringLiteral("Parsing CSL '%1'").arg(packageFile), -1);

                    QFile file(packageFile);
                    file.open(QIODevice::ReadOnly);
                    QString content;

                    QTextStream ts(&file);
                    content.append(ts.readAll());
                    file.close();
                    parseFullPackage(content, package);

                    for (const auto &plane : std::as_const(package.planes))
                    {
                        if (installedModels.containsModelString(plane.getModelName()))
                        {
                            const CStatusMessage msg = CStatusMessage(this).warning(u"XPlane model '%1' exists already! Potential model string conflict! Ignoring it.") << plane.getModelName();
                            m_loadingMessages.push_back(msg);
                            continue;
                        }

                        CAircraftModel model(plane.getModelName(), CAircraftModel::TypeOwnSimulatorModel);
                        const CAircraftIcaoCode icao(plane.icao);
                        const QFileInfo modelFileInfo(plane.filePath);
                        model.setFileDetailsAndTimestamp(modelFileInfo);
                        model.setAircraftIcaoCode(icao);

                        if (CBuildConfig::isLocalDeveloperDebugBuild())
                        {
                            BLACK_AUDIT_X(modelFileInfo.exists(), Q_FUNC_INFO, "Model does NOT exist");
                        }

                        CLivery livery;
                        livery.setCombinedCode(plane.livery);
                        CAirlineIcaoCode airline(plane.airline);
                        livery.setAirlineIcaoCode(airline);
                        model.setLivery(livery);

                        model.setSimulator(CSimulatorInfo::xplane());
                        QString modelDescription("[CSL]");
                        if (plane.objectVersion == CSLPlane::OBJ7) { modelDescription += "[OBJ7]"; }
                        else if (plane.objectVersion == CSLPlane::OBJ8) { modelDescription += "[OBJ8]"; }
                        model.setDescription(modelDescription);
                        installedModels.push_back(model);
                    }
                }
                return installedModels;
            }

            bool CAircraftModelLoaderXPlane::doPackageSub(QString &ioPath)
            {
                for (auto i = m_cslPackages.cbegin(); i != m_cslPackages.cend(); ++i)
                {
                    if (strncmp(qPrintable(i->name), qPrintable(ioPath), static_cast<size_t>(i->name.size())) == 0)
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
                    const CStatusMessage m = CStatusMessage(this).error(u"%1/xsb_aircraft.txt Line %2 : EXPORT_NAME command requires 1 argument.") << path << lineNum;
                    m_loadingMessages.push_back(m);
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
                    const CStatusMessage m = CStatusMessage(this).error(u"XPlane package name '%1' already in use by '%2' reqested by use by '%3'") << tokens[1] << p->path << path;
                    m_loadingMessages.push_back(m);
                    return false;
                }
            }

            bool CAircraftModelLoaderXPlane::parseDependencyCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum)
            {
                Q_UNUSED(package);
                if (tokens.size() != 2)
                {
                    const CStatusMessage m = CStatusMessage(this).error(u"%1/xsb_aircraft.txt Line %2 : DEPENDENCY command requires 1 argument.") << path << lineNum;
                    m_loadingMessages.push_back(m);
                    return false;
                }

                if (std::count_if(m_cslPackages.cbegin(), m_cslPackages.cend(), [&tokens](const CSLPackage & p) { return p.name == tokens[1]; }) == 0)
                {
                    const CStatusMessage m = CStatusMessage(this).error(u"XPlane required package %1 not found. Aborting processing of this package.") << tokens[1];
                    m_loadingMessages.push_back(m);
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

            bool CAircraftModelLoaderXPlane::parseObjectCommand(const QStringList &, CSLPackage &package, const QString &path, int lineNum)
            {
                package.planes.push_back(CSLPlane());

                const auto m = CStatusMessage(this).error(u"%1/xsb_aircraft.txt Line %2 : Unsupported legacy CSL format.") << path << lineNum;
                m_loadingMessages.push_back(m);
                return false;
            }

            bool CAircraftModelLoaderXPlane::parseTextureCommand(const QStringList &, CSLPackage &package, const QString &path, int lineNum)
            {
                if (!package.planes.isEmpty() && !package.planes.back().hasErrors)
                {
                    const auto m = CStatusMessage(this).error(u"%1/xsb_aircraft.txt Line %2 : Unsupported legacy CSL format.") << path << lineNum;
                    m_loadingMessages.push_back(m);
                }
                return false;
            }

            bool CAircraftModelLoaderXPlane::parseAircraftCommand(const QStringList &, CSLPackage &package, const QString &path, int lineNum)
            {
                package.planes.push_back(CSLPlane());

                const auto m = CStatusMessage(this).error(u"%1/xsb_aircraft.txt Line %2 : Unsupported legacy CSL format.") << path << lineNum;
                m_loadingMessages.push_back(m);
                return false;
            }

            bool CAircraftModelLoaderXPlane::parseObj8AircraftCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum)
            {
                package.planes.push_back(CSLPlane());

                // OBJ8_AIRCRAFT <path>
                if (tokens.size() != 2)
                {
                    const CStatusMessage m = CStatusMessage(this).warning(u"%1/xsb_aircraft.txt Line %2 : OBJ8_AIRCARFT command requires 1 argument.") << path << lineNum;
                    m_loadingMessages.push_back(m);
                    if (tokens.size() < 2)
                    {
                        return false;
                    }
                }

                package.planes.back().objectName = tokens[1];
                package.planes.back().objectVersion = CSLPlane::OBJ8;
                return true;
            }

            bool CAircraftModelLoaderXPlane::parseObj8Command(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum)
            {
                // OBJ8 <group> <animate YES|NO> <filename>
                if (tokens.size() != 4)
                {
                    if (tokens.size() == 5 || tokens.size() == 6)
                    {
                        const CStatusMessage m = CStatusMessage(this).error(u"%1/xsb_aircraft.txt Line %2 : Unsupported IVAO CSL format - consider using CSL2XSB.") << path << lineNum;
                        m_loadingMessages.push_back(m);
                    }
                    else
                    {
                        const CStatusMessage m = CStatusMessage(this).error(u"%1/xsb_aircraft.txt Line %2 : OBJ8 command takes 3 arguments.") << path << lineNum;
                        m_loadingMessages.push_back(m);
                    }
                    return false;
                }
                if (package.planes.isEmpty())
                {
                    m_loadingMessages.push_back(CStatusMessage(this).error(u"%1/xsb_aircraft.txt Line %2 : invalid position for command.") << path << lineNum);
                    return false;
                }

                if (tokens[1] != "SOLID") { return true; }

                QString relativePath(tokens[3]);
                normalizePath(relativePath);
                QString fullPath(relativePath);
                if (!doPackageSub(fullPath))
                {
                    const CStatusMessage m = CStatusMessage(this).error(u"%1/xsb_aircraft.txt Line %2 : package not found.") << path << lineNum;
                    m_loadingMessages.push_back(m);
                    return false;
                }

                // just the name of the dir containing xsbaircraft.txt
                QString packageRootDir = package.path.mid(package.path.lastIndexOf('/') + 1);
                package.planes.back().dirNames = QStringList { packageRootDir };

                package.planes.back().filePath = fullPath;
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
                    const CStatusMessage m = CStatusMessage(this).error(u"%1/xsb_aircraft.txt Line %2 : ICAO command requires 1 argument.") << path << lineNum;
                    m_loadingMessages.push_back(m);
                    return false;
                }
                if (package.planes.isEmpty())
                {
                    m_loadingMessages.push_back(CStatusMessage(this).error(u"%1/xsb_aircraft.txt Line %2 : invalid position for command.") << path << lineNum);
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
                    const CStatusMessage m = CStatusMessage(this).error(u"%1/xsb_aircraft.txt Line %2 : AIRLINE command requires 2 arguments.") << path << lineNum;
                    m_loadingMessages.push_back(m);
                    return false;
                }
                if (package.planes.isEmpty())
                {
                    m_loadingMessages.push_back(CStatusMessage(this).error(u"%1/xsb_aircraft.txt Line %2 : invalid position for command.") << path << lineNum);
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
                    const CStatusMessage m = CStatusMessage(this).error(u"%1/xsb_aircraft.txt Line %2 : LIVERY command requires 3 arguments.") << path << lineNum;
                    m_loadingMessages.push_back(m);
                    return false;
                }
                if (package.planes.isEmpty())
                {
                    m_loadingMessages.push_back(CStatusMessage(this).error(u"%1/xsb_aircraft.txt Line %2 : invalid position for command.") << path << lineNum);
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
                                if (! package.planes.empty()) { package.planes.back().hasErrors = true; }
                            }
                        }
                        else
                        {
                            const CStatusMessage m = CStatusMessage(this).error(u"%1/xsb_aircraft.txt Line %2 : Unrecognized CSL command: '%3'") << package.path << lineNum << tokens[0];
                            m_loadingMessages.push_back(m);
                        }
                    }
                }

                // Remove all planes with errors
                auto it = std::remove_if(package.planes.begin(), package.planes.end(), [](const CSLPlane &plane)
                {
                    return plane.hasErrors;
                });
                package.planes.erase(it, package.planes.end());
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
