/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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

        //! Crashdum hint
        void showCrashDumpHint();

        //! Set the checklist info
        void setChecklistInfo();

        QScopedPointer<Ui::CLegalInfoComponent> ui;
        BlackMisc::CSetting<BlackCore::Application::TCrashDumpSettings> m_crashDumpSettings { this };
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
