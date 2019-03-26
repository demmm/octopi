/*
* This file is part of Octopi, an open-source GUI for pacman.
* Copyright (C) 2019 Alexandre Albuquerque Arnt
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*/

#ifndef PKOCTOPIHELPER_H
#define PKOCTOPIHELPER_H

#include <QString>
#include <QProcess>
#include <QObject>

class OctopiHelper: QObject
{
Q_OBJECT

private:
  int m_exitCode;
  QProcess *m_process;
  QProcessEnvironment getProcessEnvironment();
  QString getTransactionTempFileName();

public:
  OctopiHelper();
  virtual ~OctopiHelper();

  int executePkgTransaction();
  int executeSysUpgrade();
  int executeSyncDB();

  inline int getExitCode() { return m_exitCode; }
};

#endif // PKOCTOPIHELPER_H
