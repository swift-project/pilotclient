//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef GUI_MESSAGES_H
#define GUI_MESSAGES_H

#include "blackmisc/message.h"

namespace BlackMisc
{
	class MSG_CONNECT_TO_VATSIM : public IMessage
    {
    public:
        MSG_CONNECT_TO_VATSIM() :  IMessage(QString("MSG_ID_CONNECT_TO_VATSIM"))
        {
        }

        QString getHost () const { return m_host; }
		quint16 getPort () const { return m_port; }
		QString getCallsign () const { return m_callsign; }
		QString getUserID () const { return m_userid; }
		QString getPassword () const { return m_password; }
		QString getRealName () const { return m_realName; }
		
		void setHost (const QString &host) { m_host = host; }
		void setPort (const quint16 &port) { m_port = port; }
		void setCallsign (const QString &callsign) { m_callsign = callsign; }
		void setUserID (const QString &id) { m_userid = id; }
		void setPassword (const QString &password) { m_password = password; }
		void setRealName (const QString &realname) { m_realName = realname; }

        virtual QDataStream& operator<< ( QDataStream& in)
        {
            in >> m_message_id;
            in >> m_host;
			in >> m_port;
			in >> m_callsign;
			in >> m_userid;
			in >> m_password;
			in >> m_realName;
            return in;
        }

        virtual QDataStream& operator>> (QDataStream& out) const
        {
            out << m_message_id;
            out << m_host;
			out << m_port;
			out << m_callsign;
			out << m_userid;
			out << m_password;
			out << m_realName;
            return out;
        }

        virtual QTextStream& operator<< ( QTextStream& in) { return in; }
        virtual QTextStream& operator>> (QTextStream& out) const { return out; }

    protected:

    private:
        QString                 m_host;
		quint16                 m_port;
		QString                 m_callsign;
		QString                 m_userid;
		QString                 m_password;
		QString                 m_realName;
    };

	class MSG_CHAT_MESSAGE : public IMessage
    {
    public:
        MSG_CHAT_MESSAGE() :  IMessage(QString("MSG_ID_CHAT_MESSAGE"))
        {
        }

		void setSource (const QString &source) { m_source = source; }
		void setDestination (const QString &destination) { m_destination = destination; }
		void setText (const QString &text) { m_source = text; }

		QString getSource() const {return m_source;}
		QString getDestination() const {return m_destination;}
		QString getText() const {return m_text;}

        virtual QDataStream& operator<< ( QDataStream& in)
        {
            in >> m_message_id;
			in >> m_source;
			in >> m_destination;
            return in;
        }

        virtual QDataStream& operator>> (QDataStream& out) const
        {
            out << m_message_id;
			out << m_source;
			out << m_destination;
            return out;
        }

        virtual QTextStream& operator<< ( QTextStream& in) { return in; }
        virtual QTextStream& operator>> (QTextStream& out) const { return out; }

    protected:

    private:
		QString m_source;
		QString m_destination;
		QString m_text;
    };
}

#endif // GUI_MESSAGES_H
