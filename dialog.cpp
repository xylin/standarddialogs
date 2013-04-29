/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include <iostream>
#include <fstream>
#include <QDir>

#include "dialog.h"

#define MESSAGE \
    Dialog::tr("<p>Message boxes have a caption, a text, " \
               "and any number of buttons, each with standard or custom texts." \
               "<p>Click a button to close the message box. Pressing the Esc button " \
               "will activate the detected escape button (if any).")

Dialog::Dialog(QWidget *parent)
    : QWidget(parent)
{


   errorMessageDialog_ =new QErrorMessage(this);

    int frameStyle = QFrame::Sunken | QFrame::Panel;

    directoryLabel = new QLabel;
    directoryLabel->setFrameStyle(frameStyle);
    QPushButton *directoryButton =
            new QPushButton(tr("Which folder?"));

    saveFileNameLabel = new QLabel;
    saveFileNameLabel->setFrameStyle(frameStyle);
    QPushButton *saveFileNameButton =
            new QPushButton(tr("Saved as?"));

    connect(directoryButton, SIGNAL(clicked()),
            this, SLOT(setExistingDirectory()));

    connect(saveFileNameButton, SIGNAL(clicked()),
            this, SLOT(setSaveFileName()));

    QGridLayout *layout = new QGridLayout;
    layout->setColumnStretch(1, 1);
    layout->setColumnMinimumWidth(1, 250);

    layout->addWidget(directoryButton, 6, 0);
    layout->addWidget(directoryLabel, 6, 1);

    layout->addWidget(saveFileNameButton, 9, 0);
    layout->addWidget(saveFileNameLabel, 9, 1);

    setLayout(layout);

    setWindowTitle(tr("List Files Dialogs"));
}

void Dialog::setExistingDirectory()
{
    QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;

    QString directory = QFileDialog::getExistingDirectory(this,
                                tr("QFileDialog::getExistingDirectory()"),
                                directoryLabel->text(),
                                options);
    if (!directory.isEmpty())
        directoryLabel->setText(directory);
}

void Dialog::setSaveFileName()
{
    QFileDialog::Options options;

    QString selectedFilter;
    QString fileName = QFileDialog::getSaveFileName(this,
                                tr("QFileDialog::getSaveFileName()"),
                                saveFileNameLabel->text(),
                                                    tr("XLS Files(*.xls);;Text Files (*.txt)"),
                                &selectedFilter,
                                options);
    if (!fileName.isEmpty())
        saveFileNameLabel->setText(fileName);

    QDir dir(directoryLabel->text());

    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        errorMessageDialog_->showMessage(tr("Cant open output files. Please close the file first."));
    }
    else
    {
        QTextStream stream(&file);
        stream.setCodec(QTextCodec::codecForName("UTF-8") );

        stream << tr("Folder Name\t") << tr("File Name\t") << tr("File Type\t") << tr("Modified Time\t") << tr("Created Time") << "\n";

        listFiles(dir, stream, "");

        file.close();

        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(this, tr("QMessageBox::information()"), "All Done!");
    }
}


void Dialog::listFiles(QDir directory, QTextStream& out, QString indent)
{
    indent += "\t";

    QDir dir(directory);

    QStringList filters;
    filters << "*.pdf" << "*.zip"  << "*.ppt" << "*.pptx << *.doc" << "*.docx" << "*.xls" << "*.xlsx";
    filters << "*.txt";

    dir.setNameFilters(filters);

    dir.setFilter(QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot);

    QFileInfoList list = dir.entryInfoList();

    foreach(QFileInfo finfo, list)
    {
        if(finfo.isDir())
        {
            listFiles(QDir(finfo.absoluteFilePath()), out, indent);
        }
        else
        {
            out << finfo.absolutePath() << "\t" << finfo.fileName() << "\t";
            out << finfo.suffix() << "\t" << finfo.lastModified().toString() << "\t" << finfo.created().toString() << "\n";
        }
    }
}


