// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/pluginselector.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVariant>
#include <Qt>
#include <QtGlobal>

#include "misc/icons.h"
#include "misc/verify.h"

namespace swift::gui
{
    CPluginSelector::CPluginSelector(QWidget *parent) : QWidget(parent)
    {
        setObjectName("CPluginSelector");

        QVBoxLayout *layout = new QVBoxLayout;
        setLayout(layout);

        connect(m_configButtonMapper, &QSignalMapper::mappedString, this, &CPluginSelector::pluginConfigRequested);
    }

    void CPluginSelector::addPlugin(const QString &identifier, const QString &name, bool hasConfig, bool enabled)
    {
        // skip if identifier is missing, which should normally not happen
        SWIFT_VERIFY_X(!identifier.isEmpty(), Q_FUNC_INFO, "Missing identifier");
        if (identifier.isEmpty()) { return; }

        QWidget *pw = new QWidget;
        QHBoxLayout *layout = new QHBoxLayout;
        layout->setContentsMargins(0, 0, 0, 0);
        pw->setLayout(layout);

        QCheckBox *cb = new QCheckBox(name);
        cb->setObjectName(identifier);
        cb->setProperty("pluginIdentifier", identifier);
        connect(cb, &QCheckBox::stateChanged, this, &CPluginSelector::handlePluginStateChange);
        if (enabled) { cb->setCheckState(Qt::Checked); }
        else { cb->setCheckState(Qt::Unchecked); }

        pw->layout()->addWidget(cb);

        if (hasConfig)
        {
            QPushButton *config = new QPushButton(swift::misc::CIcons::wrench16(), "");
            config->setToolTip("Plugin configuration");
            m_configButtonMapper->setMapping(config, identifier);
            connect(config, &QPushButton::clicked, m_configButtonMapper, qOverload<>(&QSignalMapper::map));
            pw->layout()->addWidget(config);
        }

        layout->setStretch(0, 1);
        layout->setStretch(1, 0);

        this->layout()->addWidget(pw);
    }

    void CPluginSelector::setEnabled(const QString &identifier, bool enabled)
    {
        QCheckBox *cb = findChild<QCheckBox *>(identifier);
        Q_ASSERT(cb);
        cb->setChecked(enabled);
    }

    void CPluginSelector::handlePluginStateChange()
    {
        QCheckBox *cb = qobject_cast<QCheckBox *>(sender());
        Q_ASSERT(cb);

        bool enabled = cb->checkState() != Qt::Unchecked;
        Q_ASSERT(cb->property("pluginIdentifier").isValid());
        const QString identifier = cb->property("pluginIdentifier").toString();
        Q_ASSERT(!identifier.isEmpty());

        emit this->pluginStateChanged(identifier, enabled);
    }
} // namespace swift::gui
