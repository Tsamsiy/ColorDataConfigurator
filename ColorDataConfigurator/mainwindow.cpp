#include "mainwindow.h"
#include "ui_mainwindow.h"

#define DEBUG
#ifdef DEBUG
    #include <QDebug>
#endif

//#####################################################################################################################################################################################
MainWindow::MainWindow(QWidget* parent): QMainWindow(parent), ui(new Ui::MainWindow)//------------------------------------------------------------
{
    ui->setupUi(this);
    setWindowTitle(windowTitle);

    createMenu();

    mainLayout = new layoutWidget(this, &this->displayFormat, &this->eepromSize);
    setCentralWidget(mainLayout);
    connect(mainLayout, &layoutWidget::textEdited, this, &MainWindow::textEdited);

    createStatus();
}

MainWindow::~MainWindow()//------------------------------------------------------------
{
    delete ui;
}

//#####################################################################################################################################################################################
void MainWindow::createMenu()//------------------------------------------------------------
{
    #ifdef DEBUG
        qDebug() << "createMenu\n";
    #endif

    //file menu
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenuNew = new QAction(tr("&New"), this);
    fileMenuOpen = new QAction(tr("&Open"), this);
    fileMenuSave = new QAction(tr("&Save"), this);
    fileMenuSaveAs = new QAction(tr("&Save as"), this);
    fileMenuClose = new QAction(tr("&Close"), this);

    fileMenu->addAction(fileMenuNew);
    fileMenu->addAction(fileMenuOpen);
    fileMenu->addAction(fileMenuSave);
    fileMenu->addAction(fileMenuSaveAs);
    fileMenu->addAction(fileMenuClose);

    connect(fileMenuNew, &QAction::triggered, this, &MainWindow::newFile);
    connect(fileMenuOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(fileMenuSave, &QAction::triggered, this, &MainWindow::saveFile);
    connect(fileMenuSaveAs, &QAction::triggered, this, &MainWindow::saveFileAs);
    connect(fileMenuClose, &QAction::triggered, this, &MainWindow::closeFile);

    //Format menu
    formMenu = menuBar()->addMenu(tr("&Format"));
    formMenuBIN = new QAction(tr("&BIN"), this);
    formMenuOCT = new QAction(tr("&OCT"), this);
    formMenuDEC = new QAction(tr("&DEC"), this);
    formMenuHEX = new QAction(tr("&HEX"), this);

    formMenuBIN->setCheckable(true);
    formMenuOCT->setCheckable(true);
    formMenuDEC->setCheckable(true);
    formMenuHEX->setCheckable(true);

    formMenuHEX->setChecked(true);

    //formMenu->addAction(formMenuBIN);
    formMenu->addAction(formMenuOCT);
    formMenu->addAction(formMenuDEC);
    formMenu->addAction(formMenuHEX);

    connect(formMenuBIN, &QAction::triggered, this, &MainWindow::formatBIN);
    connect(formMenuOCT, &QAction::triggered, this, &MainWindow::formatOCT);
    connect(formMenuDEC, &QAction::triggered, this, &MainWindow::formatDEC);
    connect(formMenuHEX, &QAction::triggered, this, &MainWindow::formatHEX);

    //EEPROM Menu
    eeMenu = menuBar()->addMenu(tr("&EEPROM"));
    eeMenu256 = new QAction(tr("&256"), this);
    eeMenu512 = new QAction(tr("&512"), this);
    eeMenu1024 = new QAction(tr("&1024"), this);
    eeMenu2048 = new QAction(tr("&2048"), this);
    eeMenu4096 = new QAction(tr("&4096"), this);

    eeMenu256->setCheckable(true);
    eeMenu512->setCheckable(true);
    eeMenu1024->setCheckable(true);
    eeMenu2048->setCheckable(true);
    eeMenu4096->setCheckable(true);

    eeMenu256->setChecked(true);

    eeMenu->addAction(eeMenu256);
    eeMenu->addAction(eeMenu512);
    eeMenu->addAction(eeMenu1024);
    eeMenu->addAction(eeMenu2048);
    eeMenu->addAction(eeMenu4096);

    connect(eeMenu256, &QAction::triggered, this, &MainWindow::eepromSet256);
    connect(eeMenu512, &QAction::triggered, this, &MainWindow::eepromSet512);
    connect(eeMenu1024, &QAction::triggered, this, &MainWindow::eepromSet1024);
    connect(eeMenu2048, &QAction::triggered, this, &MainWindow::eepromSet2048);
    connect(eeMenu4096, &QAction::triggered, this, &MainWindow::eepromSet4096);

    //Help menu
    helpMenu = menuBar()->addMenu("&Help");
    helpAction = new QAction("&Syntax", this);
    versionAction = new QAction("&" + appVersion, this);
    aboutAction = new QAction("&Source code", this);

    helpMenu->addAction(versionAction);
    helpMenu->addAction(helpAction);
    helpMenu->addAction(aboutAction);

    connect(helpAction, &QAction::triggered, this, &MainWindow::dispHelp);
    //connect(versionAction, &QAction::triggered, this, &MainWindow::dispVersion);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::dispInfo);
}

void MainWindow::createStatus()
{
    #ifdef DEBUG
        qDebug() << "createStatus\n";
    #endif

    //enable tooltips
    //this->Qt::WA_AlwaysShowToolTips;
    statusBar()->showMessage("Ready", 3000);
}

//#####################################################################################################################################################################################
void MainWindow::newFile()//------------------------------------------------------------
{
    #ifdef DEBUG
        qDebug() << "newFile";
    #endif

    //close old file first
    //will save file if one with changes is open
    if(closeFile())     //action was aborted
    {
        return;
    }
    setWindowTitle(windowTitle + ": New File");

    //open new draft in memory
    //set example text
    mainLayout->setEditorText("{\n\tC:048F70,1000;\n\tC:24CCA8,1000;\n\tC:FFFFFF,1000;\n\tC:79ABE0,1000;\n\tC:4E47C9,1000;\n\tC:3C1977,1000;\n\tL:0,0;\n}");
    fileChanged = false;    //pasting the text sets this, so reset it

    //fileIsDraft = true;
    fileStillOpen = true;
}

void MainWindow::openFile()//------------------------------------------------------------
{
    #ifdef DEBUG
        qDebug() << "openFile: ";
    #endif

    //close old file first
    //will save file if one with changes is open
    if(closeFile())     //action was aborted
    {
        return;
    }

    //open file dialog
    filePath = QFileDialog::getOpenFileName(this, "Open an existing RGB file", QDir::homePath(), "Text file (*.txt)");
    if(!filePath.isEmpty())    //path is specified
    {
        #ifdef DEBUG
            qDebug() << filePath << "\n";
        #endif
        statusBar()->showMessage(filePath);
        setWindowTitle(windowTitle + ": " + filePath.section('/', -1));     //display file name

        QFile file(filePath);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            //show error message
            QMessageBox errorBox;
            errorBox.setText("Could not read file");
            errorBox.exec();
            fileStillOpen = false;
            return;
        }
        else
        {
            //read file content to memory
            mainLayout->setEditorText(file.readAll());
            fileChanged = false;    //pasting the text sets this, so reset it
            //file is closed immediately after reading.
            //MainWindow::closeFile() is only a logical close
            file.close();
        }

        //fileIsDraft = false;
        fileStillOpen = true;

        mainLayout->parseCode();
    }
    else    //no path specified
    {
        #ifdef DEBUG
            qDebug() << "<none>\n";
        #endif
        fileStillOpen = false;
    }
}

