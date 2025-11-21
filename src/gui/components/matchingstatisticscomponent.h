// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_MATCHINGSTATISTICSCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_MATCHINGSTATISTICSCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CMatchingStatisticsComponent;
}
namespace swift::gui::components
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
        ~CMatchingStatisticsComponent() override;

    private:
        //! Show statistics
        void onGenerateClicked();

        QScopedPointer<Ui::CMatchingStatisticsComponent> ui;
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_MATCHINGSTATISTICSCOMPONENT_H
