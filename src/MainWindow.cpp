#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "FileManager.h"
#include "MarkdownParser.h"
#include "HTMLGenerator.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QSettings>
#include <QToolBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QFrame>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QTextCursor>
#include <QRegularExpression>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_fileManager(std::make_unique<FileManager>(this))
    , m_isDarkTheme(false)
    , m_isModified(false)
{
    ui->setupUi(this);
    
    // Enable Drag and Drop
    setAcceptDrops(true);

    // Setup Programmatic UI Parts
    setupActions();
    setupMenuBar();
    setupStatusBar();
    setupSearchPanel();

    // Connect Editor Signals
    connect(ui->markdownEdit, &QPlainTextEdit::textChanged, this, &MainWindow::onMarkdownTextChanged);
    connect(ui->markdownEdit, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::updateStats);

    // Auto-Save Setup (Timer runs every 15 seconds)
    m_autoSaveTimer = new QTimer(this);
    connect(m_autoSaveTimer, &QTimer::timeout, this, &MainWindow::handleAutoSave);
    m_autoSaveTimer->start(15000);

    // Set initial splitter sizes (equal width)
    QList<int> sizes;
    sizes << 550 << 550;
    ui->splitter->setSizes(sizes);

    // Load Persistent Settings
    QSettings settings("GoogleDeepMind", "MarkdownToHTML");
    m_isDarkTheme = settings.value("darkMode", false).toBool();
    applyTheme(m_isDarkTheme);

    updateRecentFilesMenu();
    updateStats();

    // Perform initial convert to clear empty display
    onConvertTriggered();
    m_isModified = false; // Reset modification flag after initial run
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupActions() {
    // We add a modern Toolbar
    QToolBar* mainToolBar = addToolBar(tr("Main Actions"));
    mainToolBar->setMovable(false);
    mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    // Open Action
    QAction* openAction = new QAction(tr("&Open Markdown File"), this);
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::onOpenFileTriggered);
    mainToolBar->addAction(openAction);

    // Save Action
    QAction* saveAction = new QAction(tr("&Save HTML"), this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveHtmlTriggered);
    mainToolBar->addAction(saveAction);

    // Export PDF Action
    QAction* pdfAction = new QAction(tr("Export &PDF"), this);
    pdfAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_P));
    connect(pdfAction, &QAction::triggered, this, &MainWindow::onExportPdfTriggered);
    mainToolBar->addAction(pdfAction);

    mainToolBar->addSeparator();

    // Convert Action
    QAction* convertAction = new QAction(tr("&Convert Now"), this);
    convertAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
    connect(convertAction, &QAction::triggered, this, &MainWindow::onConvertTriggered);
    mainToolBar->addAction(convertAction);

    // Clear Action
    QAction* clearAction = new QAction(tr("C&lear"), this);
    clearAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    connect(clearAction, &QAction::triggered, this, &MainWindow::onClearTriggered);
    mainToolBar->addAction(clearAction);

    mainToolBar->addSeparator();

    // Theme Switcher Checkable Action
    QAction* themeAction = new QAction(tr("&Dark Mode"), this);
    themeAction->setCheckable(true);
    themeAction->setChecked(m_isDarkTheme);
    themeAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_T));
    connect(themeAction, &QAction::toggled, this, &MainWindow::onThemeToggled);
    mainToolBar->addAction(themeAction);

    // Find Action
    QAction* findAction = new QAction(tr("&Find / Replace"), this);
    findAction->setShortcut(QKeySequence::Find);
    connect(findAction, &QAction::triggered, this, &MainWindow::showSearchPanel);
    addAction(findAction); // Added to window so shortcut works even when search panel is hidden
}

