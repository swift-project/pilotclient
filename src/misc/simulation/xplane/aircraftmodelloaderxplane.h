// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_XPLANE_AIRCRAFTMODELLOADERXPLANE_H
#define SWIFT_MISC_SIMULATION_XPLANE_AIRCRAFTMODELLOADERXPLANE_H

#include <QObject>
#include <QPointer>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QtGlobal>

#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/aircraftmodelloader.h"
#include "misc/simulation/simulatorinfo.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    class CWorker;

    namespace simulation::xplane
    {
        /*!
         * XPlane aircraft model loader
         */
        class SWIFT_MISC_EXPORT CAircraftModelLoaderXPlane : public IAircraftModelLoader
        {
            Q_OBJECT

        public:
            //! Constructor
            CAircraftModelLoaderXPlane(QObject *parent = nullptr);

            //! Virtual destructor
            virtual ~CAircraftModelLoaderXPlane() override;

            //! \name Interface functions
            //! @{
            virtual bool isLoadingFinished() const override;
            //! @}

            //! Parsed or injected models
            void updateInstalledModels(const CAircraftModelList &models);

        protected:
            //! \name Interface functions
            //! @{
            virtual void startLoadingFromDisk(LoadMode mode, const ModelConsolidationCallback &modelConsolidation,
                                              const QStringList &modelDirectories) override;
            //! @}

        private:
            //! CSL Plane data
            struct CSLPlane
            {
                enum ObjectVersion
                {
                    OBJ7,
                    OBJ8
                };

                QString getModelName() const;

                // Model name parts
                QStringList dirNames; //!< List dir names starting from xsb_aircrafts.txt parent down to obj folder
                QString objectName; //!< Complete basename of the object file
                QString textureName; //!< Complete basename of the texture file. Can be empty.

                QString filePath; //!< object filePath
                QString icao; //!< Icao type of this model
                QString airline; //!< Airline identifier. Can be empty.
                QString livery; //!< Livery identifier. Can be empty.

                ObjectVersion objectVersion;

                bool hasErrors = false;
            };

            //! CSL package
            struct CSLPackage
            {
                bool hasValidHeader() const { return !name.isEmpty() && !path.isEmpty(); }

                QString name;
                QString path;
                QVector<CSLPlane> planes;
            };

            CAircraftModelList performParsing(const QStringList &rootDirectories,
                                              const QStringList &excludeDirectories);
            CAircraftModelList parseFlyableAirplanes(const QString &rootDirectory,
                                                     const QStringList &excludeDirectories);
            CAircraftModelList parseCslPackages(const QString &rootDirectory, const QStringList &excludeDirectories);

            bool doPackageSub(QString &ioPath);

            bool parseExportCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum);
            bool parseDependencyCommand(const QStringList &tokens, CSLPackage &package, const QString &path,
                                        int lineNum);
            bool parseObjectCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum);
            bool parseTextureCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum);
            bool parseAircraftCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum);
            bool parseObj8AircraftCommand(const QStringList &tokens, CSLPackage &package, const QString &path,
                                          int lineNum);
            bool parseObj8Command(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum);
            bool parseHasGearCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum);
            bool parseIcaoCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum);
            bool parseAirlineCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum);
            bool parseLiveryCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum);
            bool parseDummyCommand(const QStringList &tokens, CSLPackage &package, const QString &path, int lineNum);

            CSLPackage parsePackageHeader(const QString &path, const QString &content);
            void parseFullPackage(const QString &content, CSLPackage &package);

            void addUniqueModel(const CAircraftModel &model, CAircraftModelList &models);

            QPointer<CWorker> m_parserWorker; //!< worker will destroy itself, so weak pointer
            QVector<CSLPackage>
                m_cslPackages; //!< Parsed Packages. No lock required since accessed only from one thread

            static const QString &fileFilterFlyable();
            static const QString &fileFilterCsl();
        };
    } // namespace simulation::xplane
} // namespace swift::misc

#endif // guard
