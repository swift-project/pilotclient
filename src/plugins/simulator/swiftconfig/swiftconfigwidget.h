/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_SWIFT_SWIFTCONFIGWIDGET_H
#define BLACKSIMPLUGIN_SWIFT_SWIFTCONFIGWIDGET_H

#include <blackgui/pluginconfigwindow.h>
#include <QScopedPointer>

namespace Ui { class CSwiftConfigWidget; }
namespace BlackSimPlugin
{
    namespace Swift
    {
        /**
         * Configuration window for CSimulatorSwiftConfig
         */
        class Q_DECL_EXPORT CSwiftConfigWidget : public BlackGui::CPluginConfigWindow
        {
            Q_OBJECT

        public:
            //! Ctor
            explicit CSwiftConfigWidget(QWidget *parent = nullptr);

            //! Dtor
            virtual ~CSwiftConfigWidget();

        private:
            QScopedPointer <Ui::CSwiftConfigWidget> ui;
        };
    } // ns
} // ns

#endif // guard
