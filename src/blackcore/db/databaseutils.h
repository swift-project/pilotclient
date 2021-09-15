/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_DB_DATABASEUTILS_H
#define BLACKCORE_DB_DATABASEUTILS_H

#include "blackcore/progress.h"
#include "blackcore/blackcoreexport.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/autopublishdata.h"

#include <QHttpPart>
#include <QUrlQuery>
#include <QByteArray>
#include <QJsonArray>
#include <QJsonObject>

namespace BlackCore::Db
{
    //! Read information about data from Database
    class BLACKCORE_EXPORT CDatabaseUtils
    {
    public:
        //! No constructor
        CDatabaseUtils() = delete;

        //! Log categories
        static const QStringList &getLogCategories();

        //! Consolidate models with simulator model data (aka "models on disk")
        //! \remark kept here with the other consolidate functions, but actually DB independent
        static BlackMisc::Simulation::CAircraftModelList consolidateModelsWithSimulatorModelsAllowsGuiRefresh(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CAircraftModelList &simulatorModels, QStringList &removedModelStrings, bool processEvents);

        //! Update directories in models with simulator model data (aka "models on disk")
        //! \remark kept here with the other consolidate functions, but actually DB independent
        static BlackMisc::Simulation::CAircraftModelList updateModelsDirectoriesAllowsGuiRefresh(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CAircraftModelList &simulatorModels, QStringList &removedModelStrings, bool processEvents);

        //! Consolidate own (aircraft) model data with DB data
        static BlackMisc::Simulation::CAircraftModel consolidateOwnAircraftModelWithDbData(const BlackMisc::Simulation::CAircraftModel &model, bool force, bool *modified = nullptr);

        //! Consolidate model data with DB data
        static BlackMisc::Simulation::CAircraftModel consolidateModelWithDbData(const BlackMisc::Simulation::CAircraftModel &model, bool force, bool *modified = nullptr);

        //! Consolidate model data with DB data
        static BlackMisc::Simulation::CAircraftModel consolidateModelWithDbData(const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::Simulation::CAircraftModel &dbModel, bool force, bool *modified);

        //! Consolidate models with DB data
        static int consolidateModelsWithDbData(BlackMisc::Simulation::CAircraftModelList &models, bool force);

        //! Consolidate models with simulator model data (aka "models on disk")
        //! \remark kept here with the other consolidate functions, but actually DB independent
        static int consolidateModelsWithDbData(const BlackMisc::Simulation::CAircraftModelList &dbModels, BlackMisc::Simulation::CAircraftModelList &simulatorModels, bool force);

        //! Consolidate models with DB data
        static int consolidateModelsWithDbDataAllowsGuiRefresh(BlackMisc::Simulation::CAircraftModelList &models, bool force, bool processEvents);

        //! Consolidate models with DB data (simpler/faster version of CAircraftModel::consolidateModelWithDbData)
        static int consolidateModelsWithDbModelAndDistributor(BlackMisc::Simulation::CAircraftModelList &models, bool force);

        //! Consolidate model data with DB distributor
        static BlackMisc::Simulation::CAircraftModel consolidateModelWithDbDistributor(const BlackMisc::Simulation::CAircraftModel &model, bool force);

        //! Consolidate model data with DB distributors
        static int consolidateModelsWithDbDistributor(BlackMisc::Simulation::CAircraftModelList &models, bool force);

        //! Fill in missing data if only the id is provided, but no data
        static int fillInMissingAircraftAndLiveryEntities(BlackMisc::Simulation::CAircraftModelList &models);

        //! Create stash models if the DB models miss that simulator
        static BlackMisc::Simulation::CAircraftModelList updateSimulatorForFsFamily(const BlackMisc::Simulation::CAircraftModelList &ownModels, BlackMisc::CStatusMessageList *updateInfo, int maxToStash = -1, BlackCore::IProgressIndicator *progressIndicator = nullptr, bool processEvents = true);

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
        static BlackMisc::Simulation::ChangedAutoPublishData autoPublishDataChanged(const QString &modelString, const BlackMisc::PhysicalQuantities::CLength &cg, const BlackMisc::Simulation::CSimulatorInfo &simulator);

        //! Which auto-publish data did change?
        //! \sa CAutoPublishData::analyzeAgainstDBData
        static BlackMisc::Simulation::ChangedAutoPublishData autoPublishDataChanged(const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::PhysicalQuantities::CLength &cg, const BlackMisc::Simulation::CSimulatorInfo &simulator);
    };
} // ns
#endif // guard
