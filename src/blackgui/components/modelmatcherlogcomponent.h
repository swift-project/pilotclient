// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENT_MODELMATCHERLOGCOMPONENT_H
#define BLACKGUI_COMPONENT_MODELMATCHERLOGCOMPONENT_H

#include <QFrame>
#include <QTabWidget>
#include <QTimer>
#include <QTextDocument>

namespace Ui
{
    class CModelMatcherLogComponent;
}
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
