/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSVIEWUPDATETIMES_H
#define BLACKGUI_COMPONENTS_SETTINGSVIEWUPDATETIMES_H

#include "blackgui/settings/viewupdatesettings.h"
#include <QFrame>

namespace Ui
{
    class CSettingsViewUpdateTimes;
}
namespace BlackGui::Components
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
        BlackMisc::CSetting<BlackGui::Settings::TViewUpdateSettings> m_settings { this, &CSettingsViewUpdateTimes::ps_settingsChanged }; //!< settings changed
    };
} // ns

#endif // guard
