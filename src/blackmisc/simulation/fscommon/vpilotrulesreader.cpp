/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "vpilotrulesreader.h"
#include "blackmisc/network/aircraftmapping.h"

#include <QtXml/QDomElement>
#include <QFile>
#include <QDir>
#include <QStandardPaths>

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

            void CVPilotRulesReader::addFilename(const QString &fileName)
            {
                if (this->m_fileList.contains(fileName)) return;
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

            int CVPilotRulesReader::countRulesLoaded() const
            {
                return m_rules.size();
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

            bool CVPilotRulesReader::read()
            {
                bool success = true;
                this->m_loadedFiles = 0;
                this->m_fileListWithProblems.clear();
                for (const QString &fn : this->m_fileList)
                {
                    this->m_loadedFiles++;
                    bool s = this->loadFile(fn);
                    if (!s) { this->m_fileListWithProblems.append(fn); }
                    success = s && success;
                }
                emit readFinished(success);
                return success;
            }

            bool CVPilotRulesReader::loadFile(const QString &fileName)
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
                QString updated = attributes.namedItem("UpdatedOn").nodeValue();
                QDateTime qt = QDateTime::fromString(updated);
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
                            this->m_rules.push_back(rule);
                        }
                    }
                    else
                    {
                        // single model
                        CVPilotModelRule rule(modelName, folder, typeCode, callsignPrefix, updatedTimestamp);
                        this->m_rules.push_back(rule);
                    }
                }
                return true;
            }

        } // namespace
    } // namespace
} // namespace
