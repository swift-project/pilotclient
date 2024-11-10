// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_COUNTRYSELECTOR_H
#define SWIFT_GUI_COMPONENTS_COUNTRYSELECTOR_H

#include "misc/country.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CCountrySelector;
}
namespace swift::gui::components
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
        void setCountry(const swift::misc::CCountry &country);

        //! Get country
        const swift::misc::CCountry &getCountry() const { return m_current; }

    signals:
        //! Changed country
        void countryChanged(const swift::misc::CCountry &country);

    private:
        //! Countries loaded
        void onCountriesLoaded();

        //! ISO has been changed
        void onIsoChanged();

        //! Country name has been changed
        void onCountryNameChanged(const QString &name);

        swift::misc::CCountry m_current;
        QScopedPointer<Ui::CCountrySelector> ui;
    };
} // ns
#endif // guard
