/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "ui_copyconfigurationcomponent.h"
#include "copyconfigurationcomponent.h"
#include "configurationwizard.h"
#include "blackgui/guiapplication.h"
#include "blackconfig/buildconfig.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/datacache.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/slot.h"

#include <QDirIterator>
#include <QFileInfoList>
#include <QFileSystemModel>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Data;
using namespace BlackConfig;
using namespace BlackGui;

namespace BlackGui
{
    namespace Components
    {
        const CLogCategoryList &CCopyConfigurationComponent::getLogCategories()
        {
            static const BlackMisc::CLogCategoryList cats { CLogCategory::guiComponent() };
            return cats;
        }

        CCopyConfigurationComponent::CCopyConfigurationComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CCopyConfigurationComponent)
        {
            ui->setupUi(this);
            this->initOtherSwiftVersions();

            connect(ui->rb_Cache, &QRadioButton::toggled, [ = ](bool) { this->initCurrentDirectories(true); });
            connect(ui->cb_OtherVersions, &QComboBox::currentTextChanged, [ = ] { this->initCurrentDirectories(true); });
            connect(ui->pb_SelectAll, &QPushButton::clicked, ui->tv_Source, &QTreeView::selectAll);
            connect(ui->pb_ClearSelection, &QPushButton::clicked, ui->tv_Source, &QTreeView::clearSelection);
            connect(ui->pb_CopyOver, &QPushButton::clicked, this, &CCopyConfigurationComponent::copySelectedFiles);

            // create default caches with timestamps
            // possible for small caches, but not the large model sets (too slow)
            m_modelSetCurrentSimulator.synchronize();
            m_modelSetCurrentSimulator.set(m_modelSetCurrentSimulator.get());
            m_modelsCurrentSimulator.synchronize();
            m_modelsCurrentSimulator.set(m_modelsCurrentSimulator.get());
            m_launcherSetup.synchronize();
            m_launcherSetup.set(m_launcherSetup.get());
            m_vatsimSetup.synchronize();
            m_vatsimSetup.set(m_vatsimSetup.get());
        }

        CCopyConfigurationComponent::~CCopyConfigurationComponent()
        { }

        void CCopyConfigurationComponent::setCacheMode()
        {
            ui->rb_Cache->setChecked(true);
        }

        void CCopyConfigurationComponent::setSettingsMode()
        {
            ui->rb_Settings->setChecked(true);
        }

        int CCopyConfigurationComponent::copySelectedFiles()
        {
            const QStringList files = this->getSelectedFiles();
            if (files.isEmpty()) { return 0; }

            const QString destinationDir = this->getThisVersionDirectory();
            const QString sourceDir = this->getOtherVersionsSelectedDirectory();
            if (destinationDir.isEmpty()) { return 0; }
            const QDir source(sourceDir);
            const QDir destination(destinationDir);
            if (!destination.exists()) { return 0; }

            // init model caches if applicable (.rev file entries)
            this->initModelCaches(files);

            int c = 0;
            QStringList copied;
            QStringList skipped;
            for (const QString &file : files)
            {
                const QString relativePath = source.relativeFilePath(file);
                const QString target = CFileUtils::appendFilePaths(destinationDir, relativePath);
                if (relativePath.contains('/'))
                {
                    const QString targetDir = CFileUtils::stripFileFromPath(target);
                    const bool dirOk = destination.mkpath(targetDir);
                    if (!dirOk) { continue; }
                }
                QFile::remove(target); // copy does not overwrite
                const bool s = QFile::copy(file, target);
                if (s)
                {
                    c++;
                    copied << target;
                }
                else
                {
                    skipped << target;
                }
            }

            if (m_logCopiedFiles)
            {
                if (!copied.isEmpty())
                {
                    CLogMessage(this).info("Copied %1 files, list: '%2'") << copied.size() << copied.join(", ");
                }
                if (!skipped.isEmpty())
                {
                    CLogMessage(this).info("Skipped %1 files, list: '%2'") << skipped.size() << skipped.join(", ");
                }
            }

            // bye
            return c;
        }

        void CCopyConfigurationComponent::preselectMissingOrOutdated()
        {
            const QString dirOther = this->getOtherVersionsSelectedDirectory();
            const QString dirCurrent = this->getThisVersionDirectory();

            ui->tv_Source->clearSelection();
            ui->tv_Destination->clearSelection();

            const CDirectoryUtils::DirComparison comp = CDirectoryUtils::compareTwoDirectories(dirOther, dirCurrent, true);
            const QFileSystemModel *sourceModel = qobject_cast<QFileSystemModel *>(ui->tv_Source->model());
            if (!sourceModel) { return; }

            QStringList select = comp.missingInTarget.toList();
            select.append(comp.newerInSource.toList());
            for (const QString &file : as_const(comp.missingInTarget))
            {
                const QModelIndex index = sourceModel->index(file);
                if (!index.isValid()) continue;
                ui->tv_Source->setCurrentIndex(index);
            }
        }

