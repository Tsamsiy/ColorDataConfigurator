#include "instrlistwidget.h"

//#####################################################################################################################################################################################
InstrListWidget::InstrListWidget(QWidget *parent) : QWidget(parent)//-----------------------------------------------------------
{
    init();
}

InstrListWidget::InstrListWidget(const QString* src, const uint8_t* mode, QWidget *parent) : QWidget(parent)//-----------------------------------------------------------
{
    init();
    setSrcRef(src);
    setDispModeRef(mode);

    buildList();
}

void InstrListWidget::setSrcRef(const QString* src)//-----------------------------------------------------------
{
    srcRef = src;
}

void InstrListWidget::setDispModeRef(const uint8_t* mode)//-----------------------------------------------------------
{
    dispModeRef = mode;
}

//#####################################################################################################################################################################################
void InstrListWidget::init()//-----------------------------------------------------------
{
    this->setMinimumSize(QSize(100, 50));
    this->setVisible(true);
}

//#####################################################################################################################################################################################
void InstrListWidget::paintEvent(QPaintEvent* event)//-----------------------------------------------------------
{
    #ifdef DEBUG
        //qDebug() << "InstrListWidget::paintEvent(): update" << instrList.length() << "elements";
    #endif

    /*
    QPainter painter(this);
    painter.fillRect(this->rect(), Qt::gray);
    painter.end();//*/

    for(int i = 0; i < instrList.length(); i++)
    {
        instrList[i]->update();
    }
}

//#####################################################################################################################################################################################
void InstrListWidget::buildList()//-----------------------------------------------------------
{
    if(!srcRef)
    {
        #ifdef DEBUG
            qDebug() << "InstrListWidget::buildList(): source is nullptr";
        #endif
        return;
    }

    #ifdef DEBUG
        qDebug() << "InstrListWidget::buildList()";
    #endif

    if(srcRef->isEmpty())
    {
        emit parseError("No code to parse!");
        return;
    }

    clearList();

    bool err = false;
    QString srcStr = *srcRef;
    QStringList srcInstr;

    //find content between '{', '}'
    srcStr = srcStr.section('{', 1, 1);
    srcStr = srcStr.section('}', 0, 0);

    //remove formating
    srcStr.remove('\n');
    srcStr.remove('\t');
    srcStr.remove(' ');

    //separate instructions
    srcInstr = srcStr.split(';', Qt::KeepEmptyParts);
    if(srcInstr.length() < 2)
    {
        #ifdef DEBUG
            qDebug() << "InstrListWidget::buildList()->error: no valid code";
        #endif

        emit parseError("No valid code!");
        return;
    }
    srcInstr.pop_back();    //separator at end of every instruction creates an empty element in list

    //fill list
    for(int i = 0; i < srcInstr.length(); i++)
    {
        InstructionWidget* tmp = new InstructionWidget(this);
        RGBInstruction tmpInstr;
        QString errStr = "";

        srcInstr[i].append(';');    //QString::split() removes separator tokens
        tmpInstr.parse(srcInstr[i], errStr);
        if(!errStr.isEmpty())
        {
            errStr = "[" + QString::number(i) + "]: " + errStr;
            #ifdef DEBUG
                qDebug() << "InstrListWidget::buildList()->error:" << errStr;
            #endif

            err = true;
            tmp->setStatus(InstructionWidget::DispStatus::Error);
            emit parseError(errStr);
        }

        tmp->setContent(tmpInstr);
        tmp->setDispMode(this->dispModeRef);
        tmp->setMaximumWidth(300);
        tmp->move(5, i * (tmp->rect().height() + 5) + 5);
        tmp->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        tmp->setVisible(true);

        this->instrList.push_back(tmp);
    }

    this->setFixedWidth(instrList[0]->rect().width() + 10);
    this->setFixedHeight(((instrList[0]->rect().height() + 5) * instrList.length()) + 5);

    if(!err)
    {
        emit parseError(QString::number(instrList.length()) + " parsed with no errors.");
    }
    this->update();
}

void InstrListWidget::clearList()//-----------------------------------------------------------
{
    #ifdef DEBUG
        qDebug() << "InstrListWidget::clearList()";
    #endif

    //empty list
    while(!instrList.empty())
    {
        delete instrList.last();
        instrList.pop_back();
    }

    this->setFixedHeight(10);
    this->update();
}

QString InstrListWidget::exportList() const//-----------------------------------------------------------
{
    #ifdef DEBUG
        qDebug() << "InstrListWidget::exportList()";
    #endif

    QString res = "{\n";

    for(int i = 0; i < instrList.length(); i++)
    {
        res += '\t';
        res += instrList[i]->content()->print();
        res += '\n';
    }
    res += '}';

    return res;
}
