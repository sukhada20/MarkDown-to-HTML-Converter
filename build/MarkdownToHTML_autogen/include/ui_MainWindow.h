/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QSplitter *splitter;
    QWidget *editorContainer;
    QVBoxLayout *editorLayout;
    QPlainTextEdit *markdownEdit;
    QTabWidget *tabWidget;
    QWidget *previewTab;
    QVBoxLayout *previewLayout;
    QTextBrowser *htmlPreview;
    QWidget *sourceTab;
    QVBoxLayout *sourceLayout;
    QPlainTextEdit *htmlSource;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1100, 750);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(8, 8, 8, 8);
        splitter = new QSplitter(centralwidget);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Horizontal);
        editorContainer = new QWidget(splitter);
        editorContainer->setObjectName("editorContainer");
        editorLayout = new QVBoxLayout(editorContainer);
        editorLayout->setObjectName("editorLayout");
        editorLayout->setContentsMargins(0, 0, 0, 0);
        markdownEdit = new QPlainTextEdit(editorContainer);
        markdownEdit->setObjectName("markdownEdit");
        QFont font;
        font.setFamilies({QString::fromUtf8("Consolas")});
        font.setPointSize(11);
        markdownEdit->setFont(font);

        editorLayout->addWidget(markdownEdit);

        splitter->addWidget(editorContainer);
        tabWidget = new QTabWidget(splitter);
        tabWidget->setObjectName("tabWidget");
        QFont font1;
        font1.setFamilies({QString::fromUtf8("Segoe UI")});
        font1.setPointSize(10);
        tabWidget->setFont(font1);
        previewTab = new QWidget();
        previewTab->setObjectName("previewTab");
        previewLayout = new QVBoxLayout(previewTab);
        previewLayout->setObjectName("previewLayout");
        previewLayout->setContentsMargins(0, 0, 0, 0);
        htmlPreview = new QTextBrowser(previewTab);
        htmlPreview->setObjectName("htmlPreview");
        QFont font2;
        font2.setFamilies({QString::fromUtf8("Segoe UI")});
        font2.setPointSize(11);
        htmlPreview->setFont(font2);
        htmlPreview->setOpenExternalLinks(true);

        previewLayout->addWidget(htmlPreview);

        tabWidget->addTab(previewTab, QString());
        sourceTab = new QWidget();
        sourceTab->setObjectName("sourceTab");
        sourceLayout = new QVBoxLayout(sourceTab);
        sourceLayout->setObjectName("sourceLayout");
        sourceLayout->setContentsMargins(0, 0, 0, 0);
        htmlSource = new QPlainTextEdit(sourceTab);
        htmlSource->setObjectName("htmlSource");
        QFont font3;
        font3.setFamilies({QString::fromUtf8("Consolas")});
        htmlSource->setFont(font3);
        htmlSource->setReadOnly(true);

        sourceLayout->addWidget(htmlSource);

        tabWidget->addTab(sourceTab, QString());
        splitter->addWidget(tabWidget);

        verticalLayout->addWidget(splitter);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1100, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Markdown to HTML Converter", nullptr));
        markdownEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "Type or paste your Markdown here...", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(previewTab), QCoreApplication::translate("MainWindow", "Live HTML Preview", nullptr));
        htmlSource->setPlaceholderText(QCoreApplication::translate("MainWindow", "HTML source code will appear here...", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(sourceTab), QCoreApplication::translate("MainWindow", "HTML Source", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
