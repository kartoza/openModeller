/***************************************************************************
 *   Copyright (C) 2006 by Tim Sutton   *
 *   tim@linfiniti.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <omglistwidget.h>
#include "omgdesignerplugin.h"

#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerFormEditorInterface>

#include <QIcon>
#include <QtPlugin>

  OmgDesignerPlugin::OmgDesignerPlugin(QObject *parent)
:QObject(parent)
{
  initialized = false;
}

QString OmgDesignerPlugin::name() const
{
  return QString("OmgListWidget");
}

QString OmgDesignerPlugin::group() const
{
  return QString("openModeller GUI [omgui]");
}

QString OmgDesignerPlugin::toolTip() const
{
  return "";
}

QString OmgDesignerPlugin::whatsThis() const
{
  return "";
}

QString OmgDesignerPlugin::includeFile() const
{
  return QString("omglistwidget.h");
}

QIcon OmgDesignerPlugin::icon() const
{
  return QIcon();
}

bool OmgDesignerPlugin::isContainer() const
{
  return false;
}

QWidget *OmgDesignerPlugin::createWidget(QWidget *parent)
{
  OmgListWidget * mypListWidget = new OmgListWidget(parent);
  return mypListWidget;
}

bool OmgDesignerPlugin::isInitialized() const
{
  return initialized;
}

void OmgDesignerPlugin::initialize(QDesignerFormEditorInterface *formEditor)
{
  if (initialized)
    return;
/*
  QExtensionManager *manager = formEditor->extensionManager();
  Q_ASSERT(manager != 0);

  manager->registerExtensions(new OmgDesignerTaskMenuFactory(manager),
      Q_TYPEID(QDesignerTaskMenuExtension));
*/
  initialized = true;
}

Q_EXPORT_PLUGIN2(omgdesignerplugin, OmgDesignerPlugin)
