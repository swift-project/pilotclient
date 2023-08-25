// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/network/url.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/propertyindexref.h"

#include <QJsonValue>
#include <QPair>
#include <QtGlobal>

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::Network, CUrl)

namespace BlackMisc::Network
{
    CUrl::CUrl(const QString &fullUrl)
    {
        if (!fullUrl.isEmpty())
        {
            setFullUrl(fullUrl);
        }
    }

    CUrl::CUrl(const char *url) : CUrl(QString(url))
    {}

    CUrl::CUrl(const QUrl &url)
    {
        this->setQUrl(url);
    }

    CUrl::CUrl(const QString &address, int port) : CUrl("", address, port, "")
    {}

    CUrl::CUrl(const QString &scheme, const QString &address, int port, const QString &path)
        : m_host(address.trimmed()), m_port(port), m_path(path.trimmed())
    {
        this->setScheme(scheme);
    }

    void CUrl::setScheme(const QString &protocol)
    {
        m_scheme = protocol.trimmed().toLower().replace("://", "");
    }

    void CUrl::setPath(const QString &path)
    {
        m_path = path.simplified();
    }

    QString CUrl::appendPath(const QString &pathToAppend)
    {
        if (pathToAppend.isEmpty()) { return m_path; }
        const QString p = CFileUtils::appendFilePaths(this->getPath(), pathToAppend);
        this->setPath(p);
        return m_path;
    }

    int CUrl::getPort() const
    {
        if (m_port > 0) { return m_port; }
        return protocolToDefaultPort(this->getScheme());
    }

    bool CUrl::hasPort() const
    {
        return m_port >= 0;
    }

    bool CUrl::isEmpty() const
    {
        return m_host.isEmpty();
    }

    bool CUrl::hasDefaultPort() const
    {
        return isDefaultPort(m_scheme, m_port);
    }

    void CUrl::setQuery(const QString &query)
    {
        const QString q(stripQueryString(query));
        m_query = q;
    }

    bool CUrl::hasQuery() const
    {
        return !m_query.isEmpty();
    }

    void CUrl::appendQuery(const QString &queryToAppend)
    {
        if (queryToAppend.isEmpty()) { return; }
        const QString q(stripQueryString(queryToAppend));
        if (q.isEmpty()) { return; }
        m_query += hasQuery() ? "&" + q : q;
    }

    void CUrl::appendQuery(const QString &key, const QString &value)
    {
        if (key.isEmpty()) { return; }
        this->appendQuery(key + "=" + value);
    }

    QString CUrl::getFullUrl(bool withQuery) const
    {
        if (m_host.isEmpty()) { return {}; }

        QString qn(m_host);
        if (!hasDefaultPort() && hasPort()) { qn = qn.append(":").append(QString::number(m_port)); }
        if (hasPath()) { qn = qn.append("/").append(m_path).replace("//", "/"); }
        if (hasQuery() && withQuery) { qn = qn.append("?").append(m_query); }
        if (hasScheme()) { qn = QString(this->getScheme()).append("://").append(qn); }
        return qn;
    }

    void CUrl::setFullUrl(const QString &fullUrl)
    {
        setQUrl(QUrl(fullUrl));
    }

    QString CUrl::getFileName() const
    {
        return toQUrl().fileName();
    }

    QUrl CUrl::toQUrl() const
    {
        return QUrl(getFullUrl());
    }

    void CUrl::setQUrl(const QUrl &url)
    {
        this->setPort(url.port());
        this->setHost(url.host());
        this->setScheme(url.scheme());
        this->setPath(url.path());
        this->setQuery(url.query());
    }

    QNetworkRequest CUrl::toNetworkRequest() const
    {
        return CNetworkUtils::getSwiftNetworkRequest(*this);
    }

    CUrl CUrl::withAppendedPath(const QString &path) const
    {
        if (path.isEmpty()) { return *this; }
        CUrl url(*this);
        url.appendPath(path);
        return url;
    }

