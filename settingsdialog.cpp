#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QColorDialog>
#include <QDialogButtonBox>
#include <QCheckBox>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , m_colorInfo("#6BCB77")
    , m_colorWarn("#FFD93D")
    , m_colorError("#FF6B6B")
    , m_colorTimestamp("#4FC3F7")
    , m_colorStacktrace("#777777")
    , m_darkTheme(true)
{
    setWindowTitle("Настройки");
    setMinimumWidth(450);

    // загружаем сохранённые настройки
    loadDefaults(*this);

    setupUI();
}

void SettingsDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // шрифт
    QGroupBox *fontGroup = new QGroupBox("Шрифт", this);
    QHBoxLayout *fontLayout = new QHBoxLayout(fontGroup);

    m_fontCombo = new QFontComboBox(this);
    m_fontSizeSpin = new QSpinBox(this);
    m_fontSizeSpin->setRange(6, 30);
    m_fontSizeSpin->setValue(10);

    // загружаем сохранённый шрифт
    QSettings settings("SigurTools", "LogViewer");
    QString savedFont = settings.value("font/family", "Consolas").toString();
    int savedSize = settings.value("font/size", 10).toInt();
    m_fontCombo->setCurrentFont(QFont(savedFont));
    m_fontSizeSpin->setValue(savedSize);

    fontLayout->addWidget(m_fontCombo);
    fontLayout->addWidget(m_fontSizeSpin);
    mainLayout->addWidget(fontGroup);

    // цвета подсветки
    QGroupBox *colorGroup = new QGroupBox("Цвета подсветки", this);
    QVBoxLayout *colorLayout = new QVBoxLayout(colorGroup);

    auto addColorRow = [&](const QString &label, QLabel *&preview, QColor &color) {
        QHBoxLayout *row = new QHBoxLayout();
        row->addWidget(new QLabel(label, this));
        row->addStretch();

        preview = new QLabel(this);
        preview->setFixedSize(60, 24);
        updatePreview(preview, color);

        QPushButton *btn = new QPushButton("Выбрать", this);
        btn->setFixedWidth(80);
        connect(btn, &QPushButton::clicked, [this, preview, &color]() {
            chooseColor(preview, color);
        });

        row->addWidget(preview);
        row->addWidget(btn);
        colorLayout->addLayout(row);
    };

    addColorRow("INFO",       m_previewInfo,       m_colorInfo);
    addColorRow("WARNING",    m_previewWarn,        m_colorWarn);
    addColorRow("ERROR",      m_previewError,       m_colorError);
    addColorRow("Timestamp",  m_previewTimestamp,   m_colorTimestamp);
    addColorRow("Stacktrace", m_previewStacktrace,  m_colorStacktrace);

    mainLayout->addWidget(colorGroup);

    // тема
    QGroupBox *themeGroup = new QGroupBox("Тема", this);
    QHBoxLayout *themeLayout = new QHBoxLayout(themeGroup);

    QCheckBox *darkCheck = new QCheckBox("Тёмная тема", this);
    darkCheck->setChecked(m_darkTheme);
    connect(darkCheck, &QCheckBox::toggled, [this](bool checked) {
        m_darkTheme = checked;
    });

    themeLayout->addWidget(darkCheck);
    mainLayout->addWidget(themeGroup);

    // кнопки
    QHBoxLayout *btnLayout = new QHBoxLayout();

    QPushButton *resetBtn = new QPushButton("Сброс", this);
    connect(resetBtn, &QPushButton::clicked, this, &SettingsDialog::onReset);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    btnLayout->addWidget(resetBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(buttonBox);
    mainLayout->addLayout(btnLayout);
}

void SettingsDialog::chooseColor(QLabel *preview, QColor &color)
{
    QColor newColor = QColorDialog::getColor(color, this, "Выбери цвет");
    if (newColor.isValid()) {
        color = newColor;
        updatePreview(preview, color);
    }
}

void SettingsDialog::updatePreview(QLabel *label, const QColor &color)
{
    label->setStyleSheet(
        "background-color: " + color.name() + ";"
                                              "border: 1px solid #555;"
                                              "border-radius: 3px;");
}

void SettingsDialog::onReset()
{
    m_fontCombo->setCurrentFont(QFont("Consolas"));
    m_fontSizeSpin->setValue(10);

    m_colorInfo = QColor("#6BCB77");
    m_colorWarn = QColor("#FFD93D");
    m_colorError = QColor("#FF6B6B");
    m_colorTimestamp = QColor("#4FC3F7");
    m_colorStacktrace = QColor("#777777");
    m_darkTheme = true;

    updatePreview(m_previewInfo, m_colorInfo);
    updatePreview(m_previewWarn, m_colorWarn);
    updatePreview(m_previewError, m_colorError);
    updatePreview(m_previewTimestamp, m_colorTimestamp);
    updatePreview(m_previewStacktrace, m_colorStacktrace);
}

QFont SettingsDialog::selectedFont() const { return m_fontCombo->currentFont(); }
int SettingsDialog::fontSize() const { return m_fontSizeSpin->value(); }
QColor SettingsDialog::colorInfo() const { return m_colorInfo; }
QColor SettingsDialog::colorWarning() const { return m_colorWarn; }
QColor SettingsDialog::colorError() const { return m_colorError; }
QColor SettingsDialog::colorTimestamp() const { return m_colorTimestamp; }
QColor SettingsDialog::colorStacktrace() const { return m_colorStacktrace; }
bool SettingsDialog::darkTheme() const { return m_darkTheme; }

void SettingsDialog::saveSettings(const SettingsDialog &dlg)
{
    QSettings settings("SigurTools", "LogViewer");

    settings.setValue("font/family", dlg.selectedFont().family());
    settings.setValue("font/size", dlg.fontSize());

    settings.setValue("colors/info", dlg.colorInfo().name());
    settings.setValue("colors/warning", dlg.colorWarning().name());
    settings.setValue("colors/error", dlg.colorError().name());
    settings.setValue("colors/timestamp", dlg.colorTimestamp().name());
    settings.setValue("colors/stacktrace", dlg.colorStacktrace().name());

    settings.setValue("theme/dark", dlg.darkTheme());
}

void SettingsDialog::loadDefaults(SettingsDialog &dlg)
{
    QSettings settings("SigurTools", "LogViewer");

    dlg.m_colorInfo = QColor(settings.value("colors/info", "#6BCB77").toString());
    dlg.m_colorWarn = QColor(settings.value("colors/warning", "#FFD93D").toString());
    dlg.m_colorError = QColor(settings.value("colors/error", "#FF6B6B").toString());
    dlg.m_colorTimestamp = QColor(settings.value("colors/timestamp", "#4FC3F7").toString());
    dlg.m_colorStacktrace = QColor(settings.value("colors/stacktrace", "#777777").toString());
    dlg.m_darkTheme = settings.value("theme/dark", true).toBool();
}