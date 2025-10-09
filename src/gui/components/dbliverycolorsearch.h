// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBLIVERYCOLORSEARCH_H
#define SWIFT_GUI_COMPONENTS_DBLIVERYCOLORSEARCH_H

#include <QFrame>
#include <QScopedPointer>

#include "misc/aviation/livery.h"

namespace Ui
{
    class CDbLiveryColorSearch;
}
namespace swift::gui::components
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
        ~CDbLiveryColorSearch() override;

        //! Found livery if any, otherwise default
        swift::misc::aviation::CLivery getLivery() const;

        //! Preset colors
        void presetColorLivery(const swift::misc::aviation::CLivery &livery);

    private:
        QScopedPointer<Ui::CDbLiveryColorSearch> ui;
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_DBLIVERYCOLORSEARCH_H
