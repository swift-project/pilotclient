// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_CCOLUMNCHECKBOXDELEGATE_H
#define BLACKGUI_CCOLUMNCHECKBOXDELEGATE_H

#include "blackgui/blackguiexport.h"

#include <QItemDelegate>
#include <QObject>
#include <QString>
#include <QStyleOptionViewItem>

class QAbstractItemModel;
class QModelIndex;
class QStyleOptionViewItem;
class QWidget;

namespace BlackGui::Views
{
    //! CheckBox for single column
    class BLACKGUI_EXPORT CCheckBoxDelegate : public QItemDelegate
    {
        Q_OBJECT

    public:
        //! Constructor
        CCheckBoxDelegate(QObject *parent = nullptr);

        //! Constructor
        CCheckBoxDelegate(const QString &iconCheckedUrl, const QString &iconUncheckedUrl, QObject *parent = nullptr);

        //! Destructor
        ~CCheckBoxDelegate();

        //! \copydoc QItemDelegate::createEditor
        virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

        //! \copydoc QItemDelegate::setEditorData
        virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;

        //! \copydoc QItemDelegate::setModelData
        virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

        //! \copydoc QItemDelegate::updateEditorGeometry
        virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    private:
        QString m_iconCheckedUrl;
        QString m_iconUncheckedUrl;
    };
} // namespace

#endif // guard
