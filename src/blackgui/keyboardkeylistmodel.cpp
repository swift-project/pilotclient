#include "keyboardkeylistmodel.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QMetaProperty>
#include <QComboBox>
#include <QTableView>
#include <QEvent>
#include <QKeyEvent>


using namespace BlackMisc::Hardware;

namespace BlackGui
{
    /*
     * Constructor
     */
    CKeyboardKeyListModel::CKeyboardKeyListModel(QObject *parent) :
        CListModelBase<BlackMisc::Hardware::CKeyboardKey, BlackMisc::Hardware::CKeyboardKeyList>("ViewKeyboardKeyList", parent)
    {
        this->m_columns.addColumn(CColumn("key", CKeyboardKey::IndexKeyAsStringRepresentation, true));
        this->m_columns.addColumn(CColumn("modifier 1", CKeyboardKey::IndexModifier1AsString, true));
        this->m_columns.addColumn(CColumn("modifier 2", CKeyboardKey::IndexModifier2AsString, true));
        this->m_columns.addColumn(CColumn("function", CKeyboardKey::IndexFunctionAsString));

        this->setSortColumnByPropertyIndex(CKeyboardKey::IndexFunctionAsString);
        this->m_sortOrder = Qt::AscendingOrder;

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ViewKeyboardKeyList", "modifier 1");
        (void)QT_TRANSLATE_NOOP("ViewKeyboardKeyList", "modifier 2");
        (void)QT_TRANSLATE_NOOP("ViewKeyboardKeyList", "function");
        (void)QT_TRANSLATE_NOOP("ViewKeyboardKeyList", "key");
    }

    /*
     * Display icons
     */
    QVariant CKeyboardKeyListModel::data(const QModelIndex &modelIndex, int role) const
    {
        // shortcut, fast check
        return CListModelBase::data(modelIndex, role);
    }

    bool CKeyboardKeyListModel::setData(const QModelIndex &index, const QVariant &value, int role)
    {
        if (role == Qt::EditRole)
        {
            int pi = this->indexToPropertyIndex(index);
            if (pi == CKeyboardKey::IndexModifier1 || pi == CKeyboardKey::IndexModifier2 || pi == CKeyboardKey::IndexModifier1AsString || pi == CKeyboardKey::IndexModifier2AsString)
            {
                if (index.row() >= this->m_container.size()) return true;
                CKeyboardKey key = this->m_container[index.row()];
                key.setPropertyByIndex(value, pi);
                key.cleanup();
                this->m_container[index.row()] = key;
                return true;
            }
            else if (pi == CKeyboardKey::IndexKey || pi == CKeyboardKey::IndexKeyAsString || pi == CKeyboardKey::IndexKeyAsStringRepresentation)
            {
                Q_ASSERT(value.canConvert<CKeyboardKey>());
                if (index.row() >= this->m_container.size()) return true;
                CKeyboardKey key = this->m_container[index.row()];
                key.setPropertyByIndex(value, CKeyboardKey::IndexKeyObject);
                key.cleanup();
                this->m_container[index.row()] = key;
                return true;
            }
        }
        return CListModelBase::setData(index, value, role);
    }

    QWidget *CKeyboardKeyItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        const CKeyboardKeyListModel *model = this->model();
        Q_ASSERT(model);

        if (index.row() < model->rowCount())
        {
            int pi = model->indexToPropertyIndex(index);
            if (pi == CKeyboardKey::IndexModifier1 || pi == CKeyboardKey::IndexModifier2 || pi == CKeyboardKey::IndexModifier1AsString || pi == CKeyboardKey::IndexModifier2AsString)
            {
                CKeyboardKey key = model->at(index);
                QString v = (pi == CKeyboardKey::IndexModifier1 || pi == CKeyboardKey::IndexModifier1AsString) ? key.getModifier1AsString() : key.getModifier2AsString();
                QComboBox *edit = new QComboBox(parent);
                edit->addItems(CKeyboardKey::modifiers());
                edit->setCurrentText(v);
                return edit;
            }
            else if (pi == CKeyboardKey::IndexKey || pi == CKeyboardKey::IndexKeyAsString || pi == CKeyboardKey::IndexKeyAsStringRepresentation)
            {
                CKeyboardKey key = model->at(index);
                CKeyboardLineEdit *edit = new CKeyboardLineEdit(key, parent);
                edit->setText(key.getKeyAsString());
                return edit;
            }
        }
        return QItemDelegate::createEditor(parent, option, index);
    }

    void CKeyboardKeyItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        QItemDelegate::setEditorData(editor, index);
    }

    void CKeyboardKeyItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        // with our own line edit we have a special treatment
        // otherwise (comboboxes) standard handling via QItemDelegate
        CKeyboardLineEdit *lineEdit = qobject_cast<CKeyboardLineEdit *>(editor);
        if (lineEdit)
            model->setData(index, lineEdit->getKey().toQVariant() , Qt::EditRole);
        else
            QItemDelegate::setModelData(editor, model, index);
    }

    void CKeyboardKeyItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QItemDelegate::updateEditorGeometry(editor, option, index);
    }

    const CKeyboardKeyListModel *CKeyboardKeyItemDelegate::model() const
    {
        if (this->parent())
        {
            QTableView *tableView = dynamic_cast<QTableView *>(this->parent());
            if (tableView)
            {
                return dynamic_cast<CKeyboardKeyListModel *>(tableView->model());
            }
        }
        return nullptr;
    }

    void CKeyboardLineEdit::keyPressEvent(QKeyEvent *event)
    {
        const Qt::Key k = static_cast<Qt::Key>(event->key());
        if (k == Qt::Key_Shift || k == Qt::Key_Control || k == Qt::Key_Meta || k == Qt::Key_Alt || k == Qt::Key_CapsLock || k == Qt::Key_NumLock || k == Qt::Key_ScrollLock) return;
        this->m_key.setKey(k);
        this->m_key.setNativeVirtualKey(event->nativeVirtualKey());
        this->setText(CKeyboardKey::toStringRepresentation(event->key()));
    }

} // namespace
