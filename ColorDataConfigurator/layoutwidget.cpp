#include "layoutwidget.h"

//#####################################################################################################################################################################################
layoutWidget::layoutWidget(QWidget* parent, uint8_t* dispMode, uint16_t* eepromSize) : QWidget(parent)//------------------------------------------------------------
{
    //copy settings
    this->displayFormat = dispMode;
    this->eepromSize = eepromSize;

    //create text editor field
    editorBox = new QTextEdit(this);
    //editorBox->setPlaceholderText("Your Code goes here");
    QString sampleText;
    sampleText = "Your code goes here:\n";
    sampleText += "\n";
    sampleText += "Available Commands:\n";
    sampleText += "--------------------------------------------\n";
    sampleText += "Color    C:rrggbb,t;\n";
    sampleText += "Displays a fixed color for set amount of time\n";
    sampleText += "     t       uint16_t     time in ms (0-65535)\n";
    sampleText += "     rr      byte     red HEX value (0-9, A-F)\n";
    sampleText += "     gg    byte     green HEX value (0-9, A-F)\n";
    sampleText += "     bb    byte     blue HEX value (0-9, A-F)\n";
    sampleText += "\n";
    sampleText += "Fade linear     F:rrggbb,rrggbb,t;";
    sampleText += "\nFades from first to second color\n";
    sampleText += "     t       uint16_t     time in ms (0-65535)\n";
    sampleText += "     rr      byte     red HEX value (0-9, A-F)\n";
    sampleText += "     gg    byte     green HEX value (0-9, A-F)\n";
    sampleText += "     bb    byte     blue HEX value (0-9, A-F)\n";
    sampleText += "\n";
    sampleText += "Loop\tL:x,t;\n";
    sampleText += "Jumps to beginning and skips x commands\n";
    sampleText += "     x       uint8_t     to skip (0-255),(0-65535)\n";
    sampleText += "     t       uint16_t     delay in ms(0-65535)\n";
    sampleText += "\n";
    sampleText += "(i) comments are not supported!\n";
    editorBox->setPlaceholderText(sampleText);
    editorBox->setWordWrapMode(QTextOption::NoWrap);
    editorBox->setTabStopDistance(30);
    editorBox->setGeometry(20, 30, 200, 300);
    editorBox->setMinimumWidth(160);
    editorBox->setMaximumWidth(500);
    editorBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    connect(editorBox, &QTextEdit::textChanged, this, &layoutWidget::textChanged);

    //create the error log output
    consoleBox = new QTextEdit(this);
    consoleBox->setPlaceholderText("No Errors");
    consoleBox->setReadOnly(true);
    consoleBox->setTabStopDistance(30);
    consoleBox->setGeometry(20, 360, 200, 30);
    consoleBox->setMinimumWidth(160);
    consoleBox->setMaximumWidth(500);
    consoleBox->setMinimumHeight(30);
    consoleBox->setMaximumHeight(60);
    consoleBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);

    //parse and check buttons
    parseFromCode = new QPushButton("->", this);
    parseFromCode->setToolTip("Translate code to list");
    parseFromCode->setMinimumWidth(20);
    parseFromCode->setMaximumWidth(30);
    parseFromCode->setMinimumHeight(20);
    parseFromCode->setMaximumHeight(30);
    connect(parseFromCode, &QPushButton::clicked, this, &layoutWidget::parseCode);

    parseToCode = new QPushButton("<-", this);
    parseToCode->setToolTip("Export list to code\n[re-formats code]");
    parseToCode->setMinimumWidth(20);
    parseToCode->setMaximumWidth(30);
    parseToCode->setMinimumHeight(20);
    parseToCode->setMaximumHeight(30);
    connect(parseToCode, &QPushButton::clicked, this, &layoutWidget::parseList);

    btnTopFill = new QLabel(this);
    btnTopFill->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    btnBotFill = new QLabel(this);
    btnBotFill->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    //list of instruction boxes
    instrList = new InstrListWidget(&editorText, this->displayFormat, this);
    //connect(parseFromCode, &QPushButton::clicked, instrList, &InstrListWidget::buildList);
    //connect(parseToCode, &QPushButton::clicked, instrList, &InstrListWidget::buildList);
    connect(instrList, &InstrListWidget::parseError, consoleBox, &QTextEdit::setText);

    //inset window which holds list of instruction boxes
    instrBox = new QScrollArea(this);
    instrBox->setWidget(instrList);
    //instrBox->setWidgetResizable(true);
    instrBox->ensureVisible(100, 60);
    instrBox->ensureWidgetVisible(instrList);
    instrBox->setMinimumWidth(80);
    instrBox->setMaximumWidth(225);
    instrBox->setMinimumHeight(80);

    eeprInfo = new QLabel(this);
    //eeprInfo->setMinimumHeight(20);
    //eeprInfo->setMaximumHeight(30);
    //eeprInfo->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    calcEEPROM();

    //Sequence animation
    instrAnimate = new InstrAnimateWidget(this);//test1->content(),
    //instrAnimate->setLoop(true);
    instrAnimate->setMinimumWidth(80);
    //instrAnimate->setMaximumWidth(200);
    instrAnimate->setMinimumHeight(160);
    //instrAnimate->setMaximumHeight(500);
    instrAnimate->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    connect(instrAnimate, &InstrAnimateWidget::done, this, &layoutWidget::seqAnimUpd);

    //animateLoopLabel = new QLabel("loop:", this);
    animateLoop = new QCheckBox("loop", this);
    animateLoop->setMinimumWidth(50);
    animateLoop->setMaximumWidth(50);
    animateLoop->setMinimumHeight(20);
    animateLoop->setMaximumHeight(30);
    connect(animateLoop, &QCheckBox::stateChanged, instrAnimate, &InstrAnimateWidget::setLoop);

    animateStart = new QPushButton("Show", this);
    animateStart->setMinimumWidth(50);
    animateStart->setMaximumWidth(80);
    animateStart->setMinimumHeight(20);
    animateStart->setMaximumHeight(30);
    connect(animateStart, &QPushButton::clicked, this, &layoutWidget::startSeqAnim);

    //Formating
    layTextIO = new QVBoxLayout;
    layTextIO->addWidget(editorBox);
    layTextIO->addWidget(consoleBox);

    layCodeBtns = new QVBoxLayout;
    layCodeBtns->addWidget(btnTopFill);
    layCodeBtns->addWidget(parseFromCode);
    layCodeBtns->addWidget(parseToCode);
    layCodeBtns->addWidget(btnBotFill);

    layInstr = new QVBoxLayout;
    layInstr->addWidget(instrBox);
    layInstr->addWidget(eeprInfo);

    layAnimBtns = new QHBoxLayout;
    //layAnimBtns->addWidget(animateLoopLabel);
    layAnimBtns->addWidget(animateLoop);
    layAnimBtns->addWidget(animateStart);

    layAnim = new QVBoxLayout;
    layAnim->addWidget(instrAnimate);
    layAnim->addLayout(layAnimBtns);

    layAll = new QHBoxLayout(this);
    layAll->addLayout(layTextIO);
    layAll->addLayout(layCodeBtns);
    layAll->addLayout(layInstr);
    layAll->addLayout(layAnim);
}

