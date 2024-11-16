// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_SCALESCREENFACTOR_H
#define SWIFT_GUI_COMPONENTS_SCALESCREENFACTOR_H

#include <QFrame>
#include <QScopedPointer>

#include "gui/swiftguiexport.h"

namespace Ui
{
    class CScaleScreenFactor;
}
namespace swift::gui::components
{
    /*!
     * UI to scale screen factor
     */
    class SWIFT_GUI_EXPORT CScaleScreenFactor : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CScaleScreenFactor(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CScaleScreenFactor() override;

        //! Minimum/maximum values
        void setMinMax(int min, int max);

        //! Scale factor
        qreal getScaleFactor() const;

        //! Scale factor as string
        QString getScaleFactorAsString() const;

    private:
        //! Slider value changed
        void onSliderChanged(int value);

        //! Line edit change
        void onEditFinished();

        QScopedPointer<Ui::CScaleScreenFactor> ui;
    };
} // namespace swift::gui::components

#endif // guard
