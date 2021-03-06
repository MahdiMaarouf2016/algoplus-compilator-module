#include "syntaxicanalyser.h"

// + LOCAL INSTANCES
    Constant    *c   = NULL;
    Function    *f   = NULL;
    Procedure   *p   = NULL;
    RecordType  *RDT = NULL;
    Array       *array= NULL;
    MatrixType  *mat = NULL;
    FileType    *file= NULL;
    AliasType*QAT = NULL;
    Argment 	*arg = NULL;
// - LOCAL INSTANCES

// + LOCAL KITS
    std::string id_type = "",containttype = "";
    std::string id_var  = "",typeVar = "";
    std::string id_Field = "",typeField = "";
    std::string id_program = "",returntype = "";
    std::string id_compteur = "",vi_pour = "",vf_pour = "";
    int unsigned memoryRange = 0;
// - LOCAL KITS

// + LOCAL COLLECTIONS
    //std::list<std::string> listVirtualID;
    QList<std::string> listVirtualID;
// - LOCAL COLLECTIONS

// + STATIC FIELDS
    MainScope *Compilateur::ParseTree = new MainScope();
// - STATIC FIELDS

SyntaxicAnalyser::SyntaxicAnalyser(QObject *parent) :
    QObject(parent)
{
    //this->currentScope = new MainScope();
    listExOPerators<< "ET"<<"OU"<<"<"<<">" <<">="<<"<="<<"="<<"<>"<<"+"<<"-"<<"MOD"<<"DIV"<<"/"<<"*";
    listExSignelOperators<<"NON"<<"-";
    listByteOpertors<<"."<<"[";
    listByteSignelOperators<<"@";
    //this->currentScope = Compilateur::ParseTree;
}