bool MainWindow::saveFile()//------------------------------------------------------------
{
    #ifdef DEBUG
        qDebug() << "saveFile\n";
    #endif

    if(fileStillOpen)
    {
        //ask for path if file is a draft
        if(filePath.isEmpty())
        {
            //opens dialog
            return saveFileAs();
        }

        //write the file
        QSaveFile file(filePath);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            //show error message
            QMessageBox errorBox;
            errorBox.setText("Could not write file");
            errorBox.exec();
            fileStillOpen = false;
            return false;
        }
        else
        {
            //write contents of project to file
            file.write(mainLayout->getEditorText().toUtf8());
            fileChanged = false;    //pasting the text sets this, so reset it
            //file is closed immediately after writing.
            //MainWindow::closeFile() is only a logical close
            file.commit();
        }

        //statusBar()->showMessage("Saved", 3000);
        statusBar()->showMessage(filePath);
        setWindowTitle(windowTitle + ": " + filePath.section('/', -1));     //display file name

        fileChanged = false;
        return true;
    }
    return true;
}

bool MainWindow::saveFileAs()//------------------------------------------------------------
{
    #ifdef DEBUG
        qDebug() << "saveFileAs: ";
    #endif

    if(fileStillOpen)
    {
        //open file dialog
        filePath = QFileDialog::getSaveFileName(this, "Save RGB file as", QDir::homePath(), "Text file (*.txt)");
        statusBar()->showMessage(filePath);
        setWindowTitle(windowTitle + ": " + filePath.section('/', -1));     //display file name
        if(filePath.isEmpty())    //no path was specified
        {
            #ifdef DEBUG
                qDebug() << "<none>\n";
            #endif

            return false;
        }
        #ifdef DEBUG
            qDebug() << filePath << "\n";
        #endif

        //write the file
        return saveFile();
    }
    return true;
}

