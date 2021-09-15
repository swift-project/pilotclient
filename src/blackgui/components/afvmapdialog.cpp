/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/guiapplication.h"
#include "blackcore/context/contextaudioimpl.h"
#include "blackcore/afv/clients/afvclient.h"
#include "blackcore/afv/model/afvmapreader.h"

#include "afvmapdialog.h"
#include "ui_afvmapdialog.h"
// #include <QQmlContext>
// #include <QQmlEngine>

using namespace BlackCore::Afv::Model;
using namespace BlackCore::Afv::Clients;

namespace BlackGui::Components
{
    CAfvMapDialog::CAfvMapDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::CAfvMapDialog)
    {
        /**
        m_afvMapReader = new CAfvMapReader(this);
        m_afvMapReader->updateFromMap();

        CAfvClient *afvClient = nullptr;
        if (sGui && !sGui->isShuttingDown() && sGui->getIContextAudio())
        {
            if (sGui->getIContextAudio()->isUsingImplementingObject())
            {
                afvClient = sGui->getCoreFacade()->getCContextAudio()->voiceClient();
            }
        }

        ui->setupUi(this);
        QQmlContext *ctxt = ui->qw_AfvMap->rootContext();
        ctxt->setContextProperty("afvMapReader", m_afvMapReader);

        if (m_afvClient)
        {
            ctxt->setContextProperty("voiceClient", afvClient);
        }

        // ui->qw_AfvMap->engine()->setBaseUrl(":/blackgui/qml");
        ui->qw_AfvMap->setSource(QUrl("qrc:/blackgui/qml/AFVMap.qml"));
        **/
    }

    CAfvMapDialog::~CAfvMapDialog() { }
} // ns
