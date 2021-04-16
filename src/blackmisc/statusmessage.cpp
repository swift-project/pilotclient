/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/statusmessage.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/iconlist.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/verify.h"

#include <QMetaEnum>
#include <QStringBuilder>
#include <QThreadStorage>

namespace BlackMisc
{
    namespace Private
    {
        QThreadStorage<QString> t_tempBuffer; // thread_local would be destroyed before function-scope statics, see T495

        QString arg(QStringView format, const QStringList &args)
        {
            if (format.isEmpty())
            {
                return args.join(u' ');
            }

            QString &temp = t_tempBuffer.localData();
            temp.resize(0); // unlike clear(), resize(0) doesn't release the capacity if there are no implicitly shared copies

            quint64 unusedArgs = (1ULL << std::min(63, args.size())) - 1;
            for (auto it = format.begin(); ;)
            {
                const auto pc = std::find(it, format.end(), u'%');
                temp.append(&*it, std::distance(it, pc));
                if ((it = pc) == format.end()) { break; }
                if (++it == format.end()) { temp += u'%'; break; }

                if (*it == u'%') { temp += u'%'; ++it; continue; }
                if (is09(*it))
                {
                    int n = it->unicode() - u'0';
                    BLACK_VERIFY(n >= 0 && n <= 9);
                    if (++it != format.end() && is09(*it)) { n = n * 10 + it->unicode() - u'0'; ++it; }
                    BLACK_VERIFY(n > 0 && n <= 99);
                    if (n > 0 && n <= args.size()) { temp += args[n - 1]; unusedArgs &= ~(1ULL << (n - 1)); }
                    else { temp += u'%' % QString::number(n); }
                }
                else { temp += u'%'; }
            }
            if (unusedArgs) { temp += QStringLiteral(" [SOME MESSAGE ARGUMENT(S) UNUSED]"); }

            QString result = temp;
            result.squeeze(); // release unused capacity and implicitly detach so temp keeps its capacity for next time
            return result;
        }
    }

    CStatusMessage::CStatusMessage(const CLogCategory &category) :
        CMessageBase(category), ITimestampBased(QDateTime::currentMSecsSinceEpoch())
    {}

    CStatusMessage::CStatusMessage(const CLogCategoryList &categories) :
        CMessageBase(categories), ITimestampBased(QDateTime::currentMSecsSinceEpoch())
    {}

    CStatusMessage::CStatusMessage(const CLogCategoryList &categories, const CLogCategory &extra) :
        CMessageBase(categories, extra), ITimestampBased(QDateTime::currentMSecsSinceEpoch())
    {}

    CStatusMessage::CStatusMessage(const CLogCategoryList &categories, const CLogCategoryList &extra) :
        CMessageBase(categories, extra), ITimestampBased(QDateTime::currentMSecsSinceEpoch())
    {}

    CStatusMessage::CStatusMessage(): ITimestampBased(QDateTime::currentMSecsSinceEpoch())
    {}

    CStatusMessage::CStatusMessage(const CStatusMessage &other) :
        CValueObject(other),
        CMessageBase(other),
        ITimestampBased(other),
        IOrderable(other)
    {
        QReadLocker lock(&other.m_lock);
        m_handledByObjects = other.m_handledByObjects;
    }

    CStatusMessage &CStatusMessage::operator =(const CStatusMessage &other)
    {
        if (this == &other) { return *this; }

        static_cast<CMessageBase &>(*this) = other;
        static_cast<ITimestampBased &>(*this) = other;
        static_cast<IOrderable &>(*this) = other;

        // locks because of mutable member
        QReadLocker readLock(&other.m_lock);
        const auto handledBy = other.m_handledByObjects;
        readLock.unlock(); // avoid deadlock

        QWriteLocker writeLock(&m_lock);
        m_handledByObjects = handledBy;
        return *this;
    }

    CStatusMessage::CStatusMessage(QStringView message) : ITimestampBased(QDateTime::currentMSecsSinceEpoch())
    {
        m_message = CStrongStringView(message.trimmed());
    }

