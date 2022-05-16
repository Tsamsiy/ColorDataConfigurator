#include "instructionwidget.h"

//#####################################################################################################################################################################################
InstructionWidget::InstructionWidget(QWidget* parent) : QWidget(parent)//------------------------------------------------------------
{
    initElements();
}

InstructionWidget::InstructionWidget(const QString& str, QWidget* parent) : QWidget(parent)//------------------------------------------------------------
{
    QString err = "";        //error string is discarded
    this->instr.parse(str, err);
    if(!err.isEmpty())
    {
        #ifdef DEBUG
            qDebug() << "RGBInstruction() bad argument:" << err;
        #endif

        setStatus(DispStatus::Error);
    }

    initElements();
}

InstructionWidget::InstructionWidget(const RGBInstruction& instr, QWidget* parent) : QWidget(parent)//------------------------------------------------------------
{
    this->instr = instr;

    initElements();
}

InstructionWidget::InstructionWidget(uint8_t com, RGBInstruction::HEXColor c1, RGBInstruction::HEXColor c2, uint16_t x, uint16_t t, QWidget* parent) : QWidget(parent)//------------------------------------------------------------
{
    this->instr.com = com;
    this->instr.c1 = c1;
    this->instr.c2 = c2;
    this->instr.x = x;
    this->instr.t = t;

    initElements();
}

//#####################################################################################################################################################################################
void InstructionWidget::setDispMode(const uint8_t* mode)//------------------------------------------------------------
{
    this->globDispMode = mode;
}

void InstructionWidget::setDispMode(const uint8_t& mode)//------------------------------------------------------------
{
    this->dispMode = mode;
}

void InstructionWidget::setStatus(uint8_t s)//------------------------------------------------------------
{
    this->status = s;
}

void InstructionWidget::setContent(const RGBInstruction& instr)//------------------------------------------------------------
{
    this->instr = instr;
}

void InstructionWidget::setContent(const RGBInstruction::HEXColor& c1, const RGBInstruction::HEXColor& c2)//------------------------------------------------------------
{
    this->instr.c1 = c1;
    this->instr.c2 = c2;
}

void InstructionWidget::setContent(uint8_t com, RGBInstruction::HEXColor c1, RGBInstruction::HEXColor c2, uint16_t x, uint16_t t)//------------------------------------------------------------
{
    this->instr.com = com;
    this->instr.c1 = c1;
    this->instr.c2 = c2;
    this->instr.x = x;
    this->instr.t = t;
}

const RGBInstruction* InstructionWidget::content()//------------------------------------------------------------
{
    return &this->instr;
}

void InstructionWidget::setPriColor(QColor c)//------------------------------------------------------------
{
    this->instr.c1.r = c.red();
    this->instr.c1.g = c.green();
    this->instr.c1.b = c.blue();
}

void InstructionWidget::setSecColor(QColor c)//------------------------------------------------------------
{
    this->instr.c2.r = c.red();
    this->instr.c2.g = c.green();
    this->instr.c2.b = c.blue();
}

//#####################################################################################################################################################################################
void InstructionWidget::initElements()
{
    //place text labels
    dispComText = new QLabel(this);
    dispComText->setToolTip("Instruction type");

    dispC1Text = new QLabel(this);
    dispC1Text->setToolTip("Primary color");

    dispC2Text = new QLabel(this);
    dispC2Text->setToolTip("Secondary color");

    dispXText = new QLabel(this);
    dispXText->setToolTip("Number of instructions to skip");

    dispTText = new QLabel(this);
    dispTText->setToolTip("Duration of instruction");

    //set height requirements
    this->setMinimumHeight(PixTotalWdgtH);
    this->setMinimumWidth(PixTotalWdgtW);

    //color picker dialog
    c1Pick = new QColorDialog(this);
    c1Pick->setWindowTitle("Select primary color");
    c1Pick->setOption(QColorDialog::ColorDialogOption::NoButtons, false);
    c1Pick->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, false);
    connect(c1Pick, &QColorDialog::colorSelected, this, &InstructionWidget::setPriColor);

    c2Pick = new QColorDialog(this);
    c2Pick->setWindowTitle("Select secondary color");
    c2Pick->setOption(QColorDialog::ColorDialogOption::NoButtons, false);
    c2Pick->setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, false);
    connect(c2Pick, &QColorDialog::colorSelected, this, &InstructionWidget::setSecColor);

    //enable focus by clicking
    this->setFocusPolicy(Qt::ClickFocus);

    //enable context menus
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &InstructionWidget::customContextMenuRequested, this, &InstructionWidget::showContextMenu);
}

