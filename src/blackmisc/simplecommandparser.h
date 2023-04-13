/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMPLECOMMANDPARSER_H
#define BLACKMISC_SIMPLECOMMANDPARSER_H

#include "blackmisc/blackmiscexport.h"
#include <QString>
#include <QStringList>

namespace BlackMisc
{
    //! \defgroup swiftdotcommands dot commands

    //! Utility methods for simple line parsing used with the command line
    //! \remarks case insensitive parsing, commands start with . as ".msg"
    class BLACKMISC_EXPORT CSimpleCommandParser
    {
    public:
        //! Constructor
        CSimpleCommandParser(const QStringList &knownCommands);

        //! Known command?
        bool isKnownCommand() const { return m_knownCommand; }

        //! Parse
        void parse(const QString &commandLine);

        //! Is given command current command of command line
        bool matchesCommand(const QString &checkCommand, const QString &alias1 = "", const QString &alias2 = "");

        //! Command starting with pattern?
        bool commandStartsWith(const QString &startPattern) const;

        //! Command ending with pattern?
        bool commandEndsWith(const QString &endPattern) const;

        //! Get part, 0 is command itself
        const QString &part(int index) const;

        //! Part and remaing string after
        //! \remark trimmed and simlified
        QString partAndRemainingStringAfter(int index) const;

        //! Count parts
        int countParts() const;

        //! Existing part
        bool hasPart(int index) const;

        //! Count parts, command excluded
        int countPartsWithoutCommand() const;

        //! Is part an integer?
        bool isInt(int index) const;

        //! Is part a double?
        bool isDouble(int index) const;

        //! Part as integer
        int toInt(int index, int def = -1) const;

        //! Part as bool
        bool toBool(int index, bool def = false) const;

        //! Part as double
        double toDouble(int index, double def = -1.0) const;

        //! Matches given part
        bool matchesPart(int index, const QString &toMatch, Qt::CaseSensitivity cs = Qt::CaseInsensitive) const;

        //! Help info
        struct CommandHtmlHelp
        {
            QString command; //!< command
            QString help; //!< help text

            //! Constructor
            CommandHtmlHelp(const QString &command, const QString &help) : command(command), help(help)
            {}

            //! Compare by command
            static bool less(const CommandHtmlHelp &a, const CommandHtmlHelp &b)
            {
                return a.command < b.command;
            }
        };

        //! Register a command
        static void registerCommand(const CommandHtmlHelp &command);

        //! Help already registered
        static bool registered(const QString &helpContext);

        //! HTML commands HELP
        static QString commandsHtmlHelp();

    private:
        QString m_originalLine; //!< line as entered by user
        QString m_cleanedLine; //!< trimmed, no double spaces etc.
        QString m_commandPart; //!< command part (e.g. ".msg", if any)
        QStringList m_splitParts; //!< split parts (split by " ")
        QStringList m_knownCommands; //!< known / handled commands
        bool m_knownCommand = false; //!< known command

        static QList<CommandHtmlHelp> s_commands; //!< all registered commands
        static QSet<QString> s_registered; //!< registered commands

        //! Avoid wrong usage
        void setCheckedCommandList(const QStringList &commands);

        //! Remove leading dot: ".msg" -> "msg"
        static QString removeLeadingDot(const QString &candidate);

        //! Clean up a command string
        static QString formatCommand(const QString &command);

        //! Command, starts with dot
        static bool isCommand(const QString &candidate);
    };
}

#endif // guard
