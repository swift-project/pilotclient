// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "legalinfocomponent.h"
#include "ui_legalinfocomponent.h"

#include "blackgui/guiapplication.h"
#include "blackcore/data/globalsetup.h"
#include "misc/network/url.h"
#include "misc/logmessage.h"
#include "misc/statusmessage.h"
#include "config/buildconfig.h"

#include <QPointer>

using namespace swift::misc;
using namespace swift::misc::network;
using namespace BlackCore::Data;
using namespace swift::config;

namespace BlackGui::Components
{
    CLegalInfoComponent::CLegalInfoComponent(QWidget *parent) : COverlayMessagesFrame(parent),
                                                                ui(new Ui::CLegalInfoComponent)
    {
        ui->setupUi(this);
        this->setChecklistInfo();

        const bool crashDumpUploadEnabled = m_crashDumpUploadEnabled.getThreadLocal();
        ui->cb_CrashDumps->setChecked(crashDumpUploadEnabled);
        ui->cb_Agree->setChecked(CBuildConfig::isLocalDeveloperDebugBuild());

        connect(ui->cb_CrashDumps, &QCheckBox::toggled, this, &CLegalInfoComponent::onAllowCrashDumps);
    }

    CLegalInfoComponent::~CLegalInfoComponent()
    {}

    bool CLegalInfoComponent::isAgreedTo() const
    {
        return ui->cb_Agree->isChecked();
    }

    bool CLegalInfoComponent::validateAgreement()
    {
        if (this->isAgreedTo()) { return true; }
        static const CStatusMessage m = CStatusMessage(this).validationError(u"You need to agree with the swift license");
        this->showOverlayMessage(m);
        return false;
    }

    void CLegalInfoComponent::onAllowCrashDumps(bool checked)
    {
        CLogMessage::preformatted(m_crashDumpUploadEnabled.setAndSave(checked));
    }

    void CLegalInfoComponent::setChecklistInfo()
    {
        if (!sGui) { return; }
        const CGlobalSetup gs = sGui->getGlobalSetup();
        const CUrl url = gs.getHelpPageUrl("checklist");
        ui->lbl_Tip->setText(QStringLiteral("Please read the <a href=\"%1\">checklist before your 1st flight</a>").arg(url.getFullUrl()));
        ui->lbl_Tip->setTextFormat(Qt::RichText);
        ui->lbl_Tip->setTextInteractionFlags(Qt::TextBrowserInteraction);
        ui->lbl_Tip->setOpenExternalLinks(true);
    }

    bool CLegalInfoWizardPage::validatePage()
    {
        return m_legalInfo && m_legalInfo->validateAgreement();
    }
} // ns