bool MainWindow::closeFile()//------------------------------------------------------------
{
    #ifdef DEBUG
        qDebug() << "closeFile\n";
    #endif

    //there is file to close
    if(fileStillOpen)
    {
        //unsaved changes?
        if(fileChanged)
        {
            //ask the user if they want to save
            QMessageBox saveBox;
            saveBox.setText("There are unsaved changes.");
            saveBox.setInformativeText("Do you want to save your changes?");
            saveBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            saveBox.setDefaultButton(QMessageBox::Discard);
            int ret = saveBox.exec();

            switch (ret)
            {
                case QMessageBox::Save:
                    saveFile();

                    //empty the editor
                    mainLayout->setEditorText("");
                    mainLayout->setConsoleText("");
                    mainLayout->clearSeqAnim();
                    mainLayout->clearList();
                    mainLayout->calcEEPROM();
                break;

                case QMessageBox::Discard:
                    //empty the editor
                    mainLayout->setEditorText("");
                    mainLayout->setConsoleText("");
                    mainLayout->clearSeqAnim();
                    mainLayout->clearList();
                    mainLayout->calcEEPROM();
                break;

                case QMessageBox::Cancel:
                    return true;
                break;

                default:
                    // should never be reached
                break;
            }
        }
        else
        {
            //empty the editor
            mainLayout->setEditorText("");
            mainLayout->setConsoleText("");
            mainLayout->clearSeqAnim();
            mainLayout->clearList();
            mainLayout->calcEEPROM();
        }
    }
    filePath.clear();
    statusBar()->showMessage(filePath);
    setWindowTitle(windowTitle);     //display file name
    fileStillOpen = false;
    fileChanged = false;

    return false;
}

//#####################################################################################################################################################################################
void MainWindow::formatBIN()//------------------------------------------------------------
{
    displayFormat = 2;

    formMenuBIN->setChecked(true);
    formMenuOCT->setChecked(false);
    formMenuDEC->setChecked(false);
    formMenuHEX->setChecked(false);

    mainLayout->update();
}

void MainWindow::formatOCT()//------------------------------------------------------------
{
    displayFormat = 8;

    formMenuBIN->setChecked(false);
    formMenuOCT->setChecked(true);
    formMenuDEC->setChecked(false);
    formMenuHEX->setChecked(false);

    mainLayout->update();
}

void MainWindow::formatDEC()//------------------------------------------------------------
{
    displayFormat = 10;

    formMenuBIN->setChecked(false);
    formMenuOCT->setChecked(false);
    formMenuDEC->setChecked(true);
    formMenuHEX->setChecked(false);

    mainLayout->update();
}

void MainWindow::formatHEX()//------------------------------------------------------------
{
    displayFormat = 16;

    formMenuBIN->setChecked(false);
    formMenuOCT->setChecked(false);
    formMenuDEC->setChecked(false);
    formMenuHEX->setChecked(true);

    mainLayout->update();
}