void SyntaxicAnalyser::AnalyseMainScope(LexicalAnalyser *lex)
{
    Token * t = lex->NextToken();
    while (t) {
        ReloadAll:{
           // qDebug()<<"ReloadAll";
            if(IN_Id_Algo)
            {
                if(IN_Algorithme){
                    Algorithme_:{
                        if(toUpper(t->Value()) != Algorithme)
                            throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Mot cle 'Algorithme' Attendu mais le mot '" + t->Value() + "' trouve " );
                        else
                        {
                            IN_Algorithme = false;
                            IN_id_program = true;
                        }
                    }
                }
                else
                {
                      if(t->Nature() == ID)
                      {
                          Compilateur::ParseTree->setName(toUpper(t->Value()));
                          id_program = toUpper(t->Value());
                          IN_id_program = false;
                          IN_Id_Algo    = false;
                          IN_Directives = true;
                          IN_Extention  = true;

                      }
                      else
                      if(t->Nature() == KeyWord )
                          throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"'" + t->Value() + "' est un mot reserve algorithmique ");
                      else
                         throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Identifateur du programme incorrecte " );
                  }
            }
            else
            if(IN_Directives)
            {
                if(IN_Extention)
                {
                    if(toUpper(t->Value()) != Extention)
                    {
                        IN_Directives = false;
                        IN_HeaderProgram = true;
                        goto ReloadAll;
                    }
                    else
                    {
                        IN_Extention = false;
                        IN_FileName = true;
                    }
                }
                else
                {
                    if(t->Nature() != STR)
                        throw SyntaxicError(t->getLineNumber(),t->getColumnNumber()," invalide nom du fichier d'extention ");
                    else
                    {
                        // TRaitement include ( t->Value());
                        listExtentions.push_back(t->Value());
                        IN_FileName = false;
                        IN_Extention = true;
                    }
                }
            }
            else
            if(IN_HeaderProgram)
            {
                   // qDebug()<< "IN_HeaderProgram "<< (t->Value())<< IN_Constantes << IN_Types << IN_Variables << IN_Fct << IN_Proc;

                    if(IN_Constantes == false && IN_Types == false && IN_Variables == false)
                    {

                        if(t->Nature() == KeyWord){

                            if(toUpper(t->Value()) == Constantes)
                                {
                                    IN_Constantes = true;
                                    IN_ListConsts = true;
                                    IN_listVars = IN_Listtypes = false;
                                    IN_IDConst = true;
                                }
                            else
                                if(toUpper(t->Value()) == Types)
                                {
                                    IN_Listtypes=true;
                                    IN_Types = true;
                                    IN_listVars = IN_ListConsts = false;
                                    IN_Id_Type = true;
                                }
                            else
                                if(toUpper(t->Value()) == Variables)
                                {
                                    IN_Types = false;
                                    IN_Variables = true;
                                    IN_Id_Var = true;
                                    IN_ListConsts = IN_Listtypes = false;
                                    IN_listVars = true;
                                }
                            else
                                if(toUpper(t->Value()) == Fonction)
                                {
                                    if(IN_Proc || IN_Fct)
                                        throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Declaration imprique des sous programmes interdit ");
                                    else
                                    {
                                        IN_ListConsts = IN_Listtypes = IN_listVars = false;
                                        IN_HeaderProgram = false;
                                        IN_Fct = true;
                                        IN_id_fct = true;
                                        f = new Function(*Compilateur::ParseTree);
                                    }
                                }
                            else
                                 if(toUpper(t->Value()) == PRocedure)
                                {
                                     if(IN_Proc || IN_Fct)
                                         throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Declaration imprique des sous programmes interdit ");
                                     else
                                     {
                                         IN_ListConsts = IN_Listtypes = IN_listVars = false;
                                         IN_HeaderProgram = false;
                                         IN_Proc   = true;
                                         IN_IdProc = true;
                                         p = new Procedure(*Compilateur::ParseTree);
                                     }

                                }
                            else
                                if(toUpper(t->Value()) == Debut)
                                {
                                     IN_ListConsts = IN_Listtypes = IN_listVars = false;
                                     IN_HeaderProgram = false; // FERMER LE HEADER DU PROGRAMME PUIS ALLER VERS LE DEBUT
                                     IN_Debut = true;
                                     goto ReloadAll;
                                     //qDebug()<< "tets : IN fuction Deb : " << IN_Fct << " IN proc deb : " << IN_Proc;
                                }
                        }
                        else
                            if(t->Nature() == ID)
                            {
                                if(IN_ListConsts)
                                {
                                    IN_Constantes = true;
                                    goto ID_Const;
                                }
                                else
                                if(IN_Listtypes)
                                {
                                    IN_Types = true;
                                    IN_Id_Type = true;
                                    goto Type;
                                }
                                else
                                if(IN_listVars)
                                {
                                    IN_Variables = true;
                                    IN_Id_Var = true;
                                    goto Var;
                                }
                                else
                                {
                                    throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"indication ambigue par le mot suivant : " + t->Value());
                                }
                            }
                            else{
                                throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"indication ambigue par le mot suivant : " + t->Value());
                            }
                    }
                    else
                    if(IN_Constantes)
                    {
                            IN_ListConsts = true;
                            if(IN_IDConst)
                            {
                                ID_Const:{
                                    if(t->Nature() != ID)
                                    {
                                        if(t->Nature() == KeyWord)
                                            throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"'" + t->Value() + "' est un mot reserve algorithmique ");
                                        else
                                            throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"identificateur du contante incorrecte");
                                    }
                                    else
                                    {
                                        c = new Constant(toUpper(t->Value()));
                                        IN_IDConst = false;
                                        IN_OperatorConst = true;
                                    }
                                }
                            }
                            else
                            if(IN_OperatorConst)
                            {
                                OperatorConst:{
                                if(t->Value() != "=")
                                    throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Erreur du synatax '=' attendu  ");
                                else
                                {
                                    IN_OperatorConst = false;
                                    IN_ConstValue = true;
                                 }
                            }
                            }else
                            {
                                ConstValue:{
                                        if(t->Value() == "-" && t->Nature() == ArithmeticOperator && !signe)
                                           {
                                               signe = true;
                                           }
                                        else
                                        if(t->Nature() == NUMBER || t->Nature() == Reel || t->Nature() == STR || t->Nature() == CHAR)
                                        {
                                            if(signe && (t->Nature() == STR || t->Nature() == CHAR))
                                                throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Signe incompatible avec le type du valeur ");
                                            else
                                            {
                                                if(signe)
                                                    c->setValue( ("-" + t->Value()));
                                                else
                                                    c->setValue( (t->Value()));
                                                try
                                                {
                                                    if(f)
                                                        f->AddConstant(*c);
                                                    else
                                                    if(p)
                                                        p->AddConstant(*c);
                                                    else
                                                        Compilateur::ParseTree->AddConstant(*c);
                                                    //this->currentScope->AddConstant(*c);

                                                }catch(std::string error)
                                                {
                                                    throw SyntaxicError(t->getLineNumber(),0,error);
                                                }

                                                c = NULL;
                                                signe = false;
                                                IN_ConstValue = false;
                                                IN_Constantes = false;
                                            }

                                        }
                                        else
                                            throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Valuer du constantes incorrecte ");
                                    }

                            }
                    }
                    else
                    if(IN_Types)
                    {
                        //qDebug()<< "in types: "<< (t->Value());
                        Type:{
                                IN_Listtypes = true;

                                if(IN_Id_Type)
                                {
                                    if(t->Nature() == ID)
                                    {
                                        id_type = toUpper( t->Value());
                                        IN_Id_Type = false;
                                        IN_OpertatorType = true;
                                    }
                                    else
                                    {
                                        if(t->Nature() == KeyWord)
                                            throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"'" + t->Value() + "' est un mot reserve algorithmique ");
                                        else
                                            throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"identificateur du type invalide ");
                                    }
                                }
                                else
                                if(IN_OpertatorType)
                                {
                                    if(t->Value() == "=")
                                    {
                                        IN_OpertatorType = false;
                                        IN_DefType = true;
                                    }
                                    else
                                        throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Mot cle '=' de deffinition du type est attendu");
                                }
                                else
                                {
                                    if(!IN_Enreg && !IN_Tab && !IN_Mat && !IN_Fich && !IN_Qalifier)
                                      {
                                          if(t->Nature() == LocationSpecifier || t->Nature() == ID)// FOR ALIAS TYPE
                                          {
                                              /*if(t->Nature() == LocationSpecifier)
                                              {*/
                                                  IN_DefType = true;
                                                  IN_Qalifier = true;
                                                  QAT = new AliasType(toUpper(id_type));
                                                  try
                                                  {
                                                      if(f)
                                                          f->AddType(QAT);
                                                      else
                                                      if(p)
                                                          p->AddType(QAT);
                                                      else
                                                          Compilateur::ParseTree->AddType(QAT);
                                                  }catch(std::string error)
                                                  {
                                                      throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),error);
                                                  }

                                                  goto Type;
                                             /* }
                                              else
                                               if(t->Nature() == KeyWord)
                                                    throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"'" + t->Value() + "' est un mot reserve algorithmique !");
                                              else
                                                  throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Indicateur du type incorrecte !");
                                            */
                                          }
                                          else
                                          if(toUpper(t->Value()) == Enregistrement)
                                          {
                                                IN_DefType = false;
                                                IN_Enreg = true;
                                                IN_ID_Field = true;
                                                IN_listFields = true;
                                                RDT = new RecordType();
                                                RDT->setName(toUpper(id_type));
                                                try{
                                                    if(f)
                                                        f->AddType(RDT);
                                                    else
                                                        if(p)
                                                            p->AddType(RDT);
                                                        else
                                                            Compilateur::ParseTree->AddType(RDT);
                                                }catch(std::string error)
                                                {
                                                    throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),error);
                                                }


                                          }
                                          else
                                          if(toUpper(t->Value()) == Tableau)
                                          {
                                                IN_DefType = false;
                                                IN_Tab = true;
                                                IN_LI = true;
                                          }
                                          else
                                          if(toUpper(t->Value()) == Ficheir)
                                          {
                                                IN_DefType = false;
                                                IN_Fich = true;
                                                IN_DE_Key = true;
                                                file = new FileType(toUpper(id_type));
                                                try{
                                                    if(f)
                                                        f->AddType(file);
                                                    else
                                                        if(p)
                                                            p->AddType(file);
                                                        else
                                                            Compilateur::ParseTree->AddType(file);
                                                }catch(std::string error)
                                                {
                                                    throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),error);
                                                }


                                          }
                                          /*else
                                          if(t->Nature() == LocationSpecifier || t->Nature() == ID)
                                          {
                                                IN_DefType = true;
                                                IN_Qalifier = true;
                                                QAT = new QalifierType( (id_type).toUpper());
                                                try
                                                {
                                                    if(f)
                                                        f->AddType(QAT);
                                                    else
                                                    if(p)
                                                        p->AddType(QAT);
                                                    else
                                                        Compilateur::ParseTree->AddType(QAT);
                                                }catch(std::string error)
                                                {
                                                    throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),error);
                                                }

                                                goto Type;
                                          }*/
                                          else
                                              throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Deffinition du type illegale '" + t->Value() + "'");

                                    }
                                    else
                                      {
                                            if(IN_Enreg)
                                            {
                                                Enreg:{
                                                    if(IN_listFields)
                                                    {
                                                        if(IN_ID_Field)
                                                        {
                                                            if(t->Nature() == ID)
                                                            {

                                                                id_Field = toUpper(t->Value());
                                                                IN_ID_Field = false;
                                                                IN_AssignType = true;

                                                            }
                                                            else
                                                            {
                                                                if(t->Nature() == KeyWord)
                                                                {
                                                                    if(toUpper(t->Value()) == FIN)
                                                                    {
                                                                        IN_listFields = false;
                                                                        IN_End = true;
                                                                    }
                                                                    else
                                                                        throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"'" + t->Value() + "' est un mot reserve algorithmique ");
                                                                }
                                                                else
                                                                    throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"identificateur du champ incorrecte ");
                                                            }
                                                        }
                                                        else
                                                        if(IN_AssignType)
                                                        {
                                                            if(t->Nature() == AssignType)
                                                            {
                                                                IN_AssignType = false;
                                                                IN_typeOfField = true;
                                                            }
                                                            else
                                                                throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"mot cle ':' de attendu ");
                                                        }
                                                        else
                                                        {
                                                            if(t->Nature() == LocationSpecifier)
                                                                memoryRange++;
                                                            else
                                                                if(t->Nature() == ID)
                                                                {

                                                                    try{
                                                                        FieldRecord field ;
                                                                        field.nom = toUpper(id_Field);

                                                                         if(f)
                                                                             field.type = f->getTypeContaint(toUpper(t->Value()));
                                                                         else
                                                                         if(p)
                                                                             field.type = p->getTypeContaint(toUpper(t->Value()));
                                                                         else
                                                                             field.type = Compilateur::ParseTree->getTypeContaint(toUpper(t->Value()));
                                                                        field.type.addMemoryRange(memoryRange);
                                                                        memoryRange = 0;
                                                                        RDT->AddField(field);

                                                                        id_Field = typeField = "";
                                                                        IN_ID_Field = true;
                                                                    }catch(std::string error)
                                                                    {
                                                                        throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),error);
                                                                    }
                                                                    IN_typeOfField = false;
                                                                }
                                                                else
                                                                    throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Type du champ incorrecte ");

                                                        }
                                                    }
                                                    else // IN END RECORD
                                                    {
                                                        IN_End  = false;
                                                        IN_Enreg = false;
                                                        IN_Types = false;
                                                        IN_DefType = false;

                                                        /*if(f)
                                                            f->AddType(*RDT);
                                                        else
                                                        if(p)
                                                            p->AddType(*RDT);
                                                        else
                                                            Compilateur::ParseTree->AddType(*RDT);
                                                        */
                                                        if(toUpper(t->Value()) != toUpper(id_type) && toUpper(t->Value()) != Enregistrement)
                                                            goto ReloadAll;
                                                    }
                                                }
                                            }
                                            else
                                            if(IN_Tab)
                                            {
                                                Tab:{
                                                        //qDebug()<< "test IN Tab : " <<  (t->Value())<<IN_LI<<IN_InitSizeT<< IN_Punctiation<<IN_size_c<<IN_RI<<IN_DE_Key<<IN_ContaintType<<"\n";
                                                        if(IN_LI)
                                                        {

                                                             if(t->Nature() == LI)
                                                             {
                                                                 IN_LI = false;
                                                                 IN_InitSizeT = true;
                                                             }
                                                             else
                                                                 throw SyntaxicError(t->getLineNumber(),t->getColumnNumber()," Mot cle '[' attendu ");

                                                        }
                                                        else
                                                        if(IN_InitSizeT)
                                                        {
                                                            if(t->Nature() == NUMBER)
                                                            {
                                                                IN_InitSizeT = false;
                                                                IN_Punctiation = true;
                                                            }else
                                                                throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Borne inferieur du tableux attendu ");
                                                        }
                                                        else
                                                        if(IN_Punctiation)
                                                        {
                                                            if(t->Nature() == Punctuation && t->Value() == "..")
                                                            {
                                                                IN_Punctiation = false;
                                                                IN_size_c = true;
                                                            }else
                                                                throw SyntaxicError(t->getLineNumber(),t->getColumnNumber()," Mot clé '..' attendu ");
                                                        }
                                                        else
                                                        if(IN_size_c)
                                                        {
                                                            if(t->Nature() == NUMBER)
                                                            {
                                                                size_c = toInt(t->Value());
                                                                IN_size_c = false;
                                                                IN_RI = true;
                                                            }else
                                                                throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Borne supperieur du tableux attendu ");
                                                        }
                                                        else
                                                        if(IN_RI)
                                                        {
                                                            if(t->Nature() == RI)
                                                            {
                                                                IN_RI = false;
                                                                IN_DE_Key = true;
                                                                array  = new Array(toUpper(id_type));
                                                                try{
                                                                    if(f)
                                                                        f->AddType(array);
                                                                    else
                                                                        if(p)
                                                                            p->AddType(array);
                                                                        else
                                                                            Compilateur::ParseTree->AddType(array);
                                                                }catch(std::string error)
                                                                {
                                                                    throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),error);
                                                                }

                                                            }
                                                            else
                                                             if(t->Nature() == Punctuation && t->Value() == ",")
                                                             {
                                                                 IN_Tab = false;
                                                                 IN_ContaintType = false;
                                                                 IN_Mat = true;
                                                                 IN_InitSizeT = true;
                                                                 mat = new MatrixType(toUpper(id_type));
                                                                 try{
                                                                     if(f)
                                                                         f->AddType(mat);
                                                                     else
                                                                         if(p)
                                                                             p->AddType(mat);
                                                                         else
                                                                             Compilateur::ParseTree->AddType(mat);
                                                                 }catch(std::string error)
                                                                 {
                                                                     throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),error);
                                                                 }
                                                             }
                                                            else
                                                                throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Mot cle ']' attendu");
                                                        }
                                                        else
                                                        if(IN_DE_Key)
                                                        {

                                                            if(t->Nature() == ID && toUpper(t->Value()) == "DE")
                                                            {
                                                                IN_DE_Key = false;
                                                                IN_ContaintType = true;
                                                            }
                                                            else
                                                                throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Mot cle 'DE' attendu");
                                                        }else
                                                        {
                                                            if(t->Nature() == LocationSpecifier)
                                                                memoryRange++;
                                                            else
                                                            if(t->Nature() == ID)
                                                            {
                                                                try {
                                                                 array->setSize(size_c);
                                                                 size_c = 0;
                                                                 TypeOfContaint T;
                                                                 if(f)
                                                                     T = f->getTypeContaint(toUpper(id_type));
                                                                 else
                                                                 if(p)
                                                                     T = p->getTypeContaint(toUpper(id_type));
                                                                 else
                                                                     T = Compilateur::ParseTree->getTypeContaint(toUpper(id_type));
                                                                 T.addMemoryRange(memoryRange);
                                                                 array->setTypeContaint(T);
                                                                 memoryRange = 0;

                                                                }catch(std::string error)
                                                                {
                                                                    throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),error);
                                                                }

                                                                IN_Tab = false;
                                                                IN_ContaintType = false;
                                                                IN_Types = false;
                                                                IN_DefType = false;
                                                            }else
                                                                throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Type de contenu du Tableau incorrecte ");

                                                        }
                                                    }
                                            }
                                            else
                                            if(IN_Mat)
                                            {
                                                Mat:{
                                                    int size_l = 0;
                                                    if(IN_InitSizeT)
                                                    {
                                                        if(t->Nature() == NUMBER)
                                                        {
                                                            IN_InitSizeT = false;
                                                            IN_Punctiation = true;
                                                        }else
                                                            throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Borne inferieur du Matrice attendu ");
                                                    }
                                                    else
                                                    if(IN_Punctiation)
                                                    {
                                                        if(t->Nature() == Punctuation && t->Value() == "..")
                                                        {
                                                            IN_Punctiation = false;
                                                            IN_size_l = true;
                                                        }else
                                                            throw SyntaxicError(t->getLineNumber(),t->getColumnNumber()," Mot clé '..' attendu ");
                                                    }
                                                    else
                                                    if(IN_size_l)
                                                    {
                                                        size_l = toInt(t->Value());
                                                        if(t->Nature() == NUMBER)
                                                        {
                                                            IN_size_l = false;
                                                            IN_RI = true;
                                                        }else
                                                            throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Borne inferieur du Matrice attendu ");
                                                    }
                                                    else
                                                    if(IN_RI)
                                                    {
                                                        if(t->Nature() == RI)
                                                        {
                                                            IN_RI = false;
                                                            IN_DE_Key = true;
                                                        }
                                                        else
                                                            throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Mot cle ']' attendu");
                                                    }
                                                    else
                                                    if(IN_DE_Key)
                                                    {
                                                        if(t->Nature() == ID && toUpper(t->Value()) == "DE")
                                                        {
                                                            IN_DE_Key = false;
                                                            IN_ContaintType = true;
                                                        }
                                                        else
                                                            throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Mot cle 'DE' attendu");
                                                    }else
                                                    {
                                                        if(t->Nature() == LocationSpecifier)
                                                            memoryRange++;
                                                        else
                                                        if(t->Nature() == ID)
                                                        {
                                                            try {
                                                             TypeOfContaint T ;
                                                             if(f)
                                                                 T = f->getTypeContaint(toUpper(t->Value()));
                                                             else
                                                             if(p)
                                                                 T = p->getTypeContaint(toUpper(t->Value()));
                                                             else
                                                                 T = Compilateur::ParseTree->getTypeContaint(toUpper(t->Value()));

                                                             T.addMemoryRange(memoryRange);
                                                             memoryRange = 0;
                                                             mat->setRowsCount(size_l);
                                                             size_l = 0;
                                                             mat->setColumnsCount(size_c);
                                                             size_c = 0;
                                                             mat->setTypeContaint(T);
                                                             /*
                                                             if(f)
                                                                 f->AddType(*mat);
                                                             else
                                                             if(p)
                                                                 f->AddType(*mat);
                                                              else
                                                                 Compilateur::ParseTree->AddType(*mat);
                                                              */
                                                             //this->currentScope->AddType(*T);
                                                             //delete mat;
                                                            }catch(std::string error)
                                                            {
                                                                throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),error);
                                                            }
                                                            IN_Types = false;
                                                            IN_Mat = false;
                                                            IN_ContaintType = false;
                                                            IN_DefType = false;
                                                        }else
                                                            throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Type de contenu du Matrice incorrecte ");
                                                    }
                                                }
                                            }
                                            else
                                            if(IN_Fich)
                                            {
                                                Fich:{
                                                       //qDebug()<< "fich" << t->Value()<<IN_DE_Key<<IN_ContaintType;
                                                        if(IN_DE_Key)
                                                        {
                                                            if(t->Nature() == ID && toUpper(t->Value()) == "DE")
                                                            {
                                                                IN_DE_Key = false;
                                                                IN_ContaintType = true;
                                                                containttype = "";
                                                            }
                                                            else
                                                                throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Mot cle 'De' attendu");
                                                        }
                                                        else
                                                        {
                                                            IN_ContaintType = true;
                                                            if(t->Nature() == LocationSpecifier)
                                                                memoryRange++;
                                                            else
                                                            if(t->Nature() == ID)
                                                            {
                                                                try {
                                                                 TypeOfContaint T;
                                                                 try
                                                                 {
                                                                     if(f)
                                                                         T = f->getTypeContaint(toUpper(t->Value()));
                                                                     else
                                                                         if(p)
                                                                             T = p->getTypeContaint(toUpper(t->Value()));
                                                                         else
                                                                             T  = Compilateur::ParseTree->getTypeContaint(toUpper(t->Value()));
                                                                     if( toUpper(t->Value()) == "TEXTE")
                                                                         qDebug()<<"WARNING ("<<t->getLineNumber() << "," << t->getColumnNumber() <<") TYPE UTILISATEUR 'TEXTE' EST IGNORE POUR CETTE FICHIER ";
                                                                 }
                                                                 catch(std::string error)
                                                                 {
                                                                     if(toUpper(t->Value()) == "TEXTE")
                                                                         T = TypeOfContaint("TEXTE");
                                                                     else
                                                                         throw error;
                                                                 }

                                                                 T.addMemoryRange(memoryRange);
                                                                 file->setTypeContaint(T);
                                                                 memoryRange = 0;

                                                                }catch(std::string error)
                                                                {
                                                                    throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),error);
                                                                }

                                                                IN_Fich = false;
                                                                IN_ContaintType = false;
                                                                IN_Types = false;
                                                                IN_DefType = false;

                                                            }
                                                            else
                                                                throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Type de contenu du fichier incorrecte ");

                                                        }

                                                    }
                                            }
                                            else
                                            if(IN_Qalifier)
                                            {
                                                Qalifier:{
                                                    if(t->Nature() == LocationSpecifier)
                                                        memoryRange++;
                                                    else
                                                    if(t->Nature() == ID)
                                                    {

                                                        try{
                                                            TypeOfContaint TQ;
                                                            if(f)
                                                                TQ = f->getTypeContaint(  toUpper(t->Value()));
                                                            else
                                                                if(p)
                                                                    TQ = p->getTypeContaint(  toUpper(t->Value()));
                                                                else
                                                                    TQ = Compilateur::ParseTree->getTypeContaint(  toUpper(t->Value()));
                                                            TQ.addMemoryRange(memoryRange);
                                                            memoryRange = 0;
                                                            QAT->setTypeQalified(TQ);
                                                            /*
                                                            if(f)
                                                                f->AddType(*QAT);
                                                            else
                                                            if(p)
                                                                p->AddType(*QAT);
                                                            else
                                                                Compilateur::ParseTree->AddType(*QAT);
                                                            */
                                                        }catch(std::string error)
                                                        {
                                                            throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),error);
                                                        }
                                                        IN_DefType = false;
                                                        IN_Qalifier = false;
                                                        IN_Types = false;
                                                    }
                                                    else
                                                        throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Nom du type qalifier attendu");
                                                }
                                            }

                                      }

                                }

                            }
                    }
                    else
                    {
                        Var:{
                            if(IN_Id_Var)
                            {
                                if(t->Nature() != ID)
                                {
                                    if(t->Nature() == KeyWord)
                                        throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"'" + t->Value() + "' est un mot reserve algorithmique ");
                                    else
                                        throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Identificateur du variable incorrecte ");
                                }
                                else
                                {
                                    IN_Id_Var = false;
                                    IN_AssignType = true;
                                    if(listVirtualID.contains(toUpper(t->Value())))
                                        throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Duplication a trouve suite de declaration du variable '" + t->Value() + "'");
                                    else
                                        listVirtualID << toUpper(t->Value());
                                }
                            }
                            else
                            if(IN_AssignType)
                            {
                                if(t->Nature() == AssignType)
                                {
                                    IN_AssignType = false;
                                    IN_TypeOfVar = true;
                                }
                                else
                                if(t->Nature() == Punctuation && t->Value() == ",")
                                {
                                    IN_AssignType = false;
                                    IN_Id_Var = true;
                                }
                                else
                                    throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Mot clé ':' attendu mais '" + t->Value() + "' trouvé ");
                            }
                            else
                            {
                                if(t->Nature() == LocationSpecifier)
                                    memoryRange++;
                                else
                                    if(t->Nature() == ID)
                                    {
                                        try{

                                            TypeOfContaint T ;
                                            if(f)
                                                T = f->getTypeContaint(  toUpper(t->Value()));
                                            else
                                            if(p)
                                                T = p->getTypeContaint( toUpper(t->Value()));
                                            else
                                                T = Compilateur::ParseTree->getTypeContaint( toUpper(t->Value()));

                                            T.addMemoryRange(memoryRange);
                                            memoryRange = 0;
                                            for(int u = 0; u < listVirtualID.size();u++)
                                            {
                                                //qDebug()<<"test : "<<  ( listVirtualID.at(u));
                                                Variable * v = new Variable( (listVirtualID[u]),T);
                                                if(f)
                                                   f->AddVariable(*v);
                                                else
                                                if(p)
                                                    p->AddVariable(*v);
                                                else
                                                    Compilateur::ParseTree->AddVariable(*v);

                                                //this->currentScope->AddVariable(*v);
                                                delete v;
                                            }
                                            listVirtualID.clear();

                                        }catch(std::string error)
                                        {
                                            throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),error);
                                        }
                                        IN_TypeOfVar = false;
                                        IN_Variables = false;
                                    }
                                    else
                                        throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Type du variable incorrecte ");

                            }
                        }
                    }

            }
            else
            if(IN_Debut)
            {
                // Au bout de IN_HeaderProgram (c à d le mot cle 'DEBUT' a ete detecté ) il va retourne à celui

                if(IN_Fct)
                {
                    //qDebug()<< "In debut fonction  " << t->toString();
                    Statment * debutFct = new Statment();
                    debutFct->SetBehavior((StatmentBehavio)0);
                    Instructions(debutFct,lex);
                    //qDebug()<< "IN END fonction "<<t->toString();
                    IN_Debut = false;
                    IN_Fct = false;
                    IN_HeaderProgram = true;
                    //qDebug()<<"name fct : " << f->getName();
                    f->setDebutStatment(debutFct);
                    if(!f->hasRetournValue())
                        Error(lex->getCurrentToken(),"Fonction illegale : Un valeur de retour requis pour cette fonction");
                    Compilateur::ParseTree->AddFunction(*f);
                    f = NULL;

                    //this->currentScope = Compilateur::ParseTree;
                }
                else
                if(IN_Proc)
                {
                   // qDebug()<< "In debut Procedure " << t->toString();
                    Statment * debutProc = new Statment();
                    debutProc->SetBehavior((StatmentBehavio)0);
                    Instructions(debutProc,lex);
                    //qDebug()<< "IN END Procedure "<<t->toString();
                    IN_Debut = false;
                    IN_Proc = false;
                    IN_HeaderProgram = true;
                    //qDebug()<<"name proc : " << p->getName();
                    p->setDebutStatment(debutProc);
                    Compilateur::ParseTree->AddProcedure(*p);
                    p = NULL;
                    //this->currentScope = Compilateur::ParseTree;
                }
                else
                {
                    //+ INCLURE LES FONCTION ET LES PROCEDURES DES EXTENTIONS


                    //- INCLURE LES FONCTION ET LES PROCEDURES DES EXTENTIONS
                    //qDebug()<< "In debut Algorithme " << t->toString();
                    Statment * debAlgo = new Statment();
                    Compilateur::ParseTree->setDebutStatment(debAlgo);
                    Instructions(debAlgo,lex);
                    //qDebug()<< "IN END Algorithme "<<t->toString();
                    IN_Debut = false;
                    return;
                }

            }
            else
            if(IN_Fct)
            {
                //qDebug()<< "In Fonction" <<  (t->Value()) << IN_id_fct << IN_Lp << IN_listArgs <<  IN_Rp << IN_AssignType;

                if(IN_id_fct)
                {
                    if(t->Nature() != ID)
                    {
                        if(t->Nature() == KeyWord)
                            throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"'" + t->Value() + "' est un mot reserve algorithmique ");
                        else
                            throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"identificateur du fonction incorrecte ");
                    }
                    else
                    {
                        f->setName( toUpper(t->Value()));
                        id_program =  toUpper(t->Value());
                        IN_id_fct = false;
                        IN_Lp  = true;
                    }
                }
                else
                if(IN_Lp)
                {
                    if(t->Nature() != Lp)
                        throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Symbole ' ( ' attendu ");
                    else
                    {
                        IN_Lp = false;
                        IN_formalArgs = true;
                        IN_ES = true;
                    }
                }
                else
                if(IN_formalArgs)
                {
                    if(t->Nature() == Rp && IN_ES && !require)
                    {
                        IN_formalArgs = false;
                        require = false;
                        IN_IdArg = false;
                        IN_Rp = true;
                        goto ReloadAll;
                    }
                    listArgs:{
                        if(IN_ES)
                        {
                            arg = new Argment();
                            if(toUpper(t->Value()) == "DON" || toUpper(t->Value()) == "RES" || toUpper(t->Value()) == " DONRES" || toUpper(t->Value()) == "DON_RES")
                            {
                                if(IN_Fct)
                                {
                                    if(toUpper(t->Value()) != "DON")
                                        throw SyntaxicError(t->getLineNumber(),t->getColumnNumber()," Mode du passage interdit ");
                                    else
                                    {
                                       arg->setIO(IN);
                                    }
                                }else
                                {
                                    if(toUpper(t->Value()) == "RES")
                                        arg->setIO(OUT);
                                    else
                                    if(toUpper(t->Value()) == "DONRES" || toUpper(t->Value()) == "DON_RES")
                                        arg->setIO( IN_OUT );
                                    else
                                        arg->setIO(IN);
                                }
                                IN_ES = false;
                                IN_IdArg = true;
                            }
                            else                             
                            {
                                arg->setIO(IN);
                                IN_ES = false;
                                IN_IdArg = true;
                                goto ReloadAll;
                            }
                        }
                        else
                        if(IN_IdArg)
                        {
                            if(t->Nature() != ID)
                            {
                                if(t->Nature() == KeyWord)
                                    throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"'" + t->Value() + "' est un mot reserve algorithmique ");
                                else
                                    throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Identificateur d'argument incorrecte " + t->Value());
                            }
                            else
                            {
                                arg->setName( (toUpper(t->Value())));
                                IN_IdArg = false;
                                IN_AssignType = true;
                            }
                        }
                        else
                        if(IN_AssignType)
                         {
                             if(t->Nature() != AssignType)
                                 throw SyntaxicError(t->getLineNumber(),t->getColumnNumber()," mot cle ':' attendu  ");
                             else
                             {
                                 IN_AssignType = false;
                                 IN_typeOfArg = true;
                             }
                         }
                        else
                        if(IN_typeOfArg)
                         {
                            if(t->Nature() == LocationSpecifier)
                                memoryRange++;
                            else
                            {
                                try
                                {
                                    TypeOfContaint T ;
                                    if(f)
                                        T = f->getTypeContaint( toUpper(t->Value()));
                                    else
                                    if(p)
                                        T = p->getTypeContaint( toUpper(t->Value()));
                                    else
                                        T = Compilateur::ParseTree->getTypeContaint( toUpper(t->Value()));
                                    //Type T = currentScope->getType();
                                    T.addMemoryRange(memoryRange);
                                    memoryRange = 0;
                                    arg->setType(T);
                                    if(IN_Fct)
                                        f->addArg(*arg);
                                    else
                                        p->addArg(*arg);
                                    arg = NULL;

                                }catch(std::string error)
                                {
                                    throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),error);
                                }
                                require = false;
                                IN_typeOfArg = false;
                            }
                         }
                        else
                         {
                             if(t->Value() == ",")
                             {
                                 IN_ES = true;
                                 require = true;
                             }
                             else
                             {
                                 IN_formalArgs = false;
                                 IN_IdArg = false;
                                 IN_Rp = true;
                                 require = false;
                                 goto ReloadAll;
                             }

                         }

                    }//listArgs
                }
                else
                if(IN_Rp)
                {
                    if(t->Nature() != Rp)
                        throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Symbole ')' attendu ");
                    else
                    {
                        IN_Rp = false;
                        IN_AssignType = true;
                    }
                }
                else
                if(IN_AssignType)
                {
                    if(t->Nature() != AssignType)
                        throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Mot cle ':' attendu ");
                    else
                     {
                        IN_AssignType = false;
                        IN_returnType = true;
                     }
                }
                else
                if(IN_returnType)
                {
                    if(t->Nature() == LocationSpecifier)
                        memoryRange++;
                    else
                    if(t->Nature() == ID)
                    {
                        try{
                            TypeOfContaint T ;
                            if(f)
                                T = f->getTypeContaint( toUpper(t->Value()));
                            else
                            if(p)
                                T = p->getTypeContaint( toUpper(t->Value()));
                             else
                                T = Compilateur::ParseTree->getTypeContaint( toUpper(t->Value()));

                            //this->currentScope->getType( toUpper(t->Value()));
                            T.addMemoryRange(memoryRange);
                            memoryRange = 0;
                            f->setReturnType(T);
                        }catch(std::string error)
                        {
                            throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),error);
                        }

                        IN_returnType = false;
                        IN_HeaderProgram = true;
                    }
                    else
                        throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"type de retour de fonction incorrecte ");
                }else
                {
                    IN_HeaderProgram = true;
                    //this->currentScope = f;
                    //  if(toUpper(t->Value()) != id_fct && t->Value() != ".")goto ReloadAll;
                }

            }
            else
            if(IN_Proc)
            {
                if(IN_IdProc)
                {
                    if(t->Nature() != ID)
                    {
                        if(t->Nature() == KeyWord)
                            throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"'" + t->Value() + "' est un mot reserve algorithmique ");
                        else
                            throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"identificateur du procedure incorrecte ");
                    }
                    else
                    {
                        p->setName( (toUpper(t->Value())));
                        id_program =  (toUpper(t->Value()));
                        IN_IdProc = false;
                        IN_Lp  = true;
                    }
                }
                else
                if(IN_Lp)
                {
                    if(t->Nature() != Lp)
                        throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Mot cle ' ( ' attendu ");
                    else
                    {
                        IN_Lp = false;
                        IN_formalArgs = true;
                        IN_ES = true;
                    }
                }
                else
                if(IN_formalArgs)
                {
                    goto listArgs;
                }
                else
                if(IN_Rp)
                {
                    if(t->Nature() != Rp)
                        throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Mot cle ')' attendu ");
                    else
                    {
                        IN_Rp = false;
                        IN_HeaderProgram = true;
                    }
                }
                else
                {
                    //this->currentScope = p;
                    IN_HeaderProgram = true;
                    //if(toUpper(t->Value()) != id_proc && t->Value() != ".") goto ReloadAll;
                }
            }
            else
            {
                /*if(IN_Fct)
                {
                    qDebug()<< "In END fonction  ";
                    if(toUpper(t->Value()) != id_fct && t->Value() != ".")
                    {
                        IN_HeaderProgram = true;
                        goto ReloadAll;
                    }
                    else
                    {
                        IN_HeaderProgram = true;
                    }
                    IN_Fct = false;

                }
                else
                if(IN_Proc)
                {
                    qDebug()<< "In END Procedure ";
                    if(toUpper(t->Value()) != id_proc && t->Value() != ".")
                    {
                        IN_HeaderProgram = true;
                        goto ReloadAll;
                    }
                    else
                    {
                        IN_HeaderProgram = true;
                    }
                    IN_Proc = false;
                    //if(toUpper(t->Value()) != id_proc && t->Value() != ".") goto ReloadAll;

                }
                else
                {
                    // include les fonctions et procecures ici
                    qDebug()<< (t->Value()) << "In END Algorithme ";
                }
                */
            }
        }
        t = lex->NextToken();
    }

    if(IN_Id_Algo)
    {
        if(IN_Algorithme)
            throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Mot cle Algorithme attendu");
        else
            throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Identificateur du programme attendu");
        // INFORMATION INCOPMLETE PROGRAM
    }
    else
    if(IN_Directives && !IN_Extention)
    {
        throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Nom d'extention attendu");
        // INFORMATION INCOPMLETE PROGRAM
    }
    else
    if(IN_HeaderProgram)
    {
        if(IN_Constantes)
        {
            if(IN_IDConst)
                throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Identificateur du constante attendu");
            else
            if(IN_OperatorConst)
                throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Mot cle '=' attendu");
            else
            if(IN_ConstValue)
                throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Valeur du constante attendu");
            else
                throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Debut du programme attendu");

        }
        else
        if(IN_Types)
        {
            if(IN_Id_Type)
                throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Identificateur du type attendu");
            else
            if(IN_OpertatorType)
                throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Mot cle '=' attendu");
            else
            if(IN_DefType)
            {
                throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Deffinition du type attendu");
            }
            else
            if(IN_Enreg)
            {
                if(IN_ID_Field)
                    throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Identificateur du champ d'enregistrement attendu");
                else
                if(IN_AssignType)
                    throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Mot cle ':' attendu");
                else
                if(IN_typeOfField)
                     throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Type de champ d'enregistrement attendu");
                else
                     throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Fin d'enregistrement attendu");
            }
            else
            if(IN_Tab)
            {
                if(IN_LI)
                    throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Deffinition du tableau attendu");
                else
                if(IN_InitSizeT)
                    throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Valeur initial du tableau attendu");
                else
                if(IN_Punctiation)
                     throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Mot cle '..' attendu");
                else
                if(IN_size_c)
                     throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Taille du tableau attendu");
                else
                if(IN_RI)
                     throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Mot cle ']' attendu");
                else
                if(IN_DE_Key)
                     throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Mot cle de attendu");
                else
                     throw SyntaxicError(lex->currentLine(),lex->currentColumn()," Type de contenu du tableau attendu");
            }
            else
            if(IN_Mat)
            {
                if(IN_LI)
                     throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Deffinition du Matrice attendu");
                else
                if(IN_InitSizeT)
                     throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Valeur initial du Matrice attendu");
                else
                if(IN_Punctiation)
                     throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Mot cle '..' attendu");
                else
                if(IN_size_c)
                     throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Taille du Matrice attendu");
                else
                if(IN_size_l)
                     throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Nombre de conlonnes du Matrice attendu");
                else
                if(IN_RI)
                    throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Mot cle ']' attendu");
                else
                if(IN_DE_Key)
                    throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Mot cle de attendu");
                else
                    throw SyntaxicError(lex->currentLine(),lex->currentColumn()," Type de contenu du tableau attendu");
            }
            else
            if(IN_Fich)
            {
                if(IN_DE_Key)
                     throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Mot cle 'DE' attendu");
                else
                      throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Type de contenu du fichier attendu");

            }
            else
               throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Debut du programme attendu");
        }
        else
        if(IN_Variables)
        {
            if(IN_Id_Var)
                  throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Identificateur du variable attendu");
            else
            if(IN_AssignType)
                  throw SyntaxicError(lex->currentLine(),lex->currentColumn()," ':' attendu ");
            else
                  throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Type du variable attendu");
        }
        else
             throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Debut du programme attendu");
        // INFORMATION INCOPMLETE PROGRAM
    }
    else
    if(IN_Fct)
    {
        if(IN_id_fct)
             throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Identificateur du fonction attendu");
        else
        if(IN_Lp)
             throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"'(' attendu");
        else
        if(IN_formalArgs)
        {
            if(IN_IdArg)
              throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Identificateur d'argument attendu");
            else
            if(IN_AssignType)
               throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"':' attendu");
            else
            if(IN_typeOfArg)
               throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Type d'argument attendu");
            else
               throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"')' attendu");
        }
        else
        if(IN_Rp)
              throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"')' attendu");
        else
        if(IN_AssignType)
             throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"':' attandu");
        else
             throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Type de retour de fonction attendu");
    }
    else
    if(IN_Proc)
    {
        if(IN_IdProc)
             throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Identificateur du procedure attendu");
        else
        if(IN_Lp)
             throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"'(' attendu");
        else
        if(IN_formalArgs)
        {
            if(IN_IdArg)
               throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Identificateur d'argument attendu");
            else
            if(IN_AssignType)
               throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"':' attendu");
            else
            if(IN_typeOfArg)
               throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Type d'argument attendu");
            else
               throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"')' attendu");
        }
        else
              throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"')' attendu");
    }
    else
    if(IN_Debut)
    {
        throw SyntaxicError(lex->currentLine(),lex->currentColumn(),"Fin du programme attendu attendu");
    }

}