    CStatusMessage::CStatusMessage(const QString &message) : ITimestampBased(QDateTime::currentMSecsSinceEpoch())
    {
        m_message = message.trimmed();
    }

    CStatusMessage::CStatusMessage(StatusSeverity severity, QStringView message)
        : CStatusMessage(message)
    {
        m_severity = severity;
    }

    CStatusMessage::CStatusMessage(StatusSeverity severity, const QString &message)
        : CStatusMessage(message)
    {
        m_severity = severity;
    }

    CStatusMessage::CStatusMessage(const CLogCategoryList &categories, StatusSeverity severity, QStringView message, bool validation)
        : CStatusMessage(severity, message)
    {
        m_categories = categories;
        if (validation)
        {
            this->addValidationCategory();
        }
    }

    CStatusMessage::CStatusMessage(const CLogCategoryList &categories, StatusSeverity severity, const QString &message, bool validation)
        : CStatusMessage(severity, message)
    {
        m_categories = categories;
        if (validation)
        {
            this->addValidationCategory();
        }
    }

    CStatusMessage::CStatusMessage(QtMsgType type, const QMessageLogContext &context, const QString &message)
        : CStatusMessage(message.trimmed())
    {
        m_categories = CLogCategoryList::fromQString(context.category);

        switch (type)
        {
        default:
        case QtDebugMsg:   m_severity = SeverityDebug; break;
        case QtInfoMsg:    m_severity = SeverityInfo; break;
        case QtWarningMsg: m_severity = SeverityWarning; break;
        case QtCriticalMsg:
        case QtFatalMsg:
            m_severity = SeverityError;
            break;
        }
    }

    void CStatusMessage::toQtLogTriple(QtMsgType *o_type, QString *o_category, QString *o_message) const
    {
        *o_category = m_categories.toQString();
        *o_message = this->getMessage();

        switch (m_severity)
        {
        default:
        case SeverityDebug:   *o_type = QtDebugMsg; break;
        case SeverityInfo:    *o_type = QtInfoMsg; break;
        case SeverityWarning: *o_type = QtWarningMsg; break;
        case SeverityError:   *o_type = QtCriticalMsg; break;
        }
    }

    QString CStatusMessage::getCategoriesAsString() const
    {
        return m_categories.toQString();
    }

    bool CStatusMessage::clampSeverity(CStatusMessage::StatusSeverity severity)
    {
        if (this->getSeverity() <= severity) { return false; }
        this->setSeverity(severity);
        return true;
    }

    bool CStatusMessage::isSeverityHigherOrEqual(CStatusMessage::StatusSeverity severity) const
    {
        return this->getSeverity() >= severity;
    }

    bool CStatusMessage::isSuccess() const
    {
        return !this->isFailure();
    }

    bool CStatusMessage::isFailure() const
    {
        return this->getSeverity() == SeverityError;
    }

    QString CStatusMessage::getMessageNoLineBreaks() const
    {
        const QString m = this->getMessage();
        if (!containsLineBreakOrTab(m)) { return m; } // by far most messages will NOT contain tabs/CR
        return removeLineBreakAndTab(m);
    }

    void CStatusMessage::prependMessage(const QString &msg)
    {
        if (msg.isEmpty()) { return; }
        m_message = QString(msg % m_message.view());
    }

    void CStatusMessage::appendMessage(const QString &msg)
    {
        if (msg.isEmpty()) { return; }
        m_message = QString(m_message.view() % msg);
    }

    void CStatusMessage::markAsHandledBy(const QObject *object) const
    {
        QWriteLocker lock(&m_lock);
        m_handledByObjects.push_back(quintptr(object));
    }

    bool CStatusMessage::wasHandledBy(const QObject *object) const
    {
        QReadLocker lock(&m_lock);
        return m_handledByObjects.contains(quintptr(object));
    }

    QString CStatusMessage::convertToQString(bool /** i18n */) const
    {
        return u"Category: " %
               m_categories.toQString() %

               u" Severity: " %
               severityToString(m_severity) %

               u" when: " %
               this->getFormattedUtcTimestampYmdhms() %

               u' ' %
               this->getMessage();
    }

