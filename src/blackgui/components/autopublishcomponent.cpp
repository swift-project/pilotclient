/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "autopublishcomponent.h"
#include "ui_autopublishcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/simulation/aircraftmodellist.h"

#include <QPushButton>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;

namespace BlackGui
{
    namespace Components
    {
        CAutoPublishComponent::CAutoPublishComponent(QWidget *parent) :
            COverlayMessagesFrame(parent),
            ui(new Ui::CAutoPublishComponent)
        {
            ui->setupUi(this);
            connect(ui->pb_Analyze,     &QPushButton::released, this, &CAutoPublishComponent::analyzeAgainstDBData, Qt::QueuedConnection);
            connect(ui->pb_SendToDB,    &QPushButton::released, this, &CAutoPublishComponent::sendToDb, Qt::QueuedConnection);
            connect(ui->pb_DeleteFiles, &QPushButton::released, this, &CAutoPublishComponent::deleteAllFiles, Qt::QueuedConnection);
        }

        CAutoPublishComponent::~CAutoPublishComponent()
        { }

        int CAutoPublishComponent::readFiles()
        {
            const int r = m_data.readFromJsonFiles();
            this->displayData();
            return r;
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
            this->readFiles()   ;
        }

    } // ns
} // ns
