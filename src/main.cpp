#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    // Set application information (used by QSettings)
    QApplication::setOrganizationName("GoogleDeepMind");
    QApplication::setApplicationName("MarkdownToHTML");
    QApplication::setApplicationVersion("1.0.0");
    
    MainWindow w;
    w.show();
    
    return a.exec();
}
