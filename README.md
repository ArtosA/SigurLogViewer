# SigurLogViewer

# Sigur Log Viewer

Desktop log analysis tool for Sigur services. Built with C++ and Qt 6.

![Qt](https://img.shields.io/badge/Qt-6.8-green?logo=qt)
![C++](https://img.shields.io/badge/C++-17-blue?logo=cplusplus)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey?logo=windows)
![License](https://img.shields.io/badge/License-MIT-yellow)

## Features

**Log Parsing**
- Auto-detection of log formats: Quarkus (local-agent), Spring Boot (notification-service), Simple semicolon-delimited (dcwdt)
- Stacktrace grouping — multi-line exceptions are joined with their parent entry
- Regex-based parsing with pre-compiled patterns for performance

**Navigation & Analysis**
- Tree panel with 3-level hierarchy: Log Level → Source Class → Individual Entry
- Click any tree entry to jump directly to the corresponding line
- Real-time statistics panel: record counts by level, time range, top sources, error percentage

**Search & Filtering**
- Full-text search with match highlighting and navigation (Enter = next match)
- Level-based toggle filters (INFO / WARN / ERROR / OTHER)
- Timestamp range filter with millisecond precision
- Combined filtering: level + time range work together

**Syntax Highlighting**
- Color-coded log levels: ERROR (red), WARNING (yellow), INFO (green), DEBUG (grey)
- Timestamp highlighting (blue)
- Stacktrace lines in italic grey
- Customizable colors through Settings

**Multi-file Support**
- Tabbed interface — open multiple logs simultaneously
- Each tab maintains its own tree, filters, and state
- Drag & Drop — drop .log/.txt files directly into the window

**Tools**
- File Splitter: split large logs by line count or extract a time range
- Line numbering with dark-themed gutter
- Background file loading via QThread — UI stays responsive

**Customization**
- Dark and Light themes (VS Code-inspired)
- Configurable font family and size
- Customizable highlight colors for each log level
- All settings persist between sessions (QSettings)

## Supported Log Formats

**Quarkus (local-agent)**
```
2026-05-06 14:02:25,039  INFO PlatformDependent:-1 - message text
```

**Spring Boot (notification-service)**
```
2026-05-06 12:31:52.221  WARN 1120 --- [thread-name] com.example.Class : message text
```

**Simple semicolon-delimited (dcwdt)**
```
2025-07-07 00:00:00; message text
```

## Requirements

- Qt 6.8+ (Widgets module)
- CMake 3.19+
- MinGW 13+ or MSVC 2022
- Windows 10/11

## Build

```bash
git clone https://github.com/ArtosA/SigurLogViewer.git
cd SigurLogViewer
cmake -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/Qt/6.8.0/mingw_64"
cmake --build build
```

Or open the project in **Qt Creator** and build with the configured kit.

## Project Structure

```
SigurLogViewer/
├── main.cpp                 — Application entry point
├── mainwindow.h/.cpp        — Main window, UI layout, signal/slot wiring
├── logentry.h               — LogEntry struct, LogLevel/LogFormat enums
├── logparser.h/.cpp         — Regex-based multi-format log parser
├── syntaxhighlighter.h/.cpp — QSyntaxHighlighter with configurable colors
├── logtab.h/.cpp            — Per-tab widget (editor + highlighter + data)
├── linenumberarea.h/.cpp    — Line number gutter for QPlainTextEdit
├── fileloader.h/.cpp        — QThread-based async file loader
├── filesplitter.h/.cpp      — File splitting dialog (by lines / by time)
├── settingsdialog.h/.cpp    — Settings dialog (font, colors, theme)
├── styles.h                 — QSS themes (dark / light)
└── CMakeLists.txt           — Build configuration
```

## Roadmap

- [ ] Warning dialog for files over 500 MB
- [ ] Memory-mapped file reading for files up to 8 GB+
- [ ] Virtual scrolling (render only visible lines)
- [ ] Side-by-side file comparison (diff tool)
- [ ] Split-view parallel file analysis
- [ ] Auto-update via GitHub release tags

## Author

**ArtosA** — [github.com/ArtosA](https://github.com/ArtosA)

## License

MIT