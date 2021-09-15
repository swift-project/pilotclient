/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/components/transpondermodeselector.h"

#include <QStringList>

using namespace BlackMisc::Aviation;

namespace BlackGui::Components
{
    CTransponderModeSelector::CTransponderModeSelector(QWidget *parent) : QComboBox(parent)
    {
        QComboBox::insertItems(0, CTransponderModeSelector::modes());
        m_resetTimer.setObjectName(this->objectName().append(":m_resetTimer"));
        connect(&m_resetTimer, &QTimer::timeout, this, &CTransponderModeSelector::resetTransponderMode);
        connect(this, &CTransponderModeSelector::currentTextChanged, this, &CTransponderModeSelector::setSelectedTransponderModeAsString);
        m_resetTimer.setInterval(5000);
    }

    const QString &CTransponderModeSelector::transponderStateStandby()
    {
        static const QString s(CTransponder::modeAsString(CTransponder::StateStandby));
        return s;
    }

    const QString &CTransponderModeSelector::transponderStateIdent()
    {
        static const QString s(CTransponder::modeAsString(CTransponder::StateIdent));
        return s;
    }

    const QString &CTransponderModeSelector::transponderModeC()
    {
        static const QString s(CTransponder::modeAsString(CTransponder::ModeC));
        return s;
    }

    const QStringList &CTransponderModeSelector::modes()
    {
        static const QStringList modes(
        {
            CTransponderModeSelector::transponderStateStandby(),
            CTransponderModeSelector::transponderModeC(),
            CTransponderModeSelector::transponderStateIdent()
        });
        return modes;
    }

    BlackMisc::Aviation::CTransponder::TransponderMode CTransponderModeSelector::getSelectedTransponderMode() const
    {
        return m_currentMode;
    }

    bool CTransponderModeSelector::isIdentSelected() const
    {
        return this->getSelectedTransponderMode() == CTransponder::StateIdent;
    }

    void CTransponderModeSelector::setSelectedTransponderMode(CTransponder::TransponderMode mode)
    {
        if (mode != CTransponder::StateIdent) { m_resetMode = mode; }
        if (m_currentMode == mode) { return; }
        if (m_currentMode == CTransponder::StateIdent) { emit this->transponderStateIdentEnded(); }
        m_currentMode = mode;
        const QString m = CTransponder::modeAsString(mode);
        QComboBox::setCurrentText(m);
        if (mode == CTransponder::StateIdent)
        {
            m_resetTimer.start();
        }
        else
        {
            m_resetTimer.stop();
        }
        emit this->transponderModeChanged(m_currentMode);
    }

    void CTransponderModeSelector::setSelectedTransponderModeStateIdent()
    {
        this->setSelectedTransponderMode(BlackMisc::Aviation::CTransponder::StateIdent);
    }

    void CTransponderModeSelector::setSelectedTransponderModeAsString(const QString &mode)
    {
        CTransponder::TransponderMode m = CTransponder::modeFromString(mode);
        if (m_currentMode == m) return; // nothing to change
        this->setSelectedTransponderMode(m);
    }

    void CTransponderModeSelector::resetTransponderMode()
    {
        if (!this->isIdentSelected()) return; // avoid unnecessary events
        this->setSelectedTransponderMode(m_resetMode);
    }
} // ns
