/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "copyconfigurationcomponent.h"
#include "blackconfig/buildconfig.h"
#include "blackmisc/directoryutils.h"
#include "ui_copyconfigurationcomponent.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/datacache.h"

#include <QDirIterator>
#include <QFileInfoList>
#include <QFileSystemModel>

using namespace BlackMisc;
using namespace BlackConfig;

namespace BlackGui
{
    namespace Components
    {
        CCopyConfigurationComponent::CCopyConfigurationComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CCopyConfigurationComponent)
        {
            ui->setupUi(this);
            ui->cb_OtherVersions->clear();
            ui->cb_OtherVersions->addItems(CDirectoryUtils::swiftApplicationDataDirectoryList(true, true));
            m_versionDirs = CDirectoryUtils::swiftApplicationDataDirectoryList(true, false); // not beautified

            this->initCurrentDirectories();
            this->preselectMissingOurOutdated();

            connect(ui->rb_Cache, &QRadioButton::toggled, this, &CCopyConfigurationComponent::initCurrentDirectories);
            connect(ui->cb_OtherVersions, &QComboBox::currentTextChanged, this, &CCopyConfigurationComponent::initCurrentDirectories);
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

        void CCopyConfigurationComponent::copySelectedFiles()
        {
            const QStringList files = this->getSelectedFiles();
            if (files.isEmpty()) { return; }
        }

        void CCopyConfigurationComponent::preselectMissingOurOutdated()
        {
            const QString dirOther = this->getOtherVersionsSelectedDirectory();
            const QString dirCurrent = this->getThisVersionDirectory();

            ui->tv_Destination->clearSelection();
            const CDirectoryUtils::DirComparison comp = CDirectoryUtils::compareTwoDirectories(dirOther, dirCurrent);
            const QFileSystemModel *model = qobject_cast<QFileSystemModel *>(ui->tv_Destination->model());

            QStringList select = comp.missingInTarget.toList();
            select.append(comp.newerInSource.toList());
            for (const QString &file : as_const(comp.missingInTarget))
            {
                ui->tv_Destination->setCurrentIndex(model->index(file));
            }
        }

        void CCopyConfigurationComponent::initCurrentDirectories()
        {
            ui->le_CurrentVersion->setText(CDirectoryUtils::applicationDirectoryPath());
            this->setComboBoxWidth();
            const QString dir = this->getOtherVersionsSelectedDirectory();
            if (dir.isEmpty()) { return; }

            // source
            QFileSystemModel *sourceModel = qobject_cast<QFileSystemModel *>(ui->tv_Source->model());
            if (!sourceModel)
            {
                sourceModel = new QFileSystemModel(this);
                sourceModel->setFilter(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
                sourceModel->setNameFilterDisables(false);
                sourceModel->setNameFilters(QStringList("*.json"));
                ui->tv_Source->setModel(sourceModel);
                connect(sourceModel, &QFileSystemModel::directoryLoaded, this, [ = ](const QString & path)
                {
                    Q_UNUSED(path);
                    ui->tv_Source->resizeColumnToContents(0);
                });
            }
            const QModelIndex sourceIndex = sourceModel->setRootPath(dir);
            ui->tv_Source->setRootIndex(sourceIndex);
            ui->tv_Source->setSortingEnabled(true);

            // destination
            QFileSystemModel *destinationModel = qobject_cast<QFileSystemModel *>(ui->tv_Destination->model());
            if (!destinationModel)
            {
                destinationModel = new QFileSystemModel(this);
                destinationModel->setFilter(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
                destinationModel->setNameFilterDisables(false);
                destinationModel->setNameFilters(QStringList("*.json"));
                ui->tv_Destination->setModel(destinationModel);
                connect(destinationModel, &QFileSystemModel::directoryLoaded, this, [ = ](const QString & path)
                {
                    Q_UNUSED(path);
                    ui->tv_Destination->resizeColumnToContents(0);
                });
            }
            const QString destinationDir = this->getThisVersionDirectory();
            const QModelIndex destinationIndex = destinationModel->setRootPath(destinationDir);
            ui->tv_Destination->setRootIndex(destinationIndex);
            ui->tv_Destination->setSortingEnabled(true);
            ui->tv_Destination->resizeColumnToContents(0);
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
            const QString s = m_versionDirs.at(ui->cb_OtherVersions->currentIndex());
            const QFileInfoList dirs(CDirectoryUtils::swiftApplicationDataDirectories());
            if (dirs.isEmpty()) { return ""; }
            QString dir;
            for (const QFileInfo &info : dirs)
            {
                if (info.absoluteFilePath().contains(s))
                {
                    dir = info.absoluteFilePath();
                    break;
                }
            }
            if (dir.isEmpty()) { return ""; }
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

        void CCopyConfigurationComponent::setComboBoxWidth()
        {
            const int width = this->width() * 0.45;
            ui->cb_OtherVersions->setFixedWidth(width);
        }

        void CCopyConfigurationWizardPage::initializePage()
        {
            Q_ASSERT_X(m_config, Q_FUNC_INFO, "Missing config");
            m_config->initCurrentDirectories();
        }

        bool CCopyConfigurationWizardPage::validatePage()
        {
            Q_ASSERT_X(m_config, Q_FUNC_INFO, "Missing config");
            m_config->copySelectedFiles();
            return true;
        }
    } // ns
} // ns