    const CIcon &CStatusMessage::convertToIcon(const CStatusMessage &statusMessage)
    {
        return convertToIcon(statusMessage.getSeverity());
    }

    const CIcon &CStatusMessage::convertToIcon(CStatusMessage::StatusSeverity severity)
    {
        switch (severity)
        {
        case SeverityDebug:   return CIcon::iconByIndex(CIcons::StandardIconUnknown16); // TODO
        case SeverityInfo:    return CIcon::iconByIndex(CIcons::StandardIconInfo16);
        case SeverityWarning: return CIcon::iconByIndex(CIcons::StandardIconWarning16);
        case SeverityError:   return CIcon::iconByIndex(CIcons::StandardIconError16);
        default: return CIcon::iconByIndex(CIcons::StandardIconInfo16);
        }
    }

    const QString &CStatusMessage::convertToIconResource(CStatusMessage::StatusSeverity severity)
    {
        static const QString d;
        static const QString i(":/pastel/icons/pastel/16/infomation.png");
        static const QString w(":/pastel/icons/pastel/16/bullet-error.png");
        static const QString e(":/pastel/icons/pastel/16/close-red.png");

        switch (severity)
        {
        case SeverityDebug:   return d;
        case SeverityInfo:    return i;
        case SeverityWarning: return w;
        case SeverityError:   return e;
        default: return d;
        }
    }

    CStatusMessage CStatusMessage::fromDatabaseJson(const QJsonObject &json)
    {
        const QString msgText(json.value("text").toString());
        const QString severityText(json.value("severity").toString());
        QString typeText(json.value("type").toString());
        StatusSeverity severity = stringToSeverity(severityText);

        typeText = u"swift.db.type." % typeText.toLower().remove(' ');
        const CStatusMessage m({ CLogCategories::swiftDbWebservice(), typeText}, severity, msgText);
        return m;
    }

    CStatusMessage CStatusMessage::fromJsonException(const CJsonException &ex, const CLogCategoryList &categories, const QString &prefix)
    {
        return CStatusMessage(categories).validationError(ex.toString(prefix));
    }

    void CStatusMessage::registerMetadata()
    {
        CValueObject<CStatusMessage>::registerMetadata();
        qRegisterMetaType<CStatusMessage::StatusSeverity>();
        qDBusRegisterMetaType<CStatusMessage::StatusSeverity>();
        qRegisterMetaTypeStreamOperators<CStatusMessage::StatusSeverity>();
    }

    CStatusMessage::StatusSeverity CStatusMessage::stringToSeverity(const QString &severity)
    {
        // pre-check
        QString severityString(severity.trimmed().toLower());
        if (severityString.isEmpty()) { return SeverityInfo; }

        // hard check
        if (severityString.compare(severityToString(SeverityDebug), Qt::CaseInsensitive) == 0) { return SeverityDebug; }
        if (severityString.compare(severityToString(SeverityInfo), Qt::CaseInsensitive) == 0) { return SeverityInfo; }
        if (severityString.compare(severityToString(SeverityWarning), Qt::CaseInsensitive) == 0) { return SeverityWarning; }
        if (severityString.compare(severityToString(SeverityError), Qt::CaseInsensitive) == 0) { return SeverityError; }

        // not found yet, lenient checks
        QChar s = severityString.at(0);
        if (s == 'd') { return SeverityDebug; }
        if (s == 'i') { return SeverityInfo; }
        if (s == 'w') { return SeverityWarning; }
        if (s == 'e') { return SeverityError; }

        return SeverityInfo;
    }

    const QString &CStatusMessage::severityToString(CStatusMessage::StatusSeverity severity)
    {
        switch (severity)
        {
        case SeverityDebug:   { static const QString d("debug");   return d; }
        case SeverityInfo:    { static const QString i("info");    return i; }
        case SeverityWarning: { static const QString w("warning"); return w; }
        case SeverityError:   { static const QString e("error");   return e; }
        default:
            {
                static const QString x("unknown severity");
                qFatal("Unknown severity");
                return x; // just for compiler warning
            }
        }
    }

