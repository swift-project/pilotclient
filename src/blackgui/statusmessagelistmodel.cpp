#include "statusmessagelistmodel.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QMetaProperty>
#include <QBrush>
#include <QIcon>

using namespace BlackMisc;

namespace BlackGui
{
    /*
     * Constructor
     */
    CStatusMessageListModel::CStatusMessageListModel(QObject *parent) :
        CListModelBase<BlackMisc::CStatusMessage, BlackMisc::CStatusMessageList>("ViewStatusMessageList", parent)
    {
        this->m_columns.addColumn(CStatusMessage::IndexTimestamp, "time");
        this->m_columns.addColumn(CStatusMessage::IndexSeverity, "", Qt::AlignCenter);
        this->m_columns.addColumn(CStatusMessage::IndexMessage, "message");
        this->m_columns.addColumn(CStatusMessage::IndexTypeAsString, "type");

        this->m_sortedColumn = CStatusMessage::IndexTimestamp;
        this->m_sortOrder = Qt::DescendingOrder;

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ViewStatusMessageList", "time");
        (void)QT_TRANSLATE_NOOP("ViewStatusMessageList", "severity");
        (void)QT_TRANSLATE_NOOP("ViewStatusMessageList", "type");
        (void)QT_TRANSLATE_NOOP("ViewStatusMessageList", "message");
    }

    /*
     * Display icons
     */
    QVariant CStatusMessageListModel::data(const QModelIndex &modelIndex, int role) const
    {
        // shortcut, fast check
        if (role != Qt::DisplayRole && role != Qt::DecorationRole) return CListModelBase::data(modelIndex, role);

        static QPixmap w(QPixmap(":/blackgui/iconsQt/warning.png").scaledToWidth(16, Qt::SmoothTransformation));
        static QPixmap e(QPixmap(":/blackgui/iconsQt/critical.png").scaledToWidth(16, Qt::SmoothTransformation));
        static QPixmap i(QPixmap(":/blackgui/iconsQt/information.png").scaledToWidth(16, Qt::SmoothTransformation));

        if (this->columnToPropertyIndex(modelIndex.column()) == CStatusMessage::IndexSeverity)
        {
            if (role == Qt::DecorationRole)
            {
                CStatusMessage msg = this->at(modelIndex);
                switch (msg.getSeverity())
                {
                case CStatusMessage::SeverityError: return e;
                case CStatusMessage::SeverityWarning: return w;
                default: return i;
                }
            }
            else if (role == Qt::DisplayRole)
            {
                // the text itself should be empty
                return QVariant("");
            }
            else if (role == Qt::ToolTipRole)
            {
                CStatusMessage msg = this->at(modelIndex);
                return QVariant(msg.getSeverityAsString());
            }
        }
        return CListModelBase::data(modelIndex, role);
    }
}
