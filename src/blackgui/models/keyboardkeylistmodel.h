/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_KEYBOARDKEYLISTMODEL_H
#define BLACKGUI_KEYBOARDKEYLISTMODEL_H

#include "blackmisc/hwkeyboardkeylist.h"
#include "blackmisc/setkeyboardhotkeylist.h"
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
        class CKeyboardKeyListModel : public CListModelBase<BlackMisc::Settings::CSettingKeyboardHotkey, BlackMisc::Settings::CSettingKeyboardHotkeyList>
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
            CKeyboardLineEdit(const BlackMisc::Settings::CSettingKeyboardHotkey &hotkey, QWidget *parent = nullptr) :
                QLineEdit(parent), m_hotkey(hotkey) { }

            //! get key
            const BlackMisc::Settings::CSettingKeyboardHotkey &getKey() const { return this->m_hotkey; }

        protected:
            //! Overriding and handling key press
            virtual void keyPressEvent(QKeyEvent *event) override;

        private:
            BlackMisc::Settings::CSettingKeyboardHotkey m_hotkey;
        };
    }
}
#endif // guard
