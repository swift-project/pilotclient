/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "vpilotrulesreader.h"

#include <QtXml/QDomElement>
#include <QFile>
#include <QDir>
#include <QStandardPaths>

using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Simulation
    {
        namespace FsCommon
        {
            CVPilotRulesReader::CVPilotRulesReader(bool standardDirectory, QObject *parent) :
                QObject(parent)
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
                QStringList nameFilters({"*.vmr"});
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
                QReadLocker l(&m_lockData);
                return m_models.size();
            }

            CAircraftModelList CVPilotRulesReader::getAsModels() const
            {
                // already cached?
                {
                    QReadLocker l(&m_lockData);
                    if (!m_models.isEmpty() || m_rules.isEmpty()) { return m_models; }
                    if (m_shutdown) { return CAircraftModelList(); }
                }

                // important: that can take a while and should normally
                // run in background
                CVPilotModelRuleSet rules(getRules()); // thread safe copy
                CAircraftModelList models(rules.toAircraftModels()); // long lasting operation
                QWriteLocker l(&m_lockData);
                m_models = models;
                return m_models;
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
                static QString directory;
                if (directory.isEmpty())
                {
                    directory = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first();
                    if (!directory.endsWith('/')) { directory.append('/'); }
                    directory.append("vPilot Files/Model Matching Rule Sets");
                }
                return directory;
            }

            bool CVPilotRulesReader::read(bool convertToModels)
            {
                bool success = true;
                int loadedFiles = 0;
                QStringList filesWithProblems;
                CVPilotModelRuleSet rules;
                QStringList fileList(getFiles());

                for (const QString &fn : fileList)
                {
                    if (m_shutdown) { return false; }
                    loadedFiles++;
                    bool s = this->loadFile(fn, rules);
                    if (!s) { this->m_fileListWithProblems.append(fn); }
                    success = s && success;
                }

                {
                    QWriteLocker l(&m_lockData);
                    this->m_loadedFiles = loadedFiles;
                    this->m_fileListWithProblems = filesWithProblems;
                    this->m_rules = rules;
                    if (convertToModels)
                    {
                        if (m_shutdown) { return false; }
                        this->m_models = rules.toAircraftModels(); // long lasting operation
                    }
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
                BlackMisc::CWorker *worker = BlackMisc::CWorker::fromTask(this, "CVPilotRulesReader", [this, convertToModels]()
                {
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

            bool CVPilotRulesReader::loadFile(const QString &fileName, CVPilotModelRuleSet &ruleSet)
            {
                QFile f(fileName);
                if (!f.exists()) { return  false; }
                if (!f.open(QFile::ReadOnly | QFile::Text)) { return false; }
                QByteArray fc = f.readAll();
                if (fc.isEmpty()) { return false; }
                QDomDocument doc;
                if (!doc.setContent(fc)) { return false; }
                QDomNodeList rules = doc.elementsByTagName("ModelMatchRule");
                if (rules.isEmpty()) { return false; }

                QDomNodeList mmRuleSet = doc.elementsByTagName("ModelMatchRuleSet");
                if (mmRuleSet.size() < 1) { return true; }

                QDomNamedNodeMap attributes = mmRuleSet.at(0).attributes();
                QString folder = attributes.namedItem("Folder").nodeValue().trimmed();
                if (folder.isEmpty())
                {
                    folder = QFileInfo(fileName).fileName().replace(".vmr", "");
                }

                // "2/1/2014 12:00:00 AM", "5/26/2014 2:00:00 PM"
                QString updated = attributes.namedItem("UpdatedOn").nodeValue();
                QDateTime qt = QDateTime::fromString(updated, "M/d/yyyy h:mm:ss AP");
                qint64 updatedTimestamp = qt.toMSecsSinceEpoch();

                int rulesSize = rules.size();
                for (int i = 0; i < rulesSize; i++)
                {
                    QDomNamedNodeMap attributes = rules.at(i).attributes();
                    const QString typeCode = attributes.namedItem("TypeCode").nodeValue();
                    const QString modelName = attributes.namedItem("ModelName").nodeValue();
                    // remark, callsign prefix is airline ICAO code
                    const QString callsignPrefix = attributes.namedItem("CallsignPrefix").nodeValue();
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
    } // namespace
} // namespace
