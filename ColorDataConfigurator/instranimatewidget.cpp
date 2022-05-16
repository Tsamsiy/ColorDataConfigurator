#include "instranimatewidget.h"

//#####################################################################################################################################################################################
InstrAnimateWidget::InstrAnimateWidget(QWidget *parent) : QWidget{parent}//-----------------------------------------------------------
{
    init();
}

InstrAnimateWidget::InstrAnimateWidget(const RGBInstruction* src, QWidget *parent) : QWidget{parent}//-----------------------------------------------------------
{
    init();

    this->instr = src;
    setup();
}

//#####################################################################################################################################################################################
void InstrAnimateWidget::init()//-----------------------------------------------------------
{
    //overall time of that instruction
    instrTmr = new QTimer(this);
    instrTmr->setTimerType(Qt::TimerType::PreciseTimer);    //minimum resolution of 1ms
    instrTmr->setSingleShot(true);
    connect(instrTmr, &QTimer::timeout, this, &InstrAnimateWidget::stop);

    //updates red subcolor
    rTmr = new QTimer(this);
    rTmr->setTimerType(Qt::TimerType::PreciseTimer);    //minimum resolution of 1ms
    rTmr->setSingleShot(false);
    connect(rTmr, &QTimer::timeout, this, &InstrAnimateWidget::refrR);

    //updates green subcolor
    gTmr = new QTimer(this);
    gTmr->setTimerType(Qt::TimerType::PreciseTimer);    //minimum resolution of 1ms
    gTmr->setSingleShot(false);
    connect(gTmr, &QTimer::timeout, this, &InstrAnimateWidget::refrG);

    //updates blue subcolor
    bTmr = new QTimer(this);
    bTmr->setTimerType(Qt::TimerType::PreciseTimer);    //minimum resolution of 1ms
    bTmr->setSingleShot(false);
    connect(bTmr, &QTimer::timeout, this, &InstrAnimateWidget::refrB);
}

void InstrAnimateWidget::setSourceRef(const RGBInstruction* src)//-----------------------------------------------------------
{
    this->instr = src;
    setup();
}

void InstrAnimateWidget::setLoop(bool l)//-----------------------------------------------------------
{
    this->loop = l;
}

void InstrAnimateWidget::startAnim()
{
    setup();

    //need this to avoid visual bug when time is 0
    if(this->instr->t == 0)
    {
        //in case of fade command, set to end color
        if(this->instr->com == RGBInstruction::F)
        {
            this->curColor = QColor(this->instr->c2.r, this->instr->c2.g, this->instr->c2.b);
        }

        emit done();
        return;
    }

    //start timer instances
    this->instrTmr->start();
    if(this->instr->com == RGBInstruction::F)
    {
        if(deltaR) this->rTmr->start();
        if(deltaG) this->gTmr->start();
        if(deltaB) this->bTmr->start();
    }
}

//#####################################################################################################################################################################################
void InstrAnimateWidget::setup()//-----------------------------------------------------------
{
    if(this->instr == nullptr)
    {
        #ifdef DEBUG
            qDebug() << "InstrAnimateWidget::setup(): source is nullptr";
        #endif
        return;
    }

    #ifdef DEBUG
        qDebug() << "InstrAnimateWidget::setup(): src = " << this->instr->print();
    #endif

    //initialize with primary color for non-loop instructions
    if(this->instr->com != RGBInstruction::L)
    {
        this->curColor = QColor(this->instr->c1.r, this->instr->c1.g, this->instr->c1.b);
    }
    //secondary color depends on command
    if(this->instr->com == RGBInstruction::F)
    {
        deltaR = this->instr->c2.r - this->instr->c1.r;
        deltaG = this->instr->c2.g - this->instr->c1.g;
        deltaB = this->instr->c2.b - this->instr->c1.b;

        if(deltaR) this->rTmr->setInterval(this->instr->t / abs(deltaR));
        if(deltaG) this->gTmr->setInterval(this->instr->t / abs(deltaG));
        if(deltaB) this->bTmr->setInterval(this->instr->t / abs(deltaB));
    }

    //instrTmr->setInterval(1);
    this->instrTmr->setInterval(this->instr->t);

    #ifdef DEBUG
        //qDebug() << "InstructionWidget::setup():";
        qDebug() << "\tdeltaR = " << deltaR << ", [" << this->rTmr->interval() << "ms]";
        qDebug() << "\tdeltaG = " << deltaG << ", [" << this->gTmr->interval() << "ms]";
        qDebug() << "\tdeltaB = " << deltaB << ", [" << this->bTmr->interval() << "ms]";
    #endif

    //show first color
    this->update();
}

void InstrAnimateWidget::refrR()//-----------------------------------------------------------
{
    #ifdef DEBUG
        qDebug() << "InstrAnimateWidget::refrR()";
    #endif

    int8_t d = 1;
    if(deltaR < 0) d = -1;
    this->curColor.setRed(this->curColor.red() + d);

    //emit currentColor(curColor);
    this->update();
}

void InstrAnimateWidget::refrG()//-----------------------------------------------------------
{
    #ifdef DEBUG
        qDebug() << "InstrAnimateWidget::refrG()";
    #endif

    int8_t d = 1;
    if(deltaG < 0) d = -1;
    this->curColor.setGreen(this->curColor.green() + d);

    //emit currentColor(curColor);
    this->update();
}

void InstrAnimateWidget::refrB()//-----------------------------------------------------------
{
    #ifdef DEBUG
        qDebug() << "InstrAnimateWidget::refrB()";
    #endif

    int8_t d = 1;
    if(deltaB < 0) d = -1;
    this->curColor.setBlue(this->curColor.blue() + d);

    //emit currentColor(curColor);
    this->update();
}

//#####################################################################################################################################################################################
void InstrAnimateWidget::paintEvent(QPaintEvent* event)//-----------------------------------------------------------
{
    //paint custom appearance
    QPainter painter(this);

    painter.fillRect(this->rect(), QBrush(this->curColor));

    //painter.restore();
    painter.end();
}

//#####################################################################################################################################################################################
void InstrAnimateWidget::start(const RGBInstruction& instr)//-----------------------------------------------------------
{
    #ifdef DEBUG
        qDebug() << "InstrAnimateWidget::start()";
    #endif

    setSourceRef(&instr);
    startAnim();
}

void InstrAnimateWidget::stop()//-----------------------------------------------------------
{
    //stop timer instance
    this->rTmr->stop();
    this->gTmr->stop();
    this->bTmr->stop();

    if(loop)
    {
        startAnim();
    }

    emit done();
}

void InstrAnimateWidget::clear()//-----------------------------------------------------------
{
    stop();
    //loop = false;
    this->curColor.setRgb(0, 0, 0);
    this->update();
}
