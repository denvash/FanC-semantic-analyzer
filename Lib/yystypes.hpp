//
// Created by Emil on 20/05/2020.
//

#ifndef FANC_SEMANTIC_ANALYZER_YYSTYPES_HPP
#define FANC_SEMANTIC_ANALYZER_YYSTYPES_HPP

#include "semantic_table.hpp"
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
    string value;
    String(string name){
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


#define YYSTYPE Node*
#endif //FANC_SEMANTIC_ANALYZER_YYSTYPES_HPP