//#####################################################################################################################################################################################
void InstructionWidget::paintEvent(QPaintEvent* event)//-----------------------------------------------------------
{
    QPoint tmpPos;
    tmpPos.setX(this->rect().x()  + PixXSpace);
    tmpPos.setY(this->rect().y()  + PixYSpace);
    QString comStr;
    QString c1Str;
    QString c2Str;
    QString xStr;
    QString tStr;

    //paint custom appearance
    QPainter painter(this);

    //draw body
    switch(this->status)
    {
        case DispStatus::NoFocus:
            painter.setPen(Qt::black);
            painter.setBrush(Qt::lightGray);
        break;

        case DispStatus::HasFocus:
            painter.setPen(Qt::cyan);
            painter.setBrush(Qt::white);
        break;

        case DispStatus::Error:
            painter.setPen(Qt::red);
            painter.setBrush(QColor(0xC0, 0x83, 0x83));
        break;

        default:
            //nope
        break;
    }
    if(this->hasFocus())
    {
        painter.setBrush(Qt::white);
    }
    //painter.drawRect(this->rect());
    painter.drawRect(this->rect().x(), this->rect().y(), this->rect().size().width() - 2, this->rect().size().height() - 2);
    //painter.fillRect(this->rect(), Qt::lightGray);

    //draw color preview
    if(this->instr.com != RGBInstruction::L)
    {
        QLinearGradient colorPrev(tmpPos, QPoint(tmpPos.x() + (PixParam1Tab / 2), tmpPos.y() + PixTxtHeight));
        colorPrev.setColorAt(0, QColor(this->instr.c1.r, this->instr.c1.g, this->instr.c1.b));
        if(this->instr.com == RGBInstruction::F)
        {
            colorPrev.setColorAt(1, QColor(this->instr.c2.r, this->instr.c2.g, this->instr.c2.b));
        }
        else
        {
            colorPrev.setColorAt(1, QColor(this->instr.c1.r, this->instr.c1.g, this->instr.c1.b));
        }
        painter.setPen(Qt::black);
        painter.setBrush(colorPrev);
        painter.drawRect(tmpPos.x(), tmpPos.y() + PixTxtHeight + PixTxtHeight, PixParam1Tab / 2, PixTxtHeight);
    }

    //painter.restore();
    painter.end();

    //update text labels
    switch(this->instr.com)
    {
        case RGBInstruction::C:
            comStr = "fix color";
        break;

        case RGBInstruction::F:
            comStr = "fade color";
        break;

        case RGBInstruction::L:
            comStr = "loop";
        break;

        default:
            //nope
            comStr = "";
        break;
    }
    dispComText->setText(comStr);
    dispComText->setMinimumSize(comStr.length() * PixTxtCharSize, PixTxtHeight);
    dispComText->move(tmpPos.x(), tmpPos.y());

    uint8_t tmpDispMode = dispMode;
    if(globDispMode != nullptr)
    {
        tmpDispMode = *globDispMode;
    }
    c1Str = QString::number(this->instr.c1.r, tmpDispMode) + ":" + QString::number(this->instr.c1.g, tmpDispMode) + ":" + QString::number(this->instr.c1.b, tmpDispMode);
    c2Str = QString::number(this->instr.c2.r, tmpDispMode) + ":" + QString::number(this->instr.c2.g, tmpDispMode) + ":" + QString::number(this->instr.c2.b, tmpDispMode);
    c1Str = c1Str.toUpper();
    c2Str = c2Str.toUpper();
    dispC1Text->setText(c1Str);
    if(this->instr.com == RGBInstruction::F)
    {
        dispC2Text->setText(c2Str);
    }
    else
    {
        dispC2Text->setText("-");
    }
    if(this->instr.com == RGBInstruction::L)
    {
        dispC1Text->setText("-");
        dispC2Text->setText("-");
    }
    dispC1Text->setMinimumSize(c1Str.length() * PixTxtCharSize, PixTxtHeight);
    dispC2Text->setMinimumSize(c2Str.length() * PixTxtCharSize, PixTxtHeight);
    //dispC1Text->move(tmpPos.x() + dispComText->size().width(), tmpPos.y());
    //dispC2Text->move(tmpPos.x() + dispComText->size().width(), tmpPos.y() + PixTxtHeight + PixTxtTxtLine);
    dispC1Text->move(tmpPos.x() + PixParam1Tab, tmpPos.y());
    dispC2Text->move(tmpPos.x() + PixParam1Tab, tmpPos.y() + PixTxtHeight + PixTxtTxtLine);

    tStr = tStr.number(this->instr.t, 10);
    dispTText->setText(tStr);
    dispTText->setMinimumSize(tStr.length() * PixTxtCharSize, PixTxtHeight);
    //dispTText->move(tmpPos.x() + dispComText->size().width() + dispC1Text->size().width(), tmpPos.y());
    dispTText->move(tmpPos.x() + PixParam2Tab, tmpPos.y());

    if(this->instr.com == RGBInstruction::L)
    {
        xStr = xStr.number(this->instr.x, 10);
        dispXText->setText(xStr);
    }
    else
    {
        dispXText->setText("-");
    }
    dispXText->setMinimumSize(xStr.length() * PixTxtCharSize, PixTxtHeight);
    //dispXText->move(tmpPos.x() + dispComText->size().width() + dispC1Text->size().width(), tmpPos.y() + PixTxtHeight + PixTxtTxtLine);
    dispXText->move(tmpPos.x() + PixParam2Tab, tmpPos.y() + PixTxtHeight + PixTxtTxtLine);
}

