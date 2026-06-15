#ifndef FILESPLITTER_H
#define FILESPLITTER_H

#include <QDialog>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>
#include <QDateTimeEdit>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>

class FileSplitter : public QDialog
{
    Q_OBJECT

public:
    explicit FileSplitter(const QString &filePath, QWidget *parent = nullptr);

private slots:
    void onSplit();

private:
    void setupUI();
    void splitByLines();
    void splitByTime();

    QString m_filePath;

    QRadioButton  *m_byLines;
    QRadioButton  *m_byTime;

    // по строкам
    QSpinBox      *m_linesPerFile;

    // по времени
    QDateTimeEdit *m_splitTimeFrom;
    QDateTimeEdit *m_splitTimeTo;

    QPushButton   *m_splitButton;
    QProgressBar  *m_progress;
    QLabel        *m_resultLabel;
};

#endif // FILESPLITTER_H