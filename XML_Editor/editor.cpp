#include "editor.h"
#include "ui_editor.h"
#include "xml_vector.h"
#include "consistency_check.h"
#include "compression.h"
#include "json.h"
#include "QFile"
#include "QFileDialog"
#include "QDir"
#include "QTextStream"
#include "QMessageBox"
#include "QFont"
#include "QFontDialog"
#include "QColor"
#include "QColorDialog"
#include "QTextEdit"
#include "QDebug"

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
    on_actionDark_Light_mode_triggered(); // To make Dark mode the default mode
    ui->actionShow_Consistency_Errors->setDisabled(true);
    ui->actionFix_Consistency_Errors->setDisabled(true);
    //ui->actionDecompress_File->setVisible(false);
}

Editor::~Editor()
{
    delete ui;
}
//---------------------------------------------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------------------------------------
void Editor::on_actionTeam_triggered()
{
    QString info;
    info = "This is a university project created by:\n-------------------------------------------\n1- Abdelrahman Hany Gamal Bakr\n2- Zeyad Mourad Abd-El Mahfouz Mohamed\n3- Menna Abd-El Hafeez Mohamed Abdelrahman\n4- Munira Nasser Fouad Hassanin";
    QMessageBox::information(this, "Info", info);
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
        ui->statusbar->showMessage("Done!         File size: " + QString::number(file.size()/1024.0) + " KB");
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
    QString filename = QFileDialog::getSaveFileName(this, "Choose the Location to Save the File", QDir::currentPath());
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
    ui->actionShow_Consistency_Errors->setDisabled(true);
    ui->actionFix_Consistency_Errors->setDisabled(true);
}
//---------------------------------------------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------------------------------------
// Font Color, Dark Mode
void Editor::on_actionFont_Color_Preferences_triggered()
{
    QColor default_color;
    if(ModeBit == 0){
        // light
        default_color = Qt::black;
    }
    else{
        // dark
        QColor def(85,255,255,1);
        default_color = def;
    }
    QColor color = QColorDialog::getColor(default_color, this);
    if(color.isValid()){
        QString color_name = color.name();
        if(ModeBit == 0){
            ui->textEdit->setStyleSheet("background-color: 'white'; font: 11pt 'Consolas'; color: '" + color_name +"';");
        }
        else{
            ui->textEdit->setStyleSheet("background-color: #404040; font: 11pt 'Consolas'; color: '" + color_name +"';");
        }
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
    //-----------------------------------------------------------------------------
    QString out = "";
    if(lines.empty()){
        QString in = ui->textEdit->toPlainText();
        if(in != ""){
            QString filename = QFileDialog::getSaveFileName(this, "Choose the Location to Save the Minified File", QDir::currentPath());
            fpath = filename;
            QFile file(filename);
            if(!file.open(QFile::WriteOnly | QFile::Text)){
                QMessageBox::warning(this, "Warning", "Cannot Save File!");
                return;
            }
            lines = create_xml_vector(in);
            for(int i=0; i<lines.size(); i++){
                if(!(lines[i][0] == '<' && lines[i][1] == '!')){
                    out += lines[i];
                }
            }
            QTextStream outt(&file);
            outt << out;
            file.flush();
            file.close();
            QMessageBox::information(this, "Info", "File Minified Successfully!\nThe Minified File Can Be Found at: " + filename);
            QFile f(filename);
            if(!f.open(QFile::ReadOnly | QFile::Text)){
                QMessageBox::warning(this, "Warning", "Cannot Open the Results File!");
                return;
            }
            else{
                QTextStream in(&f);
                QString file_text = "";
                file_text = in.readAll();
                ui->textEdit->setText(file_text);
                f.close();
                ui->statusbar->showMessage("Done!");
            }
        }
        else{
            QMessageBox::warning(this, "Warning", "No Text To Be Minified!");
            return;
        }
    }
    else{
        QString filename = QFileDialog::getSaveFileName(this, "Choose the Location to Save the Minified File", QDir::currentPath());
        fpath = filename;
        QFile file(filename);
        if(!file.open(QFile::WriteOnly | QFile::Text)){
            QMessageBox::warning(this, "Warning", "Cannot Save File!");
            return;
        }
        for(int i=0; i<lines.size(); i++){
            if(!(lines[i][0] == '<' && lines[i][1] == '!')){
                out += lines[i];
            }
        }
        QTextStream outt(&file);
        outt << out;
        file.flush();
        file.close();
        QMessageBox::information(this, "Info", "File Minified Successfully!\nThe Minified File Can Be Found at: " + filename);
        QFile f(filename);
        if(!f.open(QFile::ReadOnly | QFile::Text)){
            QMessageBox::warning(this, "Warning", "Cannot Open the Results File!");
            return;
        }
        else{
            QTextStream in(&f);
            QString file_text = "";
            file_text = in.readAll();
            ui->textEdit->setText(file_text);
            f.close();
            ui->statusbar->showMessage("Done!");
        }
    }
    //ui->textEdit->setText(out);
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
    ui->statusbar->showMessage("");
    //-----------------------------------------------------------------------------
    QString indent_char = "     ";
    if(lines.size() == 0){
        lines = create_xml_vector(ui->textEdit->toPlainText());
    }
    if(lines.size() == 0){
        QMessageBox::warning(this, "Warning", "No text to be prettified!");
        return;
    }
    QString filename = QFileDialog::getSaveFileName(this, "Choose the Location to Save the Prettified File", QDir::currentPath());
    fpath = filename;
    QFile file(filename);
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot Save File!");
        return;
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
        //ui->textEdit->setText(final);
        QTextStream out(&file);
        out << final;
        file.flush();
        file.close();
        QMessageBox::information(this, "Info", "File Prettified Successfully!\nThe Prettified File Can Be Found at: " + filename);
        ui->textEdit->setLineWrapMode(QTextEdit::NoWrap);
        QFile f(filename);
        if(!f.open(QFile::ReadOnly | QFile::Text)){
            QMessageBox::warning(this, "Warning", "Cannot Open the Results File!");
            return;
        }
        else{
            QTextStream in(&f);
            QString file_text = "";
            if(f.size() > 3*1024*1024){ // file larger than 3 MB
                ui->textEdit->setText("");
                QMessageBox::warning(this, "Warning", "Due to large file size > 3 MB, we will view the first 100 lines of the output file only");
                for(int q=0; q<100; q++){
                    file_text = in.readLine();
                    ui->textEdit->append(file_text);
                }
            }
            else{
                file_text = in.readAll();
                ui->textEdit->setText(file_text);
            }
            f.close();
            ui->statusbar->showMessage("Done!");
        }

    }
    else{
        QMessageBox::warning(this, "Warning", "Cannot Prettify an Inconsistent XML file!");
        ui->actionShow_Consistency_Errors->setDisabled(false);
        ui->actionFix_Consistency_Errors->setDisabled(false);
        return;
    }
}
//---------------------------------------------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------------------------------------
// Checks the file consistency in terms of XML standards
void Editor::on_actionCheck_XML_Consistency_triggered()
{
    ui->statusbar->showMessage("");
    ui->textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    //-----------------------------------------------------------------------------
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
        /*
        for(int i=0; i<lines.length(); i++){
            qDebug() << lines[i];
        }
        */
    }
    else{
        QMessageBox::warning(this, "Warning", "This XML file is NOT consistent");
        ui->actionShow_Consistency_Errors->setDisabled(false);
        ui->actionFix_Consistency_Errors->setDisabled(false);
        return;
    }
}
//---------------------------------------------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------------------------------------
// Show and fix consistency errors
void Editor::on_actionShow_Consistency_Errors_triggered()
{
    ui->statusbar->showMessage("");
    ui->textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    //-----------------------------------------------------------------------------
    if(lines.size() == 0){
        lines = create_xml_vector(ui->textEdit->toPlainText());
    }
    QVector<QString> temp = lines;
    QMap<qint32, QString> errors = identify_errors(temp);
    ui->textEdit->setText("");
    int i = 0;
    for(i=0; i<temp.size(); i++){
        if(errors.find(i) != errors.end()){
            ui->textEdit->setTextBackgroundColor(Qt::red);
            ui->textEdit->append(temp[i]);
            ui->textEdit->setTextBackgroundColor(Qt::transparent);
        }
        else{
            ui->textEdit->append(temp[i]);
        }
    }
    lines = temp;
    ui->actionShow_Consistency_Errors->setDisabled(false);
    ui->actionFix_Consistency_Errors->setDisabled(false);
    //-----------------------------------------------------------------------------
    ui->statusbar->showMessage("Done!     Number of Errors: " + QString::number(errors.size()));
}

