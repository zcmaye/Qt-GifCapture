#加载模块
QT +=core gui widgets
#目标模板 app lib subdirs 有三种
TEMPLATE = app
#输出的exe名称
TARGET = GifCapture
#exe输出路径
DESTDIR = $$PWD/bin
#设置exe图标
RC_ICONS += gif.ico


SOURCES += ./main.cpp \
    gifcapdialog.cpp

HEADERS += \
    gif.h \
    gifcapdialog.h

RESOURCES += \
    images.qrc

