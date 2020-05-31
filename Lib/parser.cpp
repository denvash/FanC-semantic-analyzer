#include "parser.hpp"

inline void debugParser(const char *text)
{
  debug("parser", text);
}

void init_program()
{
  table.init_table();
  table.open_scope();

  /* Insert print and printi to global scope */
  vector<TypeEnum> func_args;
  func_args.push_back(TYPE_STRING);

  /* info type:
    bool is_func;
    int size;
    TypeEnum type;
    vector<TypeEnum> arg_types;
  */
  var_info_t print_func = {true, 0, TYPE_VOID, func_args};

  func_args.pop_back();
  table.insert("print", &print_func, true);

  func_args.push_back(TYPE_INT);
  var_info_t printi_func = {true, 0, TYPE_VOID, func_args};
  table.insert("printi", &printi_func, true);
}

void close_scope()
{
  // debugParser("[parser:close-scope] closing scope");
  output::endScope();
  auto entries = table.get_current_inner_scope();
  for (int i = 0; i < entries.size(); i++)
  {
    auto type = entries[i]->type_info.type;
    auto type_str = type_to_string_map[type];
    auto id = entries[i]->name;
    auto offset = entries[i]->offset;

    // cout << "[parser:entry-id] " << id << endl;

    if (entries[i]->type_info.is_func)
    {
      auto function_entry = entries[i];
      vector<string> func_args;
      for (int i = 0; i < function_entry->type_info.arg_types.size(); i++)
      {
        auto arg_str = type_to_string_map[function_entry->type_info.arg_types[i]];
        arg_str = (arg_str == "VOID") ? "" : arg_str;
        func_args.push_back(arg_str);
      }
      auto function_return_type_str = type_to_string_map[function_entry->type_info.type];
      type_str = output::makeFunctionType(function_return_type_str, func_args);
      /* print special type */
    }

    output::printID(id, offset, type_str);
  }
  table.close_scope();
  // debugParser("[parser:close-scope] scope closed after printing IDs");
  /* print types */
}

void close_program()
{
  // debugParser("close program");
  if (!table.is_func_exists("main"))
  {
    // debugParser("No func id_is_exists");
    err(output::errorMainMissing);
  }
  if (table.get_function_args("main").front() != TYPE_VOID)
  {
    // debugParser("Main with args");
    err(output::errorMainMissing);
  }
  if (table.get_function_type("main") != TYPE_VOID)
  {
    // debugParser("wrong type");
    err(output::errorMainMissing);
  }
  close_scope();
}

void return_value_check(TypeEnum return_type)
{
  // debugParser("return value check");
  bool validConversion = converstion_map[return_type][table.get_current_function_type()];
  if (!validConversion)
  {
    err(output::errorMismatch, yylineno);
  }
}

void func_init(atom_t y_identifier, atom_t y_arguments)
{
  // debugParser("declaring function");
  // cout << "[parser:function] " << *y_identifier.STRING << endl;
  if (table.id_is_exists(*y_identifier.STRING))
  {
    err(output::errorDef, yylineno, *y_identifier.STRING);
  }
  var_info_t func;
  func.is_func = true;
  func.type = y_identifier.TYPE;

  /* if no arguments to function, args is null */
  if (y_arguments.NODE == NULL)
  {
    // debugParser("In declaring function, no arguments");
    func.arg_types.push_back(TYPE_VOID);
  }
  else
  {
    auto args = dynamic_cast<FormalsList *>(y_arguments.NODE);
    for (int i = 0; i < args->list.size(); i++)
    {
      func.arg_types.push_back(args->list[i].TYPE);
    }
  }
  func.size = 0;
  table.insert(*y_identifier.STRING, &func, true);

  // table.open_scope();
  table.func_info = func;
}