    QString CStatusMessage::severitiesToString(const QSet<CStatusMessage::StatusSeverity> &severities)
    {
        if (severities.isEmpty()) { return {}; }
        auto minmax = std::minmax_element(severities.begin(), severities.end());
        auto min = *minmax.first;
        auto max = *minmax.second;
        if (min == SeverityDebug && max == SeverityError) { static const QString all("all severities"); return all; }
        if (min == SeverityDebug) { return u"at or below " % severityToString(max); }
        if (max == SeverityError) { return u"at or above " % severityToString(min); }
        auto list = severities.values();
        std::sort(list.begin(), list.end());
        QStringList ret;
        std::transform(list.cbegin(), list.cend(), std::back_inserter(ret), severityToString);
        return ret.join("|");
    }

    const QString &CStatusMessage::getSeverityAsString() const
    {
        return severityToString(m_severity);
    }

    const CIcon &CStatusMessage::getSeverityAsIcon() const
    {
        return convertToIcon(m_severity);
    }

    const QStringList &CStatusMessage::allSeverityStrings()
    {
        static const QStringList all { severityToString(SeverityDebug), severityToString(SeverityInfo), severityToString(SeverityWarning), severityToString(SeverityError) };
        return all;
    }

    QVariant CStatusMessage::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }
        if (IOrderable::canHandleIndex(index))      { return IOrderable::propertyByIndex(index); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexMessage: return QVariant::fromValue(this->getMessage());
        case IndexSeverity: return QVariant::fromValue(m_severity);
        case IndexSeverityAsString: return QVariant::fromValue(this->getSeverityAsString());
        case IndexSeverityAsIcon: return QVariant::fromValue(this->getSeverityAsIcon());
        case IndexCategoriesAsString: return QVariant::fromValue(m_categories.toQString());
        case IndexMessageAsHtml: return QVariant::fromValue(this->toHtml(false, true));
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CStatusMessage::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself()) { (*this) = variant.value<CStatusMessage>(); return; }
        if (ITimestampBased::canHandleIndex(index)) { ITimestampBased::setPropertyByIndex(index, variant); return; }
        if (IOrderable::canHandleIndex(index))      { IOrderable::setPropertyByIndex(index, variant); return; }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexMessage:
            m_message = variant.value<QString>();
            m_args.clear();
            break;
        case IndexSeverity: m_severity = variant.value<StatusSeverity>(); break;
        case IndexCategoriesAsString: m_categories = variant.value<CLogCategoryList>();  break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    int CStatusMessage::comparePropertyByIndex(CPropertyIndexRef index, const CStatusMessage &compareValue) const
    {
        if (index.isMyself()) { return Compare::compare(this->getSeverity(), compareValue.getSeverity()); }
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::comparePropertyByIndex(index, compareValue); }
        if (IOrderable::canHandleIndex(index))      { return IOrderable::comparePropertyByIndex(index, compareValue); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexMessageAsHtml:
        case IndexMessage: return this->getMessage().compare(compareValue.getMessage());
        case IndexSeverityAsString:
        case IndexSeverityAsIcon:
        case IndexSeverity:           return Compare::compare(this->getSeverity(), compareValue.getSeverity());
        case IndexCategoriesAsString: return this->getCategoriesAsString().compare(compareValue.getCategoriesAsString());
        default: break;
        }
        return CValueObject::comparePropertyByIndex(index, compareValue);
    }

    QString CStatusMessage::toHtml(bool withIcon, bool withColors) const
    {
        QString img;
        if (withIcon)
        {
            const QString r = convertToIconResource(this->getSeverity());
            if (!r.isEmpty()) { img = QStringLiteral("<img src=\"%1\"> ").arg(r); }
        }

        if (withColors)
        {
            switch (this->getSeverity())
            {
            case SeverityWarning: return img % u"<font color=\"yellow\">" % this->getMessage() % u"</font>";
            case SeverityError:   return img % u"<font color=\"red\">" % this->getMessage() % u"</font>";
            case SeverityDebug: break;
            default: break;
            }
        }
        return img % this->getMessage();
    }
} // ns