void MainWindow::setupMenuBar() {
    QMenuBar* mBar = menuBar();
    mBar->clear();

    // File Menu
    QMenu* fileMenu = mBar->addMenu(tr("&File"));
    
    QAction* openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(QKeySequence::Open);
    connect(openAct, &QAction::triggered, this, &MainWindow::onOpenFileTriggered);
    fileMenu->addAction(openAct);

    QAction* saveAct = new QAction(tr("&Save HTML..."), this);
    saveAct->setShortcut(QKeySequence::Save);
    connect(saveAct, &QAction::triggered, this, &MainWindow::onSaveHtmlTriggered);
    fileMenu->addAction(saveAct);

    QAction* pdfAct = new QAction(tr("&Export as PDF..."), this);
    pdfAct->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_P));
    connect(pdfAct, &QAction::triggered, this, &MainWindow::onExportPdfTriggered);
    fileMenu->addAction(pdfAct);

    // Recent Files Menu
    m_recentFilesMenu = new QMenu(tr("&Recent Files"), this);
    fileMenu->addMenu(m_recentFilesMenu);

    fileMenu->addSeparator();

    QAction* clearAct = new QAction(tr("&Clear Editor"), this);
    clearAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    connect(clearAct, &QAction::triggered, this, &MainWindow::onClearTriggered);
    fileMenu->addAction(clearAct);

    fileMenu->addSeparator();

    QAction* exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(QKeySequence(Qt::ALT | Qt::Key_F4));
    connect(exitAct, &QAction::triggered, this, &MainWindow::onExitTriggered);
    fileMenu->addAction(exitAct);

    // Edit Menu
    QMenu* editMenu = mBar->addMenu(tr("&Edit"));
    
    QAction* undoAct = ui->markdownEdit->createStandardContextMenu()->actionAt(QPoint(0,0)); // standard UNDO shortcut mapping
    editMenu->addAction(tr("&Undo"), QKeySequence::Undo, ui->markdownEdit, &QPlainTextEdit::undo);
    editMenu->addAction(tr("&Redo"), QKeySequence::Redo, ui->markdownEdit, &QPlainTextEdit::redo);
    editMenu->addSeparator();
    editMenu->addAction(tr("Cu&t"), QKeySequence::Cut, ui->markdownEdit, &QPlainTextEdit::cut);
    editMenu->addAction(tr("&Copy"), QKeySequence::Copy, ui->markdownEdit, &QPlainTextEdit::copy);
    editMenu->addAction(tr("&Paste"), QKeySequence::Paste, ui->markdownEdit, &QPlainTextEdit::paste);
    editMenu->addSeparator();

    QAction* findAct = new QAction(tr("&Find / Replace"), this);
    findAct->setShortcut(QKeySequence::Find);
    connect(findAct, &QAction::triggered, this, &MainWindow::showSearchPanel);
    editMenu->addAction(findAct);

    // Tools Menu
    QMenu* toolsMenu = mBar->addMenu(tr("&Tools"));
    QAction* convertAct = new QAction(tr("&Convert Markdown"), this);
    convertAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
    connect(convertAct, &QAction::triggered, this, &MainWindow::onConvertTriggered);
    toolsMenu->addAction(convertAct);
}

void MainWindow::setupStatusBar() {
    statusBar()->showMessage(tr("Ready."));
}

