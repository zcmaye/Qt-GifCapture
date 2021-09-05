#ifndef GIFCAPDIALOG_H
#define GIFCAPDIALOG_H

#include <QDialog>
class QSpinBox;
class QLineEdit;
class QPushButton;
class QLabel;
class QHBoxLayout;
class QSizeGrip;

#include"gif.h"

class GifCapDialog : public QDialog
{
    Q_OBJECT
public:
    GifCapDialog(QWidget*parent = nullptr);


    bool eventFilter(QObject*watched,QEvent*ev) override;   //事件过滤器，实现窗口的拖动
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent *) override;


    void initUi();
    QWidget* createTopLayout();
    QLayout* createbuttomLayout();
public slots:
    void onFpsSpinbox(int value);
    void onWidthEdit();
    void onHeightEdit();
    void onStartBtn();
    void onPauseBtn();
    void onStopBtn();
    void onGiveUpBtn();

    void addFrame();    //保存一帧图片到gif中
public:
    void record();   //录制Gif
    void recordStop();
    void recordPause();
    void recordGiveUp();
private:
    qint8   borderSize;     //左右边框的宽度
    QColor  borderColor;    //边框颜色
    QRect   gifCapRect;     //捕获gif的区域矩形(即窗口透明区域)
    qint8   titleBarHeight;    //标题栏和底部的高度


    //顶部布局小控件
    QWidget*    topWidget;
    QPushButton *iconBtn;
    QLabel      *titleLabel;
    QPushButton *timeBtn;       //显示录制时间
    QPushButton *miniBtn;
    QPushButton *closeBtn;

    //底部小控件
    qint8 fps;
    QSpinBox    *fpsSpinbox;
    QLineEdit   *widthEdit;
    QLineEdit   *heightEdit;
    QPushButton *startBtn;  //开始录制
    QPushButton *pauseBtn;  //暂停
    QPushButton *stopBtn;   //停止
    QPushButton *giveUpBtn; //放弃
    QHBoxLayout *buttomLayout;

    //放置缩放手柄
    QSizeGrip   *sizeGrip;
    QSizeGrip   *buttomLeftSizeGrip;

    quint32 frameCount = 0;
    QTimer  *frameTimer;        //控制帧数的定时器
    quint32 time = 0;
    QTimer  *timeTimer;         //录制时间

    Gif gif;                    //gif类对象
    Gif::GifWriter *gifWriter = nullptr;  //gif写入对象
};

#endif // GIFCAPDIALOG_H
