// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/transpondermodeselector.h"

#include <QStringList>

using namespace swift::misc::aviation;

namespace swift::gui::components
{
    CTransponderModeSelector::CTransponderModeSelector(QWidget *parent) : QComboBox(parent)
    {
        QComboBox::insertItems(0, CTransponderModeSelector::modes());
        m_resetTimer.setObjectName(this->objectName().append(":m_resetTimer"));
        connect(&m_resetTimer, &QTimer::timeout, this, &CTransponderModeSelector::resetTransponderMode);
        connect(this, &CTransponderModeSelector::currentTextChanged, this,
                &CTransponderModeSelector::setSelectedTransponderModeAsString);
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
        static const QStringList modes({ CTransponderModeSelector::transponderStateStandby(),
                                         CTransponderModeSelector::transponderModeC(),
                                         CTransponderModeSelector::transponderStateIdent() });
        return modes;
    }

    swift::misc::aviation::CTransponder::TransponderMode CTransponderModeSelector::getSelectedTransponderMode() const
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
        m_currentMode = mode;
        const QString m = CTransponder::modeAsString(mode);
        QComboBox::setCurrentText(m);
        if (mode == CTransponder::StateIdent) { m_resetTimer.start(); }
        else { m_resetTimer.stop(); }
    }

    void CTransponderModeSelector::setSelectedTransponderModeStateIdent()
    {
        this->setSelectedTransponderMode(swift::misc::aviation::CTransponder::StateIdent);
        emit this->transponderModeChanged(m_currentMode);
    }

    void CTransponderModeSelector::setSelectedTransponderModeAsString(const QString &mode)
    {
        CTransponder::TransponderMode m = CTransponder::modeFromString(mode);
        if (m_currentMode == m) return; // nothing to change
        this->setSelectedTransponderMode(m);
        emit this->transponderModeChanged(m_currentMode);
    }

    void CTransponderModeSelector::resetTransponderMode()
    {
        if (!this->isIdentSelected()) return; // avoid unnecessary events
        this->setSelectedTransponderMode(m_resetMode);
        emit this->transponderModeChanged(m_currentMode);
    }
} // namespace swift::gui::components