void variable_init(atom_t y_identifier, bool is_local)
{
  // debugParser("Declaring var");
  // cout << "[parser:var]: " << type_to_string_map[y_identifier.TYPE] << endl;
  // cout << "[parser:STRING]: " << *y_identifier.STRING << endl;
  if (table.id_is_exists(*y_identifier.STRING))
  {
    err(output::errorDef, yylineno, *y_identifier.STRING);
  }
  var_info_t var_info;
  var_info.is_func = false;
  var_info.type = y_identifier.TYPE;
  var_info.size = type_size_map[y_identifier.TYPE];
  table.insert(*y_identifier.STRING, &var_info, is_local);
}

void declare_formals(atom_t yy_formals)
{
  if (yy_formals.NODE == NULL)
  {
    return;
  }
  auto formals_list = dynamic_cast<FormalsList *>(yy_formals.NODE);
  for (int i = 0; i < formals_list->list.size(); i++)
  {
    variable_init(formals_list->list[i], false);
  }
  table.offsets.top() = 0;
}

Exp::Exp(atom_t a, string op, atom_t b)
{
  bool mismatch = false;
  if (op == "and")
  {
    if (a.TYPE != TYPE_BOOL || b.TYPE != TYPE_BOOL)
      err(output::errorMismatch, yylineno);
    value = a.INT && b.INT;
    type = TYPE_BOOL;
  }
  else if (op == "or")
  {
    if (a.TYPE != TYPE_BOOL || b.TYPE != TYPE_BOOL)
      err(output::errorMismatch, yylineno);
    value = a.INT || b.INT;
    type = TYPE_BOOL;
  }
  else if (op == "+")
  {
    if ((a.TYPE != TYPE_INT && a.TYPE != TYPE_BYTE) ||
        (b.TYPE != TYPE_INT && b.TYPE != TYPE_BYTE))
      mismatch = true;
    if (a.TYPE == TYPE_INT || b.TYPE == TYPE_INT)
      type = TYPE_INT;
    else
      type = TYPE_BYTE;
    value = a.INT + b.INT;
  }
  else if (op == "-")
  {
    if ((a.TYPE != TYPE_INT && a.TYPE != TYPE_BYTE) ||
        (b.TYPE != TYPE_INT && b.TYPE != TYPE_BYTE))
      mismatch = true;
    if (a.TYPE == TYPE_INT || b.TYPE == TYPE_INT)
      type = TYPE_INT;
    else
      type = TYPE_BYTE;
    value = a.INT - b.INT;
  }
  else if (op == "*")
  {
    if ((a.TYPE != TYPE_INT && a.TYPE != TYPE_BYTE) ||
        (b.TYPE != TYPE_INT && b.TYPE != TYPE_BYTE))
      mismatch = true;
    if (a.TYPE == TYPE_INT || b.TYPE == TYPE_INT)
      type = TYPE_INT;
    else
      type = TYPE_BYTE;
    value = a.INT * b.INT;
  }
  else if (op == "/")
  {
    if ((a.TYPE != TYPE_INT && a.TYPE != TYPE_BYTE) ||
        (b.TYPE != TYPE_INT && b.TYPE != TYPE_BYTE))
      mismatch = true;
    if (a.TYPE == TYPE_INT || b.TYPE == TYPE_INT)
      type = TYPE_INT;
    else
      type = TYPE_BYTE;
  }
  else if (op == "<")
  {
    if ((a.TYPE != TYPE_INT && a.TYPE != TYPE_BYTE) ||
        (b.TYPE != TYPE_INT && b.TYPE != TYPE_BYTE))
      mismatch = true;
    type = TYPE_BOOL;
    value = a.INT < b.INT;
  }
  else if (op == "<=")
  {
    if ((a.TYPE != TYPE_INT && a.TYPE != TYPE_BYTE) ||
        (b.TYPE != TYPE_INT && b.TYPE != TYPE_BYTE))
      mismatch = true;
    type = TYPE_BOOL;
    value = a.INT <= b.INT;
  }
  else if (op == "==")
  {
    if ((a.TYPE != TYPE_INT && a.TYPE != TYPE_BYTE) ||
        (b.TYPE != TYPE_INT && b.TYPE != TYPE_BYTE))
    {
      mismatch = true;
    }
    type = TYPE_BOOL;
    value = a.INT == b.INT;
  }
  else if (op == ">=")
  {
    if ((a.TYPE != TYPE_INT && a.TYPE != TYPE_BYTE) ||
        (b.TYPE != TYPE_INT && b.TYPE != TYPE_BYTE))
      mismatch = true;
    type = TYPE_BOOL;
    value = a.INT >= b.INT;
  }
  else if (op == ">")
  {
    if ((a.TYPE != TYPE_INT && a.TYPE != TYPE_BYTE) ||
        (b.TYPE != TYPE_INT && b.TYPE != TYPE_BYTE))
      mismatch = true;
    type = TYPE_BOOL;
    value = a.INT > b.INT;
  }
  else if (op == "!=")
  {
    if ((a.TYPE != TYPE_INT && a.TYPE != TYPE_BYTE) ||
        (b.TYPE != TYPE_INT && b.TYPE != TYPE_BYTE))
      mismatch = true;
    type = TYPE_BOOL;
    value = a.INT != b.INT;
  }
  if (mismatch)
  {
    err(output::errorMismatch, yylineno);
  }
}

