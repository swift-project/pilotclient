/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/stylesheetutility.h"
#include "blackgui/views/checkboxdelegate.h"

#include <QAbstractItemModel>
#include <QCheckBox>
#include <QModelIndex>
#include <QVariant>
#include <QWidget>
#include <Qt>
#include <QtGlobal>

using namespace BlackGui;

namespace BlackGui::Views
{

    CCheckBoxDelegate::CCheckBoxDelegate(QObject *parent) : QItemDelegate(parent)
    {}

    CCheckBoxDelegate::CCheckBoxDelegate(const QString &iconCheckedUrl, const QString &iconUncheckedUrl, QObject *parent) : QItemDelegate(parent), m_iconCheckedUrl(iconCheckedUrl), m_iconUncheckedUrl(iconUncheckedUrl)
    {}

    CCheckBoxDelegate::~CCheckBoxDelegate() {}

    QWidget *CCheckBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        Q_UNUSED(index);
        Q_UNUSED(option);
        QCheckBox *cb = new QCheckBox(parent);
        if (!m_iconCheckedUrl.isEmpty() && !m_iconUncheckedUrl.isEmpty())
        {
            const QString style = CStyleSheetUtility::styleForIconCheckBox(m_iconCheckedUrl, m_iconUncheckedUrl);
            cb->setStyleSheet("");
            cb->setStyleSheet(style);
        }
        return cb;
    }

    void CCheckBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        const bool v = index.model()->data(index, Qt::UserRole).toBool();
        QCheckBox *cb = qobject_cast<QCheckBox *>(editor);
        cb->setChecked(v);
    }

    void CCheckBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        QCheckBox *cb = qobject_cast<QCheckBox *>(editor);
        const bool v = cb->isChecked();
        model->setData(index, QVariant(v), Qt::EditRole);
    }

    void CCheckBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        Q_UNUSED(index);
        editor->setGeometry(option.rect);
    }

} // namespace
