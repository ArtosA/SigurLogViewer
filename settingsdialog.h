#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QFontComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QColor>
#include <QLabel>
#include <QSettings>

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    // геттеры
    QFont selectedFont() const;
    int fontSize() const;
    QColor colorInfo() const;
    QColor colorWarning() const;
    QColor colorError() const;
    QColor colorTimestamp() const;
    QColor colorStacktrace() const;
    bool darkTheme() const;

    // загрузка/сохранение
    static void saveSettings(const SettingsDialog &dlg);
    static void loadDefaults(SettingsDialog &dlg);

private slots:
    void chooseColor(QLabel *preview, QColor &color);
    void onReset();

private:
    void setupUI();
    void updatePreview(QLabel *label, const QColor &color);

    QFontComboBox *m_fontCombo;
    QSpinBox      *m_fontSizeSpin;

    QLabel *m_previewInfo;
    QLabel *m_previewWarn;
    QLabel *m_previewError;
    QLabel *m_previewTimestamp;
    QLabel *m_previewStacktrace;

    QColor m_colorInfo;
    QColor m_colorWarn;
    QColor m_colorError;
    QColor m_colorTimestamp;
    QColor m_colorStacktrace;

    QPushButton *m_darkThemeBtn;
    bool m_darkTheme;
    bool m_originalDarkTheme;
};

#endif // SETTINGSDIALOG_H