#include "gifcapdialog.h"
#include<QPainter>
#include<QBoxLayout>
#include<QPushButton>
#include<QLabel>
#include<QStyle>
#include<QApplication>

#include<QSpinBox>
#include<QLineEdit>
#include<QDebug>
#include<QColor>
#include<QBrush>
#include<QMouseEvent>
#include<QSizeGrip>
#include<QFont>
#include<QTimer>
#include<QScreen>
GifCapDialog::GifCapDialog(QWidget*parent)
    :QDialog(parent)
    ,borderSize(5)
    ,borderColor(qRgb(255,255,255))
    ,titleBarHeight(35)
    ,sizeGrip(new QSizeGrip(this))
    ,buttomLeftSizeGrip(new QSizeGrip(this))
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(windowFlags() | Qt::WindowType::FramelessWindowHint);
    this->setMouseTracking(true);
    //this->setSizeGripEnabled(true); //右下角可以调整窗口大小的手柄，不知道为啥只显示一次？
    this->resize(640,480);
    this->installEventFilter(this);

    sizeGrip->setStyleSheet("border-image:url(':/images/buttom_right.png')");
    sizeGrip->setFixedSize(16,16);

    buttomLeftSizeGrip->setStyleSheet("border-image:url(':/images/buttom_left.png')");
    buttomLeftSizeGrip->setFixedSize(16,16);

    //this->setSizePolicy(QSizePolicy::Policy::Ignored,QSizePolicy::Policy::Fixed);

    frameTimer = new QTimer(this);
    connect(frameTimer,&QTimer::timeout,this,&GifCapDialog::addFrame);

    timeTimer = new QTimer(this);
    connect(timeTimer,&QTimer::timeout,this,[this]()
    {
        ++time;
        QString timeStr = QString("%1:%2").arg(time/60,2,10,QChar('0')).arg(time%60,2,10,QChar('0'));
        timeBtn->setText(timeStr);
    });

    initUi();
}

bool GifCapDialog::eventFilter(QObject *watched, QEvent *ev)
{
    static QPoint begPos;
    static bool isPressed = false;
    QMouseEvent*event = static_cast<QMouseEvent*>(ev);
    switch (event->type())
    {
    case QEvent::MouseButtonPress:
        if(event->button() == Qt::MouseButton::LeftButton)
        {
            isPressed = true;
            begPos = event->pos();
        }
    return true;
    case QEvent::MouseMove:
        if(isPressed)
        {
            this->move(event->globalPos() - begPos);
        }
    return true;
    case QEvent::MouseButtonRelease:
        if(event->button() == Qt::MouseButton::LeftButton)
        {
             isPressed = false;
        }
    return true;
    }
    return QDialog::eventFilter(watched,ev);
}

void GifCapDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(202,202,202),2.0));
    painter.setBrush(borderColor);
    //给整个窗口绘制一个填充矩形，并绘制外边线
    painter.drawRoundedRect(this->rect(),5,5);

    painter.save();
    //计算需要透明的矩形区域
    gifCapRect = QRect(QPoint(borderSize,titleBarHeight),QSize(widthEdit->text().toInt(),heightEdit->text().toInt()));
    //绘制内边线
    painter.drawRect(gifCapRect.adjusted(-1,-1,1,1));

    //目标中的像素被清除(设置为完全透明)
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.drawRect(gifCapRect);
    painter.restore();

   // qDebug()<<"paintEvent";
}

void GifCapDialog::resizeEvent(QResizeEvent *)
{
    //根据窗口大小求出gif捕获区域大小
    int w = width() - borderSize * 2;
    int h = height() - titleBarHeight*2;
    widthEdit->setText(QString::number(w));
    heightEdit->setText(QString::number(h));
    qDebug()<<"resizeEvent"<<size();

    if(width()-100 < buttomLayout->sizeHint().width())
    {
        fpsSpinbox->hide();
    }
    else
    {
        fpsSpinbox->show();
    }
    //动态调整拉伸把手的位置
    sizeGrip->move(width()-15,height()-15);
    buttomLeftSizeGrip->move(0,height()-15);
}


void GifCapDialog::initUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    //mainLayout->setSpacing(0);

    //mainLayout->addLayout(createTopLayout());
    mainLayout->addWidget(createTopLayout());
    mainLayout->addStretch();
    createbuttomLayout();
    mainLayout->addLayout(createbuttomLayout());
    mainLayout->addSpacing(1);
    setLayout(mainLayout);

    //qDebug()<<"buttomLayout sizehint"<<buttomLayout->sizeHint();
}

QWidget* GifCapDialog::createTopLayout()
{
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setMargin(0);
    hlayout->setSpacing(0);

    topWidget = new QWidget;
    timeBtn = new QPushButton;

    //图标
    iconBtn = new QPushButton;
    iconBtn->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    iconBtn->setFlat(true);
    iconBtn->setObjectName("iconBtn");
    iconBtn->setStyleSheet("#iconBtn{border:none}");
    hlayout->addWidget(iconBtn);

    //标题
    titleLabel = new QLabel;
    titleLabel->setText("GifCapture");
    titleLabel->setFont(QFont("楷体",13));
    hlayout->addWidget(titleLabel);
    titleLabel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);

    hlayout->addStretch();

    timeBtn->setText("00:00");
    timeBtn->setObjectName("timeBtn");
    timeBtn->setStyleSheet("#timeBtn{border:1px solid red;border-radius:5px;background-color:red;"
                           "font-size:16pt;"
                           "font-family:Arial;"
                           "color:white;}");
    hlayout->addWidget(timeBtn);
    timeBtn->hide();


    //最小化按钮
    miniBtn = new QPushButton;
    miniBtn->setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_TitleBarMinButton));
    miniBtn->setObjectName("miniBtn");
    miniBtn->setStyleSheet("#miniBtn{border:none}");
    miniBtn->setFixedSize(32,32);
    hlayout->addWidget(miniBtn);

    closeBtn = new QPushButton;
    closeBtn->setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_TitleBarCloseButton));
    closeBtn->setObjectName("closeBtn");
    closeBtn->setStyleSheet("#closeBtn{border:none}");
    closeBtn->setFixedSize(32,32);
    hlayout->addWidget(closeBtn);

    topWidget->setLayout(hlayout);

    connect(closeBtn,&QPushButton::clicked,qApp,&QApplication::quit);
    connect(miniBtn,&QPushButton::clicked,this,&QWidget::showMinimized);
    return topWidget;
}