void MainWindow::setupSearchPanel() {
    m_searchPanel = new QFrame(this);
    m_searchPanel->setObjectName("searchPanel");
    m_searchPanel->setFrameShape(QFrame::StyledPanel);
    
    QHBoxLayout* searchLayout = new QHBoxLayout(m_searchPanel);
    searchLayout->setContentsMargins(8, 6, 8, 6);
    searchLayout->setSpacing(8);
    
    m_findInput = new QLineEdit(m_searchPanel);
    m_findInput->setPlaceholderText(tr("Find text..."));
    m_findInput->setMinimumWidth(150);
    
    m_replaceInput = new QLineEdit(m_searchPanel);
    m_replaceInput->setPlaceholderText(tr("Replace with..."));
    m_replaceInput->setMinimumWidth(150);
    
    m_caseSensitiveCheck = new QCheckBox(tr("Match Case"), m_searchPanel);
    
    m_findPrevBtn = new QPushButton(tr("Previous"), m_searchPanel);
    m_findNextBtn = new QPushButton(tr("Next"), m_searchPanel);
    m_replaceBtn = new QPushButton(tr("Replace"), m_searchPanel);
    m_replaceAllBtn = new QPushButton(tr("Replace All"), m_searchPanel);
    
    m_closeSearchBtn = new QPushButton(tr("✕"), m_searchPanel);
    m_closeSearchBtn->setFixedWidth(28);
    m_closeSearchBtn->setCursor(Qt::PointingHandCursor);

    searchLayout->addWidget(new QLabel(tr("Find:"), m_searchPanel));
    searchLayout->addWidget(m_findInput);
    searchLayout->addWidget(new QLabel(tr("Replace:"), m_searchPanel));
    searchLayout->addWidget(m_replaceInput);
    searchLayout->addWidget(m_caseSensitiveCheck);
    searchLayout->addWidget(m_findPrevBtn);
    searchLayout->addWidget(m_findNextBtn);
    searchLayout->addWidget(m_replaceBtn);
    searchLayout->addWidget(m_replaceAllBtn);
    searchLayout->addWidget(m_closeSearchBtn);
    
    // Search connections
    connect(m_closeSearchBtn, &QPushButton::clicked, this, &MainWindow::hideSearchPanel);
    connect(m_findNextBtn, &QPushButton::clicked, this, &MainWindow::findNext);
    connect(m_findPrevBtn, &QPushButton::clicked, this, &MainWindow::findPrevious);
    connect(m_replaceBtn, &QPushButton::clicked, this, &MainWindow::replaceText);
    connect(m_replaceAllBtn, &QPushButton::clicked, this, &MainWindow::replaceAllText);
    connect(m_findInput, &QLineEdit::returnPressed, this, &MainWindow::findNext);
    
    ui->editorLayout->addWidget(m_searchPanel);
    m_searchPanel->hide();
}

void MainWindow::showSearchPanel() {
    m_searchPanel->show();
    m_findInput->setFocus();
    m_findInput->selectAll();
}

void MainWindow::hideSearchPanel() {
    m_searchPanel->hide();
    ui->markdownEdit->setFocus();
}

