/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *             SIGNATURE TABLE                            * */
/* *                                                        * */
/* *  $Module:   TABLE                                      * */ 
/* *                                                        * */
/* *  Copyright (C) 1999, 2000, 2001 MPI fuer Informatik    * */
/* *                                                        * */
/* *  This program is free software; you can redistribute   * */
/* *  it and/or modify it under the terms of the FreeBSD    * */
/* *  Licence.                                              * */
/* *                                                        * */
/* *  This program is distributed in the hope that it will  * */
/* *  be useful, but WITHOUT ANY WARRANTY; without even     * */
/* *  the implied warranty of MERCHANTABILITY or FITNESS    * */
/* *  FOR A PARTICULAR PURPOSE.  See the LICENCE file       * */
/* *  for more details.                                     * */
/* *                                                        * */
/* *                                                        * */
/* $Revision: 1.3 $                                     * */
/* $State: Exp $                                            * */
/* $Date: 2011-05-25 10:01:24 $                             * */
/* $Author: weidenb $                                         * */
/* *                                                        * */
/* *             Contact:                                   * */
/* *             Christoph Weidenbach                       * */
/* *             MPI fuer Informatik                        * */
/* *             Stuhlsatzenhausweg 85                      * */
/* *             66123 Saarbruecken                         * */
/* *             Email: spass@mpi-inf.mpg.de                * */
/* *             Germany                                    * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/


/**************************************************************/
/* Includes                                                   */
/**************************************************************/

#include "table.h"


/**************************************************************/
/* Inline functions                                           */
/**************************************************************/

 int table_Index(SYMBOL symbol)
{
  if (symbol_IsVariable(symbol))
    return -(int) symbol;
  else
    return symbol_Index(symbol);
}


 TERM table_GetTerm(TERMARRAY ta)
{
  return ta->term;
}


 TERMARRAY table_SetTerm(TERMARRAY ta, TERM term)
{
  ta->term = term;
  return ta;
}


 int table_GetStamp(TERMARRAY ta)
{
  return ta->stamp;
}


 TERMARRAY table_SetStamp(TERMARRAY ta, int stamp)
{
  ta->stamp = stamp;
  return ta;
}


 TERMARRAY table_GetChild(TERMARRAY ta)
{
  return ta->child;
}


 TERMARRAY table_SetChild(TERMARRAY ta, TERMARRAY child)
{
  ta->child = child;
  return ta;
}


 TERMARRAY table_GetTermarray(TABLE table)
{
  return table->ta;
}


 TABLE table_SetTermarray(TABLE table, TERMARRAY ta)
{
  table->ta = ta;
  return table;
}


 TERMARRAY *table_GetPoss(TABLE table)
{
  return table->pos;
}


 TERMARRAY table_GetPos(TABLE table, int tab_index)
{
  return table_GetPoss(table)[tab_index];
}


 TABLE table_SetPoss(TABLE table, TERMARRAY *ref)
{
  table->pos = ref;
  return table;
}


 TABLE table_SetPos(TABLE table, int tab_index, TERMARRAY ta)
{
  table_GetPoss(table)[tab_index] = ta;
  return table;
}


 int *table_GetPosstamps(TABLE table)
{
  return table->posstamp;
}


 int table_GetPosstamp(TABLE table, int tab_index)
{
  return table_GetPosstamps(table)[tab_index];
}


 TABLE table_SetPosstamps(TABLE table, int *ref)
{
  table->posstamp = ref;
  return table;
}


 TABLE table_SetPosstamp(TABLE table, int tab_index, int stamp)
{
  table_GetPosstamps(table)[tab_index] = stamp;
  return table;
}


 int table_GetStampcounter(TABLE table)
{
  return table->stampcounter;
}


 TABLE table_SetStampcounter(TABLE table, int stampcounter)
{
  table->stampcounter = stampcounter;
  return table;
}


 int table_GetOpbound(TABLE table)
{
  return table->opbound;
}


 TABLE table_SetOpbound(TABLE table, int opbound)
{
  table->opbound = opbound;
  return table;
}


 int table_GetVarbound(TABLE table)
{
  return table->varbound;
}


 TABLE table_SetVarbound(TABLE table, int varbound)
{
  table->varbound = varbound;
  return table;
}


 int table_GetTermbound(TABLE table)
{
  return table->termbound;
}


 TABLE table_SetTermbound(TABLE table, int termbound)
{
  table->termbound = termbound;
  return table;
}


 BOOL table_LegalPosIndex(TABLE table, int tab_index)
{
  return 0 <= tab_index && tab_index <= table_GetTermbound(table);
}


 BOOL table_Stamped(TABLE table, TERMARRAY ta)
{
  return table_GetStamp(ta) == table_GetStampcounter(table);
}


 TERMARRAY table_DelayedInit(TABLE table, TERMARRAY ta)
