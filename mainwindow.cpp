#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QTextBlock>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    setupMenuBar();
    setupStatusBar();
    setupConnections();

    setWindowTitle("Sigur Log Viewer");
    resize(1200, 700);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    m_splitter = new QSplitter(Qt::Horizontal, this);

    m_treePanel = new QTreeWidget(this);
    m_treePanel->setHeaderLabel("Log Structure");
    m_treePanel->setMinimumWidth(200);
    //m_treePanel->setMaximumWidth(400);

    m_logViewer = new QPlainTextEdit(this);
    m_logViewer->setReadOnly(true);
    m_logViewer->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_logViewer->setFont(QFont("Consolas", 10));
    m_logViewer->setPlaceholderText("Открой файл: Файл → Открыть (Ctrl+O)");

    m_highlighter = new SyntaxHighlighter(m_logViewer->document());

    m_splitter->addWidget(m_treePanel);
    m_splitter->addWidget(m_logViewer);
    m_splitter->setStretchFactor(0, 1);
    m_splitter->setStretchFactor(1, 3);

    m_searchField = new QLineEdit(this);
    m_searchField->setPlaceholderText("Поиск по логу... (Ctrl+F)");

    QShortcut *searchShortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
    connect(searchShortcut, &QShortcut::activated, [this]() {
        m_searchField->setFocus();
        m_searchField->selectAll();
    });

    // панель фильтров
    QHBoxLayout *filterLayout = new QHBoxLayout();

    m_filterInfo = new QPushButton("INFO", this);
    m_filterWarn = new QPushButton("WARN", this);
    m_filterError = new QPushButton("ERROR", this);
    m_filterOther = new QPushButton("OTHER", this);

    // делаем кнопки переключаемыми (toggle)
    m_filterInfo->setCheckable(true);
    m_filterWarn->setCheckable(true);
    m_filterError->setCheckable(true);
    m_filterOther->setCheckable(true);

    // все включены по умолчанию
    m_filterInfo->setChecked(true);
    m_filterWarn->setChecked(true);
    m_filterError->setChecked(true);
    m_filterOther->setChecked(true);

    // стилизация — цвет фона когда активна
    m_filterInfo->setStyleSheet(
        "QPushButton { padding: 4px 12px; }"
        "QPushButton:checked { background-color: #2E5733; color: #6BCB77; }"
        );
    m_filterWarn->setStyleSheet(
        "QPushButton { padding: 4px 12px; }"
        "QPushButton:checked { background-color: #554400; color: #FFD93D; }"
        );
    m_filterError->setStyleSheet(
        "QPushButton { padding: 4px 12px; }"
        "QPushButton:checked { background-color: #552222; color: #FF6B6B; }"
        );
    m_filterOther->setStyleSheet(
        "QPushButton { padding: 4px 12px; }"
        "QPushButton:checked { background-color: #444444; color: #AAAAAA; }"
        );

    filterLayout->addWidget(m_filterInfo);
    filterLayout->addWidget(m_filterWarn);
    filterLayout->addWidget(m_filterError);
    filterLayout->addWidget(m_filterOther);
    filterLayout->addStretch();  // прижимает кнопки влево

    // панель фильтра по времени
    QHBoxLayout *timeLayout = new QHBoxLayout();

    QLabel *timeLabel = new QLabel("Время:", this);

    m_timeFrom = new QDateTimeEdit(this);
    m_timeFrom->setDisplayFormat("yyyy-MM-dd HH:mm:ss.zzz");
    m_timeFrom->setCalendarPopup(false);

    QLabel *timeDash = new QLabel("—", this);

    m_timeTo = new QDateTimeEdit(this);
    m_timeTo->setDisplayFormat("yyyy-MM-dd HH:mm:ss.zzz");
    m_timeTo->setCalendarPopup(false);

    m_timeApply = new QPushButton("Применить", this);
    m_timeApply->setStyleSheet("padding: 4px 12px;");

    m_timeReset = new QPushButton("Сброс", this);
    m_timeReset->setStyleSheet("padding: 4px 12px;");

    timeLayout->addWidget(timeLabel);
    timeLayout->addWidget(m_timeFrom);
    timeLayout->addWidget(timeDash);
    timeLayout->addWidget(m_timeTo);
    timeLayout->addWidget(m_timeApply);
    timeLayout->addWidget(m_timeReset);
    timeLayout->addStretch();

    // собираем layout
    mainLayout->addLayout(filterLayout);   // фильтры уровней
    mainLayout->addLayout(timeLayout);     // фильтр времени
    mainLayout->addWidget(m_splitter);     // splitter
    mainLayout->addWidget(m_searchField);  // поиск

    // собираем layout
    mainLayout->addLayout(filterLayout);   // фильтры сверху
    mainLayout->addWidget(m_splitter);     // splitter посередине
    mainLayout->addWidget(m_searchField);  // поиск снизу

    mainLayout->addWidget(m_splitter);
    mainLayout->addWidget(m_searchField);




}