void SyntaxicAnalyser::Instructions(Statment * parentstatment,LexicalAnalyser * lex)
{
    bool IN_Si              = false;

    bool IN_Pour            = false;
        bool IN_Compter     = false;
        bool IN_TO_FOR      = false;
        bool IN_Vi          = false;
        bool IN_Vf          = false;
        bool IN_Step        = false;
        bool IN_StepValue   = false;
        bool IN_Faire       = false;

    bool IN_TanteQue        = false;

    bool IN_Repeter         = false;
        bool IN_Until       = false;
    bool IN_Selon           = false;
        bool IN_Case        = false;
        bool IN_Selector    = false;
    bool IN_returnFct       = false;
    bool IN_With            = false;

    bool IN_Expression      = false;
    bool IN_Alors           = false;
    bool IN_Else            = false;

    bool IN_CallProc        = false;

    bool IN_reference       = false;
    bool IN_End             = false;

    bool IN_AssignSymbol   = false;
    //
    IF_Statment * if_stat         = NULL;
    For_Statment * for_stat       = NULL;
    While_Statment * while_stat   = NULL;
    Repeat_Statment * repeat_stat = NULL;
    Swith_Statment * swith_stat   = NULL;
        Case_Statment * case_stat = NULL;
    Assign_Statment * assign_stat = NULL;
    With_Statment * with_stat     = NULL;
    CallProc_Statment * callProc_stat = NULL;

    Token * token = NULL;
    Token * id = NULL;

    while (token = lex->NextToken()) {
    ReloadAll:{
            if(!token)break;

            if(!IN_Si && !IN_Pour && !IN_TanteQue && !IN_Repeter && !IN_Selon && !IN_returnFct && !IN_CallProc && !IN_With && !IN_Else && !IN_AssignValue)
            {
                //qDebug()<<"Serching instriction behavior "<<token->toString();//<<!IN_Si << !IN_Pour << !IN_TanteQue << !IN_Repeter << !IN_Selon << !IN_returnFct << !IN_CallProc <<!IN_With << !IN_Else;
                if(toUpper(token->Value()) == Si)
                {
                    IN_Si = true;
                    IN_Expression = true;
                    //goto ReloadAll;
                }
                else
                if(toUpper(token->Value()) == Pour )
                {
                    IN_Pour = true;
                    IN_Compter = true;
                    for_stat = new For_Statment(parentstatment);
                }
                else
                if(toUpper(token->Value()) == TanteQue)
                {
                    IN_TanteQue = true;
                    IN_Expression = true;
                    while_stat = new While_Statment(parentstatment);
                    goto while_inst;

                }
                else
                if(toUpper(token->Value()) == Repeter)
                {
                    IN_Repeter = true;
                    repeat_stat = new Repeat_Statment(parentstatment);
                }
                else
                if(toUpper(token->Value()) == Selon )
                {
                    IN_Selon = true;
                    IN_Selector = true;
                    swith_stat = new Swith_Statment(parentstatment);
                }
                else
                if(toUpper(token->Value()) == Avec)
                {
                    IN_With = true;
                    IN_Selector = true;
                    with_stat  = new With_Statment(parentstatment);
                    goto with_stat;
                }
                else
                if(toUpper(token->Value()) == FIN || (static_cast<Repeat_Statment*>(parentstatment) && toUpper(token->Value()) == Jusqu_A) || ((static_cast<IF_Statment*>(parentstatment) || static_cast<Swith_Statment*>(parentstatment)) && (toUpper(token->Value()) == SINON || toUpper(token->Value()) == SI_NON)))
                {
                    return;
                }
                else
                if(toUpper(token->Value()) == Retourner)
                {
                    if(IN_Fct)
                    {
                        IN_returnFct  = true;
                        IN_Expression =true;
                        goto return_ftc_stat;
                    }
                    else
                      Error(token,"Intruction illegale : Retour de fonction interdit sous un autre sous programme .");
                }
                else
                if(token->Nature() == ID)
                {
                    if(toUpper(token->Value()) == id_program && IN_Fct){
                        //qDebug()<<"got return"<<token->toString();
                        token = lex->NextToken();
                        IN_returnFct = true;
                        IN_AssignValue = true;
                    }
                    else{
                        Identificateur identificateur = WhatIs(token);
                        if(identificateur.behavior == id_procedure)
                        {
                            IN_CallProc = true;
                            IN_IdProc = true;
                            goto callProc_stat;
                        }
                        else
                        if(identificateur.behavior == id_argment || identificateur.behavior == id_variable){
                            IN_AssignValue = true;
                            IN_reference = true;
                            goto AssignValue;
                        }
                        else
                        {
                            Error(token,"identification ambigue d'instruction : Utilisation d'identificateur incorrecte ou/et interdit.");
                        }
                    }

                }
                else
                    Error(token,"Instruction illegale : instruction male formé");
            }



            if(IN_Si)
            {
               if(IN_Expression)
               {
                   //qDebug()<<"BEGIN IF CONDITION "<<token->toString();
                   NodeExpression * condition = AnalyseExpression(lex);
                   token = lex->getCurrentToken();
                   //qDebug()<<"END IF CONDITION "<<token->toString();
                   if(condition){
                       if_stat = new IF_Statment(parentstatment);
                       if_stat->setCondition(condition);
                       IN_Expression = false;
                       IN_Alors = true;
                   }else{
                       Error(token,"Condition 'SI' attendu");
                   }

               }
               else
               if(IN_Alors)
               {
                  //qDebug()<< "IN ALORS IF "<<token->toString();
                  if(toUpper(token->Value()) == Alors)
                  {
                      Instructions(if_stat,lex);
                      token = lex->getCurrentToken();
                      if(toUpper(token->Value()) == SINON || toUpper(token->Value()) == SI_NON){
                          Statment * elseStatment = new Statment();
                          if_stat->setElseStatment(elseStatment);
                          Instructions(elseStatment,lex);

                      }
                      IN_Alors = false;
                      IN_End = true;
                      if_stat = NULL;
                  }
                  else
                      Error(token,"Instruction illegale : Mot cle 'Alors' attendu ");
                  //qDebug()<< "OUT ALORS IF "<<token->toString();
               }
               else
               if(IN_End)
               {
                    //qDebug()<<"IN END SI "<<token->toString();
                    if(toUpper(token->Value()) == Si)
                    {
                        IN_Si = false;
                        IN_End = false;
                        //qDebug()<<"OUT END SI "<<token->toString();
                    }
                    else
                        Error(token,"Instruction illeagel : Fin d'instruction attendu 'FIN SI");
               }
            }
            else
            if(IN_Pour)
            {
                for_inst:{
                   if(IN_Compter)
                   {
                        //qDebug()<<"BEGIN compter"<<token->toString();
                        NodeExpression * compter = AnalyseExpression(lex);
                        token = lex->getCurrentToken();
                        //qDebug()<<"END COMPTER"<<token->toString();
                        if(compter == NULL)
                            Error(token,"Instruction illeagel : Compteur du boucle FOR attendu ");
                        else
                        if(dynamic_cast<Reference*>(compter) == NULL){
                            Error(token,"Compteur n'est pas de type scalaire");
                        }
                        else
                        {
                            for_stat->setCompter(compter);
                            IN_Compter = false;
                            IN_DE_Key = true;
                            //qDebug()<<"OUT compter"<<t->toString();
                        }
                   }
                   else
                   if(IN_DE_Key)
                   {
                        //qDebug()<<"BEGIN de key"<<token->toString();
                        if(toUpper(token->Value()) == "DE")
                         {
                            IN_DE_Key = false;
                            IN_Vi = true;
                            //qDebug()<<"END de key "<<token->toString();
                            goto ReloadAll;
                         }
                         else
                            Error(token,"Instruction illeagel : Mot cle 'DE' attendu ");
                   }
                   else
                   if(IN_Vi)
                   {
                       //qDebug()<<"BEGIN VI -> "<< token->toString();
                       NodeExpression * vi = AnalyseExpression(lex);
                       token = lex->getCurrentToken();
                       //qDebug()<<"END vi "<< token->toString();
                       if(vi == NULL)
                           Error(token,"Instruction illeagel : Valuer initial du boucle FOR attendu ");
                       else
                       {
                           for_stat->setVi(vi);
                           IN_Vi = false;
                           IN_TO_FOR = true;

                           //qDebug()<<"OUT vi "<< t->toString();
                       }
                   }
                   else
                   if(IN_TO_FOR)
                   {
                       //qDebug()<<"IN_TO_FOR "<<t->toString();
                        if(toUpper(token->Value()) == "A")
                        {
                            IN_TO_FOR = false;
                            IN_Vf = true;

                            //qDebug()<<"OUT TO_FOR "<<t->toString();
                            goto ReloadAll;
                        }
                        else
                            Error(token,"Instruction illeagel : Mot cle 'A' de boucle 'Pour' attendu");
                   }
                   else
                   if(IN_Vf)
                   {
                       //qDebug()<<"BEGIN VF"<<token->toString();
                       NodeExpression * vf = AnalyseExpression(lex);
                       token = lex->getCurrentToken();
                       //qDebug()<<"END VF"<<token->toString();
                       if(vf == NULL)
                           Error(token,"Instruction illeagel : Valuer initial du boucle 'POUR' attendu ");
                       else
                       {
                           for_stat->setVf(vf);
                           IN_Vf = false;
                           IN_Faire = true;

                           //qDebug()<<"OUT VF"<<t->toString();
                       }

                   }
                   else
                   if(IN_Faire)
                   {
                        //qDebug()<<"IN FAIRE"<<t->toString();
                        if(toUpper(token->Value()) == "FAIRE")
                        {
                            IN_Faire = false;
                            IN_Step = true;

                            //qDebug()<< "OUT Faire"<<token->toString();
                        }
                        else
                            Error(token,"Instruction illeagel : Mot cle 'Faire' de boucle 'Pour' attendu");
                   }
                   else
                   if(IN_Step)
                   {
                       //qDebug()<<"IN pas "<<token->toString();
                       if(toUpper(token->Value()) == "PAS")
                       {
                           IN_Step = false;
                           IN_AssignValue = true;

                           //qDebug()<< "OUT PAS 1"<<token->toString();

                       }
                       else
                       {
                           token = lex->BackToken();
                           //qDebug()<<"here"<<token->toString();
                           Instructions(for_stat,lex);
                           IN_Step = false;
                           IN_End = true;

                           //qDebug()<< "OUT PAS 2"<<t->toString();
                       }
                   }
                   else
                   if(IN_AssignValue)
                   {
                       //qDebug()<< "IN ASIGN VALUE "<<t->toString();

                       if(token->Value() != "=")
                           Error(token,"Instruction illeagel : Mot cle '=' du pas de boucle 'Pour' attendu");
                       else
                       {
                           IN_AssignValue = false;
                           IN_StepValue = true;

                           //qDebug()<< "OUT ASIGN VALUE "<<t->toString();
                           goto ReloadAll;
                       }

                   }
                   else
                   if(IN_StepValue)
                   {
                       //qDebug()<<"BEGIN STEP VALUE "<<token->toString();
                       NodeExpression * stepValue = AnalyseExpression(lex);
                       token = lex->getCurrentToken();
                       //qDebug()<<"END STEP VALUE "<<token->toString();
                       for_stat->setStep(stepValue);
                       if(stepValue == NULL)
                           Error(token,"Instruction illeagel : Valeur du pas du boucle 'Pour' attendu");
                       else
                       {
                           Instructions(for_stat,lex);
                           parentstatment->addChildStatment(for_stat);

                           IN_StepValue = false;
                           IN_End = true;
                       }
                   }
                   else
                   if(IN_End)
                   {
                       //qDebug()<<"IN_end pour"<<t->toString();
                       if(toUpper(token->Value()) == Pour)
                       {
                           IN_Pour = false;
                           //qDebug()<<"OUT end pour"<<t->toString();
                       }
                       else
                            Error(token,"Instruction illegal : Fin d'instrction attendu 'FIN POUR' ");
                   }

                }
            }
            else
            if (IN_TanteQue)
            {
                while_inst:{
                    if(IN_Expression)
                    {
                        //qDebug()<<"BEGIN Condition while " <<token->toString();
                        NodeExpression * condition = AnalyseExpression(lex);
                        token = lex->getCurrentToken();
                        //qDebug()<<"END CONDI WHILE"<<token->toString();
                        if(condition == NULL)
                            Error(token,"Instruction illeagel : Condtion de l ' instruction 'TanteQue' attendu");
                        else
                        {
                            while_stat->setCondition(condition);
                            IN_Expression = false;
                            IN_Faire = true;
                            //qDebug()<<"OUT Condition while " <<token->toString();
                        }
                    }
                    else
                    if(IN_Faire)
                    {
                        //qDebug()<<"IN FAIRE while"<<t->toString();
                        if(toUpper(token->Value()) == Faire)
                        {
                            //endToken = NULL;
                            Instructions(while_stat,lex);

                            IN_Faire = false;
                            IN_End = true;
                            //qDebug()<<"OUT FAIRE while"<<t->toString();

                        }else
                            throw SyntaxicError(token->getLineNumber(),token->getColumnNumber(),"Mot cle 'FAIRE ' Attendu ");

                    }
                    else
                    if(IN_End)
                    {
                        //qDebug()<<"IN END TANTE QUE"<<t->toString();
                        if(toUpper(token->Value()) == TanteQue)
                        {
                            IN_TanteQue = false;
                            //qDebug()<<"OUT END TANTE QUE"<<t->toString();
                        }
                        else
                            throw SyntaxicError(token->getLineNumber(),token->getColumnNumber(),"Mot cle 'FIN TANTEQUE ' Attendu ");
                    }
                }
            }
            else
            if (IN_Repeter)
            {
                repeat_inst:{

                    if(IN_Until)
                    {
                        //qDebug()<<"IN JUSQ "<<t->toString();
                        NodeExpression * condition = AnalyseExpression(lex);
                        token = lex->getCurrentToken();
                        if(condition == NULL)
                            Error(token,"Instruction illeagel : Condtion de l instruction 'Repeter' attendu");
                        else{
                            repeat_stat->setCondition(condition);
                            IN_Repeter = false;
                            IN_Until = false;
                            //qDebug()<<"OUT JUSQ "<<t->toString();
                            //goto ReloadAll;
                        }
                    }
                    else
                    {
                        //qDebug()<< "IN CORP REPERTER"<<t->toString();
                        //endToken = NULL;
                        Instructions(repeat_stat,lex);
                        IN_Until = true;
                        //qDebug()<< "OUT CORP REPETER "<< t->toString();
                        goto repeat_inst;
                    }
                }
                // ...
            }
            else
            if (IN_Selon)
            {
                switch_stat:{
                    if(IN_Selector)
                    {
                        //qDebug()<<"IN_Selector"<<token->toString();
                        NodeExpression * selector = AnalyseExpression(lex);
                        token = lex->getCurrentToken();
                        if(!selector)
                           Error(token,"Instruction illeagel : Selecteur de choix multiple attendu ");
                        else
                        {
                            swith_stat->setSelector(selector);
                            IN_Selector = false;
                            IN_Faire  = true;
                        }
                    }
                    else
                    if(IN_Faire)
                    {
                        //qDebug()<<"IN_Faire"<<token->toString();
                        if(toUpper(token->Value()) != Faire)
                            Error(token,"Instruction illeagel : Mot cle 'Faire' attendu ");
                        else
                        {
                            IN_Faire = false;
                            IN_Case = true;
                          }
                    }
                    else
                    if(IN_Case)
                    {
                        //qDebug()<<"IN_Case"<<token->toString();
                        if(toUpper(token->Value()) == "FIN"){
                            IN_End = true;
                            IN_Case = false;
                        }
                        else
                        if(toUpper(token->Value()) == "SINON" || toUpper(token->Value()) == "SI_NON"){
                            IN_Else = true;
                            IN_Case = false;
                            goto switch_stat;
                        }
                        else
                        if(token->Nature() == Reel || token->Nature() == NUMBER || token->Nature() == CHAR || toUpper(token->Value()) == VRAI || toUpper(token->Value()) == FAUX)
                        {
                            try{
                                case_stat = new Case_Statment(swith_stat,token);
                            }catch(std::string error){
                                Error(token,error);
                            }

                            IN_Case = false;
                            IN_AssignType = true;
                        }
                        else
                            Error(token,"Le valeur '" + token->Value() + "' ne forme pas un choix conditionel pour 'Selon .. Faire'");
                    }
                    else
                    if(IN_AssignType)
                    {
                        //qDebug()<<"IN_AssignType"<<token->toString();
                        if(token->Nature() != AssignType)
                            Error(token,"Instruction illeagel : Mot cle ':' attendu ");
                        else
                        {
                            IN_AssignType = false;
                            Instructions(case_stat,lex);
                            swith_stat->addCaseStatment(case_stat);
                            //qDebug()<<"END CAS STATMEN TOKEN "<<lex->currentToken->toString();
                            IN_Case = true;
                        }
                    }else
                    if(IN_Else){
                        //qDebug()<<"IN_Else selon "<<token->toString();
                        Statment * elseStatment = new Statment(swith_stat);
                        Instructions(elseStatment,lex);
                        swith_stat->setElseStatment(elseStatment);
                        IN_End = true;
                        IN_Else = false;
                    }
                    else
                    if(IN_End){
                        //qDebug()<<"IN_End Selon"<<token->toString();
                        if(toUpper(token->Value()) == "SELON"){
                            IN_Selon = false;
                            IN_End = false;
                        }
                        else
                            Error(token,"Instruction illeagel : 'Fin Selon' attendu ");
                    }
                }
            }
            else
            if (IN_returnFct)
            {
                return_ftc_stat:{

                    if(IN_AssignValue)
                    {
                        //qDebug()<<"IN_AssignValue"<<token->toString();
                        if(token->Nature() == AssignValue){
                            IN_AssignValue = false;
                            IN_Expression = true;
                            goto return_ftc_stat;
                        }else
                            Error(token,"Expression incomplete : Symbole '<--' attendu");
                    }
                    else
                    if(IN_Expression)
                    {

                        NodeExpression * returnValue = AnalyseExpression(lex);

                        if(returnValue){

                            try{
                                f->setReturnValue(returnValue);
                            }catch(std::string error){
                                Error(token,error);
                            }

                            IN_Expression = false;
                            IN_returnFct = false;

                        }else{
                                Error(token,"Instruction incomplete : Valeur de retour de fonction attendu");
                        }

                    }
                }
            }
            else
            if (IN_CallProc)
            {
                callProc_stat:{
                    //qDebug()<<"IN callProc_stat "<<token->toString();
                    if(IN_IdProc)
                    {
                        //qDebug()<<"IN_IdProc"<<token->toString();
                        callProc_stat = new CallProc_Statment(parentstatment);
                        callProc_stat->setIdProcedure(toUpper(token->Value()));
                        IN_IdProc = false;
                        IN_Lp = true;
                    }
                    else
                    if(IN_Lp)
                    {
                        //qDebug()<<"IN_Rp"<<token->toString();

                        if(token->Nature() == Lp){
                            IN_Lp = false;
                            IN_EffectArgs = true;
                            goto ListEfArgs;
                        }else
                            Error(token,"Instruction illeagel : Symbole '(' attendu");
                    }
                    else
                    if(IN_EffectArgs){
                        //qDebug()<<"IN_EffectArgs"<<token->toString();
                        ListEfArgs:
                        {
                            do{
                                NodeExpression * arg = AnalyseExpression(lex);
                                token = lex->NextToken();
                                if(arg){
                                   callProc_stat->addEffectifArg(arg);
                                }else
                                if(!token){
                                     Error(lex->BackToken(),"Argument effective requis");
                                }else
                                if(callProc_stat->hasEffectifArgs())
                                     Error(token,"Argument effective requis");

                            }while(token->Value() == ",");

                            lex->BackToken();
                            IN_EffectArgs = false;
                            IN_Rp = true;
                        }
                    }
                    else
                    if(IN_Rp)
                    {
                        //qDebug()<<"IN_Lp"<<token->toString();
                        if(token->Nature() == Rp){
                            IN_CallProc = false;
                            IN_Lp = false;
                        }else{
                            Error(token,"Symbole ')' attendu");
                        }
                    }
                }
            }
            else
            if (IN_With)
            {
                with_stat:{
                    if(IN_Selector)
                    {
                        //qDebug()<<"BEGIN with selector "<<token->toString();
                        NodeExpression * select = AnalyseExpression(lex);
                        token = lex->getCurrentToken();
                        //qDebug()<<"END with selector "<<token->toString();
                        if(select)
                        {
                            with_stat->setSelector(select);
                            IN_Selector = false;
                            IN_Faire = true;
                            //qDebug()<< " OUT Selector "<< t->toString();
                        }
                        else
                            Error(token,"Instruction illegale : Selecteur d instruction avec attendu ");
                    }
                    else
                    if(IN_Faire)
                    {
                        //qDebug()<< "IN Faire with "<<token->toString();
                        if(toUpper(token->Value()) == Faire)
                        {
                            IN_Faire = false;
                            //endToken = NULL;
                            Instructions(with_stat,lex);
                            IN_Faire = false;
                            IN_End = true;
                            //qDebug()<< "OUT Faire with "<<t->toString();
                        }
                        else
                            Error(token,"Instrcution illeagle  : Mot lce 'Faire ' attentdu ");
                    }
                    else
                    if(IN_End)
                    {
                        //qDebug()<< "IN END with "<<t->toString();
                        if(toUpper(token->Value()) == Avec)
                        {
                            IN_End = false;
                            IN_With = false;
                            //qDebug()<< "OUT end with "<<t->toString();
                        }
                        else
                            Error(token,"Instruction illegale : Fin d instruction AVEC attendu 'FIN AVEC'");
                    }
                }
            }
            else
            if(IN_AssignValue)
            {
                AssignValue:{
                    if(IN_reference){
                        token = lex->BackToken();
                        //qDebug()<<"IN_reference"<<token->toString();
                        NodeExpression * expression = AnalyseExpression(lex);
                        if(!expression){
                            Error(token,"Instruction illeagel : form d'instrcution aimbigue");
                        }
                        else
                        if(static_cast<Reference*>(expression)){
                            assign_stat = new Assign_Statment(parentstatment);
                            assign_stat->setAssignContainer(static_cast<Reference*>(expression));
                            IN_reference = false;
                            IN_AssignSymbol = true;
                        }else
                            Error(token,"form d'instrcution aimbigue");
                    }
                    else
                    if(IN_AssignSymbol){
                        //qDebug()<<"IN_AssignSymbol"<<token->toString();
                        if(token->Nature() == AssignValue){
                            IN_AssignSymbol = false;
                            IN_Expression = true;
                            goto AssignValue;
                        }else{
                            Error(token,"form d'instruction aimbigue ou symbole '<--' attendu" );
                        }
                    }
                    else
                    if(IN_Expression)
                    {
                        //qDebug()<<"IN expression assign "<<token->toString();
                        NodeExpression * expression = AnalyseExpression(lex);
                        token = lex->getCurrentToken();
                        if(expression)
                        {
                            assign_stat->setAssignValue(expression);
                            IN_AssignValue = false;
                            IN_Expression = false;
                            //qDebug()<<"OUT expression assign "<<t->toString();
                        }
                        else
                            Error(token,"Instrcution illegale : Valeur de l'affectation attendu ");
                    }
                }
            }
        }


    }

    token = lex->BackToken();

    if /*_*/(IN_Si) {
        throw SyntaxicError(token->getLineNumber(),token->getColumnNumber(),"Syntaxe incorrecte 'SI..ALORS'  ");
    }
    else if (IN_Pour){
        throw SyntaxicError(token->getLineNumber(),token->getColumnNumber(),"Syntaxe incorrecte 'Pour .. FAIRE' ");
    }
    else if (IN_TanteQue){
        throw SyntaxicError(token->getLineNumber(),token->getColumnNumber(),"Syntaxe incorrecte 'Tanteque .. FAIRE' ");
    }
    else if (IN_Repeter){
        throw SyntaxicError(token->getLineNumber(),token->getColumnNumber(),"Syntaxe incorrecte 'Repeter .. Jusqu_a' ");

    }
    else if (IN_Selon){
        throw SyntaxicError(token->getLineNumber(),token->getColumnNumber(),"Syntaxe incorrecte 'Selon .. Faire' ");
    }
    else if (IN_returnFct){
        throw SyntaxicError(token->getLineNumber(),token->getColumnNumber(),"Syntaxe incorrecte 'Retourn fct' ");

    }
    else if (IN_CallProc){
        throw SyntaxicError(token->getLineNumber(),token->getColumnNumber(),"Syntaxe incorrecte 'call proc' ");

    }
    else if (IN_With){
        throw SyntaxicError(token->getLineNumber(),token->getColumnNumber(),"Syntaxe incorrecte 'Avec .. Faire' ");

    }
    else if (IN_Else) {
        throw SyntaxicError(token->getLineNumber(),token->getColumnNumber()," Syntaxe incorrecte 'SI NON ..' ");
    }
    else
    if(token)   // SI 'FIN'EST DETECTE OU NON
    {
        if(parentstatment->GetBehavior() != (StatmentBehavio)0) // SI LA MERE EST DIFFERENT A L'INSTRUCTION MERE ('DEBUT')
        {
            if((token = lex->NextToken()))
              {
                    switch (parentstatment->GetBehavior()) {
                    case FOR   :if(toUpper(token->Value()) != Pour)
                                    throw SyntaxicError(token->getLineNumber(),token->getColumnNumber(),"Mot cle 'FIN POUR' attendu ");
                        break;
                    case WHILE :if(toUpper(token->Value()) != TanteQue)
                                    throw SyntaxicError(token->getLineNumber(),token->getColumnNumber(),"Mot cle 'TANTEQUE' attendu ");
                        break;
                    case REPEAT:if(toUpper(token->Value()) != Jusqu_A)
                                    throw SyntaxicError(token->getLineNumber(),token->getColumnNumber(),"Mot cle 'JUSQU_A' attendu ");
                                else
                                {
                                    // expression
                                }
                        break;
                    case IF    :
                    case ELSE  :if(toUpper(token->Value()) != Si)
                                    throw SyntaxicError(token->getLineNumber(),token->getColumnNumber(),"Mot cle 'FIN SI' attendu ");
                        break;
                    case WITH  :
                    case CALL_PROCEDURE:break;
                    default:
                        break;
                    }
                    return;
             }

        }
        else // EN CAS DE L'INSTRUCTION MERE ('DEBUT')
            return;
    }
    // EN DE L'INSTRUCTION MERE ('DEBUT') N 'EST PAS ENCORE DETECTE
    //throw SyntaxicError(t->getLineNumber(),t->getColumnNumber(),"Mot cle 'FIN' attendu ");



}


