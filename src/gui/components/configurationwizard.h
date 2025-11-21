// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_CONFIGURATIONWIZARD_H
#define SWIFT_GUI_COMPONENTS_CONFIGURATIONWIZARD_H

#include <QScopedPointer>
#include <QWizard>

#include "gui/swiftguiexport.h"

namespace Ui
{
    class CConfigurationWizard;
}
namespace swift::gui::components
{
    /*!
     * Configure the most important settings
     */
    class SWIFT_GUI_EXPORT CConfigurationWizard : public QWizard
    {
        Q_OBJECT

    public:
        //! Page ids
        enum Pages
        {
            Legal,
            DataLoad,
            CopyModels,
            CopySettingsAndCaches,
            ConfigSimulator,
            FirstModelSet,
            XSwiftBus,
            ConfigHotkeys
        };

        //! Constructor
        explicit CConfigurationWizard(QWidget *parent = nullptr);

        //! Destructor
        ~CConfigurationWizard() override;

        //! Was the last step skipped?
        bool lastStepSkipped() const;

        //! Static version of CConfigurationWizard::lastStepSkipped
        static bool lastWizardStepSkipped(const QWizard *standardWizard);

    private:
        //! The current page has changed
        void wizardCurrentIdChanged(int id);

        //! Custom button was clicked
        void clickedCustomButton(int which);

        //! Accepted or rejected
        void ended();

        //! Set the parent's window opacity
        void setParentOpacity(qreal opacity);

        //! Set screen geometry based on screen resolution
        void setScreenGeometry();

        QScopedPointer<Ui::CConfigurationWizard> ui;
        int m_previousId = -1;
        int m_minId = -1;
        int m_maxId = -1;
        bool m_skipped = false;
    };
} // namespace swift::gui::components
#endif // SWIFT_GUI_COMPONENTS_CONFIGURATIONWIZARD_H
