#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QLabel>
#include <QCloseEvent>
#include <QDialog>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>

class LineNumberArea;

class TextEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    TextEditor(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    QWidget* getLineNumberArea() { return lineNumberArea; }
    void updateLineNumberAreaWidth() { updateLineNumberAreaWidth(0); }

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *lineNumberArea;

    friend class MainWindow;
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(TextEditor *editor) : QWidget(editor), textEditor(editor) {}

    QSize sizeHint() const override {
        return QSize(textEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        textEditor->lineNumberAreaPaintEvent(event);
    }

private:
    TextEditor *textEditor;
};

class FindDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FindDialog(QWidget *parent = nullptr);
    QString getFindText() const { return findEdit->text(); }
    bool isCaseSensitive() const { return caseCheckBox->isChecked(); }
    bool searchForward() const { return forwardRadio->isChecked(); }

private:
    QLineEdit *findEdit;
    QCheckBox *caseCheckBox;
    QRadioButton *forwardRadio;
    QRadioButton *backwardRadio;
    QPushButton *findButton;
    QPushButton *cancelButton;
};

class ReplaceDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ReplaceDialog(QWidget *parent = nullptr);
    QString getFindText() const { return findEdit->text(); }
    QString getReplaceText() const { return replaceEdit->text(); }
    bool isCaseSensitive() const { return caseCheckBox->isChecked(); }
    bool replaceAll() const { return replaceAllRadio->isChecked(); }

private:
    QLineEdit *findEdit;
    QLineEdit *replaceEdit;
    QCheckBox *caseCheckBox;
    QRadioButton *replaceOneRadio;
    QRadioButton *replaceAllRadio;
    QPushButton *replaceButton;
    QPushButton *cancelButton;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void documentWasModified();
    void about();
    void updateStatusBar();

    // 编辑功能
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();

    // 格式功能
    void toggleWordWrap();
    void toggleLineNumbers();
    void setFont();
    void setTextColor();
    void setTextBackgroundColor();
    void setEditorBackgroundColor();

    // 查看功能
    void toggleToolBar();
    void toggleStatusBar();

    // 查找替换
    void find();
    void replace();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    TextEditor *textEdit;
    QString curFile;

    // 菜单
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *formatMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

    // 工具栏
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *formatToolBar;

    // 状态栏
    QLabel *statusLengthLabel;
    QLabel *statusLineLabel;
    QLabel *statusLnLabel;
    QLabel *statusColLabel;
    QLabel *statusInfoLabel;

    // 动作
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;

    QAction *undoAct;
    QAction *redoAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *findAct;
    QAction *replaceAct;
    QAction *selectAllAct;

    QAction *wordWrapAct;
    QAction *lineNumbersAct;
    QAction *fontAct;
    QAction *textColorAct;
    QAction *textBgColorAct;
    QAction *editorBgColorAct;

    QAction *toolBarAct;
    QAction *statusBarAct;

    QAction *aboutAct;
};

#endif
