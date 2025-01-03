// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_URL_H
#define SWIFT_MISC_NETWORK_URL_H

#include <QJsonObject>
#include <QMetaType>
#include <QNetworkRequest>
#include <QString>
#include <QUrl>

#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::network, CUrl)

namespace swift::misc::network
{
    //! Value object encapsulating information of a location,
    //! kind of simplified CValueObject compliant version of QUrl
    class SWIFT_MISC_EXPORT CUrl : public CValueObject<CUrl>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexScheme = CPropertyIndexRef::GlobalIndexCUrl,
            IndexHost,
            IndexPort,
            IndexPath,
            IndexQuery
        };

        //! Default constructor.
        CUrl(const QString &fullUrl = QString());

        //! Construct from character array.
        CUrl(const char *url);

        //! By QUrl.
        CUrl(const QUrl &url);

        //! Constructor.
        CUrl(const QString &address, int port);

        //! Constructor.
        CUrl(const QString &scheme, const QString &address, int port, const QString &path);

        //! Get host.
        const QString &getHost() const { return m_host; }

        //! Set address (e.g. myserver.foo.com)
        void setHost(const QString &address) { m_host = address.trimmed(); }

        //! Get protocl
        const QString &getScheme() const { return m_scheme; }

        //! Set protocol
        void setScheme(const QString &protocol);

        //! Protocol
        bool hasScheme() const { return !m_scheme.isEmpty(); }

        //! Get path
        const QString &getPath() const { return m_path; }

        //! Set path
        void setPath(const QString &path);

        //! Append path
        QString appendPath(const QString &pathToAppend);

        //! Has path?
        bool hasPath() const { return !m_path.isEmpty(); }

        //! Get port
        int getPort() const;

        //! Set port
        void setPort(int port) { m_port = port; }

        //! Port?
        bool hasPort() const;

        //! Default port
        bool hasDefaultPort() const;

        //! Get query part
        QString getQuery() const { return m_query; }

        //! Set query
        void setQuery(const QString &query);

        //! Query string?
        bool hasQuery() const;

        //! Append query
        void appendQuery(const QString &queryToAppend);

        //! Append query
        void appendQuery(const QString &key, const QString &value);

        //! Fragment string?
        bool hasFragment() const;

        //! Set fragment
        void setFragment(const QString &fragment);

        //! Empty
        bool isEmpty() const;

        //! Qualified name
        QString getFullUrl(bool withQuery = true) const;

        //! Set full URL
        void setFullUrl(const QString &fullUrl);

        //! File name
        QString getFileName() const;

        //! To QUrl
        QUrl toQUrl() const;

        //! To QUrl
        void setQUrl(const QUrl &url);

        //! To request
        QNetworkRequest toNetworkRequest() const;

        //! Append path
        CUrl withAppendedPath(const QString &path) const;

        //! Append path
        CUrl withAppendedQuery(const QString &query) const;

        //! Switch protocol
        CUrl withSwitchedScheme(const QString &protocol, int port) const;

        //! Path ending with?
        bool pathEndsWith(const QString &ending, Qt::CaseSensitivity cs = Qt::CaseInsensitive) const;

        //! A path ends with "/", and file is anythingy beyond that, e.g. "path/file"
        //! \sa isFile with appendix
        bool isFile() const;

        //! Stricter version of isFile()
        bool isFileWithSuffix(const QString &suffix = {}) const;

        //! File appendix if any, otherwise empty, does not include the "."
        //! \remark similar to QFileInfo::suffix()
        QString getFileSuffix() const;

        //! Suffix plus dot
        QString getFileSuffixPlusDot() const;

        //! Likely a HTM file?
        bool isHavingHtmlSuffix() const;

        //! \copydoc swift::misc::CFileUtils::isExecutableFile
        bool isExecutableFile() const;

        //! \copydoc swift::misc::CFileUtils::isSwiftInstaller
        bool isSwiftInstaller() const;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::String::toQString()
        QString convertToQString(bool i18n = false) const;

        //! \copydoc swift::misc::mixin::JsonByMetaClass::toJson
        QJsonObject toJson() const;

        //! \copydoc swift::misc::mixin::JsonByMetaClass::convertFromJson
        void convertFromJson(const QJsonObject &json);

        //! Protocol to default port
        static int protocolToDefaultPort(const QString &protocol);

        //! Default port for given protocol
        static bool isDefaultPort(const QString &protocol, int port);

        //! From local file path
        static CUrl fromLocalFile(const QString &localFile);

        //! Convert to QUrl
        operator QUrl() const { return this->toQUrl(); }

    private:
        QString m_scheme;
        QString m_host;
        int m_port = -1;
        QString m_path;
        QString m_query;
        QString m_fragment;

        static QString stripQueryString(const QString &query);

        SWIFT_METACLASS(
            CUrl,
            SWIFT_METAMEMBER(scheme),
            SWIFT_METAMEMBER(host),
            SWIFT_METAMEMBER(port),
            SWIFT_METAMEMBER(path),
            SWIFT_METAMEMBER(query),
            SWIFT_METAMEMBER(fragment));
    };
} // namespace swift::misc::network

Q_DECLARE_METATYPE(swift::misc::network::CUrl)

#endif // SWIFT_MISC_NETWORK_URL_H
