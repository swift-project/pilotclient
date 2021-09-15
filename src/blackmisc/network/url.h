/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_URL_H
#define BLACKMISC_NETWORK_URL_H

#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/blackmiscexport.h"

#include <QJsonObject>
#include <QMetaType>
#include <QNetworkRequest>
#include <QString>
#include <QUrl>

namespace BlackMisc::Network
{
    //! Value object encapsulating information of a location,
    //! kind of simplied CValueObject compliant version of QUrl
    class BLACKMISC_EXPORT CUrl : public CValueObject<CUrl>
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

        //! \copydoc BlackMisc::CFileUtils::isExecutableFile
        bool isExecutableFile() const;

        //! \copydoc BlackMisc::CFileUtils::isSwiftInstaller
        bool isSwiftInstaller() const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc BlackMisc::Mixin::String::toQString()
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::JsonByMetaClass::toJson
        QJsonObject toJson() const;

        //! \copydoc BlackMisc::Mixin::JsonByMetaClass::convertFromJson
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
        int     m_port = -1;
        QString m_path;
        QString m_query;

        static QString stripQueryString(const QString &query);

        BLACK_METACLASS(
            CUrl,
            BLACK_METAMEMBER(scheme),
            BLACK_METAMEMBER(host),
            BLACK_METAMEMBER(port),
            BLACK_METAMEMBER(path),
            BLACK_METAMEMBER(query)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CUrl)

#endif // guard
