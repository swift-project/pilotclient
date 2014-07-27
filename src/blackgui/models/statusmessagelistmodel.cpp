#include "statusmessagelistmodel.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QMetaProperty>
#include <QBrush>
#include <QIcon>

using namespace BlackMisc;

namespace BlackGui
{
    namespace Models
    {
        /*
         * Constructor
         */
        CStatusMessageListModel::CStatusMessageListModel(QObject *parent) :
            CListModelBase<BlackMisc::CStatusMessage, BlackMisc::CStatusMessageList>("ViewStatusMessageList", parent)
        {
            this->m_columns.addColumn(CColumn("time", CStatusMessage::IndexTimestamp));
            this->m_columns.addColumn(CColumn("", CStatusMessage::IndexSeverity, Qt::AlignCenter));
            this->m_columns.addColumn(CColumn("message", CStatusMessage::IndexMessage));
            this->m_columns.addColumn(CColumn("type", CStatusMessage::IndexTypeAsString));

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
            if (this->columnToPropertyIndex(modelIndex.column()) == CStatusMessage::IndexSeverity)
            {
                if (role == Qt::DecorationRole)
                {
                    CStatusMessage msg = this->at(modelIndex);
                    return msg.toIcon();
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
}