/***************************************************************
  INPUT:   a table and a termarray
  RETURNS: the (now stamped) termarray
  EFFECT:  partially initializes table by setting the
	   termarray's entry to the empty term
***************************************************************/
{
  if (!table_Stamped(table, ta)) {
    table_SetTerm(ta, term_Null());
    table_SetStamp(ta, table_GetStampcounter(table));
  }
  return ta;
}


 BOOL table_PosStamped(TABLE table, int tab_index)
{
  return table_GetPosstamp(table, tab_index) == table_GetStampcounter(table);
}


 int table_DelayedPosInit(TABLE table, int tab_index)
/***************************************************************
  INPUT:   a table and a position tab_index
  RETURNS: the (now stamped) position tab_index
  EFFECT:  partially initializes table by setting the Indexed
	   position to the empty pointer, which means that the
	   term with this Index is not stored in table
***************************************************************/
{
  if (!table_PosStamped(table, tab_index)) {
    table_SetPos(table, tab_index, (TERMARRAY) NULL);
    table_SetPosstamp(table, tab_index, table_GetStampcounter(table));
  }
  return tab_index;
}


/**************************************************************/
/* Functions                                                  */
/**************************************************************/

TABLE table_Null(void)
{
  return (TABLE) NULL;
}


TABLE table_Create(int opbound, int varbound, int termbound)
/***************************************************************
  INPUT:   bounds for the operator symbol, variable and term
	   indices of the terms to be stored in the signature
	   table (i. e. for every such term its top symbol Index
	   has to be in [1, opbound] and the term numbers of its
	   arguments in [0, termbound] - or its variable Index
	   in [1, varbound] if it is a variable)
  RETURNS: a new (and empty) signature table 
***************************************************************/
{
  TABLE result;

#ifdef CHECK
  if (opbound < 0) {
    misc_StartErrorReport();
    misc_ErrorReport("\n In table_Create: negative opbound.");
    misc_FinishErrorReport();
  }
  if (varbound < 0) {
    misc_StartErrorReport();
    misc_ErrorReport("\n In table_Create: negative varbound.");
    misc_FinishErrorReport();
  }
  if (termbound < 0) {
    misc_StartErrorReport();
    misc_ErrorReport("\n In table_Create: negative termbound.");
    misc_FinishErrorReport();
  }
#endif

  result = (TABLE) memory_Malloc(sizeof(struct table));
  table_SetTermarray(result, (TERMARRAY) memory_Calloc (
                                           opbound + varbound + 1,
                                           sizeof(struct termarray)
                                         ) + varbound);
    /* move pointer to the middle of the array to allow negative indices */
  table_SetPoss(
    result,
    (TERMARRAY*) memory_Malloc((termbound + 1) * sizeof(TERMARRAY))
  );
  table_SetPosstamps(result, (int*) memory_Calloc(termbound + 1, sizeof(int)));
  table_SetOpbound(result, opbound);
  table_SetVarbound(result, varbound);
  table_SetTermbound(result, termbound);
  table_SetStampcounter(result, 1);
  return result;
}


static void table_FreeTermarray(TERMARRAY ta, int size)
/***************************************************************
  INPUT:  the termarray to free and its size
  EFFECT: recursively frees the tree structure allocated for the
	  signature array
***************************************************************/
{
  int i;

  if (ta) {
    for (i = 0; i < size; i++)
      table_FreeTermarray(table_GetChild(ta + i), size);
    memory_Free(ta, size * sizeof(struct termarray));
  }
}


void table_Free(TABLE table)
{
  int i;

  if (table != table_Null()) {
    for (i = -table_GetVarbound(table); i <= table_GetOpbound(table); i++)
      table_FreeTermarray(
        table_GetChild(table_GetTermarray(table) + i),
        table_GetTermbound(table) + 1
      );
    memory_Free(
      table_GetTermarray(table) - table_GetVarbound(table),
      (table_GetOpbound(table) + table_GetVarbound(table) + 1) * sizeof(struct
	termarray)
    );
    memory_Free(
      table_GetPoss(table),
      (table_GetTermbound(table) + 1) * sizeof(TERMARRAY)
    );
    memory_Free(
      table_GetPosstamps(table),
      (table_GetTermbound(table) + 1) * sizeof(int)
    );
    memory_Free(table, sizeof(struct table));
  }
}


