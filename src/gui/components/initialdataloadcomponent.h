// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_INITIALDATALOADCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_INITIALDATALOADCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>
#include <QWizardPage>

namespace Ui
{
    class CInitialDataLoadComponent;
}
namespace swift::gui::components
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
        ~CInitialDataLoadComponent() override;

        //! \copydoc swift::gui::components::CDbLoadOverviewComponent::loadAllFromShared
        void loadAllFromShared();

        //! \copydoc swift::gui::components::CDbLoadOverviewComponent::loadAllFromDb
        void loadAllFromDb();

        //! \copydoc swift::gui::components::CDbLoadOverviewComponent::isLoadInProgress
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
        void initializePage() override;

        //! \copydoc QWizardPage::validatePage
        bool validatePage() override;

    private:
        CInitialDataLoadComponent *m_config = nullptr;
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_INITIALDATALOADCOMPONENT_H