        const QStringList &CCopyConfigurationComponent::getSourceFileFilter()
        {
            if (ui->rb_Cache->isChecked())
            {
                // only copy setup and model caches
                static const QStringList cacheFilter(
                {
                    "modelset*.json",
                    "modelcache*.json",
                    "*setup.json"
                });
                return cacheFilter;
            }
            else
            {
                static const QStringList settingsFilter({ "*.json" });
                return settingsFilter;
            }
        }

        void CCopyConfigurationComponent::initCurrentDirectories(bool preselectMissingOrOutdated)
        {
            const QString destinationDir = this->getThisVersionDirectory(); // cache or settings dir
            if (m_initializedDestinationDir != destinationDir)
            {
                m_initializedDestinationDir = destinationDir;
                const QDir thisVersionDirectory(destinationDir);
                if (!thisVersionDirectory.exists())
                {
                    const bool hasDir = thisVersionDirectory.mkpath(destinationDir);
                    if (!hasDir)
                    {
                        ui->le_CurrentVersion->setText("No swift target dir");
                        return;
                    }
                }
                ui->le_CurrentVersion->setText(destinationDir);

                // destination
                QFileSystemModel *destinationModel = qobject_cast<QFileSystemModel *>(ui->tv_Destination->model());
                if (!destinationModel)
                {
                    destinationModel = new QFileSystemModel(this);
                    destinationModel->setNameFilterDisables(true);
                    destinationModel->setFilter(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
                    destinationModel->setNameFilters(this->getSourceFileFilter());
                    ui->tv_Destination->setModel(destinationModel);
                    ui->tv_Destination->setSortingEnabled(true);

                    // disconnect when done, there have been problems that the lambda was called when the view was already destroyed
                    connectOnce(destinationModel, &QFileSystemModel::directoryLoaded, this, [ = ](const QString & path)
                    {
                        Q_UNUSED(path);
                        ui->tv_Destination->resizeColumnToContents(0);
                        ui->tv_Destination->expandAll();
                    });
                }
                const QModelIndex destinationIndex = destinationModel->setRootPath(destinationDir);
                ui->tv_Destination->setRootIndex(destinationIndex);
            }

            // source
            const QString sourceDir = this->getOtherVersionsSelectedDirectory();
            if (m_initializedSourceDir != sourceDir)
            {
                m_initializedSourceDir = sourceDir;
                QFileSystemModel *sourceModel = qobject_cast<QFileSystemModel *>(ui->tv_Source->model());
                if (!sourceModel)
                {
                    sourceModel = new QFileSystemModel(this);
                    sourceModel->setNameFilterDisables(true); // hide/disable only
                    sourceModel->setFilter(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
                    sourceModel->setNameFilters(this->getSourceFileFilter());
                    ui->tv_Source->setModel(sourceModel);
                    ui->tv_Source->setSortingEnabled(true); // hide/disable only
                    connectOnce(sourceModel, &QFileSystemModel::directoryLoaded, this, [ = ](const QString & path)
                    {
                        Q_UNUSED(path);
                        ui->tv_Source->resizeColumnToContents(0);
                        ui->tv_Source->expandAll();
                        if (preselectMissingOrOutdated)
                        {
                            this->preselectMissingOrOutdated();
                        }
                    });
                }
                const QModelIndex sourceIndex = sourceModel->setRootPath(sourceDir);
                ui->tv_Source->setRootIndex(sourceIndex);
            }
        }

        bool CCopyConfigurationComponent::hasOtherVersionData() const
        {
            return !m_otherVersionDirs.isEmpty();
        }

        void CCopyConfigurationComponent::allowToggleCacheSettings(bool allow)
        {
            ui->rb_Cache->setEnabled(allow);
            ui->rb_Settings->setEnabled(allow);
        }

        void CCopyConfigurationComponent::selectAll()
        {
            ui->tv_Source->selectAll();
        }

        void CCopyConfigurationComponent::resizeEvent(QResizeEvent *event)
        {
            const int w = 0.45 * this->width();
            ui->cb_OtherVersions->setMaximumWidth(w);
            QFrame::resizeEvent(event);
        }

        void CCopyConfigurationComponent::currentVersionChanged(const QString &text)
        {
            Q_UNUSED(text);
            this->initCurrentDirectories();
        }

        const QString &CCopyConfigurationComponent::getThisVersionDirectory() const
        {
            return ui->rb_Cache->isChecked() ? CDataCache::persistentStore() : CSettingsCache::persistentStore();
        }

        QString CCopyConfigurationComponent::getOtherVersionsSelectedDirectory() const
        {
            if (ui->cb_OtherVersions->count() < 1) { return QStringLiteral(""); }
            const QFileInfoList dirs(CDirectoryUtils::applicationDataDirectories());
            if (dirs.isEmpty()) { return QStringLiteral(""); }
            const QString otherVersionDir = m_otherVersionDirs.at(ui->cb_OtherVersions->currentIndex());
            QString dir;
            for (const QFileInfo &info : dirs)
            {
                if (info.absoluteFilePath().contains(otherVersionDir))
                {
                    dir = info.absoluteFilePath();
                    break;
                }
            }
            if (dir.isEmpty()) { return QStringLiteral(""); }
            dir = CFileUtils::appendFilePaths(dir, ui->rb_Cache->isChecked() ?
                                              CDataCache::relativeFilePath() :
                                              CSettingsCache::relativeFilePath());
            return dir;
        }

        QStringList CCopyConfigurationComponent::getSelectedFiles() const
        {
            const QModelIndexList indexes = ui->tv_Source->selectionModel()->selectedIndexes();
            if (indexes.isEmpty()) { return QStringList(); }
            const QFileSystemModel *sourceModel = qobject_cast<QFileSystemModel *>(ui->tv_Source->model());

            QStringList files;
            for (const QModelIndex &index : indexes)
            {
                if (!index.isValid()) continue;
                const QString file = sourceModel->filePath(index);
                if (!files.contains(file))
                {
                    files.push_back(file);
                }
            }
            return files;
        }

        void CCopyConfigurationComponent::initModelCaches(const QStringList &files)
        {
            if (files.isEmpty()) { return; }
            if (!ui->rb_Cache->isChecked()) { return; }
            for (const QString &file : files)
            {
                CStatusMessage msg;
                IMultiSimulatorModelCaches *cache = nullptr;
                if (file.contains("modelset", Qt::CaseInsensitive))
                {
                    cache = &m_modelSetCaches;
                }
                else if (file.contains("modelcache", Qt::CaseInsensitive))
                {
                    cache = &m_modelCaches;
                }
                else
                {
                    continue;
                }

                const CSimulatorInfo info = cache->getSimulatorForFilename(file);
                if (info.isNoSimulator()) continue;
                if (cache->isSaved(info)) continue; // already a file and hence in .rev
                const QFileInfo fi(file);
                msg = cache->setCacheTimestamp(fi.lastModified(), info); // create cache file and timestamp in .rev
                if (msg.isFailure())
                {
                    CLogMessage(this).preformatted(msg);
                }
            }

            // allow the cache files to be generated before we will override them
            CGuiApplication::processEventsFor(2500);
        }

        void CCopyConfigurationComponent::initOtherSwiftVersions()
        {
            ui->cb_OtherVersions->clear();
            const QMap<QString, CApplicationInfo> otherVersions = CDirectoryUtils::applicationDataDirectoryMap(true);
            for (const QString &directory : otherVersions.keys())
            {
                const CApplicationInfo info(otherVersions.value(directory));
                if (info.isNull())
                {
                    ui->cb_OtherVersions->addItem(CDirectoryUtils::decodeNormalizedDirectory(directory));
                }
                else
                {
                    static const QString item("swift %1 (%2)");
                    ui->cb_OtherVersions->addItem(item.arg(info.getVersionString(), info.getPlatform()));
                }
                m_otherVersionDirs.push_back(directory);
            }
        }

        const CLogCategoryList &CCopyConfigurationWizardPage::getLogCategories()
        {
            static const BlackMisc::CLogCategoryList cats { CLogCategory::wizard(), CLogCategory::guiComponent() };
            return cats;
        }

        void CCopyConfigurationWizardPage::initializePage()
        {
            Q_ASSERT_X(m_config, Q_FUNC_INFO, "Missing config");
            const QString name = m_config->objectName().toLower();
            if (name.contains("setting", Qt::CaseInsensitive))
            {
                m_config->setSettingsMode();
            }
            else
            {
                m_config->setCacheMode();
            }
            m_config->allowToggleCacheSettings(false);
            m_config->initCurrentDirectories(true);
        }

        bool CCopyConfigurationWizardPage::validatePage()
        {
            if (CConfigurationWizard::lastWizardStepSkipped(this->wizard())) { return true; }
            m_config->copySelectedFiles();
            return true;
        }
    } // ns
} // ns
