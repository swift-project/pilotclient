/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENT_MODELMATCHERLOGCOMPONENT_H
#define BLACKGUI_COMPONENT_MODELMATCHERLOGCOMPONENT_H

#include <QFrame>
#include <QTabWidget>
#include <QTimer>
#include <QTextDocument>

namespace Ui { class CModelMatcherLogComponent; }
namespace BlackGui::Components
{
    /*!
     * Special logs for matching and reverse lookup
     */
    class CModelMatcherLogComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CModelMatcherLogComponent(QWidget *parent = nullptr);

        //! Constructor
        virtual ~CModelMatcherLogComponent() override;

    private:
        QScopedPointer<Ui::CModelMatcherLogComponent> ui;
        QTextDocument m_text { this };

        //! Contexts available
        bool hasContexts() const;

        //! Enabled messages
        bool enabledMessages() const;

        //! Callsign was entered
        void callsignEntered();
    };
} // ns

#endif // guard
