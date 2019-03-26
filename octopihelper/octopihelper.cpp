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

#include "../src/constants.h"
#include "octopihelper.h"
#include <QProcess>
#include <QDir>
#include <QObject>
#include <QTextStream>

OctopiHelper::OctopiHelper()
{
  m_exitCode = -9999;
  m_process = new QProcess();
  //This is the setting which enables all outputs from "pacman" go thru QProcess output methods
  m_process->setProcessChannelMode(QProcess::ForwardedChannels);
  m_process->setInputChannelMode(QProcess::ForwardedInputChannel);
}

OctopiHelper::~OctopiHelper()
{
  m_process->close();
}

/*
 * A bit of settings to better run "pacman" commands using QProcess
 */
QProcessEnvironment OctopiHelper::getProcessEnvironment()
{
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.remove("LANG");
  env.remove("LC_MESSAGES");
  env.insert("LANG", "C");
  env.insert("LC_MESSAGES", "C");
  env.remove("COLUMNS");
  env.insert("COLUMNS", "132");

  return env;
}

/*
 * Let's find the name of the Octopi's transaction tempfile in "/tmp/.qt_temp_octopi_"
 */
QString OctopiHelper::getTransactionTempFileName()
{
  QString res("");
  QDir dir(QDir::tempPath());
  dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
  dir.setSorting(QDir::Time);
  QStringList nf;
  QFileInfoList list = dir.entryInfoList(nf << ".qt_temp_octopi*");

  if (list.count() > 0) //If we find 2 or more files we use the most recent one
    res = list.first().filePath();

  return res;
}

/*
 * Executes all commands inside Octopi's transaction tempfile
 * pk-octopi-helper -t
 */
int OctopiHelper::executePkgTransaction()
{
  QString tempFile = getTransactionTempFileName();
  if (tempFile.isEmpty()) return 1;

  //Let's view the contents of the tempFile...
  QFile f(tempFile);
  if (!f.open(QFile::ReadOnly | QFile::Text)) return 1;
  QTextStream in(&f);
  QString contents = in.readAll();
  f.close();

  bool suspicious = false;
  if (contents.contains(";") || contents.contains(",") || contents.contains("|") ||
      contents.contains(">") || contents.contains("<") ||
      contents.contains(">>") || contents.contains("<<")) suspicious = true;

  QStringList lines = contents.split("\n", QString::SkipEmptyParts);

  foreach (QString line, lines){
    line = line.trimmed();
    if ((line == "killall pacman") ||
      (line == "rm " + ctn_PACMAN_DATABASE_LOCK_FILE) ||
      (line == "echo -e") ||
      (line.startsWith("echo ")) ||
      (line == "pkgfile -u") ||
      (line.startsWith("read ") && line.contains("read -n 1 -p")) ||
      (line.startsWith("paccache -r")) ||
      (line.startsWith("pacman "))) { }
    else
      suspicious = true;

    if (suspicious)
    {
      QTextStream qout(stdout);
      qout << endl << "octopi-helper[aborted]: Suspicious transaction detected -> \"" << line << "\"" << endl;
      return 1;
    }
  }

  QString command;
  m_process->setProcessEnvironment(getProcessEnvironment());
  command = "/bin/sh " + getTransactionTempFileName();
  m_process->start(command);
  m_process->waitForStarted(-1);
  m_process->waitForFinished(-1);

  return m_process->exitCode();
}

/*
 * Executes command "pacman -Su --noconfirm"
 * pk-octopi-helper -u
 */
int OctopiHelper::executeSysUpgrade()
{
  QString command;
  m_process->setProcessEnvironment(getProcessEnvironment());
  command = "/bin/sh -c \"pacman -Su --noconfirm\"";
  m_process->start(command);
  m_process->waitForStarted(-1);
  m_process->waitForFinished(-1);

  return m_process->exitCode();
}

/*
 * Executes command "pacman -Syy"
 * pk-octopi-helper -s
 */
int OctopiHelper::executeSyncDB()
{
  QString command;
  m_process->setProcessEnvironment(getProcessEnvironment());
  command = "/bin/sh -c \"pacman -Syy\"";
  m_process->start(command);
  m_process->waitForStarted(-1);
  m_process->waitForFinished(-1);

  return m_process->exitCode();
}
