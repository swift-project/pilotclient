/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBLIVERYCOLORSEARCH_H
#define BLACKGUI_COMPONENTS_DBLIVERYCOLORSEARCH_H

#include "blackmisc/aviation/livery.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CDbLiveryColorSearch; }
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
