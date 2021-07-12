#include "consistency_check.h"
#include "QStack"
#include "QMap"

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


QMap<qint32, QString> identify_errors(QVector<QString> xml){
    QMap<qint32, QString> res;
    QStack<QString> tag;
        for (int i = 0; i < xml.size(); i++) {
            QString line = xml[i];
            if (line[0] == '<') {
                if (line[1] == '/') {
                    if(!tag.empty()){
                        if (line != ("</" + tag.top())) {
                            // unmatched closing and opening tags
                            res[i] = "</" + tag.top();
                            tag.pop();
                        }
                        else {
                            tag.pop();
                        }
                    }
                    else{
                        // closing tag without opening tag

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
        return res;
}

