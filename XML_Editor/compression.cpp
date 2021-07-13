#include "compression.h"
using namespace std;

string substring(string s,int index,int len,int stringlen){
    string ss="";
    for (int i=index;i<(index+len);i++){
        ss+=s[i];
        if (i==stringlen) break;
    }
    return ss;
}

string b2s(string z){//from binary to string
    unsigned char bb=0;
    string ss="";
    for(unsigned int i=0;i<z.length();i++){
        bb=z[i];
        for(int j=0;j<8;j++){
        if (readbit(bb,7-j)==1)ss+="1";
           else ss+="0";
      }
    }
   // cout<<ss;
    return ss;
}

treenode::treenode(char ch,int frequency){
    this->ch=ch;
    this->frequency=frequency;
    k=1;//is leaf
    left=NULL;
    right=NULL;
}

treenode::treenode(){}

treenode* treenode::addparentnode(treenode*left,treenode*right){
    treenode*p=new treenode();
    p->frequency=left->frequency+right->frequency;
    p->left=left;
    p->right=right;
    k=0;//not leaf
    p->ch='\0';
    return p;
}

bool treenode::isleaf(treenode*n){
    return n->left==NULL && n->right==NULL;
}

int treenode::getfrequency(){
    return frequency;
}

char treenode::getchar(){
    return ch;
}

treenode* treenode::getleft(){
    return left;
}

treenode* treenode::getright(){
    return right;
}

treenode* treenode::setnode(char ch,int frequency,treenode*left,treenode*right){
    treenode*p=new treenode();
    p->ch=ch;
    p->frequency=frequency;
    p->left=left;
    p->right=right;
    return p;
}

bool::compare::operator()(treenode* n,treenode* m){
    return n->getfrequency() > m->getfrequency();
}

void tree::encoding(treenode* n,string c, map<char,string>&newcode){
    if(n==NULL) return;
       //to check if it is leaf or not
       if(n->isleaf(n)) {
           if (c!="") newcode[n->ch] = c;
           else newcode[n->ch] = "1";
       }
       encoding(n->left,c+"0",newcode);
       encoding(n->right,c+"1",newcode);
}

queue<unsigned char> tree::maketree(string c){
    queue<unsigned char> kj;
    if (c=="") return kj;
    //to find the frequency of every char
    map<char,int>tb;//map that save avery char with its frequency
    int frequency=0;
    for (unsigned int i=0;i<c.length();i++){
        frequency= count(c.begin(),c.end(),c[i]);
        if ((frequency!=0) && (tb.count(c[i])==0)) tb[c[i]]=frequency;
    }
    //to push the nodes
    // i will push all of them first
    treenode n;
    priority_queue<treenode*,vector<treenode*>,compare>q;//this queue will save the nodes
    for(auto i=tb.begin();i!=tb.end();i++) {
        q.push(n.setnode(i->first,i->second,NULL,NULL));
    }

    //now to make the parents nodes
    while (q.size()!=1){
        treenode* left=q.top();
        q.pop();
        treenode* right=q.top();
        q.pop();
        q.push(n.addparentnode(left,right));
    }
    root=q.top();//the root of huffman tree so this is the tree


    map<char,string> newcode;// a map that save every char with its code
    tree n1;
    n1.encoding(root,"",newcode);

    //now the text after comp
    string s="";
    for(unsigned int i=0;i<c.length();i++){
        s+=newcode[c[i]];
    }

    //to convert to from binary string to binary bits
    //to push binary numbers in queue
    unsigned char bb=0;
    string ss;
    string sss;
    for(unsigned int i=0;i<s.length();i+=8){
        //ss=s.substr(i,8);
        ss=substring(s,i,8,s.length());
        for(int j=0;j<8;j++){
            sss=ss[j];
            if(sss=="1") setbit(bb,7-j);
            else clrbit(bb,7-j);
      }
       kj.push(bb);
    }
    return kj;
}


