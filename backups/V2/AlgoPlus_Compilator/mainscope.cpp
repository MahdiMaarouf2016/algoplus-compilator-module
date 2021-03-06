#include "mainscope.h"
#include<function.h>
#include<QDebug>
MainScope::MainScope():Scope()
{
    this->listSubProgrammes = new std::map<QString,Scope*>();

    Type * T1 = new Type("ENTIER");
    this->AddType(*T1);
    Type * T2 = new Type("REEL");
    this->AddType(*T2);
    Type * T3 = new Type("CHAINE");
    this->AddType(*T3);
    Type * T4 = new Type("CARACTERE");
    this->AddType(*T4);
    Type * T5 = new Type("BOOLEEN");
    this->AddType(*T5);

}

MainScope::MainScope(QString name):MainScope()
{
    setName(name);
}

void MainScope::AddSubProgram(Scope &subprogram)
{
    this->CheckExistID(subprogram.getName().toStdString());
    if(hasSubProgram(subprogram.getName()))
        throw "Sous programe " + subprogram.getName() + " deja declare ";
    else
        listSubProgrammes->insert(std::pair<QString,Scope*>(subprogram.getName(),&subprogram));
    /*if(dynamic_cast<Function*>(&subprogram))
        qDebug()<<"______________Adding Function (" << dynamic_cast<Function*>(&subprogram)->getName() << ")" <<QString::fromStdString( dynamic_cast<Function*>(&subprogram)->toString());
    else
    if(dynamic_cast<Procedure*>(&subprogram))
        qDebug()<<"______________Adding Procedure (" << dynamic_cast<Procedure*>(&subprogram)->getName() << ")" <<QString::fromStdString( dynamic_cast<Procedure*>(&subprogram)->toString());
        */
}

Scope& MainScope::getSubProgram(QString name)
{
    if(this->hasSubProgram(name))
        return *listSubProgrammes->at(name);
    else
        throw "Sous programme  " + name + " n ' existe pas ";
}

void MainScope::CheckExistID(std::string id)
{
    if(hasSubProgram(QString::fromStdString(id).toUpper()))
        throw "'" + id + "' deja declarer comme un sous programme ";
    this->Scope::CheckExistID(id);
}

std::string MainScope::toString()
{
    std::string desc ;//= "\n CONSTANTES \n";
    desc += this->Scope::toString();

    for(std::map<QString,Scope*>::iterator it = this->listSubProgrammes->begin(); it != this->listSubProgrammes->end();++it)
    {
        Scope *p = it->second;
        desc += "\t \t";
        if(dynamic_cast<Function*>(p))
        {
            desc += dynamic_cast<Function*>(p)->toString() + "\n";
        }
        else
        if(dynamic_cast<Procedure*>(p))
        {
            desc += dynamic_cast<Procedure*>(p)->toString() + "\n";
        }
        else
        {
            desc += p->toString() + "\n";
        }
        desc += "\n *****************************************\n";
    }

    return "ALGORITHME " + name.toStdString() + "\n " + desc;
}

void MainScope::Clear()
{
    this->listConstants->clear();
    this->listSubProgrammes->clear();
    this->listTypes->clear();
    this->listVariables->clear();
}

