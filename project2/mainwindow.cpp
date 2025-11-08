//mainwindow.cpp
#include "mainwindow.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFontDialog>
#include <QColorDialog>
#include <QCloseEvent>
#include <QSettings>
#include <QTextCursor>
#include <QTextBlock>
#include <QPainter>
#include <QTextBlock>
#include <QScrollBar>
#include <QInputDialog>
#include <QDir>


// TextEditor 实现
TextEditor::TextEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    setViewportMargins(lineNumberAreaWidth(), 5, 5, 5);

    connect(this, &QPlainTextEdit::blockCountChanged, this, [this]() {
        updateLineNumberAreaWidth(0);
    });
    connect(this, &QPlainTextEdit::updateRequest, this, &TextEditor::updateLineNumberArea);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &TextEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

int TextEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void TextEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void TextEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void TextEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void TextEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(96, 96, 96); // 深灰色

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void TextEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::darkGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::white);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

// MainWindow 实现
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), textEdit(new TextEditor)
{
    setCentralWidget(textEdit);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    readSettings();

    connect(textEdit->document(), &QTextDocument::contentsChanged,
            this, &MainWindow::documentWasModified);
    connect(textEdit, &QPlainTextEdit::cursorPositionChanged,
            this, &MainWindow::updateStatusBar);
    connect(textEdit, &QPlainTextEdit::textChanged,
            this, &MainWindow::updateStatusBar);

    setCurrentFile(QString());
    setUnifiedTitleAndToolBarOnMac(true);
}


MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newFile()
{
    if (maybeSave()) {
        textEdit->clear();
        setCurrentFile(QString());
    }
}

void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool MainWindow::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted)
        return false;
    return saveFile(dialog.selectedFiles().first());
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("关于文本编辑器"),
                       tr("<h2>文本编辑器</h2>"
                          "<p>开发者：谢静蕾</p>"
                          "<p>班级：23软卓1班</p>"
                          "<p>学号：2023414300117</p>"
                          "<p>邮箱：shlzvmry0717@gmail.com</p>"
                          "<p>这是一个基于Qt开发的文本编辑器，支持多窗口、"
                          "语法高亮、行号显示等功能。</p>"));
}

void MainWindow::documentWasModified()
{
    setWindowModified(textEdit->document()->isModified());
}

