#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>

class FileManager : public QObject {
    Q_OBJECT
public:
    explicit FileManager(QObject* parent = nullptr);
    ~FileManager() = default;

    // Reads the entire contents of a file
    QString readFile(const QString& filePath, QString& errorMessage);

    // Writes content to a file
    bool writeFile(const QString& filePath, const QString& content, QString& errorMessage);

    // Exports HTML content as a PDF file
    bool exportToPdf(const QString& filePath, const QString& htmlContent, QString& errorMessage);

    // Recent files management
    QStringList getRecentFiles() const;
    void addRecentFile(const QString& filePath);
    void clearRecentFiles();

private:
    static const int MaxRecentFiles = 8;
};

#endif // FILEMANAGER_H
