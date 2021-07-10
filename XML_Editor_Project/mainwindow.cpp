#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFile"
#include "QFileDialog"
#include "QDir"
#include "QTextStream"
#include "QMessageBox"
#include "QFont"
#include "QFontDialog"
#include "QColor"
#include "QColorDialog"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->textEdit);
    ui->textEdit->setStyleSheet("font: 15pt 'Consolas';");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionNew_triggered()
{
    fpath = "";
    ui->textEdit->setText("");
}


void MainWindow::on_actionOpen_triggered()
{
    QString filter = "All Files *.* ;; XML *.xml ;; JSON *.json";
    QString filename = QFileDialog::getOpenFileName(this, "Open a File", QDir::currentPath(), filter);
    fpath = filename;
    QFile file(filename);
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot Open File!");
        return;
    }
    else{
        QTextStream in(&file);
        QString file_text = in.readAll();
        ui->textEdit->setText(file_text);
        file.close();
    }
}


void MainWindow::on_actionSave_triggered()
{
    QString filename = fpath;
    if(filename == ""){
        on_actionSave_As_triggered();
    }
    else{
        QFile file(filename);
        if(!file.open(QFile::WriteOnly | QFile::Text)){
            QMessageBox::warning(this, "Warning", "Cannot Save File!");
            return;
        }
        else{
            QTextStream out(&file);
            QString file_text = ui->textEdit->toPlainText();
            out << file_text;
            file.flush();
            file.close();
        }
    }
}


void MainWindow::on_actionSave_As_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, "Open a File", QDir::currentPath());
    fpath = filename;
    QFile file(filename);
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot Save File!");
        return;
    }
    else{
        QTextStream out(&file);
        QString file_text = ui->textEdit->toPlainText();
        out << file_text;
        file.flush();
        file.close();
    }
}


void MainWindow::on_actionCut_triggered()
{
    ui->textEdit->cut();
}


void MainWindow::on_actionCopy_triggered()
{
    ui->textEdit->copy();
}


void MainWindow::on_actionPaste_triggered()
{
    ui->textEdit->paste();
}


void MainWindow::on_actionUndo_triggered()
{
    ui->textEdit->undo();
}


void MainWindow::on_actionRedo_triggered()
{
    ui->textEdit->redo();
}

/*
void MainWindow::on_actionFont_Preferences_triggered()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);
    if(ok){
        ui->textEdit->setFont(font);
    }
    else{
        return;
    }
}
*/

void MainWindow::on_actionDark_Light_Mode_triggered()
{
    if(ModeBit == 0){   // current mode: light. Switch to dark
        ui->textEdit->setStyleSheet("background-color: #404040; font: 15pt 'Consolas'; color: 'white';");
        ModeBit = 1;
    }
    else if(ModeBit == 1){ // current mode: dark. Switch to light
        ui->textEdit->setStyleSheet("background-color: 'white'; font: 15pt 'Consolas'; color: 'black';");
        ModeBit = 0;
    }
}


void MainWindow::on_actionFont_Color_Preferences_triggered()
{
    QColor color = QColorDialog::getColor(Qt::black, this);
    QString color_name = color.name();
    if(ModeBit == 0){
        ui->textEdit->setStyleSheet("background-color: 'white'; font: 15pt 'Consolas'; color: '" + color_name +"';");
    }
    else{
        ui->textEdit->setStyleSheet("background-color: #404040; font: 15pt 'Consolas'; color: '" + color_name +"';");
    }

}

