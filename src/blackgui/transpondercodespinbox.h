/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_TRANSPONDERCODESPINBOX_H
#define BLACKGUI_TRANSPONDERCODESPINBOX_H

#include "blackgui/blackguiexport.h"
#include <QSpinBox>

namespace BlackGui
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
} // namespace

#endif // guard