void Editor::on_actionFix_Consistency_Errors_triggered()
{
    ui->statusbar->showMessage("");
    ui->textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    //-----------------------------------------------------------------------------
    QString filename = QFileDialog::getSaveFileName(this, "Choose the Location to Save the Consistent File", QDir::currentPath());
    fpath = filename;
    QFile file(filename);
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot Save File!");
        return;
    }
    if(lines.size() == 0){
        lines = create_xml_vector(ui->textEdit->toPlainText());
    }
    QVector<QString> temp = lines;
    QString consistent = "";
    QMap<qint32, QString> errors = identify_errors(temp);
    ui->textEdit->setText("");
    int i = 0;
    for(i=0; i<temp.size(); i++){
        if(errors.find(i) != errors.end()){
            if(errors[i][0] == 'A'){
                consistent += temp[i];
                consistent += '\n';
            }
            else if(errors[i][0] == 'U'){
                QString value = errors[i];
                QString viewValue = "";
                for(int k = 1; k < value.length(); k++){
                    viewValue += value[k];
                }
                consistent += temp[i];
                consistent += '\n';
                consistent += viewValue;
                consistent += '\n';
            }
            else if(errors[i][0] != 'D'){
                consistent += errors[i];
                consistent += '\n';
            }
        }
        else{
            consistent += temp[i];
            consistent += '\n';
        }
    }

    QVector<int> line_num;
    for(auto j = errors.begin(); j != errors.end(); j++){
        QString value = j.value();
        if(value[0] == 'A'){
            line_num.push_back(j.key());
        }
    }
    for(int q = line_num.length()-1; q >= 0; q--){
        QString value = errors[line_num[q]];
        QString viewValue = "";
        for(int k = 1; k < value.length(); k++){
            viewValue += value[k];
        }
        viewValue = "</" + viewValue;
        consistent += viewValue;
        consistent += '\n';
    }
    //-----------------------------------------------------------------------------
    QTextStream out(&file);
    out << consistent;
    file.flush();
    file.close();
    QMessageBox::information(this, "Info", "File Fixed Successfully!\nThe Consistent File Can Be Found at: " + filename);
    QFile f(filename);
    if(!f.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot Open the Results File!");
        return;
    }
    else{
        QTextStream in(&f);
        QString file_text = "";
        if(f.size() > 3*1024*1024){ // file larger than 3 MB
            ui->textEdit->setText("");
            QMessageBox::warning(this, "Warning", "Due to large file size > 3 MB, we will view the first 100 lines of the output file only");
            for(int q=0; q<100; q++){
                file_text = in.readLine();
                ui->textEdit->append(file_text);
            }
        }
        else{
            file_text = in.readAll();
            ui->textEdit->setText(file_text);
        }
        f.close();
        ui->statusbar->showMessage("Done!");
    }
}
//---------------------------------------------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------------------------------------
int sizee = 0;
map<char, string> new_code;
// Compress File

