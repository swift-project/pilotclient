// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_COUNTRYSELECTOR_H
#define BLACKGUI_COMPONENTS_COUNTRYSELECTOR_H

#include "blackmisc/country.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CCountrySelector;
}
namespace BlackGui::Components
{
    /*!
     * @brief Select a valid country
     */
    class CCountrySelector : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCountrySelector(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CCountrySelector();

        //! Set country
        void setCountry(const BlackMisc::CCountry &country);

        //! Get country
        const BlackMisc::CCountry &getCountry() const { return m_current; }

    signals:
        //! Changed country
        void countryChanged(const BlackMisc::CCountry &country);

    private:
        //! Countries loaded
        void onCountriesLoaded();

        //! ISO has been changed
        void onIsoChanged();

        //! Country name has been changed
        void onCountryNameChanged(const QString &name);

        BlackMisc::CCountry m_current;
        QScopedPointer<Ui::CCountrySelector> ui;
    };
} // ns
#endif // guard