//#####################################################################################################################################################################################
void MainWindow::eepromSet256()//------------------------------------------------------------
{
    this->eepromSize = 256;

    eeMenu256->setChecked(true);
    eeMenu512->setChecked(false);
    eeMenu1024->setChecked(false);
    eeMenu2048->setChecked(false);
    eeMenu4096->setChecked(false);

    mainLayout->calcEEPROM();
    mainLayout->update();
}

void MainWindow::eepromSet512()//------------------------------------------------------------
{
    this->eepromSize = 512;

    eeMenu256->setChecked(false);
    eeMenu512->setChecked(true);
    eeMenu1024->setChecked(false);
    eeMenu2048->setChecked(false);
    eeMenu4096->setChecked(false);

    mainLayout->calcEEPROM();
    mainLayout->update();
}

void MainWindow::eepromSet1024()//------------------------------------------------------------
{
    this->eepromSize = 1024;

    eeMenu256->setChecked(false);
    eeMenu512->setChecked(false);
    eeMenu1024->setChecked(true);
    eeMenu2048->setChecked(false);
    eeMenu4096->setChecked(false);

    mainLayout->calcEEPROM();
    mainLayout->update();
}

void MainWindow::eepromSet2048()//------------------------------------------------------------
{
    this->eepromSize = 2048;

    eeMenu256->setChecked(false);
    eeMenu512->setChecked(false);
    eeMenu1024->setChecked(false);
    eeMenu2048->setChecked(true);
    eeMenu4096->setChecked(false);

    mainLayout->calcEEPROM();
    mainLayout->update();
}

void MainWindow::eepromSet4096()//------------------------------------------------------------
{
    this->eepromSize = 4096;

    eeMenu256->setChecked(false);
    eeMenu512->setChecked(false);
    eeMenu1024->setChecked(false);
    eeMenu2048->setChecked(false);
    eeMenu4096->setChecked(true);

    mainLayout->calcEEPROM();
    mainLayout->update();
}

//#####################################################################################################################################################################################
void MainWindow::dispHelp()//------------------------------------------------------------
{
    QScrollArea* helpWindow = new QScrollArea();      //is new window
    QLabel* helpText = new QLabel(helpWindow);

    QString path = QCoreApplication::applicationFilePath().section('/', 0, -2) + "/README.txt";
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //show error message
        QMessageBox errorBox;
        errorBox.setText("Could not find \"README.txt\" in application folder.");
        errorBox.exec();
        //helpWindow->setText("No help for you");

        delete helpText;
        delete helpWindow;
        return;
    }
    else
    {
        //read file content to memory
        helpText->setText(file.readAll());
        file.close();
    }

    helpWindow->setWidget(helpText);
    //helpWindow->ensureVisible(100, 300);
    //helpWindow->ensureWidgetVisible(helpText);

    helpWindow->setMinimumWidth(120);
    helpWindow->setMinimumHeight(320);
    uint16_t windowW = helpText->rect().width() + 40;     //+40 =^ side croller
    if(windowW > 800) windowW = 800;
    uint16_t windowH = helpText->rect().height();
    if(windowH > 600) windowH = 600;
    helpWindow->setGeometry(this->rect().x(), this->rect().y(), windowW, windowH);

    helpWindow->setWindowTitle(windowTitle + " - Help");
    helpWindow->setWindowIcon(QIcon(QPixmap("CDC_Icon.png")));
    helpWindow->show();
}

void MainWindow::dispInfo()//------------------------------------------------------------
{
    QDesktopServices::openUrl(QUrl(appSourceCode));
}

//#####################################################################################################################################################################################
void MainWindow::textEdited()//------------------------------------------------------------
{
    if(fileStillOpen)
    {
        fileChanged = true;
        if(filePath.isEmpty())
        {
            setWindowTitle(windowTitle + ": New File" + " *");
        }
        else
        {
            setWindowTitle(windowTitle + ": " + filePath.section('/', -1) + " *");
        }
    }
}