NodeExpression * SyntaxicAnalyser::AnalyseExpression(LexicalAnalyser *lex)
{

    bool IN_Expression              = false;
        bool IN_Operator  = false;

    bool IN_ExpressionWithSignelOp  = false;
    bool IN_reference               = false;
        bool IN_IDref               = false;
        bool IN_ByteOperator        = false;
    bool IN_ConstValue              = false;
    bool IN_CallFunction            = false;
        bool IN_EffectefArgs        = false;
        bool IN_EffectArg           = false;

    // UTILISATION DOIT ETRE RECURSIVE ...
    NodeExpression * expression = NULL;
    Token * token = NULL;

    while (token = lex->NextToken()) {

        ReloadAll:{
            //qDebug()<<"HERE RELOAD ALL "<< token->toString() <<endl;

            if (!IN_Expression && !IN_Operator && !IN_ExpressionWithSignelOp)
            {
                //qDebug()<<"Searching expression direction"<<token->toString();

                if(token->Nature() == Lp && expression == NULL)
                {
                    NodeExpression * expressionBetweenParenthese = AnalyseExpression(lex);
                    token = lex->NextToken();

                    if(token){
                        if(token->Nature() != Rp)
                                Error(token,"Expression illegal : Spearateur ' ) ' attendu a la fin d expression ");
                        if(expressionBetweenParenthese == NULL)
                                Error(token,"Expression illegal : Expression vide '( )' ");
                    }else{
                        Error(lex->BackToken(),"Expression illegal : Spearateur ' ) ' attendu a la fin d expression ");
                    }


                    expression = new Expression(expressionBetweenParenthese);
                    IN_Operator = true;

                }
                else
                if(expression == NULL && listExSignelOperators.contains(toUpper(token->Value())))
                {
                    IN_ExpressionWithSignelOp = true;
                    goto ExpressionWithSignelOp;
                }
                else
                if(expression == NULL)
                {
                    IN_Expression = true;
                    goto _Expression;
                }
                else
                if(listExOPerators.contains(toUpper(token->Value())))// LOOOL :) "&& expression != NULL" :))))
                {
                    IN_Operator = true;
                    goto ExpressionOperator;
                }
                else
                {
                    // Identification d'erreur et le propager ..
                }


            }
            else
            if (IN_Operator)
            {
                ExpressionOperator:{
                    //qDebug()<<"IN_Expression_Operator "<<token->toString();
                    if(listExOPerators.contains(toUpper(token->Value())))// CURRENT TOKEN IT IS AN OPERATOR
                    {
                        if(expression->hasExpressionOperator())
                        {
                            Error(token,"Expression illegal : Duplication d 'operateur d 'expression '" + expression->getExpressionOperator()->value->Value() + "'");
                        }
                        else
                        {

                            NodeExpression::ExpressionOperator *operatorExp = new NodeExpression::ExpressionOperator();

                            operatorExp->value = token;
                            operatorExp->FirstOperand  = expression;
                            operatorExp->SecondOperand = AnalyseExpression(lex);
                            token = (lex->getCurrentToken())?lex->getCurrentToken():lex->getLastToken();
                            if(operatorExp->SecondOperand == NULL)
                                Error(token," Expression incomplete : deuxieme facteur de l'operateur '" + operatorExp->value->Value() + "' attendu  ");

                            expression->setExpressionOperator(operatorExp);

                            goto returnExpression;
                        }
                    }
                    else
                    if(expression != NULL)
                    {
                        if(expression->hasExpressionOperator())
                        {
                            if(expression->getExpressionOperator()->SecondOperand == NULL){
                                Error(token,"Expression incomplete : deuxieme facteur de l'operateur '" + expression->getExpressionOperator()->value->Value() + "' attendu ");
                            }
                            else{
                                goto returnExpressionWithBackToken;
                            }

                        }else{
                            goto returnExpressionWithBackToken;
                        }
                    }
                    else
                    {
                        goto returnExpressionWithBackToken;
                    }

                }
            }
            else
            if (IN_Expression)
            {
                _Expression:{
                    //qDebug()<<"IN_Expression "<< token->toString();
                    if (!IN_reference && !IN_CallFunction && !IN_ConstValue)
                    {
                        if(token->Nature() == ID )
                        {
                            IN_reference = true;
                            IN_IDref = true;
                            goto _Reference;
                        }
                        else
                        if(token->Nature() == KeyWord && token->Value() == FN)
                        {
                            IN_CallFunction = true;
                            IN_id_fct = true;
                        }
                        else
                        if(token->Nature() == Reel || token->Nature() == NUMBER || token->Nature() == STR || token->Nature() == CHAR || toUpper(token->Value()) == "VRAI" || toUpper(token->Value()) == "FAUX" || toUpper(token->Value()) == "NIL" || toUpper(token->Value()) == "NULLE")
                        {
                            IN_ConstValue = true;
                            goto _ConstValue;
                        }
                        else
                        if(listExOPerators.contains(token->Value()))
                        {
                            Error(token,"Expression illegal : Premier facteur de l 'expression attendu ");
                        }
                        else
                            goto returnExpressionWithBackToken;
                    }
                    else
                    if (IN_reference)
                    {
                        _Reference:{
                            //qDebug()<< "IN_Reference"<<token->toString();
                            if(IN_ByteOperator)
                            {
                                if(token->Nature() == LocationSpecifier)
                                {
                                    dynamic_cast<Reference*>(expression)->setByteOperator(new Reference::ByteOperator(token));
                                }
                                else
                                if(token->Nature() == LI)
                                {
                                    Reference::ByteOperator * aux = new Reference::ByteOperator(token);

                                    NodeExpression * index = AnalyseExpression(lex);
                                    token = lex->NextToken();
                                    if(index)
                                    {
                                        aux->setArgment(index);
                                        if(token)
                                        {
                                            if(token->Nature() != RI)
                                                Error(token,"Reference illegal : Mot ']' attendu");
                                           // t = lex->currentToken;
                                        }
                                        else
                                            Error(token," Expression incomplete : Mot ']' attendu au fin de l'expression ");
                                    }
                                    else
                                        Error(token,"Expression illegale : Argument de l'operateur bite '[]' atttendu ");
                                    dynamic_cast<Reference*>(expression)->setByteOperator(aux);

                                }
                                else
                                if(token->Value() == ".")
                                {
                                    Reference::ByteOperator * aux = new Reference::ByteOperator(token);
                                    Token * errorToken = token; // TRY/CHECK TO ELIMINATE THAT FUCKING SOON ( 10 LIGNES (WITHOUT BLANKS ) AFTER ) .
                                    token = lex->NextToken();// JE DOUTE ICI

                                    if(token)
                                    {
                                        if(token->Nature() == ID)
                                            aux->setArgment(new Reference(token));
                                        else
                                            Error(token,"Reference illegale : Argument de loperateur bite '.' incorrecte ");
                                    }
                                    else
                                        Error(errorToken,"Reference illegale : Argument de l'operateur bite '.' attendu a la fin de l expression ");

                                    dynamic_cast<Reference*>(expression)->setByteOperator(aux);
                                }
                                else
                                {
                                    IN_Expression = false;
                                    IN_reference = false;
                                    IN_Operator = true;
                                    goto ReloadAll;
                                }
                            }
                            else
                            if(IN_IDref)
                            {
                                if(token->Nature() == ID)
                                {
                                    expression = new Reference(token);
                                    IN_IDref = false;
                                    IN_ByteOperator = true;
                                }
                                else
                                    Error(token,"Expression illegale : reference attendu ");
                            }
                        }
                    }
                    else
                    if (IN_CallFunction)
                    {
                        CallFunction:{
                            //qDebug()<< "( A-Ex ))IN_CallFct "<<token->toString();
                            if(IN_id_fct)
                            {
                                if(token->Nature() == ID)
                                {
                                    expression = new FctCall(token->Value());
                                    IN_id_fct = false;
                                    IN_Lp = true;
                                }
                                else
                                    Error(token,"Expression illegal : Identificateur de fonction a apeller attendu");
                            }
                            else
                            if(IN_Lp)
                            {

                                if(token->Nature() == Lp)
                                {
                                    IN_Lp = false;
                                    IN_EffectefArgs = true;
                                    IN_EffectArg  = true;
                                    goto CallFunction;
                                }
                                else
                                    Error(token,"Expression illegal : '(' attendu ");
                            }
                            else
                            if(IN_EffectefArgs)
                            {
                               // qDebug()<<"IN_EffectefArgs "<<token->toString();

                                ListEfArgs:
                                {
                                    do{
                                        NodeExpression * arg = AnalyseExpression(lex);
                                        token = lex->getCurrentToken();
                                        if(!arg){
                                           if(dynamic_cast<FctCall*>(expression)->hasEffectArgs())
                                               Error(token,"Expression illegal : Argument effective requis ");

                                        }
                                        else{
                                            dynamic_cast<FctCall*>(expression)->AddEffectArg(arg);
                                        }
                                        token = lex->NextToken();
                                        if(!token) break;

                                    }while(token->Value() == ",");

                                    IN_EffectefArgs = false;
                                    goto CallFunction;
                                }
                            }
                            else
                            {
                                //qDebug()<< "IN_CallFunction ')'" << token->toString();
                                if(token)
                                {
                                    if(token->Nature() == Rp)
                                    {
                                        IN_CallFunction = false;
                                        IN_Expression = false;
                                        IN_Operator = true;
                                    }
                                    else
                                    if(dynamic_cast<FctCall*>(expression)->hasEffectArgs() && token->Value() == ",")
                                        Error(token,"Expression illegal : Un argument effective requis ");
                                    else
                                        Error(token,"Expression illegal : ')' attendu ");

                                }
                                else
                                    Error(new Token(lex->currentLine(),lex->currentColumn()),"Expression illegal : ')' attendu ");
                            }
                        }
                    }
                    else
                    if (IN_ConstValue)
                    {
                        _ConstValue:{
                            //qDebug()<< "( A-Ex ))IN_ConstValue "<<token->toString();
                            expression = new ConstValue(token);
                            IN_ConstValue = false;
                            IN_Expression = false;
                            IN_Operator = true;
                        }
                    }
                }

            }
            else
            if(IN_ExpressionWithSignelOp)
            {
                ExpressionWithSignelOp:{
                    Token * opval = token;
                    NodeExpression * node = AnalyseExpression(lex);
                    token = lex->getCurrentToken();
                    if(node)
                    {
                        expression =  new ExpressionWithSignelOperator(node,opval);
                        goto returnExpression;
                    }
                    else
                        Error(token,"Argmant de loperateur ' " + opval->Value() + "' attendu ");

                }

            }

        }

    }

    if(expression != NULL)
    {
        if(expression->hasExpressionOperator())
        {
            if(expression->getExpressionOperator()->SecondOperand == NULL)
                 Error(token,"Expression incomplete : deuxieme facteur de l'operateur '" + expression->getExpressionOperator()->value->Value() + "' attendu ");
        }
    }

    returnExpressionWithBackToken:{
        token = lex->BackToken();
        //if(token)
         //   qDebug()<<"Expression : " <<  (expression->toString()) <<" \n END TOKEN : " <<  (token->Value()) << "\n ****************************************************";
        return expression;
    }

    returnExpression:{
        //if(token)
         //   qDebug()<<"Expression : " <<  (expression->toString()) <<" \n END TOKEN : " <<  (token->Value()) << "\n ****************************************************";
        return expression;
    }
}

