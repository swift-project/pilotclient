/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "transpondermodeselector.h"

using namespace BlackMisc::Aviation;

namespace BlackGui
{
    CTransponderModeSelector::CTransponderModeSelector(QWidget *parent) : QComboBox(parent)
    {
        QComboBox::insertItems(0, CTransponderModeSelector::modes());
        connect(&this->m_resetTimer, &QTimer::timeout, this, &CTransponderModeSelector::resetTransponderMode);
        connect(this, &CTransponderModeSelector::currentTextChanged, this, &CTransponderModeSelector::setSelectedTransponderModeAsString);
        this->m_resetTimer.setInterval(5000);
    }

    const QString &CTransponderModeSelector::transponderStateStandby()
    {
        static const QString s(BlackMisc::Aviation::CTransponder::modeAsString(BlackMisc::Aviation::CTransponder::StateStandby));
        return s;
    }

    const QString &CTransponderModeSelector::transponderStateIdent()
    {
        static const QString s(BlackMisc::Aviation::CTransponder::modeAsString(BlackMisc::Aviation::CTransponder::StateIdent));
        return s;
    }

    const QString &CTransponderModeSelector::transponderModeC()
    {
        static const QString s(BlackMisc::Aviation::CTransponder::modeAsString(BlackMisc::Aviation::CTransponder::ModeC));
        return s;
    }

    const QStringList &CTransponderModeSelector::modes()
    {
        static QStringList modes;
        if (modes.isEmpty())
        {
            modes << CTransponderModeSelector::transponderStateStandby();
            modes << CTransponderModeSelector::transponderModeC();
            modes << CTransponderModeSelector::transponderStateIdent();
        }
        return modes;
    }

    BlackMisc::Aviation::CTransponder::TransponderMode CTransponderModeSelector::getSelectedTransponderMode() const
    {
        return this->m_currentMode;
    }

    void CTransponderModeSelector::setSelectedTransponderMode(CTransponder::TransponderMode mode)
    {
        if (mode != CTransponder::StateIdent) { this->m_resetMode = mode; }
        if (this->m_currentMode == mode) { return; }
        if (this->m_currentMode == CTransponder::StateIdent) { emit this->transponderStateIdentEnded(); }
        this->m_currentMode = mode;
        QString m = CTransponder::modeAsString(mode);
        QComboBox::setCurrentText(m);
        if (mode == CTransponder::StateIdent)
        {
            this->m_resetTimer.start();
        }
        else
        {
            this->m_resetTimer.stop();
        }
        emit this->transponderModeChanged(this->m_currentMode);
    }

    void CTransponderModeSelector::setSelectedTransponderModeStateIdent()
    {
        this->setSelectedTransponderMode(BlackMisc::Aviation::CTransponder::StateIdent);
    }

    void CTransponderModeSelector::setSelectedTransponderModeAsString(const QString &mode)
    {
        CTransponder::TransponderMode m = CTransponder::modeFromString(mode);
        if (this->m_currentMode == m) return; // nothing to change
        this->setSelectedTransponderMode(m);
    }

    void CTransponderModeSelector::resetTransponderMode()
    {
        if (!this->isIdentSelected()) return; // avoid unnecessary events
        this->setSelectedTransponderMode(this->m_resetMode);
    }
}
