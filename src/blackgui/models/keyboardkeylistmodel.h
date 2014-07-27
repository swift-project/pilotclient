/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKGUI_KEYBOARDKEYLISTMODEL_H
#define BLACKGUI_KEYBOARDKEYLISTMODEL_H

#include "blackmisc/hwkeyboardkeylist.h"
#include "blackgui/models/listmodelbase.h"
#include <QAbstractItemModel>
#include <QDBusConnection>
#include <QItemDelegate>
#include <QLineEdit>

namespace BlackGui
{
    namespace Models
    {
        /*!
         * Keyboard key list model
         */
        class CKeyboardKeyListModel : public CListModelBase<BlackMisc::Hardware::CKeyboardKey, BlackMisc::Hardware::CKeyboardKeyList>
        {

        public:

            //! Constructor
            explicit CKeyboardKeyListModel(QObject *parent = nullptr);

            //! Destructor
            virtual ~CKeyboardKeyListModel() {}

            //! \copydoc CListModelBase::data
            QVariant data(const QModelIndex &modelIndex, int role = Qt::DisplayRole) const override;

            //! \copydoc CKeyboardKeyList::initAsHotkeyList(bool reset)
            void initAsHotkeyList() { this->m_container.initAsHotkeyList(); }

            //! \copydoc QAbstractTableModel::setData
            virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
        };

        /*!
         * Special edit delegate for key sequence
         */
        class CKeyboardKeyItemDelegate : public QItemDelegate
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CKeyboardKeyItemDelegate(QObject *parent = nullptr) :
                QItemDelegate(parent) {}

            //! \copydoc QItemDelegate::createEditor
            virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

            //! \copydoc QItemDelegate::setEditorData
            virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;

            //! \copydoc QItemDelegate::setModelData
            virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

            //! \copydoc QItemDelegate::updateEditorGeometry
            virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

            //! correspondig model
            const CKeyboardKeyListModel *model() const;
        };

        /*!
         * Special edit widget for key sequence
         */
        class CKeyboardLineEdit : public QLineEdit
        {
            Q_OBJECT

        public:
            //! Constructor
            CKeyboardLineEdit(BlackMisc::Hardware::CKeyboardKey &key, QWidget *parent = nullptr) :
                QLineEdit(parent), m_key(key) { }

            //! get key
            BlackMisc::Hardware::CKeyboardKey getKey() const { return this->m_key; }

        protected:
            //! Overriding and handling key press
            virtual void keyPressEvent(QKeyEvent *event) override;

        private:
            BlackMisc::Hardware::CKeyboardKey m_key;
        };
    }
}
#endif // guard