void InstructionWidget::focusInEvent(QFocusEvent* event)//------------------------------------------------------------
{
    emit selected(this->instr);
    setStatus(DispStatus::HasFocus);
    this->update();
}

void InstructionWidget::focusOutEvent(QFocusEvent* event)//------------------------------------------------------------
{
    setStatus(DispStatus::NoFocus);
    this->update();
}

void InstructionWidget::showContextMenu(const QPoint& pos)//------------------------------------------------------------
{
    QMenu contextMenu(tr("context menu"), this);

    QAction pickC1(tr("pick primary color"), this);
    connect(&pickC1, &QAction::triggered, this, &InstructionWidget::pickPriColor);

    QAction pickC2(tr("pick secondary color"), this);
    connect(&pickC2, &QAction::triggered, this, &InstructionWidget::pickSecColor);

    if(this->instr.com != RGBInstruction::L)
    {
        contextMenu.addAction(&pickC1);

        if(this->instr.com == RGBInstruction::F)
        {
            contextMenu.addAction(&pickC2);
        }
    }

    contextMenu.exec(mapToGlobal(pos));
}

//#####################################################################################################################################################################################
void InstructionWidget::InstructionWidget::pickPriColor()//------------------------------------------------------------
{
    c1Pick->setCurrentColor(QColor(this->instr.c1.r, this->instr.c1.g, this->instr.c1.b));
    c1Pick->open();
}

void InstructionWidget::InstructionWidget::pickSecColor()//------------------------------------------------------------
{
    c2Pick->setCurrentColor(QColor(this->instr.c2.r, this->instr.c2.g, this->instr.c2.b));
    c2Pick->open();
}

