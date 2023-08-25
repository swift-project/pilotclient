// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_INITIALDATALOADCOMPONENT_H
#define BLACKGUI_COMPONENTS_INITIALDATALOADCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>
#include <QWizardPage>

namespace Ui
{
    class CInitialDataLoadComponent;
}
namespace BlackGui::Components
{
    /*!
     * Initial data load to prefill caches
     * \remark Normally used in wizard
     */
    class CInitialDataLoadComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CInitialDataLoadComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CInitialDataLoadComponent();

        //! \copydoc BlackGui::Components::CDbLoadOverviewComponent::loadAllFromShared
        void loadAllFromShared();

        //! \copydoc BlackGui::Components::CDbLoadOverviewComponent::loadAllFromDb
        void loadAllFromDb();

        //! \copydoc BlackGui::Components::CDbLoadOverviewComponent::isLoadInProgress
        bool isLoadInProgress() const;

    private:
        QScopedPointer<Ui::CInitialDataLoadComponent> ui;
    };

    /*!
     * Wizard page for CInitialDataLoadComponent
     */
    class CInitialDataLoadWizardPage : public QWizardPage
    {
        Q_OBJECT

    public:
        //! Constructors
        using QWizardPage::QWizardPage;

        //! Set config
        void setConfigComponent(CInitialDataLoadComponent *config) { m_config = config; }

        //! \copydoc QWizardPage::initializePage
        virtual void initializePage() override;

        //! \copydoc QWizardPage::validatePage
        virtual bool validatePage() override;

    private:
        CInitialDataLoadComponent *m_config = nullptr;
    };
} // ns

#endif // guard
