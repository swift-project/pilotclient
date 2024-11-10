// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_LEGALINFOCOMPONENT_H
#define BLACKGUI_COMPONENTS_LEGALINFOCOMPONENT_H

#include "blackgui/overlaymessagesframe.h"
#include "blackcore/application/applicationsettings.h"
#include <QFrame>
#include <QScopedPointer>
#include <QWizardPage>

namespace Ui
{
    class CLegalInfoComponent;
}
namespace BlackGui::Components
{
    /*!
     * swift legal information
     */
    class CLegalInfoComponent : public COverlayMessagesFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CLegalInfoComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CLegalInfoComponent();

        //! Agreed with?
        bool isAgreedTo() const;

        //! Validate that the agreement is accepted
        bool validateAgreement();

    private:
        //! Allow crash dumps
        void onAllowCrashDumps(bool checked);

        //! Set the checklist info
        void setChecklistInfo();

        QScopedPointer<Ui::CLegalInfoComponent> ui;
        swift::misc::CSetting<BlackCore::Application::TCrashDumpUploadEnabled> m_crashDumpUploadEnabled { this };
    };

    /*!
     * Wizard page for CLegalInfoComponent
     */
    class CLegalInfoWizardPage : public QWizardPage
    {
        Q_OBJECT

    public:
        //! Constructors
        using QWizardPage::QWizardPage;

        //! Set config
        void setConfigComponent(CLegalInfoComponent *config) { m_legalInfo = config; }

        //! \copydoc QWizardPage::validatePage
        virtual bool validatePage() override;

    private:
        CLegalInfoComponent *m_legalInfo = nullptr;
    };
} // ns

#endif // guard
