// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_DBLIVERYCOLORSEARCH_H
#define BLACKGUI_COMPONENTS_DBLIVERYCOLORSEARCH_H

#include "blackmisc/aviation/livery.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CDbLiveryColorSearch;
}
namespace BlackGui::Components
{
    /*!
     * Search for best matching color livery
     */
    class CDbLiveryColorSearch : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDbLiveryColorSearch(QWidget *parent = nullptr);

        //! Destructor
        ~CDbLiveryColorSearch();

        //! Found livery if any, otherwise default
        BlackMisc::Aviation::CLivery getLivery() const;

        //! Preset colors
        void presetColorLivery(const BlackMisc::Aviation::CLivery &livery);

    private:
        QScopedPointer<Ui::CDbLiveryColorSearch> ui;
    };
} // ns

#endif // guard
