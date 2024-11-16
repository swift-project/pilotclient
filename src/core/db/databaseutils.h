// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_DB_DATABASEUTILS_H
#define SWIFT_CORE_DB_DATABASEUTILS_H

#include <QByteArray>
#include <QHttpPart>
#include <QJsonArray>
#include <QJsonObject>
#include <QUrlQuery>

#include "core/progress.h"
#include "core/swiftcoreexport.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/autopublishdata.h"

namespace swift::core::db
{
    //! Read information about data from Database
    class SWIFT_CORE_EXPORT CDatabaseUtils
    {
    public:
        //! No constructor
        CDatabaseUtils() = delete;

        //! Log categories
        static const QStringList &getLogCategories();

        //! Consolidate models with simulator model data (aka "models on disk")
        //! \remark kept here with the other consolidate functions, but actually DB independent
        static swift::misc::simulation::CAircraftModelList consolidateModelsWithSimulatorModelsAllowsGuiRefresh(
            const swift::misc::simulation::CAircraftModelList &models,
            const swift::misc::simulation::CAircraftModelList &simulatorModels, QStringList &removedModelStrings,
            bool processEvents);

        //! Update directories in models with simulator model data (aka "models on disk")
        //! \remark kept here with the other consolidate functions, but actually DB independent
        static swift::misc::simulation::CAircraftModelList
        updateModelsDirectoriesAllowsGuiRefresh(const swift::misc::simulation::CAircraftModelList &models,
                                                const swift::misc::simulation::CAircraftModelList &simulatorModels,
                                                QStringList &removedModelStrings, bool processEvents);

        //! Consolidate own (aircraft) model data with DB data
        static swift::misc::simulation::CAircraftModel
        consolidateOwnAircraftModelWithDbData(const swift::misc::simulation::CAircraftModel &model, bool force,
                                              bool *modified = nullptr);

        //! Consolidate model data with DB data
        static swift::misc::simulation::CAircraftModel
        consolidateModelWithDbData(const swift::misc::simulation::CAircraftModel &model, bool force,
                                   bool *modified = nullptr);

        //! Consolidate model data with DB data
        static swift::misc::simulation::CAircraftModel
        consolidateModelWithDbData(const swift::misc::simulation::CAircraftModel &model,
                                   const swift::misc::simulation::CAircraftModel &dbModel, bool force, bool *modified);

        //! Consolidate models with DB data
        static int consolidateModelsWithDbData(swift::misc::simulation::CAircraftModelList &models, bool force);

        //! Consolidate models with simulator model data (aka "models on disk")
        //! \remark kept here with the other consolidate functions, but actually DB independent
        static int consolidateModelsWithDbData(const swift::misc::simulation::CAircraftModelList &dbModels,
                                               swift::misc::simulation::CAircraftModelList &simulatorModels,
                                               bool force);

        //! Consolidate models with DB data
        static int consolidateModelsWithDbDataAllowsGuiRefresh(swift::misc::simulation::CAircraftModelList &models,
                                                               bool force, bool processEvents);

        //! Consolidate models with DB data (simpler/faster version of CAircraftModel::consolidateModelWithDbData)
        static int consolidateModelsWithDbModelAndDistributor(swift::misc::simulation::CAircraftModelList &models,
                                                              bool force);

        //! Consolidate model data with DB distributor
        static swift::misc::simulation::CAircraftModel
        consolidateModelWithDbDistributor(const swift::misc::simulation::CAircraftModel &model, bool force);

        //! Consolidate model data with DB distributors
        static int consolidateModelsWithDbDistributor(swift::misc::simulation::CAircraftModelList &models, bool force);

        //! Fill in missing data if only the id is provided, but no data
        static int fillInMissingAircraftAndLiveryEntities(swift::misc::simulation::CAircraftModelList &models);

        //! Create stash models if the DB models miss that simulator
        static swift::misc::simulation::CAircraftModelList
        updateSimulatorForFsFamily(const swift::misc::simulation::CAircraftModelList &ownModels,
                                   swift::misc::CStatusMessageList *updateInfo, int maxToStash = -1,
                                   swift::core::IProgressIndicator *progressIndicator = nullptr,
                                   bool processEvents = true);

        //! Database JSON from content string, which can be compressed
        static QJsonDocument databaseJsonToQJsonDocument(const QString &content);

        //! QJsonDocument from database JSON file (normally shared file)
        static QJsonDocument readQJsonDocumentFromDatabaseFile(const QString &filename);

        //! QJsonObject from database JSON file (normally shared file)
        static QJsonObject readQJsonObjectFromDatabaseFile(const QString &filename);

        //! QJsonObject from database JSON file (normally shared file)
        static QJsonObject readQJsonObjectFromDatabaseFile(const QString &directory, const QString &filename);

        //! Convenience function
        static bool hasDbAircraftData();

        //! Mark as compressed
        static const QUrlQuery &getCompressedQuery();

        //! Multipart for JSON
        static QHttpPart getJsonTextMultipart(const QJsonObject &json, bool compress);

        //! Multipart for JSON
        static QHttpPart getJsonTextMultipart(const QString &json, bool compress);

        //! Multipart for JSON
        static QHttpPart getJsonTextMultipart(const QJsonArray &json, bool compress);

        //! Multipart for JSON
        static QHttpPart getJsonTextMultipart(const QByteArray &bytes, bool compress);

        //! Multipart with DEBUG FLAG for server
        static QHttpPart getMultipartWithDebugFlag();

        //! Which auto-publish data did change?
        //! \sa CAutoPublishData::analyzeAgainstDBData
        static swift::misc::simulation::ChangedAutoPublishData
        autoPublishDataChanged(const QString &modelString, const swift::misc::physical_quantities::CLength &cg,
                               const swift::misc::simulation::CSimulatorInfo &simulator);

        //! Which auto-publish data did change?
        //! \sa CAutoPublishData::analyzeAgainstDBData
        static swift::misc::simulation::ChangedAutoPublishData
        autoPublishDataChanged(const swift::misc::simulation::CAircraftModel &model,
                               const swift::misc::physical_quantities::CLength &cg,
                               const swift::misc::simulation::CSimulatorInfo &simulator);
    };
} // namespace swift::core::db
#endif // guard
