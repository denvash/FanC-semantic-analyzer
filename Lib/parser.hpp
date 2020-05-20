#ifndef PARSER_HPP
#define PARSER_HPP


#include "semantic_table.hpp"
//#include "output.hpp";
using namespace std;
//using namespace output;

extern SemanticTable semantic_table;
extern int yylineno;

class Node
{
public:
    virtual int get_value()
    {
        return 0;
    }
    virtual TypeEnum get_type()
    {
        return TYPE_UNDEFINED;
    }
};

class Num : public Node{
public:
    int value;
    Num(int number){
        value=number;
    }
};

class String : public Node{
public:
    char** value
    String(char** name){
        value=name;
    }
};

class Type : public Node{
public:
    TypeEnum type;
    Type(TypeEnum typeE){
        type=typeE;
    }
};

#define YYSTYPE Node*


//class Variable: public Node{
//public:
//    bool is_func;
//    int size;
//    TypeEnum type;
//    vector<TypeEnum> arguments_types;
//    Variable(int number){
//        value=number;
//    }
//};



#endif /* PARSER_HPP */