#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <layoutwidget.h>

#define DEBUG
#ifdef DEBUG
    #include <QDebug>
#endif

//stuff for handling files
#include <QMenu>
#include <QAction>
//#include <QKeySequence>
#include <QFileDialog>
#include <QFile>
#include <QSaveFile>
#include <QMessageBox>
#include <QDir>

#include <QDesktopServices>

//ui elements
//#include <QTextEdit>
//#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

//#####################################################################################################################################################################################
class MainWindow : public QMainWindow
{
    Q_OBJECT

public://------------------------------------------------------------
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private://------------------------------------------------------------
    Ui::MainWindow *ui;

    const QString windowTitle = "ColorDataConfigurator";
    const QString appVersion = "V1.0.1";
    const QString appSourceCode = "https://github.com/Tsamsiy/ColorDataConfigurator";
    void createMenu();
    void createStatus();

    layoutWidget* mainLayout;

    //file menu variables
    QString filePath;
    bool fileStillOpen = false;
    bool fileChanged = true;
    //bool fileIsDraft = true;      //replaced by filePath.isEmpty()

    //data display format
    //16: HEX (default)
    //10: Decimal
    //2: Binary
    //8: Octal
    uint8_t displayFormat = 16;
    uint16_t eepromSize = 256;

    //menu elements
    QMenu* fileMenu;
    QAction* fileMenuNew;
    QAction* fileMenuOpen;
    QAction* fileMenuSave;
    QAction* fileMenuSaveAs;
    QAction* fileMenuClose;

    QMenu* formMenu;
    QAction* formMenuBIN;
    QAction* formMenuOCT;
    QAction* formMenuDEC;
    QAction* formMenuHEX;

    QMenu* eeMenu;
    QAction* eeMenu256;
    QAction* eeMenu512;
    QAction* eeMenu1024;
    QAction* eeMenu2048;
    QAction* eeMenu4096;

    QMenu* helpMenu;
    QAction* helpAction;
    QAction* versionAction;
    QAction* aboutAction;

private slots://------------------------------------------------------------
    //file menu actions
    void newFile();
    void openFile();        //brings up file dialog
    bool saveFile();        //calls saveFileAs() if no file is open
    bool saveFileAs();      //brings up file dialog
    bool closeFile();       //returns true if a save action was aborted. only logical close, files are closed immediately after read/write

    //Format menu actions
    void formatBIN();
    void formatOCT();
    void formatDEC();
    void formatHEX();

    //EEPROM Menu actions
    void eepromSet256();
    void eepromSet512();
    void eepromSet1024();
    void eepromSet2048();
    void eepromSet4096();

    //Help menu actions
    void dispHelp();    //opens new window

    //About menu actions
    void dispInfo();   //opens Github page

    void textEdited();
};
#endif // MAINWINDOW_H
