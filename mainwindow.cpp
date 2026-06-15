#include <QFileInfo>
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

    // загрузчик файлов — вот сюда, после setupConnections()
    m_fileLoader = new FileLoader(this);
    connect(m_fileLoader, &FileLoader::progressChanged,
            this, &MainWindow::onLoadingProgress);
    connect(m_fileLoader, &FileLoader::loadingFinished,
            this, &MainWindow::onLoadingFinished);
    connect(m_fileLoader, &FileLoader::loadingFailed,
            this, &MainWindow::onLoadingFailed);

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

    // вкладки вместо одного QPlainTextEdit
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);

    m_splitter->addWidget(m_treePanel);
    m_splitter->addWidget(m_tabWidget);
    m_splitter->setStretchFactor(0, 1);
    m_splitter->setStretchFactor(1, 3);
    m_splitter->setSizes({250, 950});  // дерево 250px, текст 950px

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

    m_filterInfo->setCheckable(true);
    m_filterWarn->setCheckable(true);
    m_filterError->setCheckable(true);
    m_filterOther->setCheckable(true);

    m_filterInfo->setChecked(true);
    m_filterWarn->setChecked(true);
    m_filterError->setChecked(true);
    m_filterOther->setChecked(true);

    m_filterInfo->setStyleSheet(
        "QPushButton { padding: 4px 12px; }"
        "QPushButton:checked { background-color: #2E5733; color: #6BCB77; }");
    m_filterWarn->setStyleSheet(
        "QPushButton { padding: 4px 12px; }"
        "QPushButton:checked { background-color: #554400; color: #FFD93D; }");
    m_filterError->setStyleSheet(
        "QPushButton { padding: 4px 12px; }"
        "QPushButton:checked { background-color: #552222; color: #FF6B6B; }");
    m_filterOther->setStyleSheet(
        "QPushButton { padding: 4px 12px; }"
        "QPushButton:checked { background-color: #444444; color: #AAAAAA; }");

    filterLayout->addWidget(m_filterInfo);
    filterLayout->addWidget(m_filterWarn);
    filterLayout->addWidget(m_filterError);
    filterLayout->addWidget(m_filterOther);
    filterLayout->addStretch();

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

    mainLayout->addLayout(filterLayout);
    mainLayout->addLayout(timeLayout);
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

    m_statusLabel->setText("Загрузка: " + filePath);
    m_progressBar->setValue(0);
    m_progressBar->show();

    m_fileLoader->setProperty("filePath", filePath);
    m_fileLoader->loadFile(filePath);
}