QString layoutWidget::getEditorText()//------------------------------------------------------------
{
    return editorBox->toPlainText();
}

QString layoutWidget::getConsoleText()//------------------------------------------------------------
{
    return consoleBox->toPlainText();
}

//#####################################################################################################################################################################################
void layoutWidget::textChanged()//------------------------------------------------------------
{
    editorText = editorBox->toPlainText();
    emit textEdited();
}

//#####################################################################################################################################################################################
void layoutWidget::setEditorText(const QString& text)//------------------------------------------------------------
{
    editorBox->setText(text);
}

void layoutWidget::setConsoleText(const QString& text)//------------------------------------------------------------
{
    consoleBox->setText(text);
}

void layoutWidget::clearList()//------------------------------------------------------------
{
    instrList->clearList();
}

void layoutWidget::calcEEPROM()//------------------------------------------------------------
{
    int bytes;
    if(*eepromSize < 2048)
    {
        bytes = instrList->instrList.length() * 10;
    }
    {
        bytes = instrList->instrList.length() * 11;     //x parameter needs 2 bytes to adress all instruction spots
    }
    double usage = ((double)bytes / (double)*eepromSize) * 100.0;
    eeprInfo->setText("[" + QString::number(*eepromSize, 10) + "]: " + QString::number(bytes, 10) + " Bytes (" + QString::number(usage, 'd', 1) + "%)");
}

