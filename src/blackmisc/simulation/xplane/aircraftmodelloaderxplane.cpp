/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftmodelloaderxplane.h"
#include "xplaneutil.h"
#include "blackmisc/predicates.h"
#include "blackmisc/logmessage.h"

#include <QDirIterator>
#include <QTextStream>
#include <QFile>
#include <QRegularExpression>

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

            CAircraftModelLoaderXPlane::CAircraftModelLoaderXPlane()
            { }

            CAircraftModelLoaderXPlane::CAircraftModelLoaderXPlane(const CSimulatorInfo &simInfo, const QString &rootDirectory, const QStringList &exludes) :
                IAircraftModelLoader(simInfo),
                m_rootDirectory(rootDirectory),
                m_excludedDirectories(exludes)
            { }

            CAircraftModelLoaderXPlane::~CAircraftModelLoaderXPlane()
            {
                // that should be safe as long as the worker uses deleteLater (which it does)
                if (this->m_parserWorker) { this->m_parserWorker->waitForFinished(); }
            }

            bool CAircraftModelLoaderXPlane::changeRootDirectory(const QString &directory)
            {
                if (m_rootDirectory == directory) { return false; }
                if (directory.isEmpty() || !existsDir(directory)) { return false; }

                m_rootDirectory = directory;
                return true;
            }

            CPixmap CAircraftModelLoaderXPlane::iconForModel(const QString &modelString, CStatusMessage &statusMessage) const
            {
                // X-Plane does not have previews. Maybe we can just use the textures?
                Q_UNUSED(modelString)
                Q_UNUSED(statusMessage)
                return {};
            }

            void CAircraftModelLoaderXPlane::startLoading(LoadMode mode)
            {
                m_installedModels.clear();
                if (m_rootDirectory.isEmpty())
                {
                    emit loadingFinished(false);
                    return;
                }

                if (mode == ModeBackground)
                {
                    if (m_parserWorker && !m_parserWorker->isFinished()) { return; }
                    auto rootDirectory = m_rootDirectory;
                    auto excludedDirectories = m_excludedDirectories;
                    m_parserWorker = BlackMisc::CWorker::fromTask(this, "CAircraftModelLoaderXPlane::performParsing",
                                     [this, rootDirectory, excludedDirectories]()
                    {
                        auto models = performParsing(rootDirectory, excludedDirectories);
                        return models;
                    });
                    m_parserWorker->thenWithResult<CAircraftModelList>(this, [this](const CAircraftModelList & models)
                    {
                        updateInstalledModels(models);
                    });
                }
                else if (mode == ModeBlocking)
                {
                    m_installedModels = performParsing(m_rootDirectory, m_excludedDirectories);
                    emit loadingFinished(true);
                }
            }

            bool CAircraftModelLoaderXPlane::isLoadingFinished() const
            {
                return !m_parserWorker || m_parserWorker->isFinished();
            }

            CAircraftModelList CAircraftModelLoaderXPlane::getAircraftModels() const
            {
                return m_installedModels;
            }

            void CAircraftModelLoaderXPlane::updateInstalledModels(const CAircraftModelList &models)
            {
                m_installedModels = models;
                emit loadingFinished(true);
            }

            CAircraftModelList CAircraftModelLoaderXPlane::performParsing(const QString &rootDirectory, const QStringList &excludeDirectories)
            {
                Q_UNUSED(excludeDirectories);
                QStringList packages;
                QDirIterator it(rootDirectory, QDirIterator::Subdirectories);
                while (it.hasNext())
                {
                    it.next();
                    if (it.fileName() == "xsb_aircraft.txt") { packages << it.fileInfo().absolutePath(); }
                }

                m_cslPackages.clear();
                for (const auto &packageFilePath : packages)
                {
                    QString packageFile(packageFilePath);
                    packageFile += "/xsb_aircraft.txt";
                    QFile file(packageFile);
                    file.open(QIODevice::ReadOnly);
                    QString content;

                    QTextStream ts(&file);
                    content.append(ts.readAll());
                    file.close();

                    auto package = parsePackageHeader(packageFilePath, content);
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

                    for (const auto &plane : package.planes)
                    {
                        CAircraftModel model(plane.modelName, CAircraftModel::TypeOwnSimulatorModel);
                        model.setFileName(plane.filePath);

                        CAircraftIcaoCode icao(plane.icao);
                        model.setAircraftIcaoCode(icao);

                        CLivery livery;
                        livery.setCombinedCode(plane.livery);
                        CAirlineIcaoCode airline;
                        airline.setName(plane.airline);
                        livery.setAirlineIcaoCode(airline);
                        model.setLivery(livery);

                        CDistributor distributor(package.name);
                        model.setDistributor(distributor);

                        model.setSimulatorInfo(m_simulatorInfo);
                        installedModels.push_back(model);
                    }
                }
                return installedModels;
            }

            bool CAircraftModelLoaderXPlane::existsDir(const QString &directory) const
            {
                if (directory.isEmpty()) { return false; }
                QDir dir(directory);
                //! \todo not available network dir can make this hang here
                return dir.exists();
            }

            bool CAircraftModelLoaderXPlane::doPackageSub(QString &ioPath)
            {
                for (auto i = m_cslPackages.begin(); i != m_cslPackages.end(); ++i)
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
                    CLogMessage(this).warning("%1 - %2: EXPORT_NAME command requires 1 argument.") << path << lineNum;
                    return false;
                }

                auto p = std::find_if(m_cslPackages.begin(), m_cslPackages.end(), [&tokens](CSLPackage p) { return p.name == tokens[1]; });
                if (p == m_cslPackages.end())
                {
                    package.path = path;
                    package.name = tokens[1];
                    return true;
                }
                else
                {
                    CLogMessage(this).warning("WARNING: Package name %1 already in use by %2 reqested by use by %3") << tokens[1] << p->path << path;
                    return false;
                }
            }

            bool CAircraftModelLoaderXPlane::parseDependencyCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum)
            {
                Q_UNUSED(package);
                if (tokens.size() != 2)
                {
                    CLogMessage(this).warning("%1 - %2: DEPENDENCY command requires 1 argument.") << path << lineNum;
                    return false;
                }

                if (std::count_if(m_cslPackages.begin(), m_cslPackages.end(), [&tokens](CSLPackage p) { return p.name == tokens[1]; }) == 0)
                {
                    CLogMessage(this).warning("WARNING: required package %1 not found. Aborting processing of this package.") << tokens[1];
                    return false;
                }

                return true;
            }

            bool CAircraftModelLoaderXPlane::parseObjectCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum)
            {
                if (tokens.size() != 2)
                {
                    CLogMessage(this).warning("%1 - %2: OBJECT command requires 1 argument.") << path << lineNum;
                    return false;
                }
                QString relativePath(tokens[1]);
                normalizePath(relativePath);
                QString fullPath(relativePath);
                if (!doPackageSub(fullPath))
                {
                    CLogMessage(this).warning("%1 - %2: package not found..") << path << lineNum;
                    return false;
                }

                package.planes.push_back(CSLPlane());
                package.planes.back().modelName = relativePath;
                package.planes.back().filePath = fullPath;
                return true;
            }

            bool CAircraftModelLoaderXPlane::parseTextureCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum)
            {
                if (tokens.size() != 2)
                {
                    CLogMessage(this).warning("%1 - %2: TEXTURE command requires 1 argument.") << path << lineNum;
                    return false;
                }

                // Load regular texture
                QString relativeTexPath = tokens[1];
                normalizePath(relativeTexPath);
                QString absoluteTexPath(relativeTexPath);

                if (!doPackageSub(absoluteTexPath))
                {
                    CLogMessage(this).warning("%1 - %2: package not found..") << path << lineNum;
                    return false;
                }

                package.planes.back().modelName += " ";
                package.planes.back().modelName += relativeTexPath;
                return true;
            }

            bool CAircraftModelLoaderXPlane::parseAircraftCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum)
            {
                // AIRCAFT <min> <max> <path>
                if (tokens.size() != 4)
                {
                    CLogMessage(this).warning("%1 - %2: AIRCRAFT command requires 3 arguments.") << path << lineNum;
                }

                QString relativePath = tokens[3];
                normalizePath(relativePath);
                QString absolutePath(relativePath);
                if (!doPackageSub(absolutePath))
                {
                    CLogMessage(this).warning("%1 - %2: package not found..") << path << lineNum;
                    return false;
                }
                package.planes.push_back(CSLPlane());
                package.planes.back().modelName = relativePath;
                package.planes.back().filePath = absolutePath;
                return true;
            }

            bool CAircraftModelLoaderXPlane::parseObj8AircraftCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum)
            {
                Q_UNUSED(package)
                // OBJ8_AIRCRAFT <path>
                if (tokens.size() != 2)
                {
                    CLogMessage(this).warning("%1 - %2: OBJ8_AIRCARFT command requires 1 argument.") << path << lineNum;
                }

                // RW: I need an example of the file to properly implement and test it.
                qFatal("Not implemented yet.");
                return false;
            }

            bool CAircraftModelLoaderXPlane::parseObj8Command(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum)
            {
                Q_UNUSED(package)
                // OBJ8 <group> <animate YES|NO> <filename>
                if (tokens.size() != 4)
                {
                    CLogMessage(this).warning("%1 - %2: OBJ8_AIRCARFT command requires 3 arguments.") << path << lineNum;
                }

                // RW: I need an example of the file to properly implement and test it.
                qFatal("Not implemented yet.");
                return false;
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
                    CLogMessage(this).warning("%1 - %2: ICAO command requires 1 argument.") << path << lineNum;
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
                    CLogMessage(this).warning("%1 - %2: AIRLINE command requires 2 arguments.") << path << lineNum;
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
                    CLogMessage(this).warning("%1 - %2: LIVERY command requires 3 arguments.") << path << lineNum;
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
                    auto tokens = line.split(QRegularExpression("\\s+"));
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
                };

                int lineNum = 0;

                QString localCopy(content);
                QTextStream in(&localCopy);
                while (!in.atEnd())
                {
                    ++lineNum;
                    QString line = in.readLine();
                    auto tokens = line.split(QRegularExpression("\\s+"), QString::SkipEmptyParts);
                    if (!tokens.empty())
                    {
                        auto it = commands.find(tokens[0]);
                        if (it != commands.end())
                        {
                            bool result = it.value()(tokens, package, package.path, lineNum);
                            if (!result)
                            {
                                CLogMessage(this).warning("Ignoring CSL package %1") << package.name;
                                break;
                            }
                        }
                        else
                        {
                            CLogMessage(this).warning("Unrecognized command %1 in %2") << tokens[0] << package.name;
                            break;
                        }
                    }
                }
            }

        } // namespace
    } // namespace
} // namespace
