/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "vpilotmodelmappings.h"
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

            CVPilotModelMappings::CVPilotModelMappings(bool standardDirectory, QObject *parent) :
                ISimulatorModelMappings(parent)
            {
                if (standardDirectory) { this->addDirectory(CVPilotModelMappings::standardMappingsDirectory()); }
            }

            void CVPilotModelMappings::addFilename(const QString &fileName)
            {
                if (this->m_fileList.contains(fileName)) return;
                this->m_fileList.append(fileName);
            }

            void CVPilotModelMappings::addDirectory(const QString &directory)
            {
                QDir dir(directory);
                if (!dir.exists()) return;
                QStringList nameFilters({"*.vmr"});
                QFileInfoList entries = dir.entryInfoList(nameFilters, QDir::Files | QDir::Readable);
                for (const QFileInfo &file : entries)
                {
                    this->addFilename(file.absoluteFilePath());
                }
            }

            const QString &CVPilotModelMappings::standardMappingsDirectory()
            {
                static QString directory;
                if (directory.isEmpty())
                {
                    directory = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first();
                    if (!directory.endsWith('/')) directory.append('/');
                    directory.append("vPilot Files/Model Matching Rule Sets");
                }
                return directory;
            }

            bool CVPilotModelMappings::read()
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
                return success;
            }

            bool CVPilotModelMappings::loadFile(const QString &fileName)
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

                QString folder;
                QString updated;
                QDomNodeList mmRuleSet = doc.elementsByTagName("ModelMatchRuleSet");
                if (mmRuleSet.size() > 0)
                {
                    QDomNamedNodeMap attributes = mmRuleSet.at(0).attributes();
                    folder = attributes.namedItem("Folder").nodeValue();
                    updated = attributes.namedItem("UpdatedOn").nodeValue();
                }
                int rulesSize = rules.size();
                for (int i = 0; i < rulesSize; i++)
                {
                    QDomNamedNodeMap attributes = rules.at(i).attributes();
                    const QString typeCode = attributes.namedItem("TypeCode").nodeValue();
                    const QString modelName = attributes.namedItem("ModelName").nodeValue();
                    // remark, callsign prefix is airline ICAO code
                    const QString airlineCode = attributes.namedItem("CallsignPrefix").nodeValue();
                    if (modelName.isEmpty()) { continue; }

                    // split if we have multiple models
                    if (modelName.contains("//"))
                    {
                        // multiple models
                        const QStringList models = modelName.split("//");
                        for (const QString &model : models)
                        {
                            if (model.isEmpty()) { continue; }
                            CAircraftMapping mapping("vpilot", folder, typeCode, airlineCode, model);
                            this->m_mappings.push_back(mapping);
                        }
                    }
                    else
                    {
                        // single model
                        CAircraftMapping mapping("vpilot", folder, typeCode, airlineCode, modelName);
                        this->m_mappings.push_back(mapping);
                    }
                }
                return true;
            }

        } // namespace
    } // namespace
} // namespace
