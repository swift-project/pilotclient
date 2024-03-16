// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/pluginselector.h"
#include "blackmisc/icons.h"
#include "blackmisc/verify.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVariant>
#include <Qt>
#include <QtGlobal>

namespace BlackGui
{
    CPluginSelector::CPluginSelector(QWidget *parent) : QWidget(parent)
    {
        setObjectName("CPluginSelector");

        QVBoxLayout *layout = new QVBoxLayout;
        setLayout(layout);

        connect(m_detailsButtonMapper, &QSignalMapper::mappedString, this, &CPluginSelector::pluginDetailsRequested);
        connect(m_configButtonMapper, &QSignalMapper::mappedString, this, &CPluginSelector::pluginConfigRequested);
    }

    void CPluginSelector::addPlugin(const QString &identifier, const QString &name, bool hasConfig, bool enabled)
    {
        // skip if identifier is missing, which should normally not happen
        BLACK_VERIFY_X(!identifier.isEmpty(), Q_FUNC_INFO, "Missing identifier");
        if (identifier.isEmpty()) { return; }

        QWidget *pw = new QWidget;
        QHBoxLayout *layout = new QHBoxLayout;
        layout->setContentsMargins(0, 0, 0, 0);
        pw->setLayout(layout);

        QCheckBox *cb = new QCheckBox(name);
        cb->setObjectName(identifier);
        cb->setProperty("pluginIdentifier", identifier);
        connect(cb, &QCheckBox::stateChanged, this, &CPluginSelector::handlePluginStateChange);
        if (enabled)
        {
            cb->setCheckState(Qt::Checked);
        }
        else
        {
            cb->setCheckState(Qt::Unchecked);
        }

        pw->layout()->addWidget(cb);

        if (hasConfig)
        {
            QPushButton *config = new QPushButton(BlackMisc::CIcons::wrench16(), "");
            config->setToolTip("Plugin configuration");
            m_configButtonMapper->setMapping(config, identifier);
            connect(config, &QPushButton::clicked, m_configButtonMapper, qOverload<>(&QSignalMapper::map));
            pw->layout()->addWidget(config);
        }

        QPushButton *details = new QPushButton("?");
        details->setToolTip("Plugin details");
        m_detailsButtonMapper->setMapping(details, identifier);
        connect(details, &QPushButton::clicked, m_detailsButtonMapper, qOverload<>(&QSignalMapper::map));
        pw->layout()->addWidget(details);

        layout->setStretch(0, 1);
        layout->setStretch(1, 0);
        layout->setStretch(2, 0);

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
} // ns
