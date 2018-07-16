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
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/aircraftmodelloader.h"
#include "blackmisc/simulation/simulatorinfo.h"

#include <QObject>
#include <QPointer>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QtGlobal>

namespace BlackMisc
{
    class CWorker;

    namespace Simulation
    {
        namespace XPlane
        {
            /*!
             * XPlane aircraft model loader
             */
            class BLACKMISC_EXPORT CAircraftModelLoaderXPlane : public IAircraftModelLoader
            {
                Q_OBJECT

            public:
                //! Constructor
                CAircraftModelLoaderXPlane();

                //! Virtual destructor
                virtual ~CAircraftModelLoaderXPlane();

                //! \name Interface functions
                //! @{
                virtual bool isLoadingFinished() const override;
                //! @}

                //! Extract from an acf file (flyable plane) the properties needed to generate model string.
                static BlackMisc::Simulation::CAircraftModel extractAcfProperties(const QString &filePath, const QFileInfo &fileInfo);

            public slots:
                //! Parsed or injected models
                void updateInstalledModels(const BlackMisc::Simulation::CAircraftModelList &models);

            protected:
                //! \name Interface functions
                //! @{
                virtual void startLoadingFromDisk(LoadMode mode, const ModelConsolidationCallback &modelConsolidation, const QStringList &modelDirectories) override;
                //! @}

            private:
                struct CSLPlane
                {
                    QString getModelName() const;

                    // Model name parts
                    QStringList dirNames; //!< List dir names starting from xsb_aircrafts.txt parent down to obj folder
                    QString objectName;   //!< Complete basename of the object file
                    QString textureName;  //!< Complete basename of the texture file. Can be empty.

                    QString filePath; //!< object filePath
                    QString icao;     //!< Icao type of this model
                    QString airline;  //!< Airline identifier. Can be empty.
                    QString livery;   //!< Livery identifier. Can be empty.
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

                QPointer<BlackMisc::CWorker> m_parserWorker;    //!< worker will destroy itself, so weak pointer
                QVector<CSLPackage> m_cslPackages;              //!< Parsed Packages. No lock required since accessed only from one thread

                static const QString &fileFilterFlyable();
                static const QString &fileFilterCsl();
            };
        } // namespace
    } // namespace
} // namespace

#endif // guard
