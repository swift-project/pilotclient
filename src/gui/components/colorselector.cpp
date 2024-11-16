// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/colorselector.h"

#include <QColor>
#include <QColorDialog>
#include <QCompleter>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMimeData>
#include <QMouseEvent>
#include <QPalette>
#include <QPixmap>
#include <QToolButton>
#include <QVariant>
#include <Qt>
#include <QtGlobal>

#include "ui_colorselector.h"

#include "gui/guiutility.h"
#include "misc/icons.h"
#include "misc/mixin/mixincompare.h"
#include "misc/rgbcolor.h"
#include "misc/variant.h"

using namespace swift::misc;

namespace swift::gui::components
{
    CColorSelector::CColorSelector(QWidget *parent) : QFrame(parent),
                                                      ui(new Ui::CColorSelector)
    {
        ui->setupUi(this);
        ui->tb_ColorDialog->setIcon(CIcons::color16());
        this->setAcceptDrops(true);
        connect(ui->tb_ColorDialog, &QToolButton::clicked, this, &CColorSelector::colorDialog);
        connect(ui->le_Color, &QLineEdit::editingFinished, this, &CColorSelector::onReturnPressed);
        connect(ui->le_Color, &QLineEdit::returnPressed, this, &CColorSelector::onReturnPressed);

        QCompleter *completer = new QCompleter(QColor::colorNames(), this);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setMaxVisibleItems(10);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        ui->le_Color->setCompleter(completer);
        connect(completer, qOverload<const QString &>(&QCompleter::activated), this, &CColorSelector::setColorByName);
    }

    CColorSelector::~CColorSelector() {}

    void CColorSelector::setColor(const swift::misc::CRgbColor &color)
    {
        if (color == m_lastColor) { return; }
        if (!color.isValid())
        {
            this->clear();
            m_lastColor = CRgbColor();
        }
        else
        {
            ui->le_Color->setText(color.hex());
            ui->lbl_ColorIcon->setPixmap(color.toPixmap());
            m_lastColor = color;
        }
        emit colorChanged(color);
    }

    void CColorSelector::setColor(const QColor &color)
    {
        this->setColor(CRgbColor(color));
    }

    swift::misc::CRgbColor CColorSelector::getColor() const
    {
        CRgbColor q(ui->le_Color->text());
        return q;
    }

    void CColorSelector::setReadOnly(bool readOnly)
    {
        ui->le_Color->setReadOnly(readOnly);
        ui->tb_ColorDialog->setVisible(!readOnly);
        this->setAcceptDrops(!readOnly);
    }

    void CColorSelector::clear()
    {
        ui->le_Color->clear();
        ui->lbl_ColorIcon->setPixmap(QPixmap());
    }

    void CColorSelector::dragEnterEvent(QDragEnterEvent *event)
    {
        if (!event) { return; }
        setBackgroundRole(QPalette::Highlight);
        event->acceptProposedAction();
    }

    void CColorSelector::dragMoveEvent(QDragMoveEvent *event)
    {
        if (!event) { return; }
        event->acceptProposedAction();
    }

    void CColorSelector::dragLeaveEvent(QDragLeaveEvent *event)
    {
        event->accept();
    }

    void CColorSelector::dropEvent(QDropEvent *event)
    {
        if (!event) { return; }
        const QMimeData *mime = event->mimeData();
        if (!mime) { return; }

        if (mime->hasColor())
        {
            const QColor color = qvariant_cast<QColor>(event->mimeData()->colorData());
            if (!color.isValid()) { return; }
            this->setColor(color);
        }
        else if (CGuiUtility::hasSwiftVariantMimeType(mime))
        {
            const CVariant valueVariant(CGuiUtility::fromSwiftDragAndDropData(mime));
            if (valueVariant.isValid())
            {
                if (valueVariant.canConvert<CRgbColor>())
                {
                    const CRgbColor rgb(valueVariant.value<CRgbColor>());
                    if (!rgb.isValid()) { return; }
                    this->setColor(rgb);
                }
                else if (valueVariant.canConvert<QColor>())
                {
                    const QColor qColor(valueVariant.value<QColor>());
                    if (!qColor.isValid()) { return; }
                    this->setColor(qColor);
                }
            }
        }
        else if (mime->hasText())
        {
            const QString t = mime->text();
            if (t.isEmpty()) { return; }
            if (!t.contains("{"))
            {
                // we assume a color string, not an object (because there are no {})
                const CRgbColor c(t);
                if (!c.isValid()) { return; }
                this->setColor(c);
            }
        }
    }

    void CColorSelector::mousePressEvent(QMouseEvent *event)
    {
        if (!event || event->button() != Qt::LeftButton) { QFrame::mousePressEvent(event); }
        const CRgbColor c(this->getColor());
        if (!c.isValid()) { return; }

        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        mimeData->setColorData(QVariant::fromValue(c.toQColor()));
        drag->setMimeData(mimeData);
        drag->setPixmap(c.toPixmap());

        Qt::DropAction dropAction = drag->exec(Qt::CopyAction);
        Q_UNUSED(dropAction);
    }

    void CColorSelector::colorDialog()
    {
        QColor q = this->getColor().toQColor();
        if (!q.isValid()) { q = m_lastColor.toQColor(); }
        QColor newColor = QColorDialog::getColor(q, this, "Color picker");
        if (!newColor.isValid()) { return; }
        this->setColor(newColor);
    }

    void CColorSelector::onReturnPressed()
    {
        const CRgbColor color = this->getColor();
        if (color.isValid())
        {
            this->setColor(color);
        }
    }

    void CColorSelector::setColorByName(const QString &colorName)
    {
        if (colorName.isEmpty()) { return; }
        const CRgbColor c(colorName, true);
        if (c.isValid()) { this->setColor(c); }
    }

    void CColorSelector::resetToLastValidColor()
    {
        if (!m_lastColor.isValid()) { return; }
        this->setColor(m_lastColor);
    }
} // namespace swift::gui::components
