#include "vpilotmodelmappings.h"
#include "blackmisc/nwaircraftmapping.h"

#include <QtXml/QDomElement>
#include <QFile>
#include <QDir>
#include <QStandardPaths>

using namespace BlackMisc::Network;

namespace BlackSim
{

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
        foreach(QFileInfo file, entries)
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

    bool CVPilotModelMappings::load()
    {
        bool success = true;
        foreach(QString fn, this->m_fileList)
        {
            bool s = this->loadFile(fn);
            success = s && success;
        }
        return success;
    }

    bool CVPilotModelMappings::loadFile(const QString &fileName)
    {
        QFile f(fileName);
        if (!f.exists()) return false;
        if (!f.open(QFile::ReadOnly | QFile::Text)) return false;
        QByteArray fc = f.readAll();
        if (fc.isEmpty()) return false;
        QDomDocument doc;
        if (!doc.setContent(fc)) return false;
        QDomNodeList rules = doc.elementsByTagName("ModelMatchRule");
        if (rules.isEmpty()) return false;
        int size = rules.size();
        for (int i = 0; i < size; i++)
        {
            QDomNamedNodeMap attributes = rules.at(i).attributes();
            const QString typeCode = attributes.namedItem("TypeCode").nodeValue();
            const QString modelName = attributes.namedItem("ModelName").nodeValue();
            const QString callsignPrefix = attributes.namedItem("CallsignPrefix").nodeValue();
            if (modelName.isEmpty()) continue;

            if (modelName.contains("//"))
            {
                // multiple models
                QStringList models = modelName.split("//");
                foreach(QString model, models)
                {
                    if (model.isEmpty()) continue;
                    CAircraftMapping mapping(typeCode, callsignPrefix, model);
                    this->m_mappings.push_back(mapping);
                }
            }
            else
            {
                // single model
                CAircraftMapping mapping(typeCode, callsignPrefix, modelName);
                this->m_mappings.push_back(mapping);
            }
        }
        return true;
    }
} // namespace
