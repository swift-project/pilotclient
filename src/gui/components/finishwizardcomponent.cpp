// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "finishwizardcomponent.h"

#include <QPointer>

#include "ui_finishwizardcomponent.h"

#include "config/buildconfig.h"
#include "core/data/globalsetup.h"
#include "gui/guiapplication.h"
#include "misc/logmessage.h"
#include "misc/network/url.h"
#include "misc/statusmessage.h"

using namespace swift::misc;
using namespace swift::misc::network;
using namespace swift::core::data;
using namespace swift::config;

namespace swift::gui::components
{
    CFinishWizardComponent::CFinishWizardComponent(QWidget *parent)
        : COverlayMessagesFrame(parent), ui(new Ui::CFinishWizardComponent)
    {
        ui->setupUi(this);
    }

    CFinishWizardComponent::~CFinishWizardComponent() = default;

    bool CFinishWizardPage::validatePage() { return m_finishwizard; }
} // namespace swift::gui::components
