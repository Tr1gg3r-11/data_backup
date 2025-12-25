#ifndef controller_H
#define controller_H

#include"utils.h"
#include"input.h"
#include"mainwindow.h"
#include"backup.h"
#include"ui_backup.h"
#include"ui_mainwindow.h"
#include"ui_input.h"
#include "advanced.h"
#include "compressANDdecompress.h"
#include "encryptANDdecrypt.h"
#include "packANDunpack.h"
#include "primary.h"

class controller : public QWidget
{
    Q_OBJECT

public:
    explicit controller(QWidget* parent = nullptr);
    ~controller();
    void show();
private:
    input* m_input;
    MainWindow* m_mainWindow;
    backup* m_backup;
    struct stat sbuf;
    string targetFile;
    string password;
    char source_path[PATH_MAX];
    primaryBackuper user;

    void kill();

    void on_BrowseSource_Clicked();

    void on_BrowseTarget_Clicked();

    void on_ChangeRootPath_Clicked();

    void on_ChooseRootPath_Clicked();

    void on_Input2MainWindow_Clicked();

    void on_ChoosePath_Clicked();

    void on_MainWindow2backup_Clicked();

    void on_backupButton_clicked();

    void on_warn2backup_Clicked();

};
#endif // controller_H