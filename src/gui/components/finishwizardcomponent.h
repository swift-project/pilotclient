// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_FINISHWIZARDCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_FINISHWIZARDCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>
#include <QWizardPage>

#include "core/application/applicationsettings.h"
#include "gui/overlaymessagesframe.h"

namespace Ui
{
    class CFinishWizardComponent;
}
namespace swift::gui::components
{
    /*!
     * swift legal information
     */
    class CFinishWizardComponent : public COverlayMessagesFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CFinishWizardComponent(QWidget *parent = nullptr);

        //! Destructor
        ~CFinishWizardComponent() override;

        //! Agreed with?
        bool isAgreedTo() const;

        //! Validate that the agreement is accepted
        bool validateAgreement();

    private:
        //! Allow crash dumps
        void onAllowCrashDumps(bool checked);

        //! Set the checklist info
        void setChecklistInfo();

        QScopedPointer<Ui::CFinishWizardComponent> ui;
        swift::misc::CSetting<swift::core::application::TCrashDumpUploadEnabled> m_crashDumpUploadEnabled { this };
    };

    /*!
     * Wizard page for CFinishWizardComponent
     */
    class CFinishWizardPage : public QWizardPage
    {
        Q_OBJECT

    public:
        //! Constructors
        using QWizardPage::QWizardPage;

        //! Set config
        void setConfigComponent(CFinishWizardComponent *config) { m_finishwizard = config; }

        //! \copydoc QWizardPage::validatePage
        bool validatePage() override;

    private:
        CFinishWizardComponent *m_finishwizard = nullptr;
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_FINISHWIZARDCOMPONENT_H
