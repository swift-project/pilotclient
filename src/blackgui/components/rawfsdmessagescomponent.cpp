/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "ui_rawfsdmessagescomponent.h"
#include "blackgui/components/rawfsdmessagescomponent.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextnetwork.h"
#include "blackmisc/directoryutils.h"

#include <QFileDialog>
#include <QDir>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackCore;
using namespace BlackCore::Vatsim;

namespace BlackGui
{
    namespace Components
    {
        CRawFsdMessagesComponent::CRawFsdMessagesComponent(QWidget *parent) :
            QFrame(parent), ui(new Ui::CRawFsdMessagesComponent)
        {
            ui->setupUi(this);
            ui->cb_FileWritingMode->addItem(QApplication::translate("CRawFsdMessagesComponent", "Truncate", nullptr), QVariant::fromValue(CRawFsdMessageSettings::Truncate));
            ui->cb_FileWritingMode->addItem(QApplication::translate("CRawFsdMessagesComponent", "Append", nullptr), QVariant::fromValue(CRawFsdMessageSettings::Append));
            ui->cb_FileWritingMode->addItem(QApplication::translate("CRawFsdMessagesComponent", "Timestamped", nullptr), QVariant::fromValue(CRawFsdMessageSettings::Timestamped));

            QMetaObject::Connection c = sGui->getIContextNetwork()->connectRawFsdMessageSignal(this, std::bind(&CRawFsdMessagesComponent::addFsdMessage, this, std::placeholders::_1));
            if (!c)
            {
                ui->cb_EnableFileWriting->setEnabled(false);
                ui->lw_RawFsdMessages->addItem(QStringLiteral("Could not connect to raw FSD message."));
                ui->lw_RawFsdMessages->addItem(QStringLiteral("This is most likely because core is not running in this process."));
                ui->lw_RawFsdMessages->addItem(QStringLiteral("Open this component again from the process running core."));
            }
            else
            {
                m_signalConnections.append(c);
                readSettings();
                connect(ui->cb_EnableFileWriting, &QCheckBox::toggled, this, &CRawFsdMessagesComponent::changeWritingToFile);
                connect(ui->pb_SelectFileDir, &QPushButton::clicked, this, &CRawFsdMessagesComponent::selectFileDir);
                connect(ui->cb_FileWritingMode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CRawFsdMessagesComponent::changeFileWritingMode);
            }
        }

        CRawFsdMessagesComponent::~CRawFsdMessagesComponent()
        { }

        void CRawFsdMessagesComponent::changeWritingToFile(bool enable)
        {
            ui->le_FileDir->setEnabled(enable);
            ui->pb_SelectFileDir->setEnabled(enable);
            ui->cb_FileWritingMode->setEnabled(enable);
            m_setting.setProperty(Vatsim::CRawFsdMessageSettings::IndexWriteEnabled, CVariant::fromValue(enable));
        }

        void CRawFsdMessagesComponent::selectFileDir()
        {
            QString fileDir = ui->le_FileDir->text();
            fileDir = QFileDialog::getExistingDirectory(this, tr("Select File Directory"), fileDir);
            if (fileDir.isEmpty()) { return; }
            ui->le_FileDir->setText(fileDir);
            m_setting.setProperty(Vatsim::CRawFsdMessageSettings::IndexFileDir, CVariant::fromValue(fileDir));
        }

        void CRawFsdMessagesComponent::changeFileWritingMode()
        {
            CRawFsdMessageSettings::FileWriteMode mode = ui->cb_FileWritingMode->currentData().value<CRawFsdMessageSettings::FileWriteMode>();
            m_setting.setProperty(Vatsim::CRawFsdMessageSettings::IndexFileWriteMode, CVariant::fromValue(mode));
        }

        void CRawFsdMessagesComponent::setFileWritingModeFromSettings(CRawFsdMessageSettings::FileWriteMode mode)
        {
            ui->cb_FileWritingMode->setCurrentIndex(static_cast<int>(mode));
        }

        void CRawFsdMessagesComponent::addFsdMessage(const CRawFsdMessage &rawFsdMessage)
        {
            ui->lw_RawFsdMessages->addItem(rawFsdMessage.toQString());
            ui->lw_RawFsdMessages->scrollToBottom();

            while (ui->lw_RawFsdMessages->count() > 100)
            {
                QListWidgetItem *item = ui->lw_RawFsdMessages->takeItem(0);
                delete item;
            }
        }

        void CRawFsdMessagesComponent::readSettings()
        {
            const Vatsim::CRawFsdMessageSettings setting = m_setting.get();
            ui->le_FileDir->setText(setting.getFileDir());
            const bool enable = setting.isFileWritingEnabled();
            ui->cb_EnableFileWriting->setChecked(enable);
            ui->le_FileDir->setEnabled(enable);
            ui->pb_SelectFileDir->setEnabled(enable);
            ui->cb_FileWritingMode->setEnabled(enable);
            const CRawFsdMessageSettings::FileWriteMode mode = setting.getFileWriteMode();
            ui->cb_FileWritingMode->setCurrentIndex(static_cast<int>(mode));
        }
    }
} // namespace
