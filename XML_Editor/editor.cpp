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
#include "QStack"

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
    lines.erase(lines.begin(), lines.end());
    //qDebug() << lines.size();
}

QString remove_pre_post_spaces(QString line){
    qint32 j = 0;
    while(line[j] == ' ' || line[j] == '\t' || line[j] == '\n'){
        j += 1;
    }
    qint32 k = line.length() - 1;
    while(line[k] == ' ' || line[k] == '\t' || line[k] == '\n') {
        k -= 1;
    }
    QString line_without_pre_or_post_spaces = "";
    for(int q = j ; q <= k; q++){
        line_without_pre_or_post_spaces += line[q];
    }
    return line_without_pre_or_post_spaces;
}

QVector<QString> create_xml_vector(QString in){
    QVector<QString> res;
    QString temp = "";
    int i = 0;
    while(i < in.length()){
        if(in[i] == '<'){
            while(in[i] != '>' && i < in.length()){
                temp += in[i];
                i++;
            }
            temp += '>';
            temp = remove_pre_post_spaces(temp);
            i++;
            res.push_back(temp);
            temp = "";
        }
        else{
            while(in[i] != '<' && i < in.length()) {
                temp += in[i];
                i++;
            }
            bool add = false;
            for(int j=0; j<temp.length(); j++){
                if(temp[j] != ' ' && temp[j] != '\n' && temp[j] != '\t'){
                    add = true;
                    break;
                }
            }
            if(add == true){
                temp = remove_pre_post_spaces(temp);
                res.push_back(temp);
            }
            temp = "";
        }
    }
    return res;
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
        if(!lines.empty()){
            lines.erase(lines.begin(), lines.end());
        }
        file_text = in.readAll();
        ui->textEdit->setText(file_text);
        file.close();  
        lines = create_xml_vector(file_text);

        /*
        for(int i=0; i<lines.length(); i++){
            qDebug() << lines[i];
        }
        */
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
    //qDebug() << lines.size();
    QString out = "";
    if(lines.empty()){
        QString in = ui->textEdit->toPlainText();
        if(in != ""){
            lines = create_xml_vector(in);
            for(int i=0; i<lines.size(); i++){
                if(!(lines[i][0] == '<' && lines[i][1] == '!')){
                    out += lines[i];
                }
            }
        }
        else{
            QMessageBox::warning(this, "Warning", "No Text To Be Minified!");
        }
    }
    else{
        for(int i=0; i<lines.size(); i++){
            if(!(lines[i][0] == '<' && lines[i][1] == '!')){
                out += lines[i];
            }
        }
    }
    ui->textEdit->setText(out);
}


bool check_consistency(QVector<QString> xml){
    QStack<QString> tag;
        for (int i = 0; i < xml.size(); i++) {
            QString line = xml[i];
            if (line[0] == '<') {
                if (line[1] == '/') {
                    if (line != ("</" + tag.top())) {
                        return false;
                    }
                    else {
                        tag.pop();
                    }
                }
                else if(line[1] != '!' && line[1] != '?') {
                    if(!(line[line.length()-2] == '/' && line[line.length()-1] == '>')){
                        QString temp = "";
                        for(int j=1; j<line.length(); j++){
                            if(line[j] != ' ' && line[j] != '>'){
                             temp += line[j];
                            }
                            else{
                                temp += '>';
                                break;
                            }
                        }
                        tag.push(temp);
                    }
                }
            }
        }
    return tag.empty();
}



void Editor::on_actionCheck_XML_Consistency_triggered()
{
    bool message;
    if(lines.empty()){
        QString in = ui->textEdit->toPlainText();
        if(in != ""){
            lines = create_xml_vector(in);
            // check for consistency in lines_typed
            /*
             for(int i=0; i<lines.length(); i++){
                qDebug() << lines[i];
             }
            */
            message = check_consistency(lines);
        }
        else{
            QMessageBox::warning(this, "Warning", "No Text To Be Checked!");
            return;
        }
    }
    else{
        // check for consistency through lines
        /*
         for(int i=0; i<lines.length(); i++){
            qDebug() << lines[i];
         }
        */
        message = check_consistency(lines);
    }
    if(message == true){
        QMessageBox::information(this, "Info", "This XML file is consistent");
    }
    else{
        QMessageBox::warning(this, "Warning", "This XML file is NOT consistent");
    }
}

