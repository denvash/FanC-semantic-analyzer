#ifndef _236360_3_
#define _236360_3_

#include <vector>
#include <string>
using namespace std;

namespace output
{
  void endScope();
  void printID(const string &id, int offset, const string &type);

  /* Do not save the string returned from this function in a data structure
        as it is not dynamically allocated and will be destroyed(!) at the end of the calling scope.
    */
  string makeFunctionType(const string &retType, vector<string> &argTypes);

  void errorLex(int lineno);
  void errorSyn(int lineno);
  void errorUndef(int lineno, const string &id);
  void errorDef(int lineno, const string &id);
  void errorUndefFunc(int lineno, const string &id);
  void errorMismatch(int lineno);
  void errorPrototypeMismatch(int lineno, const string &id, vector<string> &argTypes);
  void errorUnexpectedBreak(int lineno);
  void errorUnexpectedContinue(int lineno);
  void errorMainMissing();
  void errorByteTooLarge(int lineno, const string &value);

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


    class Variable: public Node{
    public:
        bool is_func;
        int size;
        TypeEnum type;
        vector<TypeEnum> arguments_types;
        Variable(int number){
            value=number;
        }
    };


#define YYSTYPE Node*
} // namespace output

#endif
