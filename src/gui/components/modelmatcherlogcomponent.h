// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENT_MODELMATCHERLOGCOMPONENT_H
#define SWIFT_GUI_COMPONENT_MODELMATCHERLOGCOMPONENT_H

#include <QFrame>
#include <QTabWidget>
#include <QTextDocument>
#include <QTimer>

namespace Ui
{
    class CModelMatcherLogComponent;
}
namespace swift::gui::components
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
        ~CModelMatcherLogComponent() override;

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
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENT_MODELMATCHERLOGCOMPONENT_H
