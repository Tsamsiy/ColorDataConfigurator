#ifndef LAYOUTWIDGET_H
#define LAYOUTWIDGET_H

#include <QString>

//Formating
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>

//ui elements
#include <QTextEdit>
#include <QPushButton>
#include <QCheckBox>
#include "instructionwidget.h"
#include "instrlistwidget.h"
#include "instranimatewidget.h"

//#####################################################################################################################################################################################
class layoutWidget : public QWidget
{
    Q_OBJECT

public:
    layoutWidget(QWidget* parent = nullptr, uint8_t* dispMode = nullptr, uint16_t* eepromSize = nullptr);
    QString getEditorText();
    QString getConsoleText();
    void calcEEPROM();

private:
    //Text IO
    QTextEdit* editorBox;
    QString editorText;
    void textChanged();
    QTextEdit* consoleBox;

    //parse and check buttons
    QPushButton* parseFromCode;
    QPushButton* parseToCode;
    QLabel* btnTopFill;
    QLabel* btnBotFill;

    //Instruction boxes
    InstrListWidget* instrList;
    QScrollArea* instrBox;
    QLabel* eeprInfo;

    //Formating
    QVBoxLayout* layTextIO;
    QVBoxLayout* layCodeBtns;
    QVBoxLayout* layInstr;
    QVBoxLayout* layAnim;
    QHBoxLayout* layAnimBtns;
    QHBoxLayout* layAll;

    //Sequence animation
    InstrAnimateWidget* instrAnimate;
    QCheckBox* animateLoop;
    QLabel* animateLoopLabel;
    QPushButton* animateStart;
    bool animInPrgr = false;
    uint16_t animPtr = 0;

    //settings from MainWindow
    uint8_t* displayFormat;
    uint16_t* eepromSize;

signals:
    void textEdited();

public slots:
    void setEditorText(const QString& text);
    void setConsoleText(const QString& text);
    void clearList();

    void parseCode();
    void parseList();

    void startSeqAnim();
    void seqAnimUpd();
    void clearSeqAnim();
};

#endif // LAYOUTWIDGET_H