void MainWindow::setupMenuBar()
{
    QMenu *fileMenu = menuBar()->addMenu("Файл");

    QAction *openAction = fileMenu->addAction("Открыть");
    openAction->setShortcut(QKeySequence("Ctrl+O"));
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

    fileMenu->addSeparator();

    QAction *exitAction = fileMenu->addAction("Выход");
    exitAction->setShortcut(QKeySequence("Ctrl+Q"));
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
}

void MainWindow::setupStatusBar()
{
    m_statusLabel = new QLabel("Готово", this);
    statusBar()->addWidget(m_statusLabel);
}

void MainWindow::setupConnections()
{
    connect(m_treePanel, &QTreeWidget::itemClicked,
            this, &MainWindow::onTreeItemClicked);

    connect(m_searchField, &QLineEdit::textChanged,
            this, &MainWindow::onSearch);

    connect(m_searchField, &QLineEdit::returnPressed,
            this, &MainWindow::onSearchNext);

    // фильтры
    connect(m_filterInfo, &QPushButton::toggled,
            this, &MainWindow::onFilterChanged);
    connect(m_filterWarn, &QPushButton::toggled,
            this, &MainWindow::onFilterChanged);
    connect(m_filterError, &QPushButton::toggled,
            this, &MainWindow::onFilterChanged);
    connect(m_filterOther, &QPushButton::toggled,
            this, &MainWindow::onFilterChanged);

    // фильтр по времени
    connect(m_timeApply, &QPushButton::clicked,
            this, &MainWindow::onTimeFilterApply);
    connect(m_timeReset, &QPushButton::clicked,
            this, &MainWindow::onTimeFilterReset);
}

void MainWindow::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Открыть лог файл",
        QString(),
        "Log files (*.log *.txt);;All files (*.*)"
        );

    if (filePath.isEmpty())
        return;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка",
                              "Не удалось открыть файл: " + filePath);
        return;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    // показываем текст
    m_logViewer->setPlainText(content);

    m_logViewer->setPlainText(content);
    m_rawContent = content;  // сохраняем для фильтрации

    // парсим
    m_entries = m_parser.parse(content);
    m_currentFile = filePath;

    // устанавливаем диапазон времени из данных
    if (!m_entries.isEmpty()) {
        QDateTime minTime, maxTime;
        for (const auto& entry : m_entries) {
            if (!entry.timestamp.isValid())
                continue;
            if (!minTime.isValid() || entry.timestamp < minTime)
                minTime = entry.timestamp;
            if (!maxTime.isValid() || entry.timestamp > maxTime)
                maxTime = entry.timestamp;
        }
        m_timeFrom->setDateTime(minTime);
        m_timeTo->setDateTime(maxTime);
    }

    // наполняем дерево
    populateTree();

    // обновляем статус бар
    QString formatName;
    switch (m_parser.lastFormat()) {
    case LogFormat::Quarkus:         formatName = "Quarkus"; break;
    case LogFormat::SpringBoot:      formatName = "Spring Boot"; break;
    case LogFormat::SimpleSemicolon: formatName = "Simple"; break;
    default:                         formatName = "Unknown"; break;
    }

    m_statusLabel->setText(
        "Файл: " + filePath +
        " | Формат: " + formatName +
        " | Записей: " + QString::number(m_entries.size())
        );
}

