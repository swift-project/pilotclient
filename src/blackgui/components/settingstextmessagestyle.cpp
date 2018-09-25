/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "settingstextmessagestyle.h"
#include "ui_settingstextmessagestyle.h"
#include "settingsfontdialog.h"
#include "texteditdialog.h"
#include "settingstextmessagestyle.h"

#include <QTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QStringBuilder>

namespace BlackGui
{
    namespace Components
    {
        CSettingsTextMessageStyle::CSettingsTextMessageStyle(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CSettingsTextMessageStyle)
        {
            ui->setupUi(this);

            connect(ui->pb_Font, &QPushButton::released, this, &CSettingsTextMessageStyle::changeFont);
            connect(ui->pb_Style, &QPushButton::released, this, &CSettingsTextMessageStyle::changeStyle);
            connect(ui->pb_Reset, &QPushButton::released, this, &CSettingsTextMessageStyle::resetStyle);
            connect(ui->pb_Reset, &QPushButton::released, this, &CSettingsTextMessageStyle::changed);
        }

        CSettingsTextMessageStyle::~CSettingsTextMessageStyle()
        { }

        QStringList CSettingsTextMessageStyle::getFamilySizeStyle() const
        {
            if (m_fontSettingsDialog) { return m_fontSettingsDialog->getFamilySizeStyle(); }

            static const QStringList empty({"", "", ""});
            return empty;
        }

        void CSettingsTextMessageStyle::changeFont()
        {
            if (!m_fontSettingsDialog)
            {
                m_fontSettingsDialog = new CSettingsFontDialog(this);
                m_fontSettingsDialog->setWithColorSelection(false);
            }

            const QDialog::DialogCode r = static_cast<QDialog::DialogCode>(m_fontSettingsDialog->exec());
            if (r == QDialog::Accepted)
            {
                const QStringList familySizeStyle = this->getFamilySizeStyle();
                this->setFontFamilySizeStyle(familySizeStyle);
                emit this->changed();
            }
        }

        void CSettingsTextMessageStyle::changeStyle()
        {
            if (!m_textEditDialog)
            {
                m_textEditDialog = new CTextEditDialog(this);
                m_textEditDialog->resize(400, 300);
            }

            m_textEditDialog->textEdit()->setPlainText(m_style);
            const QDialog::DialogCode r = static_cast<QDialog::DialogCode>(m_textEditDialog->exec());
            if (r == QDialog::Accepted)
            {
                m_style = m_textEditDialog->textEdit()->toPlainText();
                emit this->changed();
            }
        }

        bool CSettingsTextMessageStyle::setFontFamilySizeStyle(const QStringList &familySizeStlye)
        {
            if (familySizeStlye.size() != 3) { return false; }
            static const QString f("font-family: \"%1\"; font-size: %2; font-style: %3");

            QString style = m_style;
            const QString tableStyle = QStringLiteral("table { ") % f.arg(familySizeStlye.at(0), familySizeStlye.at(1), familySizeStlye.at(2)) % QStringLiteral(" }");

            thread_local const QRegularExpression re("table\\s*\\{.*\\}");
            style.replace(re, tableStyle);

            m_style = style;
            return true;
        }
    } // ns
} // ns
