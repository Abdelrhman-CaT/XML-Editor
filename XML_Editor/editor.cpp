#include "editor.h"
#include "ui_editor.h"
#include "QFile"
#include "QFileDialog"
#include "QDir"
#include "QTextStream"
#include "QMessageBox"
#include "QFont"
#include "QFontDialog"
#include "QColor"
#include "QColorDialog"
#include "QVector"

Editor::Editor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Editor)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->textEdit);
    ui->textEdit->setStyleSheet("font: 11pt 'Consolas';");
}

Editor::~Editor()
{
    delete ui;
}


void Editor::on_actionNew_triggered()
{
    fpath = "";
    ui->textEdit->setText("");
}

void Editor::on_actionOpen_triggered()
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
        QString file_text = "";
        qint32 num_of_lines = 0;
        while(!in.atEnd()){
            num_of_lines +=1;
            QString temp = in.readLine();
            file_text += temp;
            file_text += "\n";
            lines.push_back(temp);
        }
        ui->textEdit->setText(file_text);
        file.close();
        ui->statusbar->showMessage("This File has " + QString::number(num_of_lines) + " Lines", 3000);
        for(int i=0; i<lines.size(); i++){
            QString line = lines[i];
            qint32 index = 0;
            qint32 j = 0;
            while(line[j] == ' '){
                index += 1;
                j += 1;
            }
            lines[i] = line.mid(j);
            //qDebug() << lines[i];
        }
    }
}


void Editor::on_actionSave_triggered()
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


void Editor::on_actionSave_As_triggered()
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


void Editor::on_actionCut_triggered()
{
    ui->textEdit->cut();
}


void Editor::on_actionCopy_triggered()
{
    ui->textEdit->copy();
}

void Editor::on_actionPaste_triggered()
{
    ui->textEdit->paste();
}


void Editor::on_actionUndo_triggered()
{
    ui->textEdit->undo();
}


void Editor::on_actionRedo_triggered()
{
    ui->textEdit->redo();
}


void Editor::on_actionFont_Color_Preferences_triggered()
{
    QColor color = QColorDialog::getColor(Qt::black, this);
    QString color_name = color.name();
    if(ModeBit == 0){
        ui->textEdit->setStyleSheet("background-color: 'white'; font: 11pt 'Consolas'; color: '" + color_name +"';");
    }
    else{
        ui->textEdit->setStyleSheet("background-color: #404040; font: 11pt 'Consolas'; color: '" + color_name +"';");
    }
}


void Editor::on_actionDark_Light_mode_triggered()
{
    if(ModeBit == 0){   // current mode: light. Switch to dark
        ui->textEdit->setStyleSheet("background-color: #404040; font: 11pt 'Consolas'; color: #55ffff;");
        ModeBit = 1;
    }
    else if(ModeBit == 1){ // current mode: dark. Switch to light
        ui->textEdit->setStyleSheet("background-color: 'white'; font: 11pt 'Consolas'; color: 'black';");
        ModeBit = 0;
    }
}


void Editor::on_actionMinify_triggered()
{
    QString out = "";
    for(int i=0; i<lines.size(); i++){
        out += lines[i];
    }
    ui->textEdit->setText(out);
}

