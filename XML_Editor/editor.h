#ifndef EDITOR_H
#define EDITOR_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class Editor; }
QT_END_NAMESPACE

class Editor : public QMainWindow
{
    Q_OBJECT

public:
    Editor(QWidget *parent = nullptr);
    ~Editor();

private slots:
    void on_actionNew_triggered();

    void on_actionPaste_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionSave_As_triggered();

    void on_actionCut_triggered();

    void on_actionCopy_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionFont_Color_Preferences_triggered();

    void on_actionDark_Light_mode_triggered();

    void on_actionMinify_triggered();

    void on_actionCheck_XML_Consistency_triggered();

    void on_textEdit_undoAvailable(bool b);

    void on_textEdit_redoAvailable(bool b);

    void on_textEdit_textChanged();

private:
    Ui::Editor *ui;
    qint8 ModeBit = 0;   // 0 --> lightMode  1 --> darkMode
    QString fpath = "";
    QVector<QString> lines;
};
#endif // EDITOR_H
