#include "FileManager.h"
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QPrinter>
#include <QTextDocument>
#include <QPageSize>

FileManager::FileManager(QObject* parent) : QObject(parent) {}

QString FileManager::readFile(const QString& filePath, QString& errorMessage) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorMessage = file.errorString();
        return QString();
    }
    
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8); // Qt6 standard encoding setup
    QString content = in.readAll();
    file.close();
    return content;
}

bool FileManager::writeFile(const QString& filePath, const QString& content, QString& errorMessage) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        errorMessage = file.errorString();
        return false;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << content;
    file.close();
    return true;
}

bool FileManager::exportToPdf(const QString& filePath, const QString& htmlContent, QString& errorMessage) {
    try {
        QPrinter printer(QPrinter::PrinterResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(filePath);
        printer.setPageSize(QPageSize(QPageSize::A4));
        
        QTextDocument doc;
        doc.setHtml(htmlContent);
        doc.print(&printer);
        return true;
    } catch (const std::exception& e) {
        errorMessage = QString::fromStdString(e.what());
        return false;
    } catch (...) {
        errorMessage = "An unknown error occurred during PDF export.";
        return false;
    }
}

QStringList FileManager::getRecentFiles() const {
    QSettings settings("GoogleDeepMind", "MarkdownToHTML");
    return settings.value("recentFiles").toStringList();
}

void FileManager::addRecentFile(const QString& filePath) {
    QSettings settings("GoogleDeepMind", "MarkdownToHTML");
    QStringList files = settings.value("recentFiles").toStringList();
    files.removeAll(filePath);
    files.prepend(filePath);
    while (files.size() > MaxRecentFiles) {
        files.removeLast();
    }
    settings.setValue("recentFiles", files);
}

void FileManager::clearRecentFiles() {
    QSettings settings("GoogleDeepMind", "MarkdownToHTML");
    settings.remove("recentFiles");
}
