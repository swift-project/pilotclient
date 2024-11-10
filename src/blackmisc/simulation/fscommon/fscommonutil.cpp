// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/simulation/fscommon/fscommonutil.h"
#include "blackmisc/simulation/fscommon/fsdirectories.h"
#include "blackmisc/simulation/fscommon/aircraftcfgparser.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/logmessage.h"
#include "config/buildconfig.h"

#include <QDir>
#include <QList>
#include <QPair>
#include <QFileInfo>
#include <QSettings>
#include <QStringList>
#include <QVariant>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDomDocument>
#include <QDomNodeList>
#include <QSettings>
#include <QStringBuilder>

using namespace swift::config;

namespace BlackMisc::Simulation::FsCommon
{
    const QStringList &CFsCommonUtil::getLogCategories()
    {
        static const QStringList cats({ CLogCategories::validation(), CLogCategories::driver() });
        return cats;
    }

    bool CFsCommonUtil::adjustFileDirectory(CAircraftModel &model, const QString &simObjectsDirectory)
    {
        if (model.hasExistingCorrespondingFile()) { return true; }
        if (simObjectsDirectory.isEmpty()) { return false; }
        if (!model.hasFileName()) { return false; } // we can do nothing here

        const QString simObjectsDirectoryFix = CFileUtils::fixWindowsUncPath(simObjectsDirectory);
        const QDir dir(simObjectsDirectoryFix);
        if (!dir.exists()) { return false; }

        const QString lastSegment = u'/' % CFileUtils::lastPathSegment(simObjectsDirectoryFix) % u'/';
        const int index = model.getFileName().lastIndexOf(lastSegment);
        if (index < 0) { return false; }
        const QString relPart = model.getFileName().mid(index + lastSegment.length());
        if (relPart.isEmpty()) { return false; }
        const QString newFile = CFileUtils::appendFilePathsAndFixUnc(simObjectsDirectory, relPart);
        const QFileInfo nf(newFile);
        if (!nf.exists()) { return false; }

        model.setFileName(newFile);
        return true;
    }

    bool CFsCommonUtil::adjustFileDirectory(CAircraftModel &model, const QStringList &simObjectsDirectories)
    {
        for (const QString &simObjectDir : simObjectsDirectories)
        {
            if (CFsCommonUtil::adjustFileDirectory(model, simObjectDir)) { return true; }
        }
        return false;
    }

    int CFsCommonUtil::copyFsxTerrainProbeFiles(const QString &simObjectDir, CStatusMessageList &messages)
    {
        messages.clear();
        if (!CDirectoryUtils::existsUnemptyDirectory(CSwiftDirectories::shareTerrainProbeDirectory()))
        {
            messages.push_back(CStatusMessage(static_cast<CFsCommonUtil *>(nullptr), CStatusMessage::SeverityError, u"No terrain probe source files in '%1'") << CSwiftDirectories::shareTerrainProbeDirectory());
            return -1;
        }

        if (simObjectDir.isEmpty())
        {
            messages.push_back(CStatusMessage(static_cast<CFsCommonUtil *>(nullptr), CStatusMessage::SeverityError, u"No simObject directory"));
            return -1;
        }

        QString targetDir = CFileUtils::appendFilePathsAndFixUnc(simObjectDir, "Misc");
        const QDir td(targetDir);
        if (!td.exists())
        {
            messages.push_back(CStatusMessage(static_cast<CFsCommonUtil *>(nullptr), CStatusMessage::SeverityError, u"Cannot access target directory '%1'") << targetDir);
            return -1;
        }

        const QString lastSegment = CFileUtils::lastPathSegment(CSwiftDirectories::shareTerrainProbeDirectory());
        targetDir = CFileUtils::appendFilePathsAndFixUnc(targetDir, lastSegment);
        const bool hasDir = td.mkpath(targetDir);
        if (!hasDir)
        {
            messages.push_back(CStatusMessage(static_cast<CFsCommonUtil *>(nullptr), CStatusMessage::SeverityError, u"Cannot create target directory '%1'") << targetDir);
            return -1;
        }

        const int copied = CDirectoryUtils::copyDirectoryRecursively(CSwiftDirectories::shareTerrainProbeDirectory(), targetDir, true);
        messages.push_back(CStatusMessage(static_cast<CFsCommonUtil *>(nullptr), CStatusMessage::SeverityInfo, u"Copied %1 files from '%2' to '%3'") << copied << CSwiftDirectories::shareTerrainProbeDirectory() << targetDir);
        return copied;
    }

