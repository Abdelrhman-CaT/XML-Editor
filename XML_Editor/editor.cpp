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
#include "QTextEdit"

//---------------------------------------------------------------------------------------------------------------------------
// Initialization
Editor::Editor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Editor)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->textEdit);
    ui->textEdit->setStyleSheet("font: 11pt 'Consolas';");
    ui->actionUndo->setDisabled(true);
    ui->actionRedo->setDisabled(true);
}

Editor::~Editor()
{
    delete ui;
}
//---------------------------------------------------------------------------------------------------------------------------





//---------------------------------------------------------------------------------------------------------------------------
// New, Open, Save, Save As
void Editor::on_actionNew_triggered()
{
    fpath = "";
    ui->textEdit->setText("");
    ui->statusbar->showMessage("");
    lines.erase(lines.begin(), lines.end());
    //qDebug() << lines.size();
}

void Editor::on_actionOpen_triggered()
{
    ui->statusbar->showMessage("");
    ui->textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
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
        ui->statusbar->showMessage("Done!");
        //lines = create_xml_vector(file_text);

        /*
        for(int i=0; i<lines.length(); i++){
            qDebug() << lines[i];
        }
        */
    }
}

void Editor::on_actionSave_triggered()
{
    ui->statusbar->showMessage("");
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
            ui->statusbar->showMessage("Done!");
        }
    }
}

void Editor::on_actionSave_As_triggered()
{
    ui->statusbar->showMessage("");
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
        ui->statusbar->showMessage("Done!");
    }
}
//---------------------------------------------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------------------------------------
// Cut, Copy, Paste, Undo, Redo
void Editor::on_actionCut_triggered()
{
    ui->statusbar->showMessage("");
    ui->textEdit->cut();
    ui->statusbar->showMessage("Done!");
}


void Editor::on_actionCopy_triggered()
{
    ui->statusbar->showMessage("");
    ui->textEdit->copy();
    ui->statusbar->showMessage("Done!");
}

void Editor::on_actionPaste_triggered()
{
    ui->statusbar->showMessage("");
    ui->textEdit->paste();
    ui->statusbar->showMessage("Done!");
}


void Editor::on_actionUndo_triggered()
{
    ui->statusbar->showMessage("");
    ui->textEdit->undo();
    ui->statusbar->showMessage("Done!");
}


void Editor::on_textEdit_undoAvailable(bool b)
{
    ui->actionUndo->setDisabled(!b);
}


void Editor::on_actionRedo_triggered()
{
    ui->statusbar->showMessage("");
    ui->textEdit->redo();
    ui->statusbar->showMessage("Done!");
}


void Editor::on_textEdit_redoAvailable(bool b)
{
    ui->actionRedo->setDisabled(!b);
}
//---------------------------------------------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------------------------------------
// What happens when a change in the content of the textEdit happens?
void Editor::on_textEdit_textChanged()
{
    /*
    qint32 num_of_lines = 0;
    QString text = ui->textEdit->toPlainText();
    for(int i=0; i<text.length(); i++){
        if(text[i] == '\n'){
            num_of_lines += 1;
        }
    }
    ui->statusbar->showMessage(QString::number(num_of_lines+1)+" Lines.");
    */
    if(lines.size() > 0){
        lines.erase(lines.begin(), lines.end());
    }
    ui->statusbar->showMessage("");
}
//---------------------------------------------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------------------------------------
// create XML vector out of XML file
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
            while(i < in.length()){
                if(in[i] != '>'){
                    temp += in[i];
                }
                else{
                    break;
                }
                i++;
            }
            temp += '>';
            temp = remove_pre_post_spaces(temp);
            i++;
            res.push_back(temp);
            temp = "";
        }
        else{
            while(i < in.length()) {
                if(in[i] != '<'){
                    temp += in[i];
                }
                else{
                    break;
                }
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
//---------------------------------------------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------------------------------------
// Font Color, Dark Mode
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
//---------------------------------------------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------------------------------------
// Minify XML - XML Minification
void Editor::on_actionMinify_triggered()
{
    //qDebug() << lines.size();
    ui->statusbar->showMessage("");
    ui->textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    QString out = "";
    if(lines.empty()){
        QString in = ui->textEdit->toPlainText();
        if(in != ""){
            lines = create_xml_vector(in);
            /*
            for(int i=0; i<lines.length(); i++){
               qDebug() << lines[i];
            }
            */
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
    ui->statusbar->showMessage("Done!");
}
//---------------------------------------------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------------------------------------
// XML Consistency Check. Returns a boolean value whether the file is XML-consistent or not
bool check_consistency(QVector<QString> xml){
    QStack<QString> tag;
        for (int i = 0; i < xml.size(); i++) {
            QString line = xml[i];
            if (line[0] == '<') {
                if (line[1] == '/') {
                    if(!tag.empty()){
                        if (line != ("</" + tag.top())) {
                            // unmatched closing and opening tags
                            return false;
                        }
                        else {
                            tag.pop();
                        }
                    }
                    else{
                        // closing tag without opening tag
                        return false;
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
    return (tag.empty());
}
//---------------------------------------------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------------------------------------
// Prettify XML - XML Prettification - Beauitfy XML - XML Beautification
// Dependant on check_consistency() boolean return
// Current indentation: Five Spaces
QString remove_one_indentation(QString str, QString indent){
    QString temp = "";
    qint32 indLen = indent.length();
    for(int i=0; i<str.length()-indLen;i++){
        temp += str[i];
    }
    return temp;
}

void Editor::on_actionPrettify_XML_triggered()
{
    QString indent_char = "     ";
    ui->statusbar->showMessage("");
    ui->textEdit->setLineWrapMode(QTextEdit::NoWrap);
    if(lines.size() == 0){
        lines = create_xml_vector(ui->textEdit->toPlainText());
    }
    if(check_consistency(lines)){
        QString indent = "";
        QString final = "";
        for(int i=0; i<lines.size(); i++){
           QString line = lines[i];
           if(line[0] == '<'){
               if(line[1] == '/'){
                    indent = remove_one_indentation(indent, indent_char);
                    final += indent+line;
                    final += "\n";
               }
               else if(line[1] != '!' && line[1] != '?'){
                   if(!(line[line.length()-2] == '/' && line[line.length()-1] == '>')){
                       final += indent+line;
                       final += "\n";
                       indent += indent_char;
                   }
                   else{
                       final += indent+line;
                       final += "\n";
                   }
               }
               else{
                   final += indent+line;
                   final += "\n";
               }
           }
           else{
               final += indent+line;
               final += "\n";
           }
        }
        ui->textEdit->setText(final);
        ui->statusbar->showMessage("Done!");
    }
    else{
        QMessageBox::warning(this, "Warning", "Cannot Prettify an Unconsistent XML file!");
        return;
    }
}
//---------------------------------------------------------------------------------------------------------------------------
// NOT COMPLETED, you must show the errors and fix them
void Editor::on_actionCheck_XML_Consistency_triggered()
{
    ui->statusbar->showMessage("");
    ui->textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
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
        ui->statusbar->showMessage("Done!");
    }
    else{
        QMessageBox::warning(this, "Warning", "This XML file is NOT consistent");
        return;
    }
}
//---------------------------------------------------------------------------------------------------------------------------