void Editor::on_actionCompress_Data_triggered()
{
    ui->statusbar->showMessage("");
    ui->textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    //-----------------------------------------------------------------------------
    // declaring vars
    queue<unsigned char> kj;
    double old_size = 0;
    double new_size = 0;
    // Handeling opening
    //----------------------------------------------------------------------------------------------------------------------
    QString filename = QFileDialog::getOpenFileName(this, "Choose a File", QDir::currentPath());
    QFile old(filename);
    if(!old.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot Open File!");
        return;
    }
    QTextStream inOld(&old);
    QString cQ = inOld.readAll();
    string c = cQ.toLocal8Bit().constData();
    // calculating old size
    old_size = old.size()/1024.0;
    old.close();
    //----------------------------------------------------------------------------------------------------------------------
    QString fname = QFileDialog::getSaveFileName(this, "Choose the Location to Save the Compressed File", QDir::currentPath());
    QFile neew(fname);
    if(!neew.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot Save File!");
        return;
    }
    // Choosing the location to save the compression key

    QString codeName = QFileDialog::getSaveFileName(this, "Choose the Location to Save the Compression Key File", QDir::currentPath());
    QFile key(codeName);
    if(!key.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot Save File!");
        return;
    }
    fstream write_to_compressed;
    fstream write_to_key;
    write_to_compressed.open(fname.toLocal8Bit().constData(), ios::out);
    write_to_key.open(codeName.toLocal8Bit().constData(), ios::out);
    //QTextStream cooode(&key);

    //QTextStream out(&neew);
    // Processing
    tree t1;
    kj = t1.maketree(c, new_code, sizee); //to write to a file
    unsigned char zx;
    string final = "";
    for (unsigned int i = 0; kj.size() != 0; i++) {
        // ssss+=kj.front();
        zx = kj.front();
        // cout<<zx<<endl;
        //out << zx;
        final += zx;
        kj.pop();
    }
    write_to_compressed << final;
    neew.flush();
    neew.close();

    // writing the compression key code file

    write_to_key << sizee << endl;
    for(auto i = new_code.begin(); i!=new_code.end(); i++){
        string temp = i->second + '&' + i->first;
        //cout << temp;
        //QString tempQ = QString::fromStdString(temp);
        write_to_key << temp;
    }
    write_to_compressed.close();
    write_to_key.close();

    // Finalization
    ui->statusbar->showMessage("Done!");
    QFile file(fname);
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot Open the Results File!");
        return;
    }
    else{
        new_size = file.size()/1024.0;
        QMessageBox::information(this, "Info", "File Compressed Successfully!\nThe Compressed File Can Be Found At: " + fname + "\nOld File Size is " + QString::number(old_size) + " KB\nNew File Size is " + QString::number(new_size) + " KB");
        QTextStream in(&file);
        QString file_text = "";
        if(new_size > 3*1024){ // file larger than 3 MB
            ui->textEdit->setText("");
            QMessageBox::warning(this, "Warning", "Due to large file size > 3 MB, we will view the first 100 lines of the output file only");
            for(int q=0; q<100; q++){
                file_text = in.readLine();
                ui->textEdit->append(file_text);
            }
        }
        else{
            file_text = in.readAll();
            ui->textEdit->setText(file_text);
        }
        file.close();
        ui->statusbar->showMessage("Done!");
    }
    //-----------------------------------------------------------------------------------------------------------------------
}