Call::Call(atom_t identifier, atom_t yy_exp_list)
{
  if (!table.is_func_exists(*identifier.STRING))
  {
    err(output::errorUndefFunc, yylineno, *identifier.STRING);
  }

  auto exp_list = dynamic_cast<ExpList *>(yy_exp_list.NODE);
  auto exp_type_list = exp_list->list;
  auto args_type = table.get_function_args(*identifier.STRING);

  if (args_type.size() != exp_type_list.size())
  {
    // debugParser("Function Call: Arguments list size mismatch");
    // cout << "[parser:Call:expected list size] " << exp_type_list.size() << endl;
    // cout << "[parser:Call:args list size] " << args_type.size() << endl;
    vector<string> args;
    for (int i = 0; i < args_type.size(); i++)
    {
      args.push_back(type_to_string_map[args_type[i]]);
      auto typeVoid = type_to_string_map[TYPE_VOID];
      auto typeUndefined = type_to_string_map[TYPE_UNDEFINED];
      args.front() = args.front() == typeVoid ? typeUndefined : args.front();
    }
    err(output::errorPrototypeMismatch, yylineno, *identifier.STRING, args);
  }

  for (int i = 0; i < args_type.size(); i++)
  {
    bool is_valid = converstion_map[exp_type_list[i].TYPE][args_type[i]];
    if (!is_valid)
    {
      // debugParser("Function Call: Type mismatch");
      vector<string> args;
      for (int j = 0; j < args_type.size(); j++)
      {
        args.push_back(type_to_string_map[args_type[j]]);
      }
      err(output::errorPrototypeMismatch, yylineno, *identifier.STRING, args);
    }
  }

  this->type = table.get_function_type(*identifier.STRING);
}

Call::Call(atom_t identifier)
{
  if (!table.is_func_exists(*identifier.STRING))
  {
    err(output::errorUndefFunc, yylineno, *identifier.STRING);
  }
  vector<TypeEnum> args_type = table.get_function_args(*identifier.STRING);
  if (args_type.size() != 1 || args_type.front() != TYPE_VOID)
  {
    vector<string> args;
    for (int j = 0; j < args_type.size(); j++)
    {
      args.push_back(type_to_string_map[args_type[j]]);
      args.front() = args.front() == "VOID" ? "" : args.front();
    }

    err(output::errorPrototypeMismatch, yylineno, *identifier.STRING, args);
  }
  this->type = table.get_function_type(*identifier.STRING);
}

void assign_value(atom_t y_identifier, atom_t y_expression)
{
  if (!table.id_is_exists(*y_identifier.STRING))
  {
    err(output::errorUndef, yylineno, *y_identifier.STRING);
  }
  auto entry = table.get_entry(*y_identifier.STRING);
  if (entry.type_info.is_func)
  {
    err(output::errorUndef, yylineno, *y_identifier.STRING);
  }
  bool is_valid = converstion_map[y_expression.TYPE][entry.type_info.type];
  if (!is_valid)
  {
    err(output::errorMismatch, yylineno);
  }
}