TABLE table_Init(TABLE table, int opbound, int varbound, int termbound)
/***************************************************************
  INPUT:   the table to recycle and bounds for the operator
	   symbol, variable and term indices of the terms to be
	   stored in the signature table (i. e. for every such
	   term its top symbol Index has to be in [1, opbound]
	   and the term numbers of its arguments in
	   [0, termbound] - or its variable Index in
	   [1, varbound] if it is a variable)
  RETURNS: a cleaned up signature table 
  CAUTION: potentially frees the old table, therefore must be
	   called inside of an assignment like:
	     table = table_Init(table, ...)
***************************************************************/
{
  int opmax, varmax, termmax, i;
  TERMARRAY old;

#ifdef CHECK
  if (opbound < 0) {
    misc_StartErrorReport();
    misc_ErrorReport("\n In table_Init: negative opbound.");
    misc_FinishErrorReport();
  }
  if (varbound < 0) {
    misc_StartErrorReport();
    misc_ErrorReport("\n In table_Init: negative varbound.");
    misc_FinishErrorReport();
  }
  if (termbound < 0) {
    misc_StartErrorReport();
    misc_ErrorReport("\n In table_Init: negative termbound.");
    misc_FinishErrorReport();
  }
#endif

  opmax   = table_GetOpbound(table) > opbound ? table_GetOpbound(table) :
	      opbound;
  varmax  = table_GetVarbound(table) > varbound ? table_GetVarbound(table) :
	      varbound;
  termmax = table_GetTermbound(table) > termbound ? table_GetTermbound(table) :
	      termbound;
  table_SetStampcounter(table, table_GetStampcounter(table) + 1);

  /* in case of stamp overflow or too small termarray nodes get a new table: */
  if (table_GetStampcounter(table)<=0 || termbound>table_GetTermbound(table)) {
    table_Free(table);
    return table_Create(opmax, varmax, termmax);
  }

  /* if only the top layer of the tree is too small get a larger top layer: */
  else if (opbound+varbound > table_GetOpbound(table)+table_GetVarbound(table)){
    old = table_GetTermarray(table);
    table_SetTermarray(table, (TERMARRAY) memory_Calloc(
					    opmax + varmax + 1,
					    sizeof(struct termarray)
					  ) + varmax);
    for (i = -table_GetVarbound(table); i <= table_GetOpbound(table); i++)
      table_SetChild(table_GetTermarray(table) + i, table_GetChild(old + i));
    memory_Free(
      old - table_GetVarbound(table),
      (table_GetOpbound(table) + table_GetVarbound(table) + 1) * sizeof(struct
	termarray)
    );
    table_SetOpbound(table, opmax);
    table_SetVarbound(table, varmax);
    return table;
  }

  else {

    /* move pointer to termarray's new middle: */
    table_SetTermarray(
      table,
      table_GetTermarray(table) + table_GetOpbound(table) - opbound
    );

    table_SetVarbound(
      table,
      table_GetOpbound(table) + table_GetVarbound(table) - opbound
    );
    table_SetOpbound(table, opbound);
    return table;
  }
}


TERM table_QueryAndEnter(TABLE table, PARTITION p, TERM term)
/***************************************************************
  RETURNS: a term with the same p-signature (sigtab_Index(top
	   symbol), [arg 1] , ..., [arg n] ) as term - or the
                           p              p
	   empty term if no such term exists
  EFFECT:  term enters table in the latter case
***************************************************************/
{
  TERMARRAY ta;
  LIST terms;

#ifdef CHECK
  if (part_Size(p) - 1 > table_GetTermbound(table)) {
    misc_StartErrorReport();
    misc_ErrorReport("\n In table_QueryAndEnter: partition not suitable.");
    misc_FinishErrorReport();
  }
  if (table_Index(term_TopSymbol(term)) > table_GetOpbound(table)) {
    misc_StartErrorReport();
    misc_ErrorReport
      ("\n In table_QueryAndEnter: term's operation symbol out of bounds.");
    misc_FinishErrorReport();
  }
  if (table_Index(term_TopSymbol(term)) < -table_GetVarbound(table)) {
    misc_StartErrorReport();
    misc_ErrorReport("\n In table_QueryAndEnter: variable out of bounds.");
    misc_FinishErrorReport();
  }
  if (!table_LegalPosIndex(table, term_Size(term))) {
    misc_StartErrorReport();
    misc_ErrorReport("\n In table_QueryAndEnter: term out of bounds.");
    misc_FinishErrorReport();
  }
#endif

  ta = table_GetTermarray(table) + table_Index(term_TopSymbol(term));
  for (terms = term_ArgumentList(term); !list_Empty(terms); terms =
      list_Cdr(terms)) {
    if (!table_GetChild(ta))
      table_SetChild(ta, (TERMARRAY) memory_Calloc (
                                       table_GetTermbound(table) + 1,
                                       sizeof(struct termarray)
                                     ));
    ta = table_GetChild(ta) + part_Find(p, term_Size(list_Car(terms)));
  }
  table_DelayedInit(table, ta);
  if (table_GetTerm(ta))
    return table_GetTerm(ta);
  else {
    table_SetTerm(ta, term);
    table_SetPos(table, table_DelayedPosInit(table, term_Size(term)), ta);
    return term_Null();
  }
}


TABLE table_Delete(TABLE table, TERM term)
/***************************************************************
  EFFECT: if term has entered table before, it is deleted
***************************************************************/
{
  int no;

#ifdef CHECK
  if (!table_LegalPosIndex(table, term_Size(term))) {
    misc_StartErrorReport();
    misc_ErrorReport("\n In table_Delete: illegal table access.");
    misc_FinishErrorReport();
  }
#endif

  no = term_Size(term);
  table_DelayedPosInit(table, no);
  if (table_GetPos(table, no)) {

#ifdef CHECK
    if (!table_Stamped(table, table_GetPos(table, no))) {
      misc_StartErrorReport();
      misc_ErrorReport("\n In table_Delete: table corrupted.");
      misc_FinishErrorReport();
    }
#endif

    table_SetTerm(table_GetPos(table, no), term_Null());
    table_SetPos(table, no, (TERMARRAY) NULL);
  }
  return table;
}