void MainWindow::populateTree()
{
    m_treePanel->clear();

    // структура: level → source → список записей
    // QMap автоматически сортирует по ключу
    struct SourceGroup {
        QVector<int> entryIndices;  // индексы записей в m_entries
    };

    struct LevelGroup {
        QMap<QString, SourceGroup> sources;
        int count = 0;
    };

    QMap<QString, LevelGroup> levels;
    levels["1_INFO"]    = LevelGroup();
    levels["2_WARNING"] = LevelGroup();
    levels["3_ERROR"]   = LevelGroup();
    levels["4_OTHER"]   = LevelGroup();

    // группируем записи
    for (int i = 0; i < m_entries.size(); i++) {
        const auto& entry = m_entries[i];

        QString levelKey;
        switch (entry.level) {
        case LogLevel::Info:    levelKey = "1_INFO";    break;
        case LogLevel::Warning: levelKey = "2_WARNING"; break;
        case LogLevel::Error:   levelKey = "3_ERROR";   break;
        default:                levelKey = "4_OTHER";   break;
        }

        // сокращаем имя источника для читаемости
        QString source = entry.source;
        if (source.isEmpty())
            source = "unknown";

        // убираем длинный путь пакета, оставляем последнюю часть
        // com.sigur.notification.NotificationServiceApp → NotificationServiceApp
        int lastDot = source.lastIndexOf('.');
        if (lastDot != -1)
            source = source.mid(lastDot + 1);

        levels[levelKey].count++;
        levels[levelKey].sources[source].entryIndices.append(i);
    }

    // строим дерево
    for (auto levelIt = levels.begin(); levelIt != levels.end(); ++levelIt) {
        QString levelName = levelIt.key().mid(2);  // убираем "1_" префикс
        int count = levelIt.value().count;

        if (count == 0)
            continue;

        // корневой узел уровня
        auto *levelItem = new QTreeWidgetItem(m_treePanel,
                                              QStringList{levelName + " (" + QString::number(count) + ")"});

        // узлы источников
        const auto& sources = levelIt.value().sources;
        for (auto srcIt = sources.begin(); srcIt != sources.end(); ++srcIt) {
            const QString& sourceName = srcIt.key();
            const auto& indices = srcIt.value().entryIndices;

            auto *sourceItem = new QTreeWidgetItem(levelItem,
                                                   QStringList{sourceName + " (" + QString::number(indices.size()) + ")"});

            // дочерние элементы — конкретные записи
            for (int idx : indices) {
                const auto& entry = m_entries[idx];

                QString label = entry.timestamp.toString("HH:mm:ss.zzz") +
                                " | " + entry.message.left(60);

                auto *child = new QTreeWidgetItem(sourceItem, QStringList{label});
                child->setData(0, Qt::UserRole, entry.lineNumber);
            }
        }
    }
}

void MainWindow::onTreeItemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    QVariant data = item->data(0, Qt::UserRole);
    if (!data.isValid())
        return;

    int lineNumber = data.toInt();

    QTextBlock block = m_logViewer->document()->findBlockByLineNumber(lineNumber - 1);
    if (block.isValid()) {
        QTextCursor cursor(block);
        m_logViewer->setTextCursor(cursor);
        m_logViewer->centerCursor();

        // подсветка выбранной строки
        QList<QTextEdit::ExtraSelection> selections;
        QTextEdit::ExtraSelection selection;
        selection.cursor = cursor;
        selection.cursor.movePosition(QTextCursor::StartOfBlock);
        selection.cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        selection.format.setBackground(QColor("#264F78"));
        selections.append(selection);

        m_logViewer->setExtraSelections(selections);
    }
}

void MainWindow::onSearch()
{
    m_searchResults.clear();
    m_searchIndex = -1;

    QString query = m_searchField->text();

    // убираем старую подсветку
    QList<QTextEdit::ExtraSelection> selections;

    if (query.isEmpty()) {
        m_logViewer->setExtraSelections(selections);
        m_statusLabel->setText("Готово");
        return;
    }

    // ищем все совпадения
    QTextDocument *doc = m_logViewer->document();
    QTextCursor cursor(doc);

    // формат подсветки найденных совпадений
    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(QColor("#665500"));
    highlightFormat.setForeground(QColor("#FFFF00"));

    while (true) {
        cursor = doc->find(query, cursor);
        if (cursor.isNull())
            break;

        QTextEdit::ExtraSelection selection;
        selection.cursor = cursor;
        selection.format = highlightFormat;
        selections.append(selection);

        m_searchResults.append(cursor.blockNumber());
    }

    m_logViewer->setExtraSelections(selections);

    if (m_searchResults.isEmpty()) {
        m_statusLabel->setText("\"" + query + "\" — не найдено");
    } else {
        m_searchIndex = 0;
        m_statusLabel->setText(
            "Найдено: " + QString::number(m_searchResults.size()) +
            " совпадений"
            );
        // переходим к первому результату
        onSearchNext();
    }
}

