#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QTimer;
class QAction;
class QMenu;
class QLineEdit;
class QPushButton;
class QCheckBox;
class QLabel;
class QSplitter;
class QTabWidget;
class QPlainTextEdit;
class QTextBrowser;
class QFrame;
QT_END_NAMESPACE

class FileManager;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    // File Actions
    void openFile(const QString &filePath);
    void onOpenFileTriggered();
    void onSaveHtmlTriggered();
    void onExportPdfTriggered();
    void onClearTriggered();
    void onExitTriggered();
    
    // Recent Files
    void updateRecentFilesMenu();
    void onRecentFileTriggered();
    void onClearRecentFilesTriggered();

    // Editor Actions
    void onMarkdownTextChanged();
    void onConvertTriggered();
    
    // Theme
    void onThemeToggled(bool isDark);

    // Search and Replace
    void showSearchPanel();
    void hideSearchPanel();
    void findText(bool forward);
    void findNext();
    void findPrevious();
    void replaceText();
    void replaceAllText();

    // Auto-save
    void handleAutoSave();

private:
    void setupActions();
    void setupMenuBar();
    void setupStatusBar();
    void setupSearchPanel();
    void applyTheme(bool dark);
    void updateStats();
    bool maybeSave();

    Ui::MainWindow *ui;
    std::unique_ptr<FileManager> m_fileManager;
    QString m_currentFilePath;
    bool m_isDarkTheme;
    bool m_isModified;

    // UI Widgets declared via UI file (but initialized either by setupUi or dynamically as backup)
    // We will lay them out in the .ui file and expose them
    QTimer *m_autoSaveTimer;

    // Search Panel widgets
    QFrame *m_searchPanel;
    QLineEdit *m_findInput;
    QLineEdit *m_replaceInput;
    QPushButton *m_findNextBtn;
    QPushButton *m_findPrevBtn;
    QPushButton *m_replaceBtn;
    QPushButton *m_replaceAllBtn;
    QPushButton *m_closeSearchBtn;
    QCheckBox *m_caseSensitiveCheck;

    // Recent Files Menu Elements
    QMenu *m_recentFilesMenu;
    QAction *m_clearRecentAction;
    QList<QAction*> m_recentFileActions;
    static const int MaxRecentActions = 8;
};

#endif // MAINWINDOW_H
