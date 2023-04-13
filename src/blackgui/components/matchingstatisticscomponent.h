/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_MATCHINGSTATISTICSCOMPONENT_H
#define BLACKGUI_COMPONENTS_MATCHINGSTATISTICSCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CMatchingStatisticsComponent;
}
namespace BlackGui::Components
{
    /*!
     * Show statistics about matching results
     */
    class CMatchingStatisticsComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CMatchingStatisticsComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CMatchingStatisticsComponent();

    private:
        //! Show statistics
        void onGenerateClicked();

        QScopedPointer<Ui::CMatchingStatisticsComponent> ui;
    };
} // ns

#endif // guard