void MainWindow::createActions()
{
    // 文件菜单动作
    newAct = new QAction(QIcon(":/icons/document-new.png"), tr("新建"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("创建一个新文件"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);

    openAct = new QAction(QIcon(":/icons/document-open.png"), tr("打开"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("打开一个已存在的文件"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);

    saveAct = new QAction(QIcon(":/icons/document-save.png"), tr("保存"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("保存当前文件"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);

    saveAsAct = new QAction(QIcon(":/icons/document-save-as.png"), tr("另存为"), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("将当前文件另存为"));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);

    exitAct = new QAction(tr("退出(&X)"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("退出应用程序"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    // 编辑菜单动作
    undoAct = new QAction(QIcon(":/icons/edit-undo.png"), tr("撤销"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("撤销上一步操作"));
    connect(undoAct, &QAction::triggered, textEdit, &TextEditor::undo);

    redoAct = new QAction(QIcon(":/icons/edit-redo.png"), tr("重做"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("恢复上一步操作"));
    connect(redoAct, &QAction::triggered, textEdit, &TextEditor::redo);

    cutAct = new QAction(QIcon(":/icons/edit-cut.png"), tr("剪切"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("剪切选中内容到剪贴板"));
    connect(cutAct, &QAction::triggered, textEdit, &TextEditor::cut);

    copyAct = new QAction(QIcon(":/icons/edit-copy.png"), tr("复制"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("复制选中内容到剪贴板"));
    connect(copyAct, &QAction::triggered, textEdit, &TextEditor::copy);

    pasteAct = new QAction(QIcon(":/icons/edit-paste.png"), tr("粘贴"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("粘贴剪贴板内容"));
    connect(pasteAct, &QAction::triggered, textEdit, &TextEditor::paste);

    findAct = new QAction(QIcon(":/icons/edit-find.png"), tr("查找"), this);
    findAct->setShortcuts(QKeySequence::Find);
    findAct->setStatusTip(tr("在文档中查找文本"));
    connect(findAct, &QAction::triggered, this, &MainWindow::find);

    replaceAct = new QAction(QIcon(":/icons/edit-find-replace.png"), tr("替换"), this);
    replaceAct->setShortcuts(QKeySequence::Replace);
    replaceAct->setStatusTip(tr("在文档中查找并替换文本"));
    connect(replaceAct, &QAction::triggered, this, &MainWindow::replace);

    selectAllAct = new QAction(tr("全选"), this);
    selectAllAct->setShortcuts(QKeySequence::SelectAll);
    selectAllAct->setStatusTip(tr("选择所有文本"));
    connect(selectAllAct, &QAction::triggered, textEdit, &TextEditor::selectAll);

    // 格式菜单动作
    wordWrapAct = new QAction(QIcon(":/icons/format-justify-fill.png"), tr("换行"), this);
    wordWrapAct->setCheckable(true);
    wordWrapAct->setChecked(true);
    wordWrapAct->setStatusTip(tr("启用或禁用自动换行"));
    connect(wordWrapAct, &QAction::triggered, this, &MainWindow::toggleWordWrap);

    lineNumbersAct = new QAction(QIcon(":/icons/view-line-numbers.png"), tr("行号"), this);
    lineNumbersAct->setCheckable(true);
    lineNumbersAct->setChecked(true);
    lineNumbersAct->setStatusTip(tr("显示或隐藏行号"));
    connect(lineNumbersAct, &QAction::triggered, this, &MainWindow::toggleLineNumbers);

    fontAct = new QAction(QIcon(":/icons/format-font.png"), tr("字体"), this);
    fontAct->setStatusTip(tr("设置字体"));
    connect(fontAct, &QAction::triggered, this, &MainWindow::setFont);

    textColorAct = new QAction(QIcon(":/icons/format-text-color.png"), tr("颜色"), this);
    textColorAct->setStatusTip(tr("设置字体颜色"));
    connect(textColorAct, &QAction::triggered, this, &MainWindow::setTextColor);

    textBgColorAct = new QAction(tr("字体背景色"), this);
    textBgColorAct->setStatusTip(tr("设置字体背景颜色"));
    connect(textBgColorAct, &QAction::triggered, this, &MainWindow::setTextBackgroundColor);

    editorBgColorAct = new QAction(tr("编辑器背景色"), this);
    editorBgColorAct->setStatusTip(tr("设置编辑器背景颜色"));
    connect(editorBgColorAct, &QAction::triggered, this, &MainWindow::setEditorBackgroundColor);

    // 查看菜单动作
    toolBarAct = new QAction(tr("工具栏(&T)"), this);
    toolBarAct->setCheckable(true);
    toolBarAct->setChecked(true);
    toolBarAct->setStatusTip(tr("显示或隐藏工具栏"));
    connect(toolBarAct, &QAction::triggered, this, &MainWindow::toggleToolBar);

    statusBarAct = new QAction(tr("状态栏(&S)"), this);
    statusBarAct->setCheckable(true);
    statusBarAct->setChecked(true);
    statusBarAct->setStatusTip(tr("显示或隐藏状态栏"));
    connect(statusBarAct, &QAction::triggered, this, &MainWindow::toggleStatusBar);

    // 帮助菜单动作
    aboutAct = new QAction(tr("关于(&A)"), this);
    aboutAct->setStatusTip(tr("显示关于对话框"));
    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);

    // 初始使能状态
    cutAct->setEnabled(false);
    copyAct->setEnabled(false);

    connect(textEdit, &TextEditor::copyAvailable, cutAct, &QAction::setEnabled);
    connect(textEdit, &TextEditor::copyAvailable, copyAct, &QAction::setEnabled);
}

void MainWindow::createMenus()
{
    // 文件菜单
    fileMenu = menuBar()->addMenu(tr("文件(&F)"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    // 编辑菜单
    editMenu = menuBar()->addMenu(tr("编辑(&E)"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addSeparator();
    editMenu->addAction(findAct);
    editMenu->addAction(replaceAct);
    editMenu->addSeparator();
    editMenu->addAction(selectAllAct);

    // 格式菜单
    formatMenu = menuBar()->addMenu(tr("格式(&O)"));
    formatMenu->addAction(wordWrapAct);
    formatMenu->addAction(lineNumbersAct);
    formatMenu->addAction(fontAct);
    formatMenu->addAction(textColorAct);
    formatMenu->addSeparator();
    formatMenu->addAction(textBgColorAct);
    formatMenu->addAction(editorBgColorAct);

    // 查看菜单
    viewMenu = menuBar()->addMenu(tr("查看(&V)"));
    viewMenu->addAction(toolBarAct);
    viewMenu->addAction(statusBarAct);

    // 帮助菜单
    helpMenu = menuBar()->addMenu(tr("帮助(&H)"));
    helpMenu->addAction(aboutAct);
}

void MainWindow::createToolBars()
{
    // 设置统一的图标大小
    QSize iconSize(32, 32);

    // 文件工具栏
    fileToolBar = addToolBar(tr("文件"));
    fileToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    fileToolBar->setIconSize(iconSize);
    fileToolBar->setMinimumHeight(60);
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->addAction(saveAsAct);

    // 编辑工具栏
    editToolBar = addToolBar(tr("编辑"));
    editToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    editToolBar->setIconSize(iconSize);
    editToolBar->setMinimumHeight(60);
    editToolBar->addAction(undoAct);
    editToolBar->addAction(redoAct);
    editToolBar->addSeparator();
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
    editToolBar->addSeparator();
    editToolBar->addAction(findAct);
    editToolBar->addAction(replaceAct);

    // 格式工具栏
    formatToolBar = addToolBar(tr("格式"));
    formatToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    formatToolBar->setIconSize(iconSize);
    formatToolBar->setMinimumHeight(60);
    formatToolBar->addAction(fontAct);
    formatToolBar->addAction(textColorAct);
    formatToolBar->addAction(lineNumbersAct);
    formatToolBar->addAction(wordWrapAct);
}

void MainWindow::createStatusBar()
{
    statusLengthLabel = new QLabel;
    statusLineLabel = new QLabel;
    statusLnLabel = new QLabel;
    statusColLabel = new QLabel;
    statusInfoLabel = new QLabel("2023414300117谢静蕾");

    statusBar()->addPermanentWidget(statusLengthLabel);
    statusBar()->addPermanentWidget(statusLineLabel);
    statusBar()->addPermanentWidget(statusLnLabel);
    statusBar()->addPermanentWidget(statusColLabel);
    statusBar()->addPermanentWidget(statusInfoLabel);

    updateStatusBar();
}

void MainWindow::readSettings()
{
    QSettings settings("MyCompany", "TextEditor");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(800, 600)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::writeSettings()
{
    QSettings settings("MyCompany", "TextEditor");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

bool MainWindow::maybeSave()
{
    if (!textEdit->document()->isModified())
        return true;

    const QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("应用程序"),
                                                                 tr("文档已被修改。\n"
                                                                    "是否保存更改？"),
                                                                 QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("应用程序"),
                             tr("无法读取文件 %1:\n%2.")
                                 .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream in(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    in.setCodec("UTF-8");
#else
    in.setEncoding(QStringConverter::Utf8);
#endif

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    textEdit->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("文件已加载"), 2000);
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("应用程序"),
                             tr("无法写入文件 %1:\n%2.")
                                 .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return false;
    }

    QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
#else
    out.setEncoding(QStringConverter::Utf8);
#endif

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << textEdit->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("文件已保存"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    textEdit->document()->setModified(false);
    setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "* 新建文本文件";

    setWindowTitle(tr("📝 %1 - 编辑器").arg(strippedName(shownName)));
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::updateStatusBar()
{
    QString text = textEdit->toPlainText();
    int length = text.length();
    int lineCount = textEdit->document()->blockCount();

    QTextCursor cursor = textEdit->textCursor();
    int line = cursor.blockNumber() + 1;
    int column = cursor.positionInBlock() + 1;

    statusLengthLabel->setText(tr("length: %1").arg(length));
    statusLineLabel->setText(tr("line: %1").arg(lineCount));
    statusLnLabel->setText(tr("Ln: %1").arg(line));
    statusColLabel->setText(tr("Col: %1").arg(column));
}

// 编辑功能实现
void MainWindow::undo()
{
    textEdit->undo();
}

void MainWindow::redo()
{
    textEdit->redo();
}

void MainWindow::cut()
{
    textEdit->cut();
}

void MainWindow::copy()
{
    textEdit->copy();
}

void MainWindow::paste()
{
    textEdit->paste();
}

void MainWindow::selectAll()
{
    textEdit->selectAll();
}

// 格式功能实现
void MainWindow::toggleWordWrap()
{
    if (wordWrapAct->isChecked()) {
        textEdit->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    } else {
        textEdit->setWordWrapMode(QTextOption::NoWrap);
    }
}

void MainWindow::toggleLineNumbers()
{
    textEdit->getLineNumberArea()->setVisible(lineNumbersAct->isChecked());
    textEdit->updateLineNumberAreaWidth();
}


void MainWindow::setFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, textEdit->font(), this);
    if (ok) {
        textEdit->setFont(font);
    }
}

void MainWindow::setTextColor()
{
    QColor color = QColorDialog::getColor(Qt::black, this);
    if (color.isValid()) {
        QTextCharFormat format;
        format.setForeground(color);
        QTextCursor cursor = textEdit->textCursor();
        if (!cursor.hasSelection())
            cursor.select(QTextCursor::WordUnderCursor);
        cursor.mergeCharFormat(format);
    }
}

void MainWindow::setTextBackgroundColor()
{
    QColor color = QColorDialog::getColor(Qt::white, this);
    if (color.isValid()) {
        QTextCharFormat format;
        format.setBackground(color);
        QTextCursor cursor = textEdit->textCursor();
        if (!cursor.hasSelection())
            cursor.select(QTextCursor::WordUnderCursor);
        cursor.mergeCharFormat(format);
    }
}

void MainWindow::setEditorBackgroundColor()
{
    QColor color = QColorDialog::getColor(Qt::white, this);
    if (color.isValid()) {
        QPalette p = textEdit->palette();
        p.setColor(QPalette::Base, color);
        textEdit->setPalette(p);
    }
}

// 查看功能实现
void MainWindow::toggleToolBar()
{
    fileToolBar->setVisible(toolBarAct->isChecked());
    editToolBar->setVisible(toolBarAct->isChecked());
    formatToolBar->setVisible(toolBarAct->isChecked());
}

void MainWindow::toggleStatusBar()
{
    statusBar()->setVisible(statusBarAct->isChecked());
}

void MainWindow::find()
{
    FindDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString findText = dialog.getFindText();
        if (!findText.isEmpty()) {
            QTextDocument::FindFlags flags;
            if (!dialog.searchForward()) {
                flags |= QTextDocument::FindBackward;
            }
            if (dialog.isCaseSensitive()) {
                flags |= QTextDocument::FindCaseSensitively;
            }

            if (!textEdit->find(findText, flags)) {
                QMessageBox::information(this, tr("查找"), tr("找不到 \"%1\"").arg(findText));
            }
        }
    }
}

void MainWindow::replace()
{
    ReplaceDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString findText = dialog.getFindText();
        QString replaceText = dialog.getReplaceText();

        if (!findText.isEmpty()) {
            if (dialog.replaceAll()) {
                // 全部替换
                QString text = textEdit->toPlainText();
                Qt::CaseSensitivity sensitivity = dialog.isCaseSensitive() ? Qt::CaseSensitive : Qt::CaseInsensitive;
                text.replace(findText, replaceText, sensitivity);
                textEdit->setPlainText(text);
            } else {
                // 替换当前
                QTextDocument::FindFlags flags;
                if (dialog.isCaseSensitive()) {
                    flags |= QTextDocument::FindCaseSensitively;
                }

                if (textEdit->textCursor().hasSelection() &&
                    textEdit->textCursor().selectedText() == findText) {
                    textEdit->textCursor().insertText(replaceText);
                }

                if (!textEdit->find(findText, flags)) {
                    QMessageBox::information(this, tr("替换"), tr("找不到 \"%1\"").arg(findText));
                }
            }
        }
    }
}

// ==================== 对话框类实现 ====================

FindDialog::FindDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("查找"));
    setFixedSize(350, 200);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);

    // 查找内容
    QHBoxLayout *findLayout = new QHBoxLayout();
    findLayout->addWidget(new QLabel(tr("查找内容:")));
    findEdit = new QLineEdit(this);
    findLayout->addWidget(findEdit);
    mainLayout->addLayout(findLayout);

    // 选项区域
    QGroupBox *optionsGroup = new QGroupBox(tr("选项"));
    QVBoxLayout *optionsLayout = new QVBoxLayout(optionsGroup);
    optionsLayout->setSpacing(8);

    // 区分大小写
    caseCheckBox = new QCheckBox(tr("区分大小写"));
    optionsLayout->addWidget(caseCheckBox);

    // 搜索方向
    QGroupBox *directionGroup = new QGroupBox(tr("搜索方向"));
    QHBoxLayout *directionLayout = new QHBoxLayout(directionGroup);
    directionLayout->setSpacing(15);
    forwardRadio = new QRadioButton(tr("向下搜索"));
    backwardRadio = new QRadioButton(tr("向上搜索"));
    forwardRadio->setChecked(true);
    directionLayout->addWidget(forwardRadio);
    directionLayout->addWidget(backwardRadio);
    directionLayout->addStretch();

    optionsLayout->addWidget(directionGroup);
    mainLayout->addWidget(optionsGroup);

    // 按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    findButton = new QPushButton(tr("查找下一个"));
    findButton->setDefault(true);
    findButton->setMinimumWidth(80);
    cancelButton = new QPushButton(tr("取消"));
    cancelButton->setMinimumWidth(80);
    buttonLayout->addWidget(findButton);
    buttonLayout->addSpacing(10);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    // 设置焦点
    findEdit->setFocus();

    connect(findButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // 回车键查找
    connect(findEdit, &QLineEdit::returnPressed, this, &QDialog::accept);
}

// ReplaceDialog 构造函数
ReplaceDialog::ReplaceDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("替换"));
    setFixedSize(400, 280);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);

    // 查找内容
    QHBoxLayout *findLayout = new QHBoxLayout();
    findLayout->addWidget(new QLabel(tr("查找内容:")));
    findEdit = new QLineEdit(this);
    findLayout->addWidget(findEdit);
    mainLayout->addLayout(findLayout);

    // 替换为
    QHBoxLayout *replaceLayout = new QHBoxLayout();
    replaceLayout->addWidget(new QLabel(tr("替换为:")));
    replaceEdit = new QLineEdit(this);
    replaceLayout->addWidget(replaceEdit);
    mainLayout->addLayout(replaceLayout);

    mainLayout->addSpacing(5);

    // 选项区域
    QGroupBox *optionsGroup = new QGroupBox(tr("选项"));
    QVBoxLayout *optionsLayout = new QVBoxLayout(optionsGroup);
    optionsLayout->setSpacing(10);

    // 区分大小写
    caseCheckBox = new QCheckBox(tr("区分大小写"));
    optionsLayout->addWidget(caseCheckBox);

    // 替换方式
    QGroupBox *replaceGroup = new QGroupBox(tr("替换方式"));
    QVBoxLayout *replaceLayout2 = new QVBoxLayout(replaceGroup);
    replaceLayout2->setSpacing(8);
    replaceOneRadio = new QRadioButton(tr("替换当前选中内容"));
    replaceAllRadio = new QRadioButton(tr("全部替换"));
    replaceOneRadio->setChecked(true);
    replaceLayout2->addWidget(replaceOneRadio);
    replaceLayout2->addWidget(replaceAllRadio);

    optionsLayout->addWidget(replaceGroup);
    mainLayout->addWidget(optionsGroup);

    mainLayout->addSpacing(5);

    // 按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    replaceButton = new QPushButton(tr("替换"));
    replaceButton->setDefault(true);
    replaceButton->setMinimumWidth(80);
    cancelButton = new QPushButton(tr("取消"));
    cancelButton->setMinimumWidth(80);
    buttonLayout->addWidget(replaceButton);
    buttonLayout->addSpacing(10);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    // 设置焦点
    findEdit->setFocus();

    connect(replaceButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // 回车键替换
    connect(findEdit, &QLineEdit::returnPressed, this, &QDialog::accept);
    connect(replaceEdit, &QLineEdit::returnPressed, this, &QDialog::accept);
}
