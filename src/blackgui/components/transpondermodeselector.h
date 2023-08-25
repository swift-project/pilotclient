// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_TRANSPONDERMODESELECTOR_H
#define BLACKGUI_COMPONENTS_TRANSPONDERMODESELECTOR_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/aviation/transponder.h"

#include <QComboBox>
#include <QObject>
#include <QString>
#include <QTimer>

class QStringList;

namespace BlackGui::Components
{
    //! Selector for the transponder mode
    //! \remarks features ident reset
    class BLACKGUI_EXPORT CTransponderModeSelector : public QComboBox
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
        BlackMisc::Aviation::CTransponder::TransponderMode getSelectedTransponderMode() const;

        //! Ident selected
        bool isIdentSelected() const;

        //! reset to last mode (unequal ident)
        void resetTransponderMode();

        //! Selected transponder mode
        void setSelectedTransponderModeAsString(const QString &mode);

        //! Selected transponder mode
        void setSelectedTransponderMode(BlackMisc::Aviation::CTransponder::TransponderMode mode);

        //! Set to ident (transponder state)
        void setSelectedTransponderModeStateIdent();

    signals:
        //! Mode / state has been changed
        void transponderModeChanged(BlackMisc::Aviation::CTransponder::TransponderMode newMode);

        //! Ident phase ended
        void transponderStateIdentEnded();

    private:
        BlackMisc::Aviation::CTransponder::TransponderMode m_currentMode = BlackMisc::Aviation::CTransponder::StateStandby;
        BlackMisc::Aviation::CTransponder::TransponderMode m_resetMode = BlackMisc::Aviation::CTransponder::StateStandby;
        QTimer m_resetTimer;
    };
} // ns

#endif // guard
