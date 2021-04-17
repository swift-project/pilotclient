/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/simplecommandparser.h"
#include "blackmisc/pq/pqstring.h"
#include "blackmisc/stringutils.h"

#include <QList>
#include <QSet>
#include <Qt>
#include <QtGlobal>
#include <QStringBuilder>
#include <algorithm>

using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    QList<CSimpleCommandParser::CommandHtmlHelp> CSimpleCommandParser::s_commands;
    QSet<QString> CSimpleCommandParser::s_registered;

    CSimpleCommandParser::CSimpleCommandParser(const QStringList &knownCommands)
    {
        this->setCheckedCommandList(knownCommands);
    }

    void CSimpleCommandParser::parse(const QString &commandLine)
    {
        m_knownCommand = false;
        m_originalLine = commandLine;
        m_cleanedLine = commandLine.trimmed().simplified();
        if (!m_cleanedLine.isEmpty())
        {
            m_splitParts = m_cleanedLine.split(' ');
            if (!m_splitParts.isEmpty())
            {
                const QString &first = m_splitParts.constFirst();
                const QString formatted = formatCommand(first);
                if (isCommand(first))
                {
                    m_commandPart = formatted;
                    m_knownCommand = m_knownCommands.contains(formatted);
                }
            }
        }
    }

    const QString &CSimpleCommandParser::part(int index) const
    {
        static const QString empty("");
        if (index < 0 || index >= m_splitParts.size()) return empty;
        return m_splitParts.at(index);
    }

    QString CSimpleCommandParser::partAndRemainingStringAfter(int index) const
    {
        if (index < 0) { return m_cleanedLine; }
        const QString p = this->part(index);
        const int from = index < 1 ? 0 : nthIndexOf(m_cleanedLine, ' ', index, Qt::CaseInsensitive);
        const int fi = m_cleanedLine.indexOf(p, from, Qt::CaseInsensitive);
        if (fi < 0) { return {}; }
        return m_originalLine.mid(fi).trimmed();
    }

    int CSimpleCommandParser::countParts() const
    {
        return m_splitParts.count();
    }

    bool CSimpleCommandParser::hasPart(int index) const
    {
        return index >= 0 && index < this->countParts();
    }

    int CSimpleCommandParser::countPartsWithoutCommand() const
    {
        int c = countParts();
        return c > 0 ? c - 1 : 0;
    }

    bool CSimpleCommandParser::isInt(int index) const
    {
        const QString p = this->part(index);
        if (p.isEmpty()) { return false; }
        bool ok = false;
        // cppcheck-suppress ignoredReturnValue
        p.toInt(&ok);
        return ok;
    }

    bool CSimpleCommandParser::isDouble(int index) const
    {
        const QString p = this->part(index);
        if (p.isEmpty()) { return false; }
        bool ok = false;
        CPqString::parseNumber(p, ok, CPqString::SeparatorBestGuess);
        return ok;
    }

    int CSimpleCommandParser::toInt(int index, int def) const
    {
        const QString p = this->part(index);
        if (p.isEmpty()) { return def; }
        bool ok = false;
        int i = p.toInt(&ok);
        return ok ? i : def;
    }

    bool CSimpleCommandParser::toBool(int index, bool def) const
    {
        const QString p = this->part(index);
        if (p.isEmpty()) { return def; }
        const bool b = stringToBool(p);
        return b;
    }

    double CSimpleCommandParser::toDouble(int index, double def) const
    {
        const QString p = this->part(index);
        if (p.isEmpty()) { return def; }
        bool ok = false;
        double d = CPqString::parseNumber(p, ok, CPqString::SeparatorBestGuess);
        return ok ? d : def;
    }

    bool CSimpleCommandParser::matchesPart(int index, const QString &toMatch, Qt::CaseSensitivity cs) const
    {
        if (toMatch.isEmpty()) { return false; }
        if (!this->hasPart(index)) { return false; }
        const QString p(this->part(index));
        if (p.isEmpty()) { return false; }
        return (p.length() == toMatch.length() && p.startsWith(toMatch, cs));
    }

    void CSimpleCommandParser::registerCommand(const CSimpleCommandParser::CommandHtmlHelp &command)
    {
        for (const CommandHtmlHelp &help : std::as_const(s_commands))
        {
            // avoid duplicates
            if (help.command == command.command) { return; }
        }
        s_commands.append(command);
    }

    bool CSimpleCommandParser::registered(const QString &helpContext)
    {
        if (s_registered.contains(helpContext)) { return true; }
        s_registered.insert(helpContext);
        return false;
    }

    QString CSimpleCommandParser::commandsHtmlHelp()
    {
        if (s_commands.isEmpty()) { return {}; }

        static const QString html("<table style=\"font-size: 8pt; white-space: nowrap;\">\n%1\n</table>");
        static const QString row("<td>%1</td><td>%2</td>");
        static const QString rowHeader("<th>%1</th><th>%2</th><th>%3</th><th>%4</th>");

        QString rows;
        QList<CommandHtmlHelp> cmds(s_commands);
        std::sort(cmds.begin(), cmds.end(), CommandHtmlHelp::less);

        static const QString cmdCol(QString().fill('-', 20));
        static const QString textCol(QString().fill('-', 40));
        rows += u"<tr>" % rowHeader.arg("command", "synopsis", "command", "synopsis") % u"</tr>\n" %
                u"<tr>" % rowHeader.arg(cmdCol, textCol, cmdCol, textCol) % u"</tr>\n";

        for (int i = 0; i < cmds.size(); i++)
        {
            CommandHtmlHelp help = cmds[i];
            rows += u"<tr>" % row.arg(help.command, help.help);
            i++;
            if (i < cmds.size())
            {
                help = cmds[i];
                rows += row.arg(help.command, help.help);
            }
            else
            {
                rows += row.arg("", "");
            }
            rows += "</tr>\n";
        }
        return html.arg(rows);
    }

    QString CSimpleCommandParser::removeLeadingDot(const QString &candidate)
    {
        if (!candidate.startsWith('.')) { return candidate; }
        return candidate.right(candidate.length() - 1);
    }

    QString CSimpleCommandParser::formatCommand(const QString &command)
    {
        return removeLeadingDot(command.trimmed().toLower());
    }

    bool CSimpleCommandParser::isCommand(const QString &candidate)
    {
        return candidate.startsWith('.');
    }

    bool CSimpleCommandParser::matchesCommand(const QString &checkCommand, const QString &alias1, const QString &alias2)
    {
        if (m_commandPart.isEmpty()) { return false; }
        if (!checkCommand.isEmpty() && formatCommand(checkCommand) == m_commandPart) { return true; }
        if (!alias1.isEmpty() && formatCommand(alias1) == m_commandPart) { return true; }
        if (!alias2.isEmpty() && formatCommand(alias2) == m_commandPart) { return true; }
        return false;
    }

    bool CSimpleCommandParser::commandStartsWith(const QString &startPattern) const
    {
        if (m_commandPart.isEmpty()) { return false; }
        return m_commandPart.startsWith(formatCommand(startPattern));
    }

    bool CSimpleCommandParser::commandEndsWith(const QString &endPattern) const
    {
        if (m_commandPart.isEmpty()) { return false; }
        return m_commandPart.endsWith(endPattern);
    }

    void CSimpleCommandParser::setCheckedCommandList(const QStringList &commands)
    {
        for (const QString &c : commands)
        {
            m_knownCommands.append(formatCommand(c));
        }
    }
} // namespace