    CUrl CUrl::withSwitchedScheme(const QString &scheme, int port) const
    {
        if (getPort() == port && getScheme() == scheme) { return *this; }
        QUrl qurl(this->toQUrl());
        qurl.setPort(port);
        qurl.setScheme(scheme);
        const CUrl url(qurl);
        return url;
    }

    bool CUrl::pathEndsWith(const QString &ending, Qt::CaseSensitivity cs) const
    {
        return m_path.endsWith(ending, cs);
    }

    bool CUrl::isFile() const
    {
        const QString f(this->getFileName());
        return !f.isEmpty();
    }

    bool CUrl::isFileWithSuffix(const QString &suffix) const
    {
        const QString f(this->getFileName());
        if (suffix.isEmpty()) { return f.contains('.'); }
        return f.endsWith(suffix, Qt::CaseInsensitive);
    }

    bool CUrl::isExecutableFile() const
    {
        return CFileUtils::isExecutableFile(this->getFileName());
    }

    bool CUrl::isSwiftInstaller() const
    {
        return CFileUtils::isSwiftInstaller(this->getFileName());
    }

    QString CUrl::getFileSuffix() const
    {
        const QString f(this->getFileName());
        const int i = f.lastIndexOf('.');
        if (i < 0) return {};
        if (f.length() <= i + 1) return {}; // ends with "."
        return f.mid(i + 1); // suffix without dot
    }

    QString CUrl::getFileSuffixPlusDot() const
    {
        const QString f(this->getFileName());
        const int i = f.lastIndexOf('.');
        if (i < 0) return {};
        if (f.length() <= i + 1) return {}; // ends with "."
        return f.mid(i); // suffix with dot
    }

    bool CUrl::isHavingHtmlSuffix() const
    {
        static const QString h1(".html");
        static const QString h2(".htm");
        return this->isFileWithSuffix(h1) || this->isFileWithSuffix(h2);
    }

    CUrl CUrl::withAppendedQuery(const QString &query) const
    {
        if (query.isEmpty()) { return *this; }
        CUrl url(*this);
        url.appendQuery(query);
        return url;
    }

    QString CUrl::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return getFullUrl();
    }

    QJsonObject CUrl::toJson() const
    {
        const QPair<QString, QJsonValue> v("url", getFullUrl());
        const QJsonObject json({ v });
        return json;
    }

    void CUrl::convertFromJson(const QJsonObject &json)
    {
        const QJsonValue value = json.value("url");
        if (value.isUndefined()) { throw CJsonException("Missing 'url'"); }
        this->setFullUrl(value.toString());
    }

    int CUrl::protocolToDefaultPort(const QString &protocol)
    {
        const QString p(protocol.trimmed().toLower());
        if (p == "ftp") return 20;
        if (p == "https") return 443;
        if (p == "http") return 80;
        return -1;
    }

    bool CUrl::isDefaultPort(const QString &protocol, int port)
    {
        const int p = protocolToDefaultPort(protocol);
        if (p < 0) { return false; }
        return port == p;
    }

    CUrl CUrl::fromLocalFile(const QString &localFile)
    {
        return QUrl::fromLocalFile(localFile);
    }

    QString CUrl::stripQueryString(const QString &query)
    {
        QString q(query.trimmed());
        if (q.startsWith("?") || q.startsWith("&"))
        {
            q = q.mid(1);
        }
        if (q.endsWith("?") || q.endsWith("&"))
        {
            q = q.left(q.size() - 1);
        }
        return q;
    }

    QVariant CUrl::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexHost: return QVariant::fromValue(m_host);
        case IndexPort: return QVariant::fromValue(m_port);
        case IndexScheme: return QVariant::fromValue(m_scheme);
        case IndexPath: return QVariant::fromValue(m_path);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CUrl::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CUrl>();
            return;
        }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexHost: this->setHost(variant.value<QString>()); break;
        case IndexPort: this->setPort(variant.value<qint32>()); break;
        case IndexPath: this->setPath(variant.value<QString>()); break;
        case IndexScheme: this->setScheme(variant.value<QString>()); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }
} // namespace
