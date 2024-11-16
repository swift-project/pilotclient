// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_TRANSPONDERMODESELECTOR_H
#define SWIFT_GUI_COMPONENTS_TRANSPONDERMODESELECTOR_H

#include <QComboBox>
#include <QObject>
#include <QString>
#include <QTimer>

#include "gui/swiftguiexport.h"
#include "misc/aviation/transponder.h"

namespace swift::gui::components
{
    //! Selector for the transponder mode
    //! \remarks features ident reset
    class SWIFT_GUI_EXPORT CTransponderModeSelector : public QComboBox
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CTransponderModeSelector(QWidget *parent = nullptr);

        //! Standby string
        static const QString &transponderStateStandby();

        //! Ident string
        static const QString &transponderStateIdent();

        //! Mode C string
        static const QString &transponderModeC();

        //! All relevant modes for GUI
        static const QStringList &modes();

        //! Selected transponder mode
        swift::misc::aviation::CTransponder::TransponderMode getSelectedTransponderMode() const;

        //! Ident selected
        bool isIdentSelected() const;

        //! reset to last mode (unequal ident)
        void resetTransponderMode();

        //! Selected transponder mode
        void setSelectedTransponderModeAsString(const QString &mode);

        //! Selected transponder mode
        void setSelectedTransponderMode(swift::misc::aviation::CTransponder::TransponderMode mode);

        //! Set to ident (transponder state)
        void setSelectedTransponderModeStateIdent();

    signals:
        //! Mode / state has been changed
        void transponderModeChanged(swift::misc::aviation::CTransponder::TransponderMode newMode);

        //! Ident phase ended
        void transponderStateIdentEnded();

    private:
        swift::misc::aviation::CTransponder::TransponderMode m_currentMode = swift::misc::aviation::CTransponder::StateStandby;
        swift::misc::aviation::CTransponder::TransponderMode m_resetMode = swift::misc::aviation::CTransponder::StateStandby;
        QTimer m_resetTimer;
    };
} // namespace swift::gui::components

#endif // guard
