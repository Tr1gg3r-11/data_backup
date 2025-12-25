#include"controller.h"

controller::controller(QWidget *parent) :
    QWidget(parent),
    m_input(new input()),
    m_mainWindow(new MainWindow()),
    m_backup(new backup())
{
    connect(m_backup->backupui->browseSourceButton, &QPushButton::clicked, this, &controller::on_BrowseSource_Clicked);
    connect(m_backup->backupui->browseTargetButton, &QPushButton::clicked, this, &controller::on_BrowseTarget_Clicked);
    connect(m_backup->backupui->closebutton, &QPushButton::clicked, this, &controller::kill);
    connect(m_backup->backupui->pushButton, &QPushButton::clicked, this, &controller::on_backupButton_clicked);

    connect(m_mainWindow->ui->pushButton, &QPushButton::clicked, this, &controller::on_ChangeRootPath_Clicked);
    connect(m_mainWindow->ui->pushButton_2, &QPushButton::clicked, this, &controller::on_ChoosePath_Clicked);
    connect(m_mainWindow->ui->pushButton_3, &QPushButton::clicked, this, &controller::on_MainWindow2backup_Clicked);

    connect(m_input->inputui->pushButton, &QPushButton::clicked, this, &controller::on_ChooseRootPath_Clicked);
    connect(m_input->inputui->pushButton_2, &QPushButton::clicked, this, &controller::on_Input2MainWindow_Clicked);
}

controller::~controller()
{
    delete m_input;
    delete m_mainWindow;
    delete m_backup;
}

void controller::show()
{
    m_backup->show();
}

void controller::kill()
{
    exit(0);
}

void controller::on_BrowseSource_Clicked()
{
    m_backup->hide();
    m_mainWindow->findSource = true;
    m_mainWindow->show();
}

void controller::on_BrowseTarget_Clicked()
{
    m_backup->hide();
    m_mainWindow->findTarget = true;
    m_mainWindow->show();
}

void controller::on_ChangeRootPath_Clicked()
{
    m_mainWindow->hide();
    m_input->show();
}

void controller::on_ChooseRootPath_Clicked()
{
    QString text = m_input->inputui->textEdit->toPlainText().trimmed();
    if (!text.isEmpty() && QDir(text).exists())
    {
        m_mainWindow->updateRootPath(text);
        m_mainWindow->show();
    } else {
        m_input->inputui->textEdit->setPlaceholderText("路径不存在，请重新输入！");
        m_input->inputui->textEdit->clear();
        return;
    }
    m_input->hide();
}

void controller::on_Input2MainWindow_Clicked()
{
    m_input->hide();
    m_mainWindow->show();
}

void controller::on_ChoosePath_Clicked()
{
    if (m_mainWindow->m_selectedPath.isEmpty())
    {
        QMessageBox::warning(this, "警告", "请先在左侧选择一个文件或目录！");
        return;
    }
    QFileInfo fileInfo(m_mainWindow->m_selectedPath);
    if(m_mainWindow->findSource)
    {
        if(!fileInfo.exists())
        {
            QMessageBox::warning(this, "警告", "所选路径不存在，请重新选择！");
            return;
        }
        m_backup->updateSourcePath(m_mainWindow->m_selectedPath);
        m_mainWindow->findSource = false;
    }
    else if(m_mainWindow->findTarget)
    {
        if(!fileInfo.exists())
        {
            QMessageBox::warning(this, "警告", "所选路径不存在，请重新选择！");
            return;
        }
        else if(!fileInfo.isDir())
        {
            QMessageBox::warning(this, "警告", "目标路径必须是一个目录，请重新选择！");
            return;
        }
        m_backup->updateTargetPath(m_mainWindow->m_selectedPath);
        m_mainWindow->findTarget = false;
    }
    m_mainWindow->hide();
    m_backup->show();
}

void controller::on_MainWindow2backup_Clicked()
{
    m_mainWindow->hide();
    m_mainWindow->findSource = false;
    m_mainWindow->findTarget = false;
    m_backup->show();
}