void MainWindow::findText(bool forward) {
    QString query = m_findInput->text();
    if (query.isEmpty()) return;
    
    QTextDocument::FindFlags flags = {};
    if (!forward) {
        flags |= QTextDocument::FindBackward;
    }
    if (m_caseSensitiveCheck->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    
    bool found = ui->markdownEdit->find(query, flags);
    if (!found) {
        // Wrap search around
        QTextCursor cursor = ui->markdownEdit->textCursor();
        if (forward) {
            cursor.movePosition(QTextCursor::Start);
        } else {
            cursor.movePosition(QTextCursor::End);
        }
        ui->markdownEdit->setTextCursor(cursor);
        found = ui->markdownEdit->find(query, flags);
    }
    
    if (!found) {
        statusBar()->showMessage(tr("Text '%1' not found.").arg(query), 2000);
    }
}

void MainWindow::findNext() {
    findText(true);
}

void MainWindow::findPrevious() {
    findText(false);
}

void MainWindow::replaceText() {
    QTextCursor cursor = ui->markdownEdit->textCursor();
    if (cursor.hasSelection() && 
        cursor.selectedText().compare(m_findInput->text(), m_caseSensitiveCheck->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive) == 0) {
        cursor.insertText(m_replaceInput->text());
    }
    findNext();
}

void MainWindow::replaceAllText() {
    QString query = m_findInput->text();
    QString replacement = m_replaceInput->text();
    if (query.isEmpty()) return;
    
    QTextCursor cursor = ui->markdownEdit->textCursor();
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::Start);
    ui->markdownEdit->setTextCursor(cursor);
    
    QTextDocument::FindFlags flags = {};
    if (m_caseSensitiveCheck->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    
    int count = 0;
    while (ui->markdownEdit->find(query, flags)) {
        ui->markdownEdit->textCursor().insertText(replacement);
        count++;
    }
    cursor.endEditBlock();
    statusBar()->showMessage(tr("Replaced %1 occurrences.").arg(count), 3000);
}

void MainWindow::openFile(const QString &filePath) {
    if (!maybeSave()) return;

    QString error;
    QString content = m_fileManager->readFile(filePath, error);
    if (content.isNull()) {
        QMessageBox::critical(this, tr("Error"), tr("Could not read file:\n%1").arg(error));
        return;
    }

    ui->markdownEdit->setPlainText(content);
    m_currentFilePath = filePath;
    m_isModified = false;

    // Track in recents
    m_fileManager->addRecentFile(filePath);
    updateRecentFilesMenu();

    // Trigger conversion
    onConvertTriggered();
    
    statusBar()->showMessage(tr("Loaded file: %1").arg(filePath), 3000);
    updateStats();
}

void MainWindow::onOpenFileTriggered() {
    QString filePath = QFileDialog::getOpenFileName(
        this, 
        tr("Open Markdown File"), 
        QString(), 
        tr("Markdown Files (*.md *.markdown *.txt);;All Files (*)")
    );
    if (!filePath.isEmpty()) {
        openFile(filePath);
    }
}

void MainWindow::onSaveHtmlTriggered() {
    QString defaultName = "output.html";
    if (!m_currentFilePath.isEmpty()) {
        QFileInfo info(m_currentFilePath);
        defaultName = info.completeBaseName() + ".html";
    }

    QString filePath = QFileDialog::getSaveFileName(
        this, 
        tr("Save HTML File"), 
        defaultName, 
        tr("HTML Files (*.html *.htm);;All Files (*)")
    );
    
    if (filePath.isEmpty()) return;

    // Generate output
    std::string markdown = ui->markdownEdit->toPlainText().toStdString();
    Markdown::MarkdownParser parser;
    auto blocks = parser.parse(markdown);
    
    Markdown::HTMLGenerator generator;
    std::string fullHtml = generator.generateDocument(
        blocks,
        m_isDarkTheme ? Markdown::HTMLGenerator::Theme::Dark : Markdown::HTMLGenerator::Theme::Light,
        "Converted Document"
    );

    QString error;
    if (!m_fileManager->writeFile(filePath, QString::fromStdString(fullHtml), error)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not save HTML file:\n%1").arg(error));
    } else {
        statusBar()->showMessage(tr("HTML successfully saved to: %1").arg(filePath), 3000);
        // If we saved it, mark the original markdown file as saved too, if they match.
        if (!m_currentFilePath.isEmpty()) {
            m_isModified = false;
            updateStats();
        }
    }
}

void MainWindow::onExportPdfTriggered() {
    QString defaultName = "output.pdf";
    if (!m_currentFilePath.isEmpty()) {
        QFileInfo info(m_currentFilePath);
        defaultName = info.completeBaseName() + ".pdf";
    }

    QString filePath = QFileDialog::getSaveFileName(
        this, 
        tr("Export PDF"), 
        defaultName, 
        tr("PDF Files (*.pdf);;All Files (*)")
    );
    
    if (filePath.isEmpty()) return;

    // Generate HTML
    std::string markdown = ui->markdownEdit->toPlainText().toStdString();
    Markdown::MarkdownParser parser;
    auto blocks = parser.parse(markdown);
    
    Markdown::HTMLGenerator generator;
    std::string fullHtml = generator.generateDocument(
        blocks,
        m_isDarkTheme ? Markdown::HTMLGenerator::Theme::Dark : Markdown::HTMLGenerator::Theme::Light,
        "Exported Document"
    );

    QString error;
    if (!m_fileManager->exportToPdf(filePath, QString::fromStdString(fullHtml), error)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not export PDF:\n%1").arg(error));
    } else {
        statusBar()->showMessage(tr("PDF successfully exported to: %1").arg(filePath), 3000);
    }
}

void MainWindow::onClearTriggered() {
    if (ui->markdownEdit->toPlainText().isEmpty()) return;
    
    auto button = QMessageBox::question(
        this, 
        tr("Confirm Clear"), 
        tr("Are you sure you want to clear the editor? Unsaved changes will be lost."),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (button == QMessageBox::Yes) {
        ui->markdownEdit->clear();
        m_currentFilePath.clear();
        m_isModified = false;
        onConvertTriggered();
        statusBar()->showMessage(tr("Editor cleared."), 2000);
        updateStats();
    }
}

void MainWindow::onExitTriggered() {
    close();
}

void MainWindow::updateRecentFilesMenu() {
    m_recentFilesMenu->clear();
    QStringList files = m_fileManager->getRecentFiles();

    m_recentFileActions.clear();
    for (const auto& file : files) {
        QAction* action = new QAction(QFileInfo(file).fileName(), this);
        action->setData(file);
        action->setToolTip(file);
        connect(action, &QAction::triggered, this, &MainWindow::onRecentFileTriggered);
        m_recentFilesMenu->addAction(action);
        m_recentFileActions.append(action);
    }

    if (files.isEmpty()) {
        QAction* noRecentAct = new QAction(tr("No Recent Files"), this);
        noRecentAct->setEnabled(false);
        m_recentFilesMenu->addAction(noRecentAct);
    } else {
        m_recentFilesMenu->addSeparator();
        m_clearRecentAction = new QAction(tr("Clear Recent List"), this);
        connect(m_clearRecentAction, &QAction::triggered, this, &MainWindow::onClearRecentFilesTriggered);
        m_recentFilesMenu->addAction(m_clearRecentAction);
    }
}

void MainWindow::onRecentFileTriggered() {
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        QString filePath = action->data().toString();
        openFile(filePath);
    }
}

void MainWindow::onClearRecentFilesTriggered() {
    m_fileManager->clearRecentFiles();
    updateRecentFilesMenu();
    statusBar()->showMessage(tr("Recent files list cleared."), 2000);
}

void MainWindow::onMarkdownTextChanged() {
    m_isModified = true;
    updateStats();
    
    // Automatically trigger convert for live preview
    // Standard Qt objects can trigger this efficiently.
    std::string markdown = ui->markdownEdit->toPlainText().toStdString();
    
    Markdown::MarkdownParser parser;
    auto blocks = parser.parse(markdown);
    
    Markdown::HTMLGenerator generator;
    std::string fullHtml = generator.generateDocument(
        blocks, 
        m_isDarkTheme ? Markdown::HTMLGenerator::Theme::Dark : Markdown::HTMLGenerator::Theme::Light,
        "Live Preview"
    );
    
    ui->htmlPreview->setHtml(QString::fromStdString(fullHtml));
    ui->htmlSource->setPlainText(QString::fromStdString(fullHtml));
}

void MainWindow::onConvertTriggered() {
    std::string markdown = ui->markdownEdit->toPlainText().toStdString();
    
    Markdown::MarkdownParser parser;
    auto blocks = parser.parse(markdown);
    
    Markdown::HTMLGenerator generator;
    std::string fullHtml = generator.generateDocument(
        blocks, 
        m_isDarkTheme ? Markdown::HTMLGenerator::Theme::Dark : Markdown::HTMLGenerator::Theme::Light,
        "Live Preview"
    );
    
    ui->htmlPreview->setHtml(QString::fromStdString(fullHtml));
    ui->htmlSource->setPlainText(QString::fromStdString(fullHtml));
    statusBar()->showMessage(tr("Markdown converted to HTML successfully."), 2000);
}

void MainWindow::onThemeToggled(bool isDark) {
    m_isDarkTheme = isDark;
    
    // Save to settings
    QSettings settings("GoogleDeepMind", "MarkdownToHTML");
    settings.setValue("darkMode", m_isDarkTheme);
    
    applyTheme(m_isDarkTheme);
    onConvertTriggered(); // Re-render preview with the new theme stylesheet
}

void MainWindow::applyTheme(bool dark) {
    QString stylesheet;
    if (dark) {
        stylesheet = R"(
            QMainWindow {
                background-color: #0f172a;
            }
            QMenuBar {
                background-color: #0f172a;
                border-bottom: 1px solid #334155;
                color: #f8fafc;
            }
            QMenuBar::item:selected {
                background-color: #1e293b;
            }
            QStatusBar {
                background-color: #0f172a;
                border-top: 1px solid #334155;
                color: #94a3b8;
            }
            QToolBar {
                background-color: #1e293b;
                border-bottom: 1px solid #334155;
                spacing: 8px;
                padding: 4px;
            }
            QToolButton {
                color: #f8fafc;
                background-color: transparent;
                border: 1px solid transparent;
                border-radius: 4px;
                padding: 6px 10px;
            }
            QToolButton:hover {
                background-color: #334155;
                border-color: #475569;
            }
            QToolButton:checked {
                background-color: #475569;
            }
            QPlainTextEdit, QTextBrowser {
                background-color: #1e293b;
                border: 1px solid #334155;
                border-radius: 6px;
                padding: 8px;
                color: #f8fafc;
            }
            QPlainTextEdit:focus, QTextBrowser:focus {
                border: 1px solid #60a5fa;
            }
            QTabWidget::pane {
                border: 1px solid #334155;
                border-radius: 6px;
                background-color: #1e293b;
            }
            QTabBar::tab {
                background-color: #0f172a;
                color: #94a3b8;
                border: 1px solid #334155;
                border-bottom-color: none;
                border-top-left-radius: 4px;
                border-top-right-radius: 4px;
                padding: 6px 12px;
                margin-right: 2px;
            }
            QTabBar::tab:selected {
                background-color: #1e293b;
                color: #f8fafc;
                border-bottom-color: #1e293b;
            }
            QFrame#searchPanel {
                background-color: #1e293b;
                border: 1px solid #334155;
                border-radius: 6px;
                padding: 6px;
            }
            QLineEdit {
                background-color: #0f172a;
                color: #f8fafc;
                border: 1px solid #334155;
                border-radius: 4px;
                padding: 4px 8px;
            }
            QLineEdit:focus {
                border: 1px solid #60a5fa;
            }
            QPushButton {
                background-color: #3b82f6;
                color: #ffffff;
                border: none;
                border-radius: 4px;
                padding: 6px 12px;
            }
            QPushButton:hover {
                background-color: #60a5fa;
            }
            QPushButton:pressed {
                background-color: #2563eb;
            }
        )";
    } else {
        // Light Theme QSS
        stylesheet = R"(
            QMainWindow {
                background-color: #f8f9fa;
            }
            QMenuBar {
                background-color: #f8f9fa;
                border-bottom: 1px solid #e2e8f0;
            }
            QMenuBar::item:selected {
                background-color: #e2e8f0;
            }
            QStatusBar {
                background-color: #f8f9fa;
                border-top: 1px solid #e2e8f0;
                color: #4a5568;
            }
            QToolBar {
                background-color: #ffffff;
                border-bottom: 1px solid #e2e8f0;
                spacing: 8px;
                padding: 4px;
            }
            QToolButton {
                background-color: transparent;
                border: 1px solid transparent;
                border-radius: 4px;
                padding: 6px 10px;
            }
            QToolButton:hover {
                background-color: #f1f5f9;
                border-color: #cbd5e1;
            }
            QToolButton:checked {
                background-color: #e2e8f0;
            }
            QPlainTextEdit, QTextBrowser {
                background-color: #ffffff;
                border: 1px solid #cbd5e1;
                border-radius: 6px;
                padding: 8px;
                color: #0f172a;
            }
            QPlainTextEdit:focus, QTextBrowser:focus {
                border: 1px solid #3b82f6;
            }
            QTabWidget::pane {
                border: 1px solid #cbd5e1;
                border-radius: 6px;
                background-color: #ffffff;
            }
            QTabBar::tab {
                background-color: #e2e8f0;
                border: 1px solid #cbd5e1;
                border-bottom-color: none;
                border-top-left-radius: 4px;
                border-top-right-radius: 4px;
                padding: 6px 12px;
                margin-right: 2px;
            }
            QTabBar::tab:selected {
                background-color: #ffffff;
                border-bottom-color: #ffffff;
            }
            QFrame#searchPanel {
                background-color: #ffffff;
                border: 1px solid #cbd5e1;
                border-radius: 6px;
                padding: 6px;
            }
            QLineEdit {
                border: 1px solid #cbd5e1;
                border-radius: 4px;
                padding: 4px 8px;
            }
            QLineEdit:focus {
                border: 1px solid #3b82f6;
            }
            QPushButton {
                background-color: #3b82f6;
                color: #ffffff;
                border: none;
                border-radius: 4px;
                padding: 6px 12px;
            }
            QPushButton:hover {
                background-color: #2563eb;
            }
            QPushButton:pressed {
                background-color: #1d4ed8;
            }
        )";
    }
    qApp->setStyleSheet(stylesheet);
}

