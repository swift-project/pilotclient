/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "autopublishdata.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/json.h"
#include "blackmisc/logcategories.h"

#include <QStringList>
#include <QStringBuilder>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>

using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Simulation
    {
        void CAutoPublishData::insert(const QString &modelString, const PhysicalQuantities::CLength &cg)
        {
            if (cg.isNull() || modelString.isEmpty()) { return; }
            m_modelStringVsCG.insert(modelString.toUpper(), cg);
        }

        void CAutoPublishData::insert(const QString &modelString, const CSimulatorInfo &simulator)
        {
            if (!simulator.isSingleSimulator() || modelString.isEmpty()) { return; }
            m_modelStringVsSimulatorInfo.insert(modelString.toUpper(), simulator);
        }

        void CAutoPublishData::clear()
        {
            m_modelStringVsCG.clear();
            m_modelStringVsSimulatorInfo.clear();
        }

        bool CAutoPublishData::isEmpty() const
        {
            return m_modelStringVsCG.isEmpty() && m_modelStringVsSimulatorInfo.isEmpty();
        }

        QString CAutoPublishData::toDatabaseJson() const
        {
            // used simple string JSON generation as it is faster
            QString json;

            for (const auto [string, cg] : makePairsRange(m_modelStringVsCG))
            {
                json += QStringLiteral("{ \"type\": \"cg\", \"modelstring\": \"%1\", \"cgft\": %2 },\n").arg(string, cg.valueRoundedAsString(CLengthUnit::ft(), 1));
            }

            for (const auto [string, sim] : makePairsRange(m_modelStringVsSimulatorInfo))
            {
                json += QStringLiteral("{ \"type\": \"simulatorupdate\", \"modelstring\": \"%1\", \"simulator\": \"%2\" },\n").arg(string, sim.toQString(false));
            }

            if (json.isEmpty()) { return {}; }
            json.chop(2); // remove 2 chars at end
            return u"[\n" % json % u"\n]\n";
        }

        int CAutoPublishData::fromDatabaseJson(const QString &jsonData, bool clear)
        {
            if (clear) { this->clear(); }
            if (jsonData.isEmpty()) { return 0; }
            const QJsonArray array = Json::jsonArrayFromString(jsonData);
            if (array.isEmpty()) { return 0; }

            for (const QJsonValue &value : array)
            {
                const QJsonObject obj = value.toObject();
                const QString t = obj["type"].toString();
                const QString m = obj["modelstring"].toString();
                if (m.isEmpty()) { continue; }

                if (t.startsWith("simulator", Qt::CaseInsensitive))
                {
                    const QString simulator = obj["simulator"].toString();
                    const CSimulatorInfo si(simulator);
                    if (si.isSingleSimulator())
                    {
                        this->insert(m, si);
                    }
                }
                else if (t.startsWith("cg", Qt::CaseInsensitive))
                {
                    const double cgFt = obj["cgft"].toDouble(-1);
                    if (cgFt < 0) { continue; }
                    this->insert(m, CLength(cgFt, CLengthUnit::ft()));
                }
            }

            return m_modelStringVsCG.size() + m_modelStringVsSimulatorInfo.size();
        }

        bool CAutoPublishData::writeJsonToFile() const
        {
            if (this->isEmpty()) { return false; }
            const QString fn = fileBaseName() % u'_' % QDateTime::currentDateTimeUtc().toString("yyyyMMddHHmmss") % fileAppendix();
            return this->writeJsonToFile(CFileUtils::appendFilePaths(CSwiftDirectories::logDirectory(), fn));
        }

        bool CAutoPublishData::writeJsonToFile(const QString &pathAndFile) const
        {
            if (this->isEmpty()) { return false; }
            const QString json = this->toDatabaseJson();
            return CFileUtils::writeStringToFile(json, pathAndFile);
        }

        bool CAutoPublishData::readFromJsonFile(const QString &fileAndPath, bool clear)
        {
            const QString json = CFileUtils::readFileToString(fileAndPath);
            if (json.isEmpty()) { return false; }
            this->fromDatabaseJson(json, clear);
            return true;
        }

        int CAutoPublishData::readFromJsonFiles(const QString &dirPath)
        {
            const QStringList fileList = findAndCleanupPublishFiles(dirPath);
            if (fileList.isEmpty()) { return 0; }
            this->clear();

            int c = 0;
            for (const QString &file : fileList)
            {
                // read from file
                if (this->readFromJsonFile(CFileUtils::appendFilePaths(dirPath, file), false)) { c++; }
            }
            return c;
        }

        CStatusMessageList CAutoPublishData::analyzeAgainstDBData(const CAircraftModelList &dbModels)
        {
            static const CLogCategoryList cats({ CLogCategories::mapping(), CLogCategories::webservice() });
            if (dbModels.isEmpty()) { return CStatusMessage(this).validationError(u"No DB data"); }
            if (this->isEmpty())    { return CStatusMessage(this).validationWarning(u"No data"); }

            CStatusMessageList msgs;
            msgs.push_back(CStatusMessage(cats).validationInfo(u"DB models: %1") << dbModels.size());

            QSet<QString> newModelStrings; // not in DB yet
            QSet<QString> unchangedCG;

            for (const QString &modelString : m_modelStringVsCG.keys())
            {
                const CAircraftModel dbModel = dbModels.findFirstByModelStringOrDefault(modelString);
                if (dbModel.hasValidDbKey())
                {
                    if (dbModel.getCG() == m_modelStringVsCG[modelString])
                    {
                        unchangedCG.insert(modelString);
                    }
                }
                else
                {
                    // not in DB
                    newModelStrings << modelString;
                }
            }

            QSet<QString> unchangedSim;
            for (const QString &modelString : m_modelStringVsSimulatorInfo.keys())
            {
                const CAircraftModel dbModel = dbModels.findFirstByModelStringOrDefault(modelString);
                if (dbModel.hasValidDbKey())
                {
                    if (dbModel.getSimulator().matchesAny(m_modelStringVsSimulatorInfo[modelString]))
                    {
                        unchangedSim.insert(modelString);
                    }
                }
                else
                {
                    newModelStrings << modelString;
                }
            }

            // remove
            if (!unchangedCG.isEmpty())
            {
                msgs.push_back(CStatusMessage(cats).validationInfo(u"Removing unchanged CGs: %1") << unchangedCG.size());
                for (const QString &m : unchangedCG) { m_modelStringVsCG.remove(m); }
            }

            if (!unchangedSim.isEmpty())
            {
                msgs.push_back(CStatusMessage(cats).validationInfo(u"Removing unchanged simulators: %1") << unchangedSim.size());
                for (const QString &m : unchangedSim) { m_modelStringVsSimulatorInfo.remove(m); }
            }

            msgs.push_back(CStatusMessage(this).validationInfo(this->getSummary()));
            return msgs;
        }

        QString CAutoPublishData::getSummary() const
        {
            return QStringLiteral("Changed CGs: %1 | sim.entries: %2").arg(m_modelStringVsCG.size()).arg(m_modelStringVsSimulatorInfo.size());
        }

        QSet<QString> CAutoPublishData::allModelStrings() const
        {
            QSet<QString> allStrings(m_modelStringVsCG.keyBegin(), m_modelStringVsCG.keyEnd());
            allStrings.unite(QSet<QString>(m_modelStringVsSimulatorInfo.keyBegin(), m_modelStringVsSimulatorInfo.keyEnd()));
            return allStrings;
        }

        void CAutoPublishData::testData()
        {
            this->clear();

            const CLength cg1(10, CLengthUnit::ft());
            const CLength cg2(40, CLengthUnit::ft());
            const CLength cg3(30, CLengthUnit::ft());

            this->insert("testModelString1", cg1);
            this->insert("testModelString2", cg2);
            this->insert("testModelString3", cg3);

            this->insert("testModelString1", CSimulatorInfo::fs9());
            this->insert("testModelString2", CSimulatorInfo::xplane());
            this->insert("testModelString3", CSimulatorInfo::fg());
            this->insert("testModelString4", CSimulatorInfo::p3d());
            this->insert("testModelString5", CSimulatorInfo::fsx());
            this->insert("testModelString6", CSimulatorInfo::fsx());
        }

        const QString &CAutoPublishData::fileBaseName()
        {
            static const QString fn("autopublish");
            return fn;
        }

        const QString &CAutoPublishData::fileAppendix()
        {
            static const QString a(".json");
            return a;
        }

        bool CAutoPublishData::existAutoPublishFiles(const QString &dirPath)
        {
            return findAndCleanupPublishFiles(dirPath).size() > 0;
        }

        int CAutoPublishData::deleteAutoPublishFiles(const QString &dirPath)
        {
            const QStringList fileList = findAndCleanupPublishFiles(dirPath);
            if (fileList.isEmpty()) { return 0; }

            int c = 0;
            for (const QString &file : fileList)
            {
                const QString fn = CFileUtils::appendFilePaths(dirPath, file);
                QFile f(fn);
                if (!f.exists()) { continue; }
                if (f.remove()) { c++; }
            }
            return c;

        }

        QStringList CAutoPublishData::findAndCleanupPublishFiles(const QString &dirPath)
        {
            QDir dir(dirPath);
            if (!dir.exists()) { return {}; }

            const QString filter = fileBaseName() % u'*' % fileAppendix();
            const QStringList filters({ filter });
            dir.setNameFilters(filters);
            dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
            const QStringList fileList = dir.entryList();
            if (fileList.isEmpty()) { return fileList; }

            // avoid outdated files (e.g. if format changes)
            QStringList correctedList;
            const QDateTime deadline = QDateTime::currentDateTimeUtc().addDays(-30);
            for (const QString &fn : fileList)
            {
                const QFileInfo fi(fn.contains(dir.absolutePath()) ? fn : CFileUtils::appendFilePathsAndFixUnc(dir.absolutePath(), fn));
                if (!fi.exists()) { continue; }
                const QDateTime created = fi.birthTime().toUTC();
                if (deadline < created)
                {
                    correctedList << fn;
                }
                else
                {
                    QFile deleteFile(fn);
                    deleteFile.remove();
                }
            }

            return correctedList;
        }

    } // namespace
} // namespace
