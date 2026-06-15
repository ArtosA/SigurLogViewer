#include <QApplication>
#include "filesplitter.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>

FileSplitter::FileSplitter(const QString &filePath, QWidget *parent)
    : QDialog(parent)
    , m_filePath(filePath)
{
    setWindowTitle("Разбивка файла");
    setMinimumWidth(450);
    setupUI();
}

void FileSplitter::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // информация о файле
    QFileInfo fi(m_filePath);
    QLabel *fileLabel = new QLabel("Файл: " + fi.fileName(), this);
    double sizeMB = fi.size() / (1024.0 * 1024.0);
    QLabel *sizeLabel = new QLabel(
        "Размер: " + QString::number(sizeMB, 'f', 2) + " MB", this);
    mainLayout->addWidget(fileLabel);
    mainLayout->addWidget(sizeLabel);

    // режим разбивки
    QGroupBox *modeGroup = new QGroupBox("Режим разбивки", this);
    QVBoxLayout *modeLayout = new QVBoxLayout(modeGroup);

    m_byLines = new QRadioButton("По количеству строк", this);
    m_byTime = new QRadioButton("Вырезать временной диапазон", this);
    m_byLines->setChecked(true);

    modeLayout->addWidget(m_byLines);
    modeLayout->addWidget(m_byTime);
    mainLayout->addWidget(modeGroup);

    // настройки — по строкам
    QGroupBox *linesGroup = new QGroupBox("Строк в каждом файле", this);
    QHBoxLayout *linesLayout = new QHBoxLayout(linesGroup);

    m_linesPerFile = new QSpinBox(this);
    m_linesPerFile->setMinimum(100);
    m_linesPerFile->setMaximum(10000000);
    m_linesPerFile->setValue(10000);
    m_linesPerFile->setSingleStep(1000);

    linesLayout->addWidget(m_linesPerFile);
    mainLayout->addWidget(linesGroup);

    // настройки — по времени
    QGroupBox *timeGroup = new QGroupBox("Временной диапазон", this);
    QHBoxLayout *timeLayout = new QHBoxLayout(timeGroup);

    m_splitTimeFrom = new QDateTimeEdit(this);
    m_splitTimeFrom->setDisplayFormat("yyyy-MM-dd HH:mm:ss.zzz");
    m_splitTimeFrom->setCalendarPopup(false);

    QLabel *dash = new QLabel("—", this);

    m_splitTimeTo = new QDateTimeEdit(this);
    m_splitTimeTo->setDisplayFormat("yyyy-MM-dd HH:mm:ss.zzz");
    m_splitTimeTo->setCalendarPopup(false);

    timeLayout->addWidget(m_splitTimeFrom);
    timeLayout->addWidget(dash);
    timeLayout->addWidget(m_splitTimeTo);
    mainLayout->addWidget(timeGroup);

    // по умолчанию скрываем блок времени
    timeGroup->setEnabled(false);
    linesGroup->setEnabled(true);

    connect(m_byLines, &QRadioButton::toggled, [linesGroup, timeGroup](bool checked) {
        linesGroup->setEnabled(checked);
        timeGroup->setEnabled(!checked);
    });

    // прогресс
    m_progress = new QProgressBar(this);
    m_progress->hide();
    mainLayout->addWidget(m_progress);

    // результат
    m_resultLabel = new QLabel("", this);
    mainLayout->addWidget(m_resultLabel);

    // кнопка
    m_splitButton = new QPushButton("Разбить", this);
    m_splitButton->setStyleSheet("padding: 6px 20px;");
    mainLayout->addWidget(m_splitButton);

    connect(m_splitButton, &QPushButton::clicked, this, &FileSplitter::onSplit);
}

void FileSplitter::onSplit()
{
    if (m_byLines->isChecked())
        splitByLines();
    else
        splitByTime();
}

void FileSplitter::splitByLines()
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл");
        return;
    }

    m_splitButton->setEnabled(false);
    m_progress->show();
    m_progress->setValue(0);

    QFileInfo fi(m_filePath);
    QString baseName = fi.completeBaseName();
    QString ext = fi.suffix();
    QString outDir = fi.absolutePath() + "/" + baseName + "_split";

    QDir().mkpath(outDir);

    QTextStream in(&file);
    int linesPerFile = m_linesPerFile->value();
    int fileIndex = 1;
    int lineCount = 0;
    int totalLines = 0;

    // считаем общее количество строк
    while (!in.atEnd()) {
        in.readLine();
        totalLines++;
    }
    in.seek(0);

    int currentLine = 0;
    QFile *outFile = nullptr;
    QTextStream *outStream = nullptr;

    while (!in.atEnd()) {
        if (lineCount % linesPerFile == 0) {
            if (outFile) {
                outStream->flush();
                outFile->close();
                delete outStream;
                delete outFile;
            }

            QString outPath = outDir + "/" + baseName +
                              "_part" + QString::number(fileIndex) + "." + ext;
            outFile = new QFile(outPath);
            outFile->open(QIODevice::WriteOnly | QIODevice::Text);
            outStream = new QTextStream(outFile);
            fileIndex++;
        }

        *outStream << in.readLine() << "\n";
        lineCount++;
        currentLine++;

        if (currentLine % 1000 == 0) {
            int percent = (int)((double)currentLine / totalLines * 100);
            m_progress->setValue(percent);
            QApplication::processEvents();
        }
    }

    if (outFile) {
        outStream->flush();
        outFile->close();
        delete outStream;
        delete outFile;
    }

    file.close();
    m_progress->setValue(100);
    m_splitButton->setEnabled(true);

    int totalFiles = fileIndex - 1;
    m_resultLabel->setText(
        "Готово! Создано " + QString::number(totalFiles) +
        " файлов в папке:\n" + outDir);

    QDesktopServices::openUrl(QUrl::fromLocalFile(outDir));
}

void FileSplitter::splitByTime()
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл");
        return;
    }

    m_splitButton->setEnabled(false);
    m_progress->show();

    QDateTime from = m_splitTimeFrom->dateTime();
    QDateTime to = m_splitTimeTo->dateTime();

    QFileInfo fi(m_filePath);
    QString outPath = fi.absolutePath() + "/" +
                      fi.completeBaseName() + "_" +
                      from.toString("HHmmss") + "-" +
                      to.toString("HHmmss") + "." + fi.suffix();

    QTextStream in(&file);
    QFile outFile(outPath);
    outFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&outFile);

    QRegularExpression timeRegex(
        R"((\d{4}-\d{2}-\d{2}\s+\d{2}:\d{2}:\d{2}))"
        );

    int count = 0;
    bool inside = false;

    while (!in.atEnd()) {
        QString line = in.readLine();

        auto match = timeRegex.match(line);
        if (match.hasMatch()) {
            QDateTime lineTime = QDateTime::fromString(
                match.captured(1), "yyyy-MM-dd HH:mm:ss");

            if (lineTime.isValid()) {
                inside = (lineTime >= from && lineTime <= to);
            }
        }

        if (inside) {
            out << line << "\n";
            count++;
        }
    }

    outFile.close();
    file.close();

    m_progress->setValue(100);
    m_splitButton->setEnabled(true);

    m_resultLabel->setText(
        "Готово! Извлечено " + QString::number(count) +
        " строк в:\n" + outPath);

    QDesktopServices::openUrl(
        QUrl::fromLocalFile(fi.absolutePath()));
}