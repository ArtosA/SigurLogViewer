#ifndef STYLES_H
#define STYLES_H

#include <QString>

namespace Styles {

inline QString darkTheme()
{
    return R"(
        QMainWindow {
            background-color: #1E1E1E;
        }

        QMenuBar {
            background-color: #2D2D2D;
            color: #CCCCCC;
            border-bottom: 1px solid #3D3D3D;
            padding: 2px;
        }
        QMenuBar::item:selected {
            background-color: #3D3D3D;
        }

        QMenu {
            background-color: #2D2D2D;
            color: #CCCCCC;
            border: 1px solid #3D3D3D;
        }
        QMenu::item:selected {
            background-color: #094771;
        }
        QMenu::separator {
            height: 1px;
            background: #3D3D3D;
            margin: 4px 8px;
        }

        QPlainTextEdit, CodeEditor {
            background-color: #1E1E1E;
            color: #D4D4D4;
            border: 1px solid #3D3D3D;
            selection-background-color: #264F78;
            selection-color: #FFFFFF;
        }

        QTreeWidget {
            background-color: #252526;
            color: #CCCCCC;
            border: 1px solid #3D3D3D;
            outline: none;
        }
        QTreeWidget::item:hover {
            background-color: #2A2D2E;
        }
        QTreeWidget::item:selected {
            background-color: #094771;
            color: #FFFFFF;
        }
        QTreeWidget::branch:has-children:closed {
            image: none;
            border-image: none;
        }

        QLineEdit {
            background-color: #3C3C3C;
            color: #CCCCCC;
            border: 1px solid #3D3D3D;
            border-radius: 4px;
            padding: 4px 8px;
            selection-background-color: #264F78;
        }
        QLineEdit:focus {
            border: 1px solid #007ACC;
        }

        QPushButton {
            background-color: #3C3C3C;
            color: #CCCCCC;
            border: 1px solid #3D3D3D;
            border-radius: 4px;
            padding: 4px 12px;
        }
        QPushButton:hover {
            background-color: #4C4C4C;
        }
        QPushButton:pressed {
            background-color: #094771;
        }

        QTabWidget::pane {
            border: 1px solid #3D3D3D;
            background-color: #1E1E1E;
        }
        QTabBar::tab {
            background-color: #2D2D2D;
            color: #888888;
            border: 1px solid #3D3D3D;
            border-bottom: none;
            padding: 6px 16px;
            margin-right: 2px;
        }
        QTabBar::tab:selected {
            background-color: #1E1E1E;
            color: #FFFFFF;
            border-bottom: 2px solid #007ACC;
        }
        QTabBar::tab:hover:!selected {
            background-color: #353535;
            color: #CCCCCC;
        }
        QTabBar::close-button {
            image: none;
            subcontrol-position: right;
        }
        QTabBar::close-button:hover {
            background-color: #C42B1C;
            border-radius: 2px;
        }

        QSplitter::handle {
            background-color: #3D3D3D;
            width: 2px;
        }
        QSplitter::handle:hover {
            background-color: #007ACC;
        }

        QStatusBar {
            background-color: #007ACC;
            color: #FFFFFF;
        }
        QStatusBar QLabel {
            color: #FFFFFF;
        }

        QProgressBar {
            background-color: #3C3C3C;
            border: none;
            border-radius: 2px;
            text-align: center;
            color: #FFFFFF;
        }
        QProgressBar::chunk {
            background-color: #007ACC;
            border-radius: 2px;
        }

        QDockWidget {
            color: #CCCCCC;
            titlebar-close-icon: none;
        }
        QDockWidget::title {
            background-color: #252526;
            padding: 6px;
            border-bottom: 1px solid #3D3D3D;
        }

        QTextBrowser {
            background-color: #252526;
            color: #CCCCCC;
            border: none;
        }

        QDateTimeEdit {
            background-color: #3C3C3C;
            color: #CCCCCC;
            border: 1px solid #3D3D3D;
            border-radius: 4px;
            padding: 3px 6px;
        }
        QDateTimeEdit:focus {
            border: 1px solid #007ACC;
        }

        QScrollBar:vertical {
            background: #1E1E1E;
            width: 12px;
            margin: 0;
        }
        QScrollBar::handle:vertical {
            background: #424242;
            min-height: 20px;
            border-radius: 3px;
            margin: 2px;
        }
        QScrollBar::handle:vertical:hover {
            background: #555555;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }

        QScrollBar:horizontal {
            background: #1E1E1E;
            height: 12px;
            margin: 0;
        }
        QScrollBar::handle:horizontal {
            background: #424242;
            min-width: 20px;
            border-radius: 3px;
            margin: 2px;
        }
        QScrollBar::handle:horizontal:hover {
            background: #555555;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0;
        }

        QLabel {
            color: #CCCCCC;
        }

        QSpinBox, QFontComboBox, QComboBox {
            background-color: #3C3C3C;
            color: #CCCCCC;
            border: 1px solid #3D3D3D;
            border-radius: 4px;
            padding: 3px 6px;
        }

        QGroupBox {
            color: #CCCCCC;
            border: 1px solid #3D3D3D;
            border-radius: 4px;
            margin-top: 8px;
            padding-top: 16px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 4px;
        }

        QCheckBox {
            color: #CCCCCC;
            spacing: 8px;
        }

        QDialogButtonBox QPushButton {
            min-width: 80px;
        }

        QHeaderView::section {
            background-color: #252526;
            color: #CCCCCC;
            border: 1px solid #3D3D3D;
            padding: 4px;
        }
    QDialog {
                background-color: #2D2D2D;
                color: #CCCCCC;
            }
    )";
}

inline QString lightTheme()
{
    return R"(
        QMainWindow {
            background-color: #F5F5F5;
        }

        QMenuBar {
            background-color: #E8E8E8;
            color: #333333;
            border-bottom: 1px solid #D0D0D0;
            padding: 2px;
        }
        QDialog {
                    background-color: #F5F5F5;
                    color: #333333;
        }
        QMenuBar::item:selected {
            background-color: #D0D0D0;
        }

        QMenu {
            background-color: #FFFFFF;
            color: #333333;
            border: 1px solid #D0D0D0;
        }
        QMenu::item:selected {
            background-color: #0078D4;
            color: #FFFFFF;
        }

        QPlainTextEdit, CodeEditor {
            background-color: #FFFFFF;
            color: #1E1E1E;
            border: 1px solid #D0D0D0;
            selection-background-color: #ADD6FF;
            selection-color: #000000;
        }

        QTreeWidget {
            background-color: #F3F3F3;
            color: #333333;
            border: 1px solid #D0D0D0;
        }
        QTreeWidget::item:hover {
            background-color: #E8E8E8;
        }
        QTreeWidget::item:selected {
            background-color: #0078D4;
            color: #FFFFFF;
        }

        QLineEdit {
            background-color: #FFFFFF;
            color: #333333;
            border: 1px solid #D0D0D0;
            border-radius: 4px;
            padding: 4px 8px;
        }
        QLineEdit:focus {
            border: 1px solid #0078D4;
        }

        QPushButton {
            background-color: #E8E8E8;
            color: #333333;
            border: 1px solid #D0D0D0;
            border-radius: 4px;
            padding: 4px 12px;
        }
        QPushButton:hover {
            background-color: #D8D8D8;
        }
        QPushButton:pressed {
            background-color: #0078D4;
            color: #FFFFFF;
        }

        QTabWidget::pane {
            border: 1px solid #D0D0D0;
            background-color: #FFFFFF;
        }
        QTabBar::tab {
            background-color: #E8E8E8;
            color: #666666;
            border: 1px solid #D0D0D0;
            border-bottom: none;
            padding: 6px 16px;
            margin-right: 2px;
        }
        QTabBar::tab:selected {
            background-color: #FFFFFF;
            color: #333333;
            border-bottom: 2px solid #0078D4;
        }

        QSplitter::handle {
            background-color: #D0D0D0;
            width: 2px;
        }

        QStatusBar {
            background-color: #0078D4;
            color: #FFFFFF;
        }
        QStatusBar QLabel {
            color: #FFFFFF;
        }

        QProgressBar {
            background-color: #E0E0E0;
            border: none;
            border-radius: 2px;
        }
        QProgressBar::chunk {
            background-color: #0078D4;
            border-radius: 2px;
        }

        QDockWidget::title {
            background-color: #F3F3F3;
            padding: 6px;
            border-bottom: 1px solid #D0D0D0;
        }

        QTextBrowser {
            background-color: #F3F3F3;
            color: #333333;
            border: none;
        }

        QDateTimeEdit {
            background-color: #FFFFFF;
            color: #333333;
            border: 1px solid #D0D0D0;
            border-radius: 4px;
            padding: 3px 6px;
        }

        QScrollBar:vertical {
            background: #F5F5F5;
            width: 12px;
        }
        QScrollBar::handle:vertical {
            background: #C1C1C1;
            min-height: 20px;
            border-radius: 3px;
            margin: 2px;
        }
        QScrollBar::handle:vertical:hover {
            background: #999999;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }

        QScrollBar:horizontal {
            background: #F5F5F5;
            height: 12px;
        }
        QScrollBar::handle:horizontal {
            background: #C1C1C1;
            min-width: 20px;
            border-radius: 3px;
            margin: 2px;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0;
        }

        QLabel {
            color: #333333;
        }

        QSpinBox, QFontComboBox, QComboBox {
            background-color: #FFFFFF;
            color: #333333;
            border: 1px solid #D0D0D0;
            border-radius: 4px;
            padding: 3px 6px;
        }

        QGroupBox {
            color: #333333;
            border: 1px solid #D0D0D0;
            border-radius: 4px;
            margin-top: 8px;
            padding-top: 16px;
        }

        QCheckBox {
            color: #333333;
        }

        QHeaderView::section {
            background-color: #F3F3F3;
            color: #333333;
            border: 1px solid #D0D0D0;
            padding: 4px;
        }
    )";
}



} // namespace Styles

#endif // STYLES_H