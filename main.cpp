#include <QApplication>
#include "gifcapdialog.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GifCapDialog gifCapDialog;
    gifCapDialog.show();

    return a.exec();
}
