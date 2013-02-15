#include "blackmisc/debug.h"

#include "blackcore/fsd_messages.h"

using namespace BlackMisc;

namespace FSD
{
    qint32 FSD_MSG::unpack(const QString &line, QStringVector &tokens)
    {
        const QString separator(":");

        int begin = 0;
        int end = 0;
        int token_position = 0;

		qint32 count = line.count(separator);
		tokens.resize(count+1);

        while ( (end = line.indexOf(separator, begin, Qt::CaseInsensitive)) != -1)
        {
            if (token_position < tokens.size())
            {
                tokens.replace(token_position, line.mid(begin, end - begin));
                begin = end + separator.size();
                ++token_position;
            }
            else
            {
                bError << "Cannot split message. Vector is to small";
                return -1;
            }
        }
        if (begin < line.size() && token_position < tokens.size())
        {
            tokens.replace(token_position, line.mid(begin));
        }
        else
            return -1;

        return token_position + 1;
    }

    QString FSD_MSG::pack(const QStringVector &tokens) const
    {
        QString line;
        int length = 0;
        const QString separator(":");
        const QString line_feed("\r\n");
        const int size = tokens.size();

        // Calculate the total message length
        for (int ii = 0; ii < size; ++ii)
            length += tokens.at(ii).size();

        // Add the size of the separator
        length += separator.size() * (size - 1);

        // Add size of the line feed
        length += line_feed.size();

		// Add size of message ID
		length += getID().size();

        if (length == 0)
            return line;

        line.reserve(length);

		line = getID();
        for (int ii = 0; ii < size; ++ii)
        {
            if (ii)
                line += separator;
            line += tokens.at(ii);
        }

        line += line_feed;

        return line;
    }

    QTextStream& FSD_MSG_Plane_Position::operator<< ( QTextStream& in)
    {
        QString message = in.readAll();
        m_message_tokens.resize(10);
        qint32 size = unpack(message, m_message_tokens);
        bAssert ( size == 10 );

        return in;
    }

    QTextStream& FSD_MSG_Plane_Position::operator>> (QTextStream& out) const
    {
        out << pack(m_message_tokens);
        return out;
    }
}
