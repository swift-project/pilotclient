/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "blackcore/afv/clients/afvclient.h"
#include "blackcore/afv/model/afvmapreader.h"

#include "afvmapdialog.h"
#include "ui_afvmapdialog.h"
#include <QQmlContext>

using namespace BlackCore::Afv::Model;
using namespace BlackCore::Afv::Clients;

namespace BlackGui
{
    namespace Components
    {
        CAfvMapDialog::CAfvMapDialog(QWidget *parent) :
            QDialog(parent),
            ui(new Ui::CAfvMapDialog)
        {
            ui->setupUi(this);
            m_afvMapReader = new CAfvMapReader(this);
            m_afvMapReader->updateFromMap();
            m_afvClient = new CAfvClient("https://voice1.vatsim.uk");

            QQmlContext *ctxt = ui->qw_AfvMap->rootContext();
            ctxt->setContextProperty("afvMapReader", m_afvMapReader);
            ctxt->setContextProperty("voiceClient", m_afvClient);
        }

        CAfvMapDialog::~CAfvMapDialog() { }
    } // ns
} // ns