void MainWindow::onSearchNext()
{
    if (m_searchResults.isEmpty())
        return;

    // переходим к следующему результату циклически
    m_searchIndex = (m_searchIndex + 1) % m_searchResults.size();

    int blockNumber = m_searchResults[m_searchIndex];
    QTextBlock block = m_logViewer->document()->findBlockByNumber(blockNumber);

    if (block.isValid()) {
        QTextCursor cursor(block);
        m_logViewer->setTextCursor(cursor);
        m_logViewer->centerCursor();
    }

    m_statusLabel->setText(
        "Совпадение " + QString::number(m_searchIndex + 1) +
        " из " + QString::number(m_searchResults.size())
        );
}


void MainWindow::onFilterChanged()
{
    applyFilter();
}

void MainWindow::applyFilter()
{
    if (m_entries.isEmpty())
        return;

    // если все фильтры включены — показываем оригинал
    if (m_filterInfo->isChecked() && m_filterWarn->isChecked() &&
        m_filterError->isChecked() && m_filterOther->isChecked()) {
        m_logViewer->setPlainText(m_rawContent);
        m_statusLabel->setText(
            "Файл: " + m_currentFile +
            " | Записей: " + QString::number(m_entries.size())
            );
        return;
    }

    // собираем отфильтрованный текст
    QString filtered;
    int count = 0;

    for (const auto& entry : m_entries) {
        bool show = false;

        switch (entry.level) {
        case LogLevel::Info:
            show = m_filterInfo->isChecked();
            break;
        case LogLevel::Warning:
            show = m_filterWarn->isChecked();
            break;
        case LogLevel::Error:
            show = m_filterError->isChecked();
            break;
        default:
            show = m_filterOther->isChecked();
            break;
        }

        if (show) {
            // проверяем время если фильтр активен
            if (entry.timestamp.isValid()) {
                QDateTime from = m_timeFrom->dateTime();
                QDateTime to = m_timeTo->dateTime();
                if (entry.timestamp < from || entry.timestamp > to)
                    continue;
            }

            filtered += entry.rawText + "\n";
            for (const auto& st : entry.stacktrace)
                filtered += st + "\n";
            count++;
        }
    }

    m_logViewer->setPlainText(filtered);

    m_statusLabel->setText(
        "Фильтр | Показано: " + QString::number(count) +
        " из " + QString::number(m_entries.size())
        );
}

void MainWindow::onTimeFilterApply()
{
    if (m_entries.isEmpty())
        return;

    QDateTime from = m_timeFrom->dateTime();
    QDateTime to = m_timeTo->dateTime();

    QString filtered;
    int count = 0;

    for (const auto& entry : m_entries) {
        // проверяем уровень
        bool levelOk = false;
        switch (entry.level) {
        case LogLevel::Info:    levelOk = m_filterInfo->isChecked(); break;
        case LogLevel::Warning: levelOk = m_filterWarn->isChecked(); break;
        case LogLevel::Error:   levelOk = m_filterError->isChecked(); break;
        default:                levelOk = m_filterOther->isChecked(); break;
        }

        if (!levelOk)
            continue;

        // проверяем время
        if (entry.timestamp.isValid()) {
            if (entry.timestamp < from || entry.timestamp > to)
                continue;
        }

        filtered += entry.rawText + "\n";
        for (const auto& st : entry.stacktrace)
            filtered += st + "\n";
        count++;
    }

    m_logViewer->setPlainText(filtered);

    m_statusLabel->setText(
        "Временной фильтр | Показано: " + QString::number(count) +
        " из " + QString::number(m_entries.size()) +
        " | " + from.toString("HH:mm:ss") +
        " — " + to.toString("HH:mm:ss")
        );
}

void MainWindow::onTimeFilterReset()
{
    if (m_entries.isEmpty())
        return;

    // возвращаем полный диапазон
    QDateTime minTime, maxTime;
    for (const auto& entry : m_entries) {
        if (!entry.timestamp.isValid())
            continue;
        if (!minTime.isValid() || entry.timestamp < minTime)
            minTime = entry.timestamp;
        if (!maxTime.isValid() || entry.timestamp > maxTime)
            maxTime = entry.timestamp;
    }
    m_timeFrom->setDateTime(minTime);
    m_timeTo->setDateTime(maxTime);

    // показываем всё
    m_logViewer->setPlainText(m_rawContent);

    m_statusLabel->setText(
        "Файл: " + m_currentFile +
        " | Записей: " + QString::number(m_entries.size())
        );
}