void MainWindow::populateTree()
{
    m_treePanel->clear();

    LogTab *tab = currentTab();
    if (!tab)
        return;

    const auto& entries = tab->entries;

    struct SourceGroup {
        QVector<int> entryIndices;
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

    for (int i = 0; i < entries.size(); i++) {
        const auto& entry = entries[i];

        QString levelKey;
        switch (entry.level) {
        case LogLevel::Info:    levelKey = "1_INFO";    break;
        case LogLevel::Warning: levelKey = "2_WARNING"; break;
        case LogLevel::Error:   levelKey = "3_ERROR";   break;
        default:                levelKey = "4_OTHER";   break;
        }

        QString source = entry.source;
        if (source.isEmpty())
            source = "unknown";

        int lastDot = source.lastIndexOf('.');
        if (lastDot != -1)
            source = source.mid(lastDot + 1);

        levels[levelKey].count++;
        levels[levelKey].sources[source].entryIndices.append(i);
    }

    for (auto levelIt = levels.begin(); levelIt != levels.end(); ++levelIt) {
        QString levelName = levelIt.key().mid(2);
        int count = levelIt.value().count;

        if (count == 0)
            continue;

        auto *levelItem = new QTreeWidgetItem(m_treePanel,
                                              QStringList{levelName + " (" + QString::number(count) + ")"});

        const auto& sources = levelIt.value().sources;
        for (auto srcIt = sources.begin(); srcIt != sources.end(); ++srcIt) {
            const QString& sourceName = srcIt.key();
            const auto& indices = srcIt.value().entryIndices;

            auto *sourceItem = new QTreeWidgetItem(levelItem,
                                                   QStringList{sourceName + " (" + QString::number(indices.size()) + ")"});

            for (int idx : indices) {
                const auto& entry = entries[idx];

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

    LogTab *tab = currentTab();
    if (!tab) return;

    QVariant data = item->data(0, Qt::UserRole);
    if (!data.isValid()) return;

    int lineNumber = data.toInt();

    QTextBlock block = tab->logViewer->document()->findBlockByLineNumber(lineNumber - 1);
    if (block.isValid()) {
        QTextCursor cursor(block);
        tab->logViewer->setTextCursor(cursor);
        tab->logViewer->centerCursor();

        QList<QTextEdit::ExtraSelection> selections;
        QTextEdit::ExtraSelection selection;
        selection.cursor = cursor;
        selection.cursor.movePosition(QTextCursor::StartOfBlock);
        selection.cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        selection.format.setBackground(QColor("#264F78"));
        selections.append(selection);

        tab->logViewer->setExtraSelections(selections);
    }
}

void MainWindow::onSearch()
{
    m_searchResults.clear();
    m_searchIndex = -1;

    LogTab *tab = currentTab();
    if (!tab) return;

    QString query = m_searchField->text();

    QList<QTextEdit::ExtraSelection> selections;

    if (query.isEmpty()) {
        tab->logViewer->setExtraSelections(selections);
        m_statusLabel->setText("Готово");
        return;
    }

    QTextDocument *doc = tab->logViewer->document();
    QTextCursor cursor(doc);

    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(QColor("#665500"));
    highlightFormat.setForeground(QColor("#FFFF00"));

    while (true) {
        cursor = doc->find(query, cursor);
        if (cursor.isNull()) break;

        QTextEdit::ExtraSelection selection;
        selection.cursor = cursor;
        selection.format = highlightFormat;
        selections.append(selection);

        m_searchResults.append(cursor.blockNumber());
    }

    tab->logViewer->setExtraSelections(selections);

    if (m_searchResults.isEmpty()) {
        m_statusLabel->setText("\"" + query + "\" — не найдено");
    } else {
        m_searchIndex = 0;
        m_statusLabel->setText(
            "Найдено: " + QString::number(m_searchResults.size()) + " совпадений");
        onSearchNext();
    }
}

void MainWindow::onSearchNext()
{
    if (m_searchResults.isEmpty()) return;

    LogTab *tab = currentTab();
    if (!tab) return;

    m_searchIndex = (m_searchIndex + 1) % m_searchResults.size();

    int blockNumber = m_searchResults[m_searchIndex];
    QTextBlock block = tab->logViewer->document()->findBlockByNumber(blockNumber);

    if (block.isValid()) {
        QTextCursor cursor(block);
        tab->logViewer->setTextCursor(cursor);
        tab->logViewer->centerCursor();
    }

    m_statusLabel->setText(
        "Совпадение " + QString::number(m_searchIndex + 1) +
        " из " + QString::number(m_searchResults.size()));
}

void MainWindow::onFilterChanged()
{
    applyFilter();
}

void MainWindow::applyFilter()
{
    LogTab *tab = currentTab();
    if (!tab) return;

    const auto& entries = tab->entries;

    if (m_filterInfo->isChecked() && m_filterWarn->isChecked() &&
        m_filterError->isChecked() && m_filterOther->isChecked()) {
        tab->logViewer->setPlainText(tab->rawContent);
        m_statusLabel->setText(
            "Файл: " + tab->filePath +
            " | Записей: " + QString::number(entries.size()));
        return;
    }

    QString filtered;
    int count = 0;

    for (const auto& entry : entries) {
        bool show = false;
        switch (entry.level) {
        case LogLevel::Info:    show = m_filterInfo->isChecked(); break;
        case LogLevel::Warning: show = m_filterWarn->isChecked(); break;
        case LogLevel::Error:   show = m_filterError->isChecked(); break;
        default:                show = m_filterOther->isChecked(); break;
        }

        if (show) {
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

    tab->logViewer->setPlainText(filtered);
    m_statusLabel->setText(
        "Фильтр | Показано: " + QString::number(count) +
        " из " + QString::number(entries.size()));
}

void MainWindow::onTimeFilterApply()
{
    LogTab *tab = currentTab();
    if (!tab) return;

    const auto& entries = tab->entries;
    QDateTime from = m_timeFrom->dateTime();
    QDateTime to = m_timeTo->dateTime();

    QString filtered;
    int count = 0;

    for (const auto& entry : entries) {
        bool levelOk = false;
        switch (entry.level) {
        case LogLevel::Info:    levelOk = m_filterInfo->isChecked(); break;
        case LogLevel::Warning: levelOk = m_filterWarn->isChecked(); break;
        case LogLevel::Error:   levelOk = m_filterError->isChecked(); break;
        default:                levelOk = m_filterOther->isChecked(); break;
        }
        if (!levelOk) continue;

        if (entry.timestamp.isValid()) {
            if (entry.timestamp < from || entry.timestamp > to)
                continue;
        }

        filtered += entry.rawText + "\n";
        for (const auto& st : entry.stacktrace)
            filtered += st + "\n";
        count++;
    }

    tab->logViewer->setPlainText(filtered);
    m_statusLabel->setText(
        "Временной фильтр | Показано: " + QString::number(count) +
        " из " + QString::number(entries.size()) +
        " | " + from.toString("HH:mm:ss") +
        " — " + to.toString("HH:mm:ss"));
}

void MainWindow::onTimeFilterReset()
{
    LogTab *tab = currentTab();
    if (!tab) return;

    QDateTime minTime, maxTime;
    for (const auto& entry : tab->entries) {
        if (!entry.timestamp.isValid()) continue;
        if (!minTime.isValid() || entry.timestamp < minTime)
            minTime = entry.timestamp;
        if (!maxTime.isValid() || entry.timestamp > maxTime)
            maxTime = entry.timestamp;
    }
    m_timeFrom->setDateTime(minTime);
    m_timeTo->setDateTime(maxTime);

    tab->logViewer->setPlainText(tab->rawContent);
    m_statusLabel->setText(
        "Файл: " + tab->filePath +
        " | Записей: " + QString::number(tab->entries.size()));
}



void MainWindow::setupStatusBar()
{
    m_statusLabel = new QLabel("Готово", this);
    statusBar()->addWidget(m_statusLabel);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setMaximumWidth(200);
    m_progressBar->setMaximumHeight(16);
    m_progressBar->hide();  // скрыт по умолчанию
    statusBar()->addPermanentWidget(m_progressBar);
}


void MainWindow::onLoadingProgress(int percent)
{
    m_progressBar->setValue(percent);
}

void MainWindow::onLoadingFinished(const QString &content,
                                   const QVector<LogEntry> &entries,
                                   LogFormat format)
{
    m_progressBar->hide();

    // создаём новую вкладку
    LogTab *tab = new LogTab(this);
    tab->rawContent = content;
    tab->entries = entries;
    tab->format = format;
    tab->filePath = m_fileLoader->property("filePath").toString();

    tab->logViewer->setPlainText(content);

    // имя файла для вкладки
    QString fileName = QFileInfo(tab->filePath).fileName();
    int index = m_tabWidget->addTab(tab, fileName);
    m_tabWidget->setCurrentIndex(index);

    // дерево и время обновятся через onTabChanged
}

void MainWindow::onLoadingFailed(const QString &error)
{
    m_progressBar->hide();
    QMessageBox::critical(this, "Ошибка", error);
    m_statusLabel->setText("Ошибка загрузки");
}

void MainWindow::setupConnections()
{
    connect(m_treePanel, &QTreeWidget::itemClicked,
            this, &MainWindow::onTreeItemClicked);

    connect(m_searchField, &QLineEdit::textChanged,
            this, &MainWindow::onSearch);
    connect(m_searchField, &QLineEdit::returnPressed,
            this, &MainWindow::onSearchNext);

    connect(m_filterInfo, &QPushButton::toggled,
            this, &MainWindow::onFilterChanged);
    connect(m_filterWarn, &QPushButton::toggled,
            this, &MainWindow::onFilterChanged);
    connect(m_filterError, &QPushButton::toggled,
            this, &MainWindow::onFilterChanged);
    connect(m_filterOther, &QPushButton::toggled,
            this, &MainWindow::onFilterChanged);

    connect(m_timeApply, &QPushButton::clicked,
            this, &MainWindow::onTimeFilterApply);
    connect(m_timeReset, &QPushButton::clicked,
            this, &MainWindow::onTimeFilterReset);

    // вкладки
    connect(m_tabWidget, &QTabWidget::currentChanged,
            this, &MainWindow::onTabChanged);
    connect(m_tabWidget, &QTabWidget::tabCloseRequested,
            this, &MainWindow::onTabClosed);
}


LogTab* MainWindow::currentTab()
{
    return qobject_cast<LogTab*>(m_tabWidget->currentWidget());
}

void MainWindow::onTabChanged(int index)
{
    Q_UNUSED(index);

    LogTab *tab = currentTab();
    if (!tab)
        return;

    // обновляем дерево для текущей вкладки
    populateTree();

    // обновляем диапазон времени
    if (!tab->entries.isEmpty()) {
        QDateTime minTime, maxTime;
        for (const auto& entry : tab->entries) {
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

    // обновляем статус
    QString formatName;
    switch (tab->format) {
    case LogFormat::Quarkus:         formatName = "Quarkus"; break;
    case LogFormat::SpringBoot:      formatName = "Spring Boot"; break;
    case LogFormat::SimpleSemicolon: formatName = "Simple"; break;
    default:                         formatName = "Unknown"; break;
    }

    m_statusLabel->setText(
        "Файл: " + tab->filePath +
        " | Формат: " + formatName +
        " | Записей: " + QString::number(tab->entries.size())
        );
}

void MainWindow::onTabClosed(int index)
{
    QWidget *tab = m_tabWidget->widget(index);
    m_tabWidget->removeTab(index);
    delete tab;

    if (m_tabWidget->count() == 0) {
        m_treePanel->clear();
        m_statusLabel->setText("Готово");
    }
}