void Editor::on_actionDecompress_File_triggered()
{
    ui->statusbar->showMessage("");
    ui->textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    double old_size = 0;
    double new_size = 0;
    //----------------------------------------------------------------------------
    // reading compressed file
    QString compressedFileName = QFileDialog::getOpenFileName(this, "Choose the Compressed File", QDir::currentPath());
    /*
    QFile compressedFile(compressedFileName);
    if(!compressedFile.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot Open The Compressed File!");
        return;
    }
    */
    string COMPRESSED = readfile(compressedFileName.toLocal8Bit().constData());

    //QTextStream inOld(&compressedFile);
    //QString cQ = inOld.readAll();
    //string compress = cQ.toLocal8Bit().constData();      // contains the compressed file
    //compressedFile.close();
    //old_size = compressedFile.size();
    //string cf = compressedFileName.toLocal8Bit().constData();
    // reading compression key file
    QString compressionKeyFileName = QFileDialog::getOpenFileName(this, "Choose the Compression Key File", QDir::currentPath());
    /*
    QFile compressionKeyFile(compressionKeyFileName);
    if(!compressionKeyFile.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot Open The Compression Key File!");
        return;
    }
    */
    string KEY = readfile(compressionKeyFileName.toLocal8Bit().constData());

    //QTextStream inKey(&compressionKeyFile);
    //QString cQQ = compressionKeyFile.readAll();
    //string compressKey = cQ.toLocal8Bit().constData();      // contains the compression key
    //compressedFile.close();
    //string ck = compressionKeyFileName.toLocal8Bit().constData();
    QString decompressedFileName = QFileDialog::getSaveFileName(this, "Choose the Location to Save the Decompressed File", QDir::currentPath());
    //QFile decomp(decompressedFileName);
    //if(!decomp.open(QFile::WriteOnly | QFile::Text)){
        //QMessageBox::warning(this, "Warning", "Cannot Save File!");
        //return;
    //}
    fstream write_to_decomp;
    write_to_decomp.open(decompressedFileName.toLocal8Bit().constData(), ios::out);
    if(!write_to_decomp){
        QMessageBox::warning(this, "Warning", "Cannot Save File!");
        return;
    }
    //string dist = decompressedFileName.toLocal8Bit().constData();
    //----------------------------------------------------------------------------------------------
    // processing
    dectree t2;
    string resQ;
    string CC = b2s(COMPRESSED);
    resQ = t2.makedectree2(KEY,CC);
    for(unsigned int i=0; i<resQ.length(); i++){
        write_to_decomp << resQ[i];
    }
    write_to_decomp.close();
    //QString res = QString::fromStdString(resQ);
    //----------------------------------------------------------------------------------------------
    // writing the decoded file
    /*
    QTextStream out(&decomp);
    out << res;
    decomp.flush();
    decomp.close();
    */
    // reading the output file
    QMessageBox::information(this, "Info", "File Decompressed Successfully!\nThe Decompressed File Can Be Found At: " + decompressedFileName);
    QFile file(decompressedFileName);
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot Open the Results File!");
        return;
    }
    else{
        QTextStream in(&file);
        QString file_text = "";
        if(new_size > 3*1024){ // file larger than 3 MB
            ui->textEdit->setText("");
            QMessageBox::warning(this, "Warning", "Due to large file size > 3 MB, we will view the first 100 lines of the output file only");
            for(int q=0; q<100; q++){
                file_text = in.readLine();
                ui->textEdit->append(file_text);
            }
        }
        else{
            file_text = in.readAll();
            ui->textEdit->setText(file_text);
        }
        file.close();
        ui->statusbar->showMessage("Done!");
    }
}
//---------------------------------------------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------------------------------------
// convert XML to JSON
void Editor::on_actionConvert_to_JSON_triggered(){
    ui->statusbar->showMessage("");
        //-----------------------------------------------------------------------------
        QString in = ui->textEdit->toPlainText();
        if(in == ""){
            QMessageBox::warning(this, "Warning", "No Text To Be Converted");
            return;
        }
        /*
        // File input
        QString filename = QFileDialog::getSaveFileName(this, "Choose the Location to Save the JSON File", QDir::currentPath());
        fpath = filename;
        QFile file(filename);
        if(!file.open(QFile::WriteOnly | QFile::Text)){
            QMessageBox::warning(this, "Warning", "Cannot Save File!");
            return;
        }
        */


        // Processing

        lines = create_xml_vector(in);
        if(!check_consistency(lines)){
            QMessageBox::warning(this, "Warning", "Cannot Convert An Inconsistent XML File");
            return;
        }
        string inq = in.toLocal8Bit().constData();
        vector<string> linesQ = create_xml_vectorQ(inq);
        int linesQSize = linesQ.size();
        node *root;
        root = create_tree(linesQ, linesQSize);
        string out = "";
        out  = iteration(root, out);
        QString res = QString::fromStdString(out);



        /*
        for(int i=0; i<linesQ.size(); i++){
            cout << linesQ[i] << '\n';
        }
        */
        // Output
        //--------------
        // on screen
        ui->textEdit->setText(res);

        // to file
        QString fname = QFileDialog::getSaveFileName(this, "Choose the Location to Save the JSON File", QDir::currentPath());
        QFile fileQ(fname);
        if(!fileQ.open(QFile::WriteOnly | QFile::Text)){
            QMessageBox::warning(this, "Warning", "Cannot Save File!");
            return;
        }
        QTextStream outt(&fileQ);
        outt << res;
        fileQ.flush();
        fileQ.close();
        QMessageBox::information(this, "Info", "File Coverted Successfully!\nThe JSON File Can Be Found at: " + fname);
        // read the file after the output
    ui->textEdit->setLineWrapMode(QTextEdit::NoWrap);
        QFile inn(fname);
        if(!fileQ.open(QFile::ReadOnly | QFile::Text)){
            QMessageBox::warning(this, "Warning", "Cannot Open the Results File!");
            return;
        }
        QTextStream inp(&inn);
        QString text_in_area;
        if(inn.size() > 3*1024*1024){
            ui->textEdit->setText("");
            QMessageBox::warning(this, "Warning", "Due to large file size > 3 MB, we will view the first 100 lines of the output file only");
            for(int q=0; q<100; q++){
                text_in_area = inp.readLine();
                ui->textEdit->append(text_in_area);
            }
        }
        else{
            text_in_area = inp.readAll();
            ui->textEdit->setText(text_in_area);
        }
        inn.close();
        //-----------------------------------------------------------------------------
        ui->statusbar->showMessage("Done!");
}