Identificateur SyntaxicAnalyser::WhatIs(Token * idToken)
{
    if(idToken->Nature() != ID)
        throw SyntaxicError(idToken->getLineNumber(),idToken->getColumnNumber(),"'" + idToken->Value() + "' n est pas un identificateur ");
    
    Identificateur id;
    id.value = toUpper(idToken->Value());
    id.token = *idToken;
     if(IN_Proc)
        {
             if(p->hasArg( (id.value)))
                 id.behavior = id_argment;
             else
             if(p->hasConstant( (id.value)))
                 id.behavior = id_constante;
             else
             if(p->hasType( (id.value)))
                 id.behavior = id_Type;
             else
             if(p->hasVariable( (id.value)))
                 id.behavior = id_variable;
             else
             if(Compilateur::ParseTree->hasFunction( (id.value)))
             {
                 id.behavior = id_Fucntion;
             }
             else
             if(Compilateur::ParseTree->hasProcedure( (id.value)))
             {
                 id.behavior = id_procedure;
             }
             else
                 throw SyntaxicError(idToken->getLineNumber(),idToken->getColumnNumber(),"identificateur '" + idToken->Value() + "' n est pas declare dans ce programme ");
         }
     else
     if(IN_Fct)
         {
             if(f->hasArg( (id.value)))
                 id.behavior = id_argment;
             else
             if(f->hasConstant( (id.value)))
                 id.behavior = id_constante;
             else
             if(f->hasType( (id.value)))
                 id.behavior = id_Type;
             else
             if(f->hasVariable( (id.value)))
                 id.behavior = id_variable;
             else
             if(Compilateur::ParseTree->hasFunction( (id.value)))
             {
                 id.behavior = id_Fucntion;
             }
             else
                 throw SyntaxicError(idToken->getLineNumber(),idToken->getColumnNumber(),"identificateur '" + idToken->Value() + "' n est pas declare dans ce programme ");
         }
     else
         {
             if(Compilateur::ParseTree->hasConstant( (id.value)))
                 id.behavior = id_constante;
             else
             if(Compilateur::ParseTree->hasType( (id.value)))
                 id.behavior = id_Type;
             else
             if(Compilateur::ParseTree->hasVariable( (id.value)))
                 id.behavior = id_variable;
             else
             if(Compilateur::ParseTree->hasFunction( (id.value)))
             {
                 id.behavior = id_Fucntion;
             }
             else
             if(Compilateur::ParseTree->hasProcedure( (id.value)))
             {
                 id.behavior = id_procedure;
             }
             else
                 throw SyntaxicError(idToken->getLineNumber(),idToken->getColumnNumber(),"identificateur '" + idToken->Value() + "' n est pas declare dans ce programme ");

         }
     return id;
    
}

