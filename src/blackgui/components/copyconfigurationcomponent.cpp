/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "ui_copyconfigurationcomponent.h"
#include "copyconfigurationcomponent.h"
#include "configurationwizard.h"
#include "blackcore/data/globalsetup.h"
#include "blackgui/guiapplication.h"
#include "blackconfig/buildconfig.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/datacache.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/slot.h"

#include <QDirIterator>
#include <QFileInfoList>
#include <QFileSystemModel>
#include <QDir>
#include <QDesktopServices>
#include <QPointer>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Data;
using namespace BlackConfig;
using namespace BlackGui;
using namespace BlackCore;
using namespace BlackCore::Data;

namespace BlackGui
{
    namespace Components
    {
        const QStringList &CCopyConfigurationComponent::getLogCategories()
        {
            static const QStringList cats { CLogCategories::guiComponent() };
            return cats;
        }

        CCopyConfigurationComponent::CCopyConfigurationComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CCopyConfigurationComponent)
        {
            ui->setupUi(this);
            this->initOtherSwiftVersions();
            this->setWidths();
            m_hasOtherSwiftVersions = CApplicationInfoList::hasOtherSwiftDataDirectories();

            ui->cb_ShowAll->setChecked(m_nameFilterDisables);
            connect(ui->rb_Cache, &QRadioButton::toggled, [ = ](bool) { this->initCurrentDirectories(true); });
            connect(ui->cb_OtherVersions, &QComboBox::currentTextChanged, [ = ] { this->initCurrentDirectories(true); });
            connect(ui->pb_SelectAll, &QPushButton::clicked, ui->tv_Source, &QTreeView::selectAll);
            connect(ui->pb_ClearSelection, &QPushButton::clicked, ui->tv_Source, &QTreeView::clearSelection);
            connect(ui->pb_CopyOver, &QPushButton::clicked, this, &CCopyConfigurationComponent::copySelectedFiles);
            connect(ui->cb_ShowAll, &QCheckBox::released, this, &CCopyConfigurationComponent::changeNameFilterDisables);
            connect(ui->tb_OpenOtherVersionsDir, &QToolButton::clicked, this, &CCopyConfigurationComponent::openOtherVersionsSelectedDirectory);
            connect(ui->tb_OpenThisVersionDir, &QToolButton::clicked, this, &CCopyConfigurationComponent::openOtherVersionsSelectedDirectory);

            // create default caches with timestamps on disk
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
            this->initCaches(files);

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
                    CLogMessage(this).info(u"Copied %1 files, list: '%2'") << copied.size() << copied.join(", ");
                }
                if (!skipped.isEmpty())
                {
                    CLogMessage(this).info(u"Skipped %1 files, list: '%2'") << skipped.size() << skipped.join(", ");
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

            QStringList select = comp.missingInTarget.values();
            select.append(comp.newerInSource.values());
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
                static const QStringList cacheFilter = [ = ]
                {
                    QStringList cf({
                        m_modelSetCurrentSimulator.getFilename(),
                        m_modelsCurrentSimulator.getFilename(),
                        m_launcherSetup.getFilename(),
                        m_vatsimSetup.getFilename(),
                        m_lastVatsimServer.getFilename(),
                        m_lastServer.getFilename(),
                        m_lastAircraftModel.getFilename()
                    });
                    cf.append(m_modelSetCaches.getAllFilenames());
                    cf.append(m_modelCaches.getAllFilenames());
                    return CFileUtils::getFileNamesOnly(cf);
                }();
                if (!m_withBootstrapFile) { return cacheFilter; }

                static const QStringList cacheFilterBs = [ = ]
                {
                    QStringList f(cacheFilter);
                    f.push_back(CSwiftDirectories::bootstrapFileName());
                    return f;
                }();
                return cacheFilterBs;
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

            QFileSystemModel *destinationModel = qobject_cast<QFileSystemModel *>(ui->tv_Destination->model());
            QFileSystemModel *sourceModel = qobject_cast<QFileSystemModel *>(ui->tv_Source->model());

            if (!destinationModel || m_initializedDestinationDir != destinationDir)
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
                if (!destinationModel)
                {
                    destinationModel = new QFileSystemModel(this);
                    destinationModel->setFilter(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
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
            destinationModel->setNameFilters(this->getSourceFileFilter());
            destinationModel->setNameFilterDisables(m_nameFilterDisables);

            // source
            const QString sourceDir = this->getOtherVersionsSelectedDirectory();
            if (!m_hasOtherSwiftVersions)
            {
                // no ther versions
                return;
            }
            else if (!sourceModel || m_initializedSourceDir != sourceDir)
            {
                m_initializedSourceDir = sourceDir;
                if (!sourceModel)
                {
                    sourceModel = new QFileSystemModel(this);
                    sourceModel->setFilter(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
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
            sourceModel->setNameFilters(this->getSourceFileFilter());
            sourceModel->setNameFilterDisables(m_nameFilterDisables);
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

        void CCopyConfigurationComponent::setNameFilterDisables(bool disable)
        {
            if (m_nameFilterDisables == disable) { return; }
            m_nameFilterDisables = disable;
            this->initCurrentDirectories(true);
        }

        void CCopyConfigurationComponent::resizeEvent(QResizeEvent *event)
        {
            this->setWidths();
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
            if (ui->cb_OtherVersions->count() < 1) { return {}; }
            const QFileInfoList dirs(CSwiftDirectories::applicationDataDirectories());
            if (dirs.isEmpty()) { return {}; }
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
            if (dir.isEmpty()) { return {}; }
            dir = CFileUtils::appendFilePaths(dir, ui->rb_Cache->isChecked() ?
                                              CDataCache::relativeFilePath() :
                                              CSettingsCache::relativeFilePath());
            return dir;
        }

        void CCopyConfigurationComponent::openOtherVersionsSelectedDirectory()
        {
            const QObject *s = sender();
            const QString d = (s == ui->tb_OpenOtherVersionsDir) ?
                              this->getOtherVersionsSelectedDirectory() :
                              this->getThisVersionDirectory();
            if (d.isEmpty()) { return; }
            QDir dir(d);
            if (!dir.exists()) { return; }
            const QUrl url = QUrl::fromLocalFile(dir.path());
            QDesktopServices::openUrl(url);
        }

        QStringList CCopyConfigurationComponent::getSelectedFiles() const
        {
            if (!m_hasOtherSwiftVersions) { return QStringList(); }
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

        void CCopyConfigurationComponent::initCaches(const QStringList &files)
        {
            if (files.isEmpty()) { return; }
            if (!ui->rb_Cache->isChecked()) { return; }
            for (const QString &file : files)
            {
                if (file.contains("modelset", Qt::CaseInsensitive))
                {
                    this->initMultiSimulatorCache(&m_modelSetCaches, file);
                }
                else if (file.contains("modelcache", Qt::CaseInsensitive))
                {
                    this->initMultiSimulatorCache(&m_modelCaches, file);
                }
                else if (file.contains(CSwiftDirectories::bootstrapFileName()))
                {
                    CData<TGlobalSetup> setup { this };   //!< data cache setup
                    const CGlobalSetup s = CGlobalSetup::fromJsonFile(file, true);
                    setup.set(s);
                }
            }

            // allow the cache files to be generated before we will override them
            CGuiApplication::processEventsFor(2500);
        }

        void CCopyConfigurationComponent::initMultiSimulatorCache(IMultiSimulatorModelCaches *cache, const QString &fileName)
        {
            const CSimulatorInfo info = cache->getSimulatorForFilename(fileName);
            if (info.isNoSimulator()) { return; }
            if (cache->isSaved(info)) { return; } // already a file and hence in .rev
            const QFileInfo fi(fileName);
            const CStatusMessage msg = cache->setCacheTimestamp(fi.lastModified(), info); // create cache file and timestamp in .rev
            if (msg.isFailure())
            {
                CLogMessage(this).preformatted(msg);
            }
        }

        void CCopyConfigurationComponent::initOtherSwiftVersions()
        {
            ui->cb_OtherVersions->clear();
            const QMap<QString, CApplicationInfo> otherVersions = CApplicationInfoList::currentApplicationDataDirectoryMapWithoutCurrentVersion();
            for (const auto [dir, info] : makePairsRange(otherVersions))
            {
                if (info.isNull())
                {
                    const QString infoString = CDirectoryUtils::decodeNormalizedDirectory(dir);
                    ui->cb_OtherVersions->addItem(infoString);
                }
                else
                {
                    ui->cb_OtherVersions->addItem(QStringLiteral("swift %1 (%2)").arg(info.getVersionString(), info.getPlatform()));
                }
                m_otherVersionDirs.push_back(dir);
            }
        }

        void CCopyConfigurationComponent::changeNameFilterDisables()
        {
            this->setNameFilterDisables(ui->cb_ShowAll->isChecked());
        }

        void CCopyConfigurationComponent::setWidths()
        {
            const int w = this->width();
            const int wCb = qRound(0.45 * w);
            const int wView = qRound(0.4 * w);
            ui->cb_OtherVersions->setMaximumWidth(wCb);
            ui->tv_Destination->setMinimumWidth(wView);
            ui->tv_Source->setMinimumWidth(wView);
        }

        const QStringList &CCopyConfigurationWizardPage::getLogCategories()
        {
            static const QStringList cats { CLogCategories::wizard(), CLogCategories::guiComponent() };
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
