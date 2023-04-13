/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/logmessage.h"
#include "blackmisc/simulation/fscommon/vpilotmodelrule.h"
#include "blackmisc/simulation/fscommon/vpilotrulesreader.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/worker.h"
#include "blackmisc/stringutils.h"

#include <QByteArray>
#include <QDateTime>
#include <QDir>
#include <QDomDocument>
#include <QDomNamedNodeMap>
#include <QDomNode>
#include <QDomNodeList>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QFlags>
#include <QIODevice>
#include <QReadLocker>
#include <QStandardPaths>
#include <QTimer>
#include <QWriteLocker>
#include <QtGlobal>

using namespace BlackMisc;

namespace BlackMisc::Simulation::FsCommon
{
    CVPilotRulesReader::CVPilotRulesReader(bool standardDirectory, QObject *parent) : QObject(parent)
    {
        if (standardDirectory) { this->addDirectory(CVPilotRulesReader::standardMappingsDirectory()); }
    }

    CVPilotRulesReader::~CVPilotRulesReader()
    {
        gracefulShutdown();
    }

    QStringList CVPilotRulesReader::getFiles() const
    {
        QReadLocker l(&m_lockData);
        return m_fileList;
    }

    bool CVPilotRulesReader::hasFiles() const
    {
        QReadLocker l(&m_lockData);
        return !m_fileList.isEmpty();
    }

    void CVPilotRulesReader::addFilename(const QString &fileName)
    {
        QWriteLocker l(&m_lockData);
        if (this->m_fileList.contains(fileName)) { return; }
        this->m_fileList.append(fileName);
    }

    void CVPilotRulesReader::addDirectory(const QString &directory)
    {
        QDir dir(directory);
        if (!dir.exists()) { return; }
        QStringList nameFilters({ "*.vmr" });
        QFileInfoList entries = dir.entryInfoList(nameFilters, QDir::Files | QDir::Readable);
        for (const QFileInfo &file : entries)
        {
            this->addFilename(file.absoluteFilePath());
        }
    }

    int CVPilotRulesReader::countFilesLoaded() const
    {
        QReadLocker l(&m_lockData);
        return m_loadedFiles;
    }

    CVPilotModelRuleSet CVPilotRulesReader::getRules() const
    {
        QReadLocker l(&m_lockData);
        return m_rules;
    }

    int CVPilotRulesReader::getModelsCount() const
    {
        return this->m_cachedVPilotModels.getThreadLocal().size();
    }

    CAircraftModelList CVPilotRulesReader::getAsModels()
    {
        // already cached?
        CAircraftModelList vPilotModels(this->m_cachedVPilotModels.get());
        if (!vPilotModels.isEmpty() || m_rules.isEmpty()) { return vPilotModels; }

        // important: that can take a while and should normally
        // run in background
        const CVPilotModelRuleSet rules(getRules()); // thread safe copy
        vPilotModels = rules.toAircraftModels(); // long lasting operation
        this->ps_setCache(vPilotModels);
        return vPilotModels;
    }

    CAircraftModelList CVPilotRulesReader::getAsModelsFromCache() const
    {
        return this->m_cachedVPilotModels.get();
    }

    int CVPilotRulesReader::countRulesLoaded() const
    {
        QReadLocker l(&m_lockData);
        return m_rules.size();
    }

    void CVPilotRulesReader::gracefulShutdown()
    {
        QWriteLocker l(&m_lockData);
        m_shutdown = true;
    }