void layoutWidget::parseCode()//------------------------------------------------------------
{
    //stop a running animation
    if(animInPrgr) startSeqAnim();     //pressing start during running animation stops it

    //parse code to objects
    instrList->buildList();

    //connect objects to animation windoww
    if(!instrList->instrList.isEmpty())
    {
        for(int i = 0; i < instrList->instrList.length(); i++)
        {
            connect(instrList->instrList[i], &InstructionWidget::selected, instrAnimate, &InstrAnimateWidget::start);
        }
    }

    //calculate storage usage
    calcEEPROM();
}

void layoutWidget::parseList()//------------------------------------------------------------
{
    editorBox->setText(instrList->exportList());
}

void layoutWidget::startSeqAnim()//------------------------------------------------------------
{
    //list needs to contain at least one instruction!
    if(instrList->instrList.length() < 1)
    {
        setConsoleText("Instruction list is empty!");
        return;
    }

    //if start pressed while running: stop
    if(animInPrgr)
    {
        //take away focus marking before reset
        instrList->instrList[animPtr]->setStatus(InstructionWidget::DispStatus::NoFocus);

        animPtr = 0;
        animInPrgr = false;
        animateStart->setText("Show");
        instrAnimate->stop();
        return;
    }

    animPtr = 0;
    animInPrgr = true;
    animateStart->setText("Stop");

    instrList->instrList[animPtr]->setStatus(InstructionWidget::DispStatus::HasFocus);
    instrAnimate->start(*instrList->instrList[animPtr]->content());//seqAnimUpd();
}

void layoutWidget::seqAnimUpd()//------------------------------------------------------------
{
    //previous instruction is done
    //check for next action if animation is running
    if(animInPrgr)
    {
        instrList->instrList[animPtr]->setStatus(InstructionWidget::DispStatus::NoFocus);

        //check for loop instruction
        if(instrList->instrList[animPtr]->content()->com == RGBInstruction::CommandID::L)
        {
            //jump to designated adress
            animPtr = instrList->instrList[animPtr]->content()->x;
            if(animPtr >= instrList->instrList.length())     //if out of bounds, strart at beginning
            {
                animPtr = 0;
            }
            instrList->instrList[animPtr]->setStatus(InstructionWidget::DispStatus::HasFocus);
            instrAnimate->start(*instrList->instrList[animPtr]->content());
        }
        else if(animPtr < (instrList->instrList.length() - 1))   //end not reached
        {
            //advance to next instr if not loop instr
            animPtr++;
            instrList->instrList[animPtr]->setStatus(InstructionWidget::DispStatus::HasFocus);
            instrAnimate->start(*instrList->instrList[animPtr]->content());
        }
        else    //end reached
        {
            //animInPrgr needs to be cleared for auto restart
            animPtr = 0;
            animInPrgr = false;

            //restart animation
            if(animateLoop->isChecked())
            {
                startSeqAnim();
            }
        }
    }
}

void layoutWidget::clearSeqAnim()//------------------------------------------------------------
{
    if(animInPrgr) startSeqAnim();     //pressing start during running animation stops it
    instrAnimate->clear();
}