QLayout *GifCapDialog::createbuttomLayout()
{
    QHBoxLayout* buttomLayout = new QHBoxLayout;
    buttomLayout->setMargin(0);
    buttomLayout->setSpacing(0);
    //buttomLayout->setSizeConstraint(QBoxLayout::SizeConstraint::SetFixedSize);

    fpsSpinbox = new QSpinBox;
    widthEdit = new QLineEdit;
    heightEdit = new QLineEdit;
    startBtn = new QPushButton;
    pauseBtn = new QPushButton;
    stopBtn = new QPushButton;
    giveUpBtn = new QPushButton;

    //fps
    fpsSpinbox->setValue(10);
    fpsSpinbox->setRange(1,30);
    fpsSpinbox->setFixedSize(45,25);
    fpsSpinbox->setObjectName("fpsSpinbox");
    QString fpsSpinboxStyle = "#fpsSpinbox::up-button,#fpsSpinbox::down-button{border:none;}"
                              "#fpsSpinbox::up-button{background-image:url(':/images/up_button.png');}"
                              "#fpsSpinbox::down-button{background-image:url(':/images/down_button.png');}"
                              "#fpsSpinbox::up-button:hover{background-color:rgb(227,235,246);}"
                              "#fpsSpinbox::down-button:hover{background-color:rgb(227,235,246);}";

    //width
    widthEdit->setFixedSize(35,25);
    widthEdit->setText("640");
    widthEdit->setAlignment(Qt::AlignCenter);
    widthEdit->setObjectName("widthEdit");
    //widthEdit->setSizePolicy(QSizePolicy::Policy::Preferred,QSizePolicy::Policy::Preferred);

    //height
    heightEdit->setFixedSize(35,25);
    heightEdit->setText("480");
    heightEdit->setAlignment(Qt::AlignCenter);
    heightEdit->setObjectName("heightEdit");

    //录制
    startBtn->setFixedSize(55,titleBarHeight-3);
    startBtn->setText("录制");
    startBtn->setIcon(QIcon(":/images/start.png"));
    startBtn->setObjectName("startBtn");

    //暂停
    pauseBtn->setMinimumSize(55,titleBarHeight-3);
    pauseBtn->setText("暂停");
    pauseBtn->setIcon(QIcon(":/images/pause.png"));
    pauseBtn->setObjectName("pauseBtn");

    //停止
    stopBtn->setMinimumSize(55,titleBarHeight-3);
    stopBtn->setText("停止");
    stopBtn->setIcon(QIcon(":/images/stop.png"));
    stopBtn->setObjectName("stopBtn");

    //放弃
    giveUpBtn->setMinimumSize(55,titleBarHeight-3);
    giveUpBtn->setText("放弃");
    giveUpBtn->setIcon(QIcon(":/images/giveUp.png"));
    giveUpBtn->setObjectName("giveUpBtn");

    this->setStyleSheet(fpsSpinboxStyle + "#startBtn,#pauseBtn,#stopBtn,#giveUpBtn{border:none;} "
                        "#startBtn:hover,#pauseBtn:hover,#stopBtn:hover,#giveUpBtn:hover{background-color:rgb(227,235,246);}"
                        "#widthEdit,#heightEdit,#fpsSpinbox{border:1px solid gray;border-radius:5px;}");

    buttomLayout->addStretch();
    buttomLayout->addWidget(fpsSpinbox);
    buttomLayout->addWidget(new QLabel("fps"));

    buttomLayout->addWidget(widthEdit);
    buttomLayout->addWidget(new QLabel("×"));
    buttomLayout->addWidget(heightEdit);
    buttomLayout->addWidget(new QLabel("px"));

    buttomLayout->addWidget(giveUpBtn);
    buttomLayout->addWidget(startBtn);
    buttomLayout->addWidget(pauseBtn);
    buttomLayout->addWidget(stopBtn);

    buttomLayout->addSpacing(borderSize+5);

    //先隐藏掉这三个，需要的时候再显示
    pauseBtn->hide();
    stopBtn->hide();
    giveUpBtn->hide();


    connect(fpsSpinbox,QOverload<int>::of(&QSpinBox::valueChanged),this,&GifCapDialog::onFpsSpinbox);
    connect(widthEdit,&QLineEdit::editingFinished,this,&GifCapDialog::onWidthEdit);
    connect(heightEdit,&QLineEdit::editingFinished,this,&GifCapDialog::onHeightEdit);
    connect(startBtn,&QPushButton::released,this,&GifCapDialog::onStartBtn);
    connect(pauseBtn,&QPushButton::released,this,&GifCapDialog::onPauseBtn);
    connect(stopBtn,&QPushButton::released,this,&GifCapDialog::onStopBtn);
    connect(giveUpBtn,&QPushButton::released,this,&GifCapDialog::onGiveUpBtn);
    this->buttomLayout = buttomLayout;
    return buttomLayout;
}

void GifCapDialog::onFpsSpinbox(int value)
{
    fps = value;
    qDebug()<<"onFpsSpinbox"<<value;
}

