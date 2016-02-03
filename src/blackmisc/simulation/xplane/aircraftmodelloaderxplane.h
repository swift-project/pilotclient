/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_XPLANE_AIRCRAFTMODELLOADERXPLANE_H
#define BLACKMISC_SIMULATION_XPLANE_AIRCRAFTMODELLOADERXPLANE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/worker.h"
#include "blackmisc/pixmap.h"
#include "blackmisc/simulation/aircraftmodelloader.h"
#include "blackmisc/simulation/aircraftmodel.h"

#include <QPointer>

namespace BlackMisc
{
    namespace Simulation
    {
        namespace XPlane
        {
            /*!
             * \brief XPlane aircraft model loader
             * \todo Obj8Aircraft and Obj8 parsers are not yet implemented
             */
            class BLACKMISC_EXPORT CAircraftModelLoaderXPlane : public BlackMisc::Simulation::IAircraftModelLoader
            {
                Q_OBJECT

            public:
                //! Constructor
                CAircraftModelLoaderXPlane();

                //! Constructor
                CAircraftModelLoaderXPlane(const BlackMisc::Simulation::CSimulatorInfo &simInfo, const QString &rootDirectory, const QStringList &exludes = {});

                //! Virtual destructor
                virtual ~CAircraftModelLoaderXPlane();

                //! Change the directory
                bool changeRootDirectory(const QString &directory);

                //! Current root directory
                QString getRootDirectory() const { return this->m_rootDirectory; }

                //! \copydoc IAircraftModelLoader::iconForModel
                virtual BlackMisc::CPixmap iconForModel(const QString &modelName, BlackMisc::CStatusMessage &statusMessage) const override;

                //! \copydoc IAircraftModelLoader::startLoading
                virtual void startLoading(LoadMode mode = ModeBackground) override;

                //! \copydoc IAircraftModelLoader::isLoadingFinished
                virtual bool isLoadingFinished() const override;

                //! \copydoc IAircraftModelLoader::getAircraftModels
                virtual BlackMisc::Simulation::CAircraftModelList getAircraftModels() const override;

            public slots:
                //! Parsed or injected models
                void updateInstalledModels(const BlackMisc::Simulation::CAircraftModelList &models);

            private:
                struct CSLPlane
                {
                    QString modelName;  //!< Unique model name
                    QString filePath;   //!< object filePath
                    QString icao;       //!< Icao type of this model
                    QString airline;    //!< Airline identifier. Can be empty.
                    QString livery;     //!< Livery identifier. Can be empty.
                };

                struct CSLPackage
                {
                    bool hasValidHeader() const
                    {
                        return !name.isEmpty() && !path.isEmpty();
                    }

                    QString name;
                    QString path;
                    QVector<CSLPlane> planes;
                };

                BlackMisc::Simulation::CAircraftModelList performParsing(const QString &rootDirectory, const QStringList &excludeDirectories);
                BlackMisc::Simulation::CAircraftModelList parseFlyableAirplanes(const QString &rootDirectory, const QStringList &excludeDirectories);
                BlackMisc::Simulation::CAircraftModelList parseCslPackages(const QString &rootDirectory, const QStringList &excludeDirectories);

                //! Does the directory exist?
                bool existsDir(const QString &directory = QString()) const;
                bool doPackageSub(QString &ioPath);

                bool parseExportCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum);
                bool parseDependencyCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum);
                bool parseObjectCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum);
                bool parseTextureCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum);
                bool parseAircraftCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum);
                bool parseObj8AircraftCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum);
                bool parseObj8Command(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum);
                bool parseHasGearCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum);
                bool parseIcaoCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum);
                bool parseAirlineCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum);
                bool parseLiveryCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum);
                bool parseDummyCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum);

                CSLPackage parsePackageHeader(const QString &path, const QString &content);
                void parseFullPackage(const QString &content, CSLPackage &package);

                QString m_rootDirectory;                        //!< root directory parsing aircraft.cfg files
                QStringList m_excludedDirectories;              //!< directories not to be parsed
                QPointer<BlackMisc::CWorker> m_parserWorker;    //!< worker will destroy itself, so weak pointer
                QVector<CSLPackage> m_cslPackages;              //!< Parsed Packages. No lock required since accessed only from one thread
                BlackMisc::Simulation::CAircraftModelList m_installedModels;
            };
        } // namespace
    } // namespace
} // namespace

#endif // guard