void MainWindow::updateStats() {
    QString text = ui->markdownEdit->toPlainText();
    
    // Count Words
    int wordCount = 0;
    if (!text.isEmpty()) {
        QStringList words = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        wordCount = words.size();
    }
    
    // Estimate Reading Time (avg. 200 words per minute)
    int readingTimeSeconds = static_cast<int>(wordCount * 0.3); // 200 wpm is 3.3 words/sec, so 0.3s per word.
    QString readingTimeStr;
    if (readingTimeSeconds < 60) {
        readingTimeStr = tr("< 1 min");
    } else {
        int minutes = readingTimeSeconds / 60;
        readingTimeStr = tr("%1 min").arg(minutes);
    }
    
    // Get cursor position (line, col)
    QTextCursor cursor = ui->markdownEdit->textCursor();
    int line = cursor.blockNumber() + 1;
    int col = cursor.columnNumber() + 1;
    
    // Current file name or "Untitled"
    QString fileStatus = m_currentFilePath.isEmpty() ? tr("Untitled") : QFileInfo(m_currentFilePath).fileName();
    if (m_isModified) {
        fileStatus += "*";
    }

    QString stats = tr("%1  |  Words: %2  |  Reading Time: %3  |  Ln: %4, Col: %5")
        .arg(fileStatus)
        .arg(wordCount)
        .arg(readingTimeStr)
        .arg(line)
        .arg(col);
        
    statusBar()->showMessage(stats);
}

