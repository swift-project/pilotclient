// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "settingstextmessagestyle.h"

#include <QKeySequence>
#include <QPushButton>
#include <QRegularExpression>
#include <QShortcut>
#include <QStringBuilder>
#include <QTextEdit>

#include "ui_settingstextmessagestyle.h"

#include "gui/components/settingsfontdialog.h"
#include "gui/components/texteditdialog.h"
#include "gui/shortcut.h"

namespace swift::gui::components
{
    CSettingsTextMessageStyle::CSettingsTextMessageStyle(QWidget *parent)
        : QFrame(parent), ui(new Ui::CSettingsTextMessageStyle)
    {
        ui->setupUi(this);

        connect(ui->pb_Font, &QPushButton::released, this, &CSettingsTextMessageStyle::changeFont);
        connect(ui->pb_Style, &QPushButton::released, this, &CSettingsTextMessageStyle::changeStyle);
        connect(ui->pb_Reset, &QPushButton::released, this, &CSettingsTextMessageStyle::resetStyle);
        connect(ui->pb_Reset, &QPushButton::released, this, &CSettingsTextMessageStyle::changed);
        connect(ui->pb_FontMinus, &QPushButton::released, this, &CSettingsTextMessageStyle::fontSizeMinus);
        connect(ui->pb_FontPlus, &QPushButton::released, this, &CSettingsTextMessageStyle::fontSizePlus);

        //        QShortcut *sc = new QShortcut(CShortcut::keyFontMinus(), this);
        //        sc->setContext(Qt::WidgetWithChildrenShortcut);
        //        QObject::connect(sc, &QShortcut::activatedAmbiguously, this,
        //        &CSettingsTextMessageStyle::fontSizePlus);
    }

    CSettingsTextMessageStyle::~CSettingsTextMessageStyle() = default;

    QStringList CSettingsTextMessageStyle::getFamilySizeStyle() const
    {
        if (m_fontSettingsDialog) { return m_fontSettingsDialog->getFamilySizeStyle(); }

        static const QStringList empty({ "", "", "" });
        return empty;
    }

    void CSettingsTextMessageStyle::changeFont()
    {
        if (!m_fontSettingsDialog)
        {
            m_fontSettingsDialog = new CSettingsFontDialog(this);
            m_fontSettingsDialog->setWithColorSelection(false);
        }

        const auto r = static_cast<QDialog::DialogCode>(m_fontSettingsDialog->exec());
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
        const auto r = static_cast<QDialog::DialogCode>(m_textEditDialog->exec());
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

        const QString tableStyle =
            u"table { " % f.arg(familySizeStlye.at(0), familySizeStlye.at(1), familySizeStlye.at(2)) % u" }";
        this->replaceTableStyle(tableStyle);
        return true;
    }

    void CSettingsTextMessageStyle::replaceTableStyle(const QString &newTableStyle)
    {
        QString style = m_style;
        thread_local const QRegularExpression re("table\\s*\\{.*\\}");
        style.replace(re, newTableStyle);
        m_style = style;
    }

    void CSettingsTextMessageStyle::fontSizeMinus()
    {
        if (this->changeFontSize(false)) { emit this->changed(); }
    }

    void CSettingsTextMessageStyle::fontSizePlus()
    {
        if (this->changeFontSize(true)) { emit this->changed(); }
    }

    bool CSettingsTextMessageStyle::changeFontSize(bool increase)
    {
        QString style = m_style;
        thread_local const QRegularExpression re("table\\s*\\{.*:\\s*(\\d{1,2}).*\\}");
        const QRegularExpressionMatch match = re.match(style);
        const QStringList matches = match.capturedTexts();
        if (matches.size() != 2) { return false; }

        bool ok;
        int ptSize = matches.last().toInt(&ok);
        if (!ok) { return false; }
        if (increase)
        {
            ptSize++;
            if (ptSize > 16) { return false; }
        }
        else
        {
            ptSize--;
            if (ptSize < 6) { return false; }
        }

        const QString pt = QString::number(ptSize) % u"pt";
        QString tableStyle = matches.front();
        thread_local const QRegularExpression rePt("\\d{1,2}pt");
        tableStyle.replace(rePt, pt);
        this->replaceTableStyle(tableStyle);
        return true;
    }
} // namespace swift::gui::components