    CStatusMessageList CFsCommonUtil::validateAircraftConfigFiles(const CAircraftModelList &models, CAircraftModelList &validModels, CAircraftModelList &invalidModels, bool ignoreEmptyFileNames, int stopAtFailedFiles, std::atomic_bool &wasStopped)
    {
        CStatusMessage m;
        CAircraftModelList sorted(models);
        sorted.sortByFileName();
        wasStopped = false;
        CStatusMessageList msgs = sorted.validateFiles(validModels, invalidModels, ignoreEmptyFileNames, stopAtFailedFiles, wasStopped, QString(), true);
        if (wasStopped || validModels.isEmpty()) { return msgs; }

        const CAircraftModelList nonFsModels = validModels.findNonFsFamilyModels();
        if (!nonFsModels.isEmpty())
        {
            for (const CAircraftModel &model : nonFsModels)
            {
                m = CStatusMessage(static_cast<CFsCommonUtil *>(nullptr), CStatusMessage::SeverityError, QStringLiteral("Removed '%1' non FS family model").arg(model.getModelStringAndDbKey()), true);
                msgs.push_back(m);
                if (wasStopped) { break; } // allow to break from "outside"
            }

            const int d = validModels.removeIfNotFsFamily();
            m = CStatusMessage(static_cast<CFsCommonUtil *>(nullptr), CStatusMessage::SeverityError, QStringLiteral("Removed %1 non FS family models").arg(d), true);
            msgs.push_back(m);
        }

        // all those files should work
        int removedCfgEntries = 0;
        const QSet<QString> fileNames = validModels.getAllFileNames();
        for (const QString &fileName : fileNames)
        {
            bool ok = false;
            if (wasStopped) { break; } // allow to break from "outside"
            const CAircraftCfgEntriesList entries = CAircraftCfgParser::performParsingOfSingleFile(fileName, ok, msgs);
            const QSet<QString> removeModelStrings = entries.getTitleSetUpperCase();
            const CAircraftModelList removedModels = validModels.removeIfFileButNotInSet(fileName, removeModelStrings);
            for (const CAircraftModel &removedModel : removedModels)
            {
                removedCfgEntries++;
                m = CStatusMessage(static_cast<CFsCommonUtil *>(nullptr), CStatusMessage::SeverityError, QStringLiteral("'%1' removed because no longer in '%2'").arg(removedModel.getModelStringAndDbKey(), removedModel.getFileName()), true);
                msgs.push_back(m);
                CAircraftModelList::addAsValidOrInvalidModel(removedModel, false, validModels, invalidModels);
            }
        }

        if (removedCfgEntries < 1)
        {
            m = CStatusMessage(static_cast<CFsCommonUtil *>(nullptr), CStatusMessage::SeverityInfo, QStringLiteral("Not removed any models, all OK!"), true);
            msgs.push_back(m);
        }

        if (!validModels.isEmpty())
        {
            m = CStatusMessage(static_cast<CFsCommonUtil *>(nullptr), CStatusMessage::SeverityInfo, QStringLiteral("cfg validation, valid models: %1").arg(validModels.size()), true);
            msgs.push_back(m);
        }
        if (!invalidModels.isEmpty())
        {
            m = CStatusMessage(static_cast<CFsCommonUtil *>(nullptr), CStatusMessage::SeverityWarning, QStringLiteral("cfg validation, invalid models: %1").arg(invalidModels.size()), true);
            msgs.push_back(m);
        }

        // finished
        return msgs;
    }