    const QString &CVPilotRulesReader::standardMappingsDirectory()
    {
        //! \fixme not threadsafe
        static QString directory;
        if (directory.isEmpty())
        {
            directory = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).constFirst();
            if (!directory.endsWith('/')) { directory.append('/'); }
            directory.append("vPilot Files/Model Matching Rule Sets");
        }
        return directory;
    }

    bool CVPilotRulesReader::read(bool convertToModels)
    {
        int loadedFiles = 0;
        QStringList filesWithProblems;
        CVPilotModelRuleSet rules;
        const QStringList fileList(getFiles());
        for (const QString &fn : fileList)
        {
            if (m_shutdown) { return false; }
            loadedFiles++;
            bool s = this->loadFile(fn, rules);
            if (!s) { this->m_fileListWithProblems.append(fn); }
        }

        {
            QWriteLocker l(&m_lockData);
            this->m_loadedFiles = loadedFiles;
            this->m_fileListWithProblems = filesWithProblems;
            this->m_rules = rules;
            if (m_shutdown) { return false; }
        }

        const bool success = loadedFiles > 0;
        if (convertToModels)
        {
            const CAircraftModelList vPilotModels(rules.toAircraftModels()); // long lasting operation
            this->ps_setCache(vPilotModels);
        }

        emit readFinished(success);
        return success;
    }

    CWorker *CVPilotRulesReader::readInBackground(bool convertToModels)
    {
        // set a thread safe flag
        {
            QWriteLocker l(&m_lockData);
            if (m_asyncLoadInProgress || m_shutdown) { return nullptr; }
            m_asyncLoadInProgress = true;
        }
        BlackMisc::CWorker *worker = BlackMisc::CWorker::fromTask(this, "CVPilotRulesReader", [this, convertToModels]() {
            this->read(convertToModels);
        });
        worker->then(this, &CVPilotRulesReader::ps_readInBackgroundFinished);
        return worker;
    }

    void CVPilotRulesReader::ps_readInBackgroundFinished()
    {
        QWriteLocker l(&m_lockData);
        m_asyncLoadInProgress = false;
    }

    void CVPilotRulesReader::ps_onVPilotCacheChanged()
    {
        // void
    }

    void CVPilotRulesReader::ps_setCache(const CAircraftModelList &models)
    {
        if (this->m_cachedVPilotModels.isOwnerThread())
        {
            CStatusMessage m;
            {
                QWriteLocker l(&m_lockData);
                m = this->m_cachedVPilotModels.set(models);
            }
            if (m.isFailure())
            {
                CLogMessage::preformatted(m);
            }
        }
        else
        {
            QTimer::singleShot(0, this, [this, models]() { this->ps_setCache(models); });
        }
    }

    bool CVPilotRulesReader::loadFile(const QString &fileName, CVPilotModelRuleSet &ruleSet)
    {
        QFile f(fileName);
        if (!f.exists()) { return false; }
        if (!f.open(QFile::ReadOnly | QFile::Text)) { return false; }
        QByteArray fc = f.readAll();
        if (fc.isEmpty()) { return false; }
        QDomDocument doc;
        if (!doc.setContent(fc)) { return false; }
        QDomNodeList rules = doc.elementsByTagName("ModelMatchRule");
        if (rules.isEmpty()) { return false; }

        QDomNodeList mmRuleSet = doc.elementsByTagName("ModelMatchRuleSet");
        if (mmRuleSet.size() < 1) { return true; }

        const QDomNamedNodeMap attributes = mmRuleSet.at(0).attributes();
        QString folder = attributes.namedItem("Folder").nodeValue().trimmed();
        if (folder.isEmpty())
        {
            folder = QFileInfo(fileName).fileName().replace(".vmr", "");
        }

        // "2/1/2014 12:00:00 AM", "5/26/2014 2:00:00 PM"
        const QString updated = attributes.namedItem("UpdatedOn").nodeValue();
        QDateTime qt = fromStringUtc(updated, "M/d/yyyy h:mm:ss AP");
        qint64 updatedTimestamp = qt.toMSecsSinceEpoch();

        int rulesSize = rules.size();
        for (int i = 0; i < rulesSize; i++)
        {
            const QDomNamedNodeMap ruleAttributes = rules.at(i).attributes();
            const QString typeCode = ruleAttributes.namedItem("TypeCode").nodeValue();
            const QString modelName = ruleAttributes.namedItem("ModelName").nodeValue();
            // remark, callsign prefix is airline ICAO code
            const QString callsignPrefix = ruleAttributes.namedItem("CallsignPrefix").nodeValue();
            if (modelName.isEmpty()) { continue; }

            // split if we have multiple models
            if (modelName.contains("//"))
            {
                // multiple models
                const QStringList models = modelName.split("//");
                for (const QString &model : models)
                {
                    if (model.isEmpty()) { continue; }
                    CVPilotModelRule rule(model, folder, typeCode, callsignPrefix, updatedTimestamp);
                    ruleSet.push_back(rule);
                }
            }
            else
            {
                // single model
                CVPilotModelRule rule(modelName, folder, typeCode, callsignPrefix, updatedTimestamp);
                ruleSet.push_back(rule);
            }
        }
        return true;
    }

} // namespace