void GifCapDialog::onWidthEdit()
{
    //改变窗口大小
    QSize Size = this->size();
    Size.setWidth(widthEdit->text().toInt()+borderSize*2);
    setFixedSize(Size);

    //根据文字调整LineEdit的宽度
    int textwidth = widthEdit->text().size()* widthEdit->font().pointSize();
    widthEdit->setFixedWidth(textwidth);

    //输入完成之后清除焦点
    widthEdit->clearFocus();
    qDebug()<<"onWidthEdit"<<widthEdit->font().pixelSize()<<widthEdit->font().pointSize();
}

void GifCapDialog::onHeightEdit()
{
    QSize Size = this->size();
    Size.setHeight(heightEdit->text().toInt()+titleBarHeight*2);
    setFixedSize(Size);
    heightEdit->clearFocus();
    qDebug()<<"onHeightEdit";
}

void GifCapDialog::onStartBtn()
{
    startBtn->hide();   //录制按钮隐藏
    giveUpBtn->hide();
    pauseBtn->show();
    stopBtn->show();

    timeBtn->show();

    //禁止调整窗口大小
    sizeGrip->setEnabled(false);
    buttomLeftSizeGrip->setEnabled(false);
    fpsSpinbox->setEnabled(false);
    widthEdit->setEnabled(false);
    heightEdit->setEnabled(false);

    record();

    qDebug()<<"onStartBtn";
}

void GifCapDialog::onPauseBtn()
{
    pauseBtn->hide();
    startBtn->show();
    giveUpBtn->show();

    recordPause();
}

void GifCapDialog::onStopBtn()
{
    sizeGrip->setEnabled(true);
    buttomLeftSizeGrip->setEnabled(true);
    fpsSpinbox->setEnabled(true);
    widthEdit->setEnabled(true);
    heightEdit->setEnabled(true);

    startBtn->show();
    pauseBtn->hide();
    stopBtn->hide();
    giveUpBtn->hide();
    timeBtn->hide();

    recordStop();
}

void GifCapDialog::onGiveUpBtn()
{
    qDebug()<<"onGiveUpBtn";
    recordGiveUp();
}

void GifCapDialog::addFrame()
{
    QScreen *screen = QApplication::primaryScreen();
    QPixmap pix = screen->grabWindow(0, x() + gifCapRect.x(), y() + gifCapRect.y(), gifCapRect.width(), gifCapRect.height());
    QImage image = pix.toImage().convertToFormat(QImage::Format_RGBA8888);

    //image.save(QString("E:/gitHub/QtProject/GifCapture/bin/%1.png").arg(frameCount));

    gif.GifWriteFrame(gifWriter, image.bits(), gifCapRect.width(), gifCapRect.height(), fps);

    //timeStr = QString("正在录制 第 %1 帧").arg(frameCount++);
    //qDebug()<<"addframe"<<frameCount<<pix;
}

void GifCapDialog::record()
{
     QString fileName("E:/gitHub/QtProject/GifCapture/bin/test.gif");
    if(gifWriter == nullptr)
    {
        gifWriter = new Gif::GifWriter;
        bool bOk = gif.GifBegin(gifWriter, fileName.toLocal8Bit().data(), widthEdit->text().toInt(), heightEdit->text().toInt(), fps);
        if (!bOk) {
            delete gifWriter;
            gifWriter = nullptr;
            qDebug()<<"gif录制失败"<<__LINE__;
        }
    }


    frameTimer->start((int)(1000/fpsSpinbox->text().toInt()));
    timeTimer->start(1000);
}

void GifCapDialog::recordStop()
{
    timeTimer->stop();
    frameTimer->stop();
    frameCount = 0;
    time = 0;

    gif.GifEnd(gifWriter);
    if(gifWriter)
    {
        delete gifWriter;
        gifWriter = nullptr;
    }
}

void GifCapDialog::recordPause()
{
    timeTimer->stop();
    frameTimer->stop();
}

void GifCapDialog::recordGiveUp()
{
    timeTimer->stop();
    frameTimer->stop();
    frameCount = 0;
    time = 0;

    if(gifWriter)
    {
        delete gifWriter;
        gifWriter = nullptr;
    }
}
