// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_SETTINGSVIEWUPDATETIMES_H
#define SWIFT_GUI_COMPONENTS_SETTINGSVIEWUPDATETIMES_H

#include <QFrame>

#include "gui/settings/viewupdatesettings.h"

namespace Ui
{
    class CSettingsViewUpdateTimes;
}
namespace swift::gui::components
{
    /*!
     * Update rates / times
     */
    class CSettingsViewUpdateTimes : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CSettingsViewUpdateTimes(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CSettingsViewUpdateTimes();

    private slots:
        //! Slider changed
        void ps_sliderChanged();

        //! Settings have been changed
        void ps_settingsChanged();

        //! Reset all update times
        void ps_resetUpdateTimes();

    private:
        QScopedPointer<Ui::CSettingsViewUpdateTimes> ui;
        swift::misc::CSetting<swift::gui::settings::TViewUpdateSettings> m_settings { this, &CSettingsViewUpdateTimes::ps_settingsChanged }; //!< settings changed
    };
} // namespace swift::gui::components

#endif // guard
