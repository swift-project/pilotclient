// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "autopublishcomponent.h"
#include "ui_autopublishcomponent.h"
#include "gui/guiapplication.h"
#include "gui/guiutility.h"

#include "core/webdataservices.h"
#include "core/db/databasewriter.h"
#include "misc/simulation/aircraftmodellist.h"

#include <QDateTime>
#include <QPushButton>
#include <QPointer>
#include <QTimer>
#include <QDialog>

using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::core::db;

namespace swift::gui::components
{
    CAutoPublishComponent::CAutoPublishComponent(QWidget *parent) : COverlayMessagesFrame(parent),
                                                                    ui(new Ui::CAutoPublishComponent)
    {
        ui->setupUi(this);
        connect(ui->pb_Analyze, &QPushButton::released, this, &CAutoPublishComponent::analyzeAgainstDBData, Qt::QueuedConnection);
        connect(ui->pb_SendToDB, &QPushButton::released, this, &CAutoPublishComponent::sendToDb, Qt::QueuedConnection);
        connect(ui->pb_DeleteFiles, &QPushButton::released, this, &CAutoPublishComponent::deleteAllFiles, Qt::QueuedConnection);

        if (sGui && sGui->hasWebDataServices() && sGui->getWebDataServices()->getDatabaseWriter())
        {
            CDatabaseWriter *w = sGui->getWebDataServices()->getDatabaseWriter();
            connect(w, &CDatabaseWriter::autoPublished, this, &CAutoPublishComponent::onAutoPublished, Qt::QueuedConnection);
        }
    }

    CAutoPublishComponent::~CAutoPublishComponent()
    {}

    int CAutoPublishComponent::readFiles()
    {
        const int r = m_data.readFromJsonFiles();
        this->displayData();
        return r;
    }

    bool CAutoPublishComponent::isEmpty() const
    {
        return m_data.isEmpty();
    }

    void CAutoPublishComponent::analyzeAgainstDBData()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        const CAircraftModelList dbModels = sGui->getWebDataServices()->getModels();
        const CStatusMessageList msgs = m_data.analyzeAgainstDBData(dbModels);
        this->showOverlayMessages(msgs);
    }

    void CAutoPublishComponent::sendToDb()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        if (m_data.isEmpty())
        {
            this->showOverlayHTMLMessage("No data!", 5000);
            return;
        }

        if (!sGui->hasWebDataServices())
        {
            this->showOverlayHTMLMessage("No publishing web service!", 5000);
            return;
        }

        const CAircraftModelList dbModels = sGui->getWebDataServices()->getModels();
        CStatusMessageList msgs = m_data.analyzeAgainstDBData(dbModels);
        if (!msgs.hasErrorMessages())
        {
            const CStatusMessageList publishMsgs = sGui->getWebDataServices()->asyncAutoPublish(m_data);
            msgs.push_back(publishMsgs);
        }
        this->showOverlayMessages(msgs);
    }

    void CAutoPublishComponent::displayData()
    {
        ui->pte_Json->setPlainText(m_data.toDatabaseJson());
        ui->le_Summary->setText(m_data.getSummary());
    }

    void CAutoPublishComponent::deleteAllFiles()
    {
        const int c = CAutoPublishData::deleteAutoPublishFiles();
        if (c > 0)
        {
            this->showOverlayHTMLMessage(QStringLiteral("Deleted %1 file(s)").arg(c));
        }
        this->readFiles();
    }

    void CAutoPublishComponent::onAutoPublished(bool success, const QString &url, const CStatusMessageList &msgs)
    {
        Q_UNUSED(url)
        Q_UNUSED(success)

        if (success)
        {
            QPointer<CAutoPublishComponent> myself(this);
            this->showOverlayMessagesWithConfirmation(msgs, true, "Clean up auto publish files?", [=] {
                if (!myself) { return; }
                const int timeoutMs = 5000;
                myself->deleteAllFiles();
                myself->showOverlayHTMLMessage("Cleaned auto publish files after uploading them to DB", timeoutMs);

                QTimer::singleShot(timeoutMs * 1.2, this, [=] {
                    if (!myself) { return; }
                    myself->closeParentDialog();
                });
            });
            m_lastAutoPublish.set(QDateTime::currentMSecsSinceEpoch());
        }
        else
        {
            this->showOverlayMessages(msgs, true);
        }
    }

    void CAutoPublishComponent::closeParentDialog()
    {
        QDialog *d = CGuiUtility::findParentDialog(this);
        if (d) { d->close(); }
    }

} // ns
