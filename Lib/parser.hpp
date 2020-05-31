#ifndef PARSER_HPP
#define PARSER_HPP
#include "output.hpp"
#include "table.hpp"
#include "err.hpp"
#include "atoms.hpp"
using namespace std;
#define YYSTYPE atom_t
extern int while_scope_count;
void init_program();
void close_program();
void return_value_check(TypeEnum return_type);
void close_scope();
void func_init(atom_t y_identifier, atom_t y_arguments);
void declare_formals(atom_t yy_formals);
void assign_value(atom_t y_identifier, atom_t y_expression);
void variable_init(atom_t y_identifier, bool is_local);

SemanticTable table;
int while_scope_count = 0;

/*
  TYPE_UNDEFINED,
  TYPE_VOID,
  TYPE_BOOL,
  TYPE_INT,
  TYPE_BYTE,
  TYPE_STRING,
  TYPE_B,
  HELP_TYPE_NUM_TO_STRING
  */

/* Order matters */
int type_size_map[HELP_TYPE_NUM] = {
    0, // TYPE_NONE
    0, // TYPE_VOID
    1, // TYPE_BOOL
    1, // TYPE_INT
    1, // TYPE_BYTE
    1, // TYPE_STRING
    1  // TYPE_B
};

/* Order matters */
string type_to_string_map[HELP_TYPE_NUM] = {
    "",
    "VOID",
    "BOOL",
    "INT",
    "BYTE",
    "STRING",
    "B",
};

/* Order matters */
bool converstion_map[HELP_TYPE_NUM][HELP_TYPE_NUM] = {
    /* TYPE_UNDEFINED to  */ {
        /* TYPE_UNDEFINED */ true,
        /* TYPE_VOID      */ false,
        /* TYPE_BOOL      */ false,
        /* TYPE_INT       */ false,
        /* TYPE_BYTE      */ false,
        /* TYPE_STRING    */ false,
        /* TYPE_B         */ false,
    },
    /* TYPE_VOID to       */ {
        /* TYPE_UNDEFINED */ false,
        /* TYPE_VOID      */ true,
        /* TYPE_BOOL      */ false,
        /* TYPE_INT       */ false,
        /* TYPE_BYTE      */ false,
        /* TYPE_STRING    */ false,
        /* TYPE_B         */ false,
    },
    /* TYPE_BOOL to       */ {
        /* TYPE_UNDEFINED */ false,
        /* TYPE_VOID      */ false,
        /* TYPE_BOOL      */ true,
        /* TYPE_INT       */ false,
        /* TYPE_BYTE      */ false,
        /* TYPE_STRING    */ false,
        /* TYPE_B         */ false,
    },
    /* TYPE_INT to        */ {
        /* TYPE_UNDEFINED */ false,
        /* TYPE_VOID      */ false,
        /* TYPE_BOOL      */ false,
        /* TYPE_INT       */ true,
        /* TYPE_BYTE      */ false,
        /* TYPE_STRING    */ false,
    },
    /* TYPE_BYTE to       */ {
        /* TYPE_UNDEFINED */ false,
        /* TYPE_VOID      */ false,
        /* TYPE_BOOL      */ false,
        /* TYPE_INT       */ true,
        /* TYPE_BYTE      */ true,
        /* TYPE_STRING    */ false,
        /* TYPE_B         */ false,
    },
    /* TYPE_STRING to     */ {
        /* TYPE_UNDEFINED */ false,
        /* TYPE_VOID      */ false,
        /* TYPE_BOOL      */ false,
        /* TYPE_INT       */ false,
        /* TYPE_BYTE      */ false,
        /* TYPE_STRING    */ true,
        /* TYPE_B         */ false,
    },
    /* TYPE_B to */ {
        /* TYPE_UNDEFINED */ false,
        /* TYPE_VOID      */ false,
        /* TYPE_BOOL      */ false,
        /* TYPE_INT       */ false,
        /* TYPE_BYTE      */ false,
        /* TYPE_STRING    */ true,
        /* TYPE_B         */ true,
    }};

#endif /* PARSER_HPP */