void MainWindow::handleAutoSave() {
    if (m_isModified && !m_currentFilePath.isEmpty()) {
        QString error;
        if (m_fileManager->writeFile(m_currentFilePath, ui->markdownEdit->toPlainText(), error)) {
            m_isModified = false;
            statusBar()->showMessage(tr("Document auto-saved."), 3000);
            updateStats();
        }
    }
}

bool MainWindow::maybeSave() {
    if (!m_isModified) return true;

    auto button = QMessageBox::warning(
        this, 
        tr("Unsaved Changes"),
        tr("The document has been modified.\nDo you want to save your changes?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
    );

    if (button == QMessageBox::Save) {
        onSaveHtmlTriggered();
        return !m_isModified; // returns true if save succeeded and reset modification flag
    } else if (button == QMessageBox::Cancel) {
        return false;
    }
    return true;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty()) {
            QString path = urls.first().toLocalFile();
            if (path.endsWith(".md", Qt::CaseInsensitive) || 
                path.endsWith(".markdown", Qt::CaseInsensitive) || 
                path.endsWith(".txt", Qt::CaseInsensitive)) {
                event->acceptProposedAction();
            }
        }
    }
}

void MainWindow::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty()) {
            QString path = urls.first().toLocalFile();
            openFile(path);
            event->acceptProposedAction();
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}
