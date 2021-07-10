#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionNew_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionSave_As_triggered();

    void on_actionCut_triggered();

    void on_actionCopy_triggered();

    void on_actionPaste_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionFont_Preferences_triggered();

    void on_actionDark_Light_Mode_triggered();

    void on_actionFont_Color_Preferences_triggered();

private:
    Ui::MainWindow *ui;
    qint8 ModeBit = 0;   // 0 --> lightMode  1 --> darkMode
    QString fpath = "";
};
#endif // MAINWINDOW_H