void controller::on_backupButton_clicked()
{
    QString text = m_backup->backupui->textEdit->toPlainText();
    QString text1 = m_backup->backupui->textEdit_2->toPlainText();
    string sourceDirOrFile = text.toStdString();
    string targetDir = text1.toStdString();
    if(sourceDirOrFile.empty() || targetDir.empty())
    {
        QMessageBox::warning(this, "警告", "源路径或目标路径不能为空，请重新选择！");
        return;
    }
    size_t pos = sourceDirOrFile.find_last_of("/");
    string checkpath = sourceDirOrFile;
    string targetname = sourceDirOrFile;
    if(pos != string::npos)
    {
        checkpath = sourceDirOrFile.substr(0, pos);
        targetname = sourceDirOrFile.substr(pos);
    }
    if(checkpath == targetDir)
    {
        QMessageBox::warning(this, "警告", "源路径和目标路径不能相同，请重新选择！");
        return;
    }
    checkpath = targetDir + targetname;
    if(access(checkpath.c_str(), F_OK) == 0)
    {
        QMessageBox::warning(this, "警告", "目标路径下已有同名文件或目录，请重新选择！");
        return;
    }

    //hard
    if (m_backup->backupui->radioButton->isChecked())
    {
        if (access(sourceDirOrFile.c_str(), F_OK))
            {
                QMessageBox::warning(this, "警告", "所选路径不存在，请重新选择！");
                return;
            }
        lstat(sourceDirOrFile.c_str(), &sbuf);
        if (!(S_ISFIFO(sbuf.st_mode) || S_ISLNK(sbuf.st_mode) || S_ISREG(sbuf.st_mode)))
            {
                QMessageBox::warning(this, "警告", "所选路径不是硬链接文件，请重新选择！");
                return;
            }

        strcpy(source_path, sourceDirOrFile.c_str());
        targetFile = targetDir + '/' + basename(source_path);

        ADBackuper adworker;
        bool success = adworker.backupRegFile(sourceDirOrFile, targetFile);
        if(success)
        {
            QMessageBox::information(this, "", "备份完成！");
        }
        else
        {
            QMessageBox::warning(this, "警告", "备份过程中出现错误，备份失败！");
        }
    }
    //soft
    else if (m_backup->backupui->radioButton_2->isChecked())
    {
        if (access(sourceDirOrFile.c_str(), F_OK))
        {
            QMessageBox::warning(this, "警告", "所选路径不存在，请重新选择！");
            return;
        }
        lstat(sourceDirOrFile.c_str(), &sbuf);
        if (!S_ISLNK(sbuf.st_mode))
        {
            QMessageBox::warning(this, "警告", "所选路径不是软链接，请重新选择！");
            return;
        }

        strcpy(source_path, sourceDirOrFile.c_str());
        targetFile = targetDir + '/' + basename(source_path);

        ADBackuper ADworker;
        bool success = ADworker.backupLINK(sourceDirOrFile, targetFile);
        if(success)
        {
            QMessageBox::information(this, "", "备份完成！");
        }
        else
        {
            QMessageBox::warning(this, "警告", "备份过程中出现错误，备份失败！");
        }
    }
    //pipe
    else if (m_backup->backupui->radioButton_3->isChecked())
    {
        if (access(sourceDirOrFile.c_str(), F_OK))
        {
            QMessageBox::warning(this, "警告", "所选路径不存在，请重新选择！");
            return;
        }
        lstat(sourceDirOrFile.c_str(), &sbuf);
        if (!S_ISFIFO(sbuf.st_mode))
        {
            QMessageBox::warning(this, "警告", "所选路径不是管道文件，请重新选择！");
            return;
        }

        strcpy(source_path, sourceDirOrFile.c_str());
        targetFile = targetDir + '/' + basename(source_path);

        ADBackuper ADworker;
        bool success = ADworker.backupFIFO(sourceDirOrFile, targetFile);
        if(success)
        {
            QMessageBox::information(this, "", "备份完成！");
        }
        else
        {
            QMessageBox::warning(this, "警告", "备份过程中出现错误，备份失败！");
        }
    }
    //directory
    else if (m_backup->backupui->radioButton_4->isChecked())
    {
        if (access(sourceDirOrFile.c_str(), F_OK))
        {
            QMessageBox::warning(this, "警告", "所选路径不存在，请重新选择！");
            return;
        }
        stat(sourceDirOrFile.c_str(), &sbuf);
        if (!S_ISDIR(sbuf.st_mode))
        {
            QMessageBox::warning(this, "警告", "所选路径不是目录，请重新选择！");
            return;
        }

        if (strcmp(sourceDirOrFile.c_str(), targetDir.c_str()) == 0)
        {
            QMessageBox::warning(this, "警告", "源目录和目标目录不能相同，请重新选择！");
            return;
        }
        ADBackuper ADworker;
        bool success = ADworker.ADBackupDir(sourceDirOrFile, targetDir);
        if(success)
        {
            QMessageBox::information(this, "", "备份完成！");
        }
        else
        {
            QMessageBox::warning(this, "警告", "备份过程中出现错误，备份失败！");
        }
    }
    //restore
    else if (m_backup->backupui->radioButton_5->isChecked())
    {
        if (access(sourceDirOrFile.c_str(), F_OK))
        {
            QMessageBox::warning(this, "警告", "所选路径不存在，请重新选择！");
            return;
        }
        lstat(sourceDirOrFile.c_str(), &sbuf);
        if (!(S_ISFIFO(sbuf.st_mode) || S_ISLNK(sbuf.st_mode) || S_ISREG(sbuf.st_mode) || S_ISDIR(sbuf.st_mode)))
        {
            QMessageBox::warning(this, "警告", "所选路径不是有效的备份文件，请重新选择！");
            return;
        }
        stat(sourceDirOrFile.c_str(), &sbuf);

        ADBackuper ADworker;
        bool success = ADworker.ADmoveFileOrDir(sourceDirOrFile, targetDir);
        if(success)
        {
            QMessageBox::information(this, "", "还原完成！");
        }
        else
        {
            QMessageBox::warning(this, "警告", "还原过程中出现错误，还原失败！");
        }
    }
    //pack
    else if (m_backup->backupui->radioButton_6->isChecked())
    {
        if (access(sourceDirOrFile.c_str(), F_OK) || user.EmptyDir(sourceDirOrFile))
        {
            QMessageBox::warning(this, "警告", "所选路径不存在或为空目录，请重新选择！");
            return;
        }
        else
        {
            pack_worker paworker;
            bool success = paworker.packDir(sourceDirOrFile, targetDir);
            if(success)
            {
                QMessageBox::information(this, "", "打包完成！");
            }
        }
    }
    //unpack
    else if (m_backup->backupui->radioButton_7->isChecked())
    {
        if (access(sourceDirOrFile.c_str(), F_OK))
        {
            QMessageBox::warning(this, "警告", "所选路径不存在，请重新选择！");
            return;
        }
        else
        {
            pack_worker paworker;
            if (access(targetDir.c_str(), F_OK))
            {
                paworker.mkDir(targetDir);
            }
            bool success = paworker.unpackBag(sourceDirOrFile, targetDir);
            if(success)
            {
                QMessageBox::information(this, "", "解包完成！");
            }
        }
    }
    //compress
    else if (m_backup->backupui->radioButton_8->isChecked())
    {
        if (access(sourceDirOrFile.c_str(), F_OK))
        {
            QMessageBox::warning(this, "警告", "所选路径不存在，请重新选择！");
            return;
        }
        else
        {
            compress_worker cpworker;
            bool success = cpworker.compress(sourceDirOrFile, targetDir);
            if(success)
            {
                QMessageBox::information(this, "", "压缩完成！");
            }
        }
    }
    //decompress
    else if (m_backup->backupui->radioButton_9->isChecked())
    {
        if (access(sourceDirOrFile.c_str(), F_OK))
        {
            QMessageBox::warning(this, "警告", "所选路径不存在，请重新选择！");
            return;
        }
        else
        {
            if (access(targetDir.c_str(), F_OK))
            {
                user.mkDir(targetDir);
            }
            compress_worker cpworker;
            bool success = cpworker.decompress(sourceDirOrFile, targetDir);
            if(success)
            {
                QMessageBox::information(this, "", "解压完成！");
            }
        }
    }
    //encrypt
    else if (m_backup->backupui->radioButton_10->isChecked())
    {
        if (access(sourceDirOrFile.c_str(), F_OK))
        {
            QMessageBox::warning(this, "警告", "所选路径不存在，请重新选择！");
            return;
        }
        else
        {
            string md5, md5md5;
            encrypt_worker worker;
            MD5 md5_worker;
            password = worker.getPSWD();
            md5 = md5_worker.encode(password);
            md5md5 = md5_worker.encode(md5);
            if (access(targetDir.c_str(), F_OK))
            {
                user.mkDir(targetDir);
            }
            bool success = worker.encyptFile(md5, md5md5, sourceDirOrFile, targetDir);
            if(success)
            {
                QMessageBox::information(this, "", "加密完成！");
            }
        }
    }
    //decrypt
    else if (m_backup->backupui->radioButton_11->isChecked())
    {
        if (access(sourceDirOrFile.c_str(), F_OK))
        {
            QMessageBox::warning(this, "警告", "所选路径不存在，请重新选择！");
            return;
        }
        else
        {
            string md5, md5md5;
            encrypt_worker worker;
            MD5 md5_worker;
            password = worker.getPSWD();
            md5 = md5_worker.encode(password);
            md5md5 = md5_worker.encode(md5);
            if (access(targetDir.c_str(), F_OK))
            {
                user.mkDir(targetDir);
            }
            bool success = worker.decryptFile(md5, md5md5, sourceDirOrFile, targetDir);
            if(success)
            {
                QMessageBox::information(this, "", "解密完成！");
            }
        }
    }
    //none selected
    else
    {
        QMessageBox::warning(this, "警告", "请选择一种操作模式！");
        return;
    }
}