    CStatusMessageList CFsCommonUtil::validateP3DSimObjectsPath(const CAircraftModelList &models, CAircraftModelList &validModels, CAircraftModelList &invalidModels, bool ignoreEmptyFileNames, int stopAtFailedFiles, std::atomic_bool &wasStopped, const QString &simulatorDir)
    {
        const QString simObjectsDir = simulatorDir.isEmpty() ? CFsDirectories::p3dSimObjectsDir() : CFsDirectories::p3dSimObjectsDirFromSimDir(simulatorDir);
        const QStringList simObjectPaths = CFsDirectories::p3dSimObjectsDirPlusAddOnXmlSimObjectsPaths(simObjectsDir, CFsDirectories::guessP3DVersion(simObjectsDir));
        return CFsCommonUtil::validateSimObjectsPath(QSet<QString>(simObjectPaths.begin(), simObjectPaths.end()), models, validModels, invalidModels, ignoreEmptyFileNames, stopAtFailedFiles, wasStopped);
    }

    CStatusMessageList CFsCommonUtil::validateFSXSimObjectsPath(const CAircraftModelList &models, CAircraftModelList &validModels, CAircraftModelList &invalidModels, bool ignoreEmptyFileNames, int stopAtFailedFiles, std::atomic_bool &stopped, const QString &simulatorDir)
    {
        Q_UNUSED(simulatorDir)
        const QStringList simObjectPaths = CFsDirectories::fsxSimObjectsDirPlusAddOnXmlSimObjectsPaths();
        return CFsCommonUtil::validateSimObjectsPath(QSet<QString>(simObjectPaths.begin(), simObjectPaths.end()), models, validModels, invalidModels, ignoreEmptyFileNames, stopAtFailedFiles, stopped);
    }

    CStatusMessageList CFsCommonUtil::validateSimObjectsPath(
        const QSet<QString> &simObjectDirs, const CAircraftModelList &models,
        CAircraftModelList &validModels, CAircraftModelList &invalidModels,
        bool ignoreEmptyFileNames, int stopAtFailedFiles, std::atomic_bool &stopped)
    {
        CStatusMessageList msgs;
        if (simObjectDirs.isEmpty())
        {
            msgs.push_back(CStatusMessage(static_cast<CFsCommonUtil *>(nullptr)).validationInfo(u"No SimObject directories from cfg files, skipping validation"));
            return msgs;
        }

        CAircraftModelList sortedModels(models);
        sortedModels.sortByFileName();
        if (sortedModels.isEmpty())
        {
            msgs.push_back(CStatusMessage(static_cast<CFsCommonUtil *>(nullptr)).validationInfo(u"No models to validate"));
            return msgs;
        }

        // info
        const QString simObjDirs = joinStringSet(simObjectDirs, ", ");
        msgs.push_back(CStatusMessage(static_cast<CFsCommonUtil *>(nullptr)).validationInfo(u"Validating %1 models against %2 SimObjects path(s): '%3'") << models.size() << simObjectDirs.size() << simObjDirs);

        // validate
        int failed = 0;
        for (const CAircraftModel &model : models)
        {
            if (!model.hasFileName())
            {
                if (ignoreEmptyFileNames) { continue; }
                msgs.push_back(CStatusMessage(static_cast<CFsCommonUtil *>(nullptr)).validationWarning(u"No file name for model '%1'") << model.getModelString());
                CAircraftModelList::addAsValidOrInvalidModel(model, false, validModels, invalidModels);
                continue;
            }

            bool ok = false;
            for (const QString &path : simObjectDirs)
            {
                if (!model.isInPath(path, CFileUtils::osFileNameCaseSensitivity())) { continue; }
                ok = true;
                break;
            }
            CAircraftModelList::addAsValidOrInvalidModel(model, ok, validModels, invalidModels);
            if (!ok)
            {
                msgs.push_back(CStatusMessage(static_cast<CFsCommonUtil *>(nullptr)).validationWarning(u"Model '%1' '%2' in none of the %3 SimObjects path(s)") << model.getModelString() << model.getFileName() << simObjectDirs.size());
                failed++;
            }

            if (stopAtFailedFiles > 0 && failed >= stopAtFailedFiles)
            {
                stopped = true;
                msgs.push_back(CStatusMessage(static_cast<CFsCommonUtil *>(nullptr)).validationWarning(u"Stopping after %1 failed models") << failed);
                break;
            }
        } // models

        return msgs;
    }
} // namespace
