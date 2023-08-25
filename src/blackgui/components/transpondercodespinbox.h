// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_TRANSPONDERCODESPINBOX_H
#define BLACKGUI_COMPONENTS_TRANSPONDERCODESPINBOX_H

#include "blackgui/blackguiexport.h"

#include <QObject>
#include <QSpinBox>
#include <QString>

namespace BlackGui::Components
{
    //! Specialized spin box for Transponder codes
    class BLACKGUI_EXPORT CTransponderCodeSpinBox : public QSpinBox
    {
        Q_OBJECT
    public:
        //! Constructor
        explicit CTransponderCodeSpinBox(QWidget *parent = nullptr);

        //! \copydoc QDoubleSpinBox::textFromValue
        virtual QString textFromValue(int value) const override;
    };
} // ns

#endif // guard
