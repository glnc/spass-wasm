/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *         RECURSIVE PATH ORDERING WITH STATUS            * */
/* *                                                        * */
/* *  $Module:   RPOS                                       * */
/* *                                                        * */
/* *  Copyright (C) 1997, 1998, 1999, 2000, 2001            * */
/* *  MPI fuer Informatik                                   * */
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
/* $Revision: 1.5 $                                         * */
/* $State: Exp $                                            * */
/* $Date: 2011-11-27 12:46:46 $                             * */
/* $Author: weidenb $                                       * */
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

     
/* $RCSfile: rpos.c,v $ */

#include "rpos.h"

BOOL       rpos_ContEqual(VARCONT, VARCONT, TERM, VARCONT, VARCONT, TERM);
ord_RESULT rpos_ContGreaterEqual(VARCONT, VARCONT, TERM, VARCONT, VARCONT, TERM, BOOL);

/**************************************************************/
/* Comparisons                                                */
/**************************************************************/

BOOL rpos_Greater(TERM T1, TERM T2, BOOL VarIsConst)
{
  return ord_IsGreaterThan(rpos_GreaterEqual(T1, T2, VarIsConst));
}

BOOL rpos_ContGreaterAux(VARCONT GlobalC1, VARCONT TermC1, TERM T1,
			 VARCONT GlobalC2, VARCONT TermC2, TERM T2,
			 BOOL VarIsConst)
{
  return ord_IsGreaterThan(rpos_ContGreaterEqual(GlobalC1, TermC1, T1, 
                                                 GlobalC2, TermC2, T2,
						 VarIsConst));
}

BOOL rpos_ContGreater(VARCONT GlobalC1, VARCONT TermC1, TERM T1,
			 VARCONT GlobalC2, VARCONT TermC2, TERM T2)
{
  return ord_IsGreaterThan(rpos_ContGreaterEqual(GlobalC1, TermC1, T1, 
                                                 GlobalC2, TermC2, T2,
						 FALSE));
}

BOOL rpos_ContGreaterSkolemSubst(VARCONT GlobalC1, VARCONT TermC1, TERM T1,
			 VARCONT GlobalC2, VARCONT TermC2, TERM T2)
{
  return ord_IsGreaterThan(rpos_ContGreaterEqual(GlobalC1, TermC1, T1, 
                                                 GlobalC2, TermC2, T2,
						 TRUE));
}

/**************************************************************/
/* Top Down Version                                           */
/**************************************************************/

static LIST rpos_MultisetDifference(TERM T1, TERM T2)
/**************************************************************
  INPUT:   Two terms.
  RETURNS: The multiset difference between the arguments
           of both terms with respect to rpos_Equal.
***************************************************************/
{
  LIST result;

  result = list_Copy(term_ArgumentList(T1));
  result = list_NMultisetDifference(result, term_ArgumentList(T2),
				    (BOOL (*)(POINTER,POINTER)) rpos_Equal);
  return result;
}
  
  
static ord_RESULT rpos_MulGreaterEqual(TERM T1, TERM T2, BOOL VarIsConst)
/**************************************************************
  INPUT:   Two terms with equal top symbols and multiset status.
  RETURNS: ord_GREATER_THAN if <T1> is greater than <T2>,
	   ord_EQUAL        if both terms are equal and
	   ord_UNCOMPARABLE otherwise.
  CAUTION: If <VarIsConst> is set then variables are interpreted as constants
           with lowest precedence. They are ranked to each other using
           their variable index.
***************************************************************/
{
  LIST l1, l2;

  l1 = rpos_MultisetDifference(T1, T2);
  if (list_Empty(l1))
    /* If |M| = |N| and M-N = {} then N-M = {} */ 
    return ord_Equal();   /* Terms are equal */
  else {
    LIST scan;
    BOOL greater;

    l2 = rpos_MultisetDifference(T2, T1);

    for (greater = TRUE; !list_Empty(l2) && greater; l2 = list_Pop(l2)) {
      for (scan = l1, greater = FALSE; !list_Empty(scan) && !greater; scan = list_Cdr(scan))
	greater = rpos_Greater(list_Car(scan), list_Car(l2), VarIsConst);
    }
    list_Delete(l1); /* l2 was freed in the outer for loop */
    if (greater)
      return ord_GreaterThan();
    else
      return ord_Uncomparable();
  }
}

static ord_RESULT rpos_LexGreaterEqual(TERM T1, TERM T2, BOOL VarIsConst)
/**************************************************************
  INPUT:   Two terms with equal top symbols and lexicographic status.
  RETURNS: ord_GREATER_THAN if <T1> is greater than <T2>,
	   ord_EQUAL        if both terms are equal and 
	   ord_UNCOMPARABLE otherwise.
  CAUTION: If <VarIsConst> is set then variables are interpreted as constants
           with lowest precedence. They are ranked to each other using
           their variable index.
***************************************************************/
{
  ord_RESULT result;
  LIST       l1, l2, scan1, scan2;

  if (symbol_HasProperty(term_TopSymbol(T1), ORDRIGHT)) {
    l1 = list_Reverse(term_ArgumentList(T1)); /* Create new lists */
    l2 = list_Reverse(term_ArgumentList(T2));
  } else {
    l1 = term_ArgumentList(T1);
    l2 = term_ArgumentList(T2);
  }
  /* First ignore equal arguments */
  result = ord_Equal();
  for (scan1 = l1, scan2 = l2; !list_Empty(scan1);
       scan1 = list_Cdr(scan1), scan2 = list_Cdr(scan2)) {
    result = rpos_GreaterEqual(list_Car(scan1), list_Car(scan2), VarIsConst);
    if (!ord_IsEqual(result))
      break;
  }

  if (ord_IsEqual(result))  /* All arguments are equal, so the terms */
    /* empty */;            /* are equal with respect to RPOS */
  else if (ord_IsGreaterThan(result)) {
    /* Check if T1 > each remaining argument of T2 */
    for (scan2 = list_Cdr(scan2); !list_Empty(scan2) && 
                                rpos_Greater(T1, list_Car(scan2), VarIsConst);
	 scan2 = list_Cdr(scan2)); /* Empty body */
    if (list_Empty(scan2))
      result = ord_GreaterThan();
    else
      result = ord_Uncomparable();
  }
  else {
    /* Argument of T1 was not >= argument of T2. */

    /* Try to find an argument of T1 that is >= T2 */
    for (scan1 = list_Cdr(scan1), result = ord_Uncomparable();
	 !list_Empty(scan1) && !ord_IsGreaterThan(result);
	 scan1 = list_Cdr(scan1)) {
      if (!ord_IsUncomparable(rpos_GreaterEqual(list_Car(scan1), T2, VarIsConst)))
	result = ord_GreaterThan();
    }
  }

  if (symbol_HasProperty(term_TopSymbol(T1), ORDRIGHT)) {
    list_Delete(l1);  /* Delete the lists create above */
    list_Delete(l2);
  }
  return result;
}


BOOL rpos_Equal(TERM T1, TERM T2)
/**************************************************************
  INPUT:   Two terms.
  RETURNS: TRUE, if <T1> is equal to <T2> and
           FALSE otherwise.
***************************************************************/
{
  LIST l1, l2;

  if (!term_EqualTopSymbols(T1, T2))
    return FALSE;
  else if (!term_IsComplex(T1))  /* Equal variable or constant */
    return TRUE;
  else {
    if (symbol_HasProperty(term_TopSymbol(T1), ORDMUL)) {  /* MUL case */
      l1 = rpos_MultisetDifference(T1, T2);
      if (list_Empty(l1))
	return TRUE;
      else {
	list_Delete(l1);
	return FALSE;
      }
    } else {   /* LEX case */
      for (l1 = term_ArgumentList(T1), l2 = term_ArgumentList(T2);
	   !list_Empty(l1) &&  rpos_Equal(list_Car(l1), list_Car(l2));
	   l1 = list_Cdr(l1), l2 = list_Cdr(l2))
	/* empty */;
      return list_Empty(l1);  /* All arguments were equal */
    }
  }
}


ord_RESULT rpos_GreaterEqual(TERM T1, TERM T2, BOOL VarIsConst)
/**************************************************************
  INPUT:   Two terms.
  RETURNS: ord_GREATER_THAN if <T1> is greater than <T2>
	   ord_EQUAL        if both terms are equal
	   ord_UNCOMPARABLE otherwise.
  CAUTION: The precedence from the order module is used to determine
           the precedence of symbols!
           If <VarIsConst> is set then variables are interpreted as constants
           with lowest precedence. They are ranked to each other using
           their variable index.
***************************************************************/
{
  LIST scan;

  if (term_IsVariable(T1)) {
    if (term_EqualTopSymbols(T1, T2))
      return ord_Equal();   /* T2 is the same variable */
    else if(VarIsConst && term_IsVariable(T2)) {
        if(term_TopSymbol(T1) > term_TopSymbol(T2))
                return ord_GreaterThan();
        else
                return ord_Uncomparable();
    }
    else
      /* A variable can't be greater than another term */
      return ord_Uncomparable();
  } else if (!VarIsConst && term_IsVariable(T2)) {   /* T1 isn't a variable */
    if (term_ContainsSymbol(T1, term_TopSymbol(T2)))
      return ord_GreaterThan();
    else
      return ord_Uncomparable();
  } else if(VarIsConst && term_IsVariable(T2)){
      return ord_GreaterThan();    
  } else if (term_EqualTopSymbols(T1, T2)) {
    if (symbol_HasProperty(term_TopSymbol(T1), ORDMUL))
      return rpos_MulGreaterEqual(T1, T2, VarIsConst);
    else
      return rpos_LexGreaterEqual(T1, T2, VarIsConst);
  } else {
    if (symbol_PrecedenceGreater(ord_PRECEDENCE, term_TopSymbol(T1),
				 term_TopSymbol(T2))) {
      /* Different top symbols, symbol of T1 > symbol of T2. */
      /* Try if T1 > each argument of T2.                    */
      for (scan = term_ArgumentList(T2); !list_Empty(scan); scan = list_Cdr(scan))
	if (!rpos_Greater(T1,  list_Car(scan), VarIsConst))
	  return ord_Uncomparable();
      return ord_GreaterThan();
    } else {
      /* Try to find an argument of T1 that is >= T2 */
      for (scan = term_ArgumentList(T1); !list_Empty(scan); scan = list_Cdr(scan))
	if (!ord_IsUncomparable(rpos_GreaterEqual(list_Car(scan), T2, VarIsConst)))
	  return ord_GreaterThan();    /* Argument of T1 >= T2 */
      return ord_Uncomparable();
    }
  }
}

ord_RESULT rpos_CompareAux(TERM T1, TERM T2, BOOL VarIsConst)
/**************************************************************
  INPUT:   Two terms, a boolean flag indicating if variables
           of <T1> and <T2> are interpreted as skolem constants
  RETURNS: The relation between the two terms with respect to the
           RPOS ordering:
           ord_GREATER_THAN if <T1> is greater than <T2>,
	   ord_EQUAL        if both terms are equal,
           ord_SMALLER_THAN if <T2> is greater than <T1> and
	   ord_UNCOMPARABLE otherwise.
  CAUTION: The precedence from the order module is used to determine
           the precedence of symbols!
           If <VarIsConst> is set then variables are interpreted as constants
           with lowest precedence. They are ranked to each other using
           their variable index.
***************************************************************/
{
  ord_RESULT result;

  result = rpos_GreaterEqual(T1, T2, VarIsConst);
  if (!ord_IsUncomparable(result))
    return result;
  else if (rpos_Greater(T2, T1, VarIsConst))
    return ord_SmallerThan();
  else
    return ord_UNCOMPARABLE;
}

ord_RESULT rpos_Compare(TERM T1, TERM T2)
/**************************************************************
  INPUT:   Two terms.
  RETURNS: The relation between the two terms with respect to the
           RPOS ordering:
           ord_GREATER_THAN if <T1> is greater than <T2>,
	   ord_EQUAL        if both terms are equal,
           ord_SMALLER_THAN if <T2> is greater than <T1> and
	   ord_UNCOMPARABLE otherwise.
  CAUTION: The precedence from the order module is used to determine
           the precedence of symbols!
***************************************************************/
{
    return rpos_CompareAux(T1, T2, FALSE);
}

ord_RESULT rpos_CompareSkolem(TERM T1, TERM T2)
/**************************************************************
  INPUT:   Two terms.
  RETURNS: The relation between the two terms with respect to the
           RPOS ordering:
           ord_GREATER_THAN if <T1> is greater than <T2>,
	   ord_EQUAL        if both terms are equal,
           ord_SMALLER_THAN if <T2> is greater than <T1> and
	   ord_UNCOMPARABLE otherwise.
  CAUTION: The precedence from the order module is used to determine
           the precedence of symbols! Variables are threated
           as constants
***************************************************************/
{
    return rpos_CompareAux(T1, T2, TRUE);
}

/**************************************************************/
/* Term comparison with respect to bindings                   */
/**************************************************************/

static LIST rpos_ContMultisetDifference(VARCONT GlobalC1, VARCONT TermC1, TERM T1, 
                                        VARCONT GlobalC2, VARCONT TermC2, TERM T2)
/**************************************************************
  INPUT:      Four contexts and two terms.
  RETURNS:    The multiset difference between the arguments
              of both terms with respect to rpos_ContEqual.
  EFFECT:     Variable bindings are considered.
  ASSUMPTION: All index variables of <T1> and <T2> are bound in
              <GlobalC1> and <GlobalCt2>, respectively

***************************************************************/
{
  LIST result, scan1, scan2;

  /* Don't apply bindings at top level, since that happened */
  /* in rpos_ContGreaterEqual */

  /* We can't use list_NMultisetDifference, since that function   */
  /* expects an equality functions for terms that takes two terms */
  /* as arguments. We also need the two contexts resolve variable */
  /* bindings. */
  result = list_Copy(term_ArgumentList(T1));
  for (scan2 = term_ArgumentList(T2); !list_Empty(scan2);
       scan2 = list_Cdr(scan2)) {
    /* Delete at most one occurrence of the */
    /* current element of list2 from list1 */
    for (scan1 = result; !list_Empty(scan1); scan1 = list_Cdr(scan1)) {
      if (list_Car(scan1) != NULL &&
	  rpos_ContEqual(GlobalC1, TermC1, list_Car(scan1), 
                         GlobalC2, TermC2, list_Car(scan2))) {
	/* arg of list1 wasn't deleted earlier and terms are equal */
	list_Rplaca(scan1, NULL);  /* Mark argument of T1 as deleted */
	break;
      }
    }
  }
  return list_PointerDeleteElement(result, NULL); /* Delete all marked terms */
}
  
  
static ord_RESULT rpos_ContMulGreaterEqual(VARCONT GlobalC1, VARCONT TermC1, TERM T1,
					   VARCONT GlobalC2, VARCONT TermC2, TERM T2,
					   BOOL VarIsConst)
/**************************************************************
  INPUT:      Two contexts and two terms with equal top symbols
              and multiset status.
  RETURNS:      ord_GREATER_THAN if <T1> is greater than <T2>,
	        ord_EQUAL        if both terms are equal and
	        ord_UNCOMPARABLE otherwise.
  EFFECT:     Variable bindings are considered.
  ASSUMPTION: All index variables of <T1> and <T2> are bound in
              <GlobalC1> and <GlobalCt2>, respectively
***************************************************************/
{
  LIST l1, l2;

  /* Don't apply bindings at top level, since that happened */
  /* in rpos_ContGreaterEqual. */

  l1 = rpos_ContMultisetDifference(GlobalC1, TermC1, T1, GlobalC2, TermC2, T2);
  if (list_Empty(l1))
    /* If |M| = |N| and M-N = {} then N-M = {} */ 
    return ord_Equal();   /* Terms are equal */
  else {
    LIST scan;
    BOOL greater;

    l2 = rpos_ContMultisetDifference(GlobalC2, TermC2, T2, GlobalC1, TermC1, T1);

    for (greater = TRUE; !list_Empty(l2) && greater; l2 = list_Pop(l2)) {
      for (scan = l1, greater = FALSE; !list_Empty(scan) && !greater;
	   scan = list_Cdr(scan))
	greater = rpos_ContGreaterAux(GlobalC1, TermC1, list_Car(scan), 
                                   GlobalC2, TermC2, list_Car(l2),
				   VarIsConst);
    }
    list_Delete(l1); /* l2 was freed in the outer for loop */
    if (greater)
      return ord_GreaterThan();
    else
      return ord_Uncomparable();
  }
}

static ord_RESULT rpos_ContLexGreaterEqual(VARCONT GlobalC1, VARCONT TermC1, TERM T1,
					   VARCONT GlobalC2, VARCONT TermC2, TERM T2,
					   BOOL VarIsConst)
/**************************************************************
  INPUT:      Two contexts and two terms with equal top symbols
              and lexicographic status and a flag.
  RETURNS:      ord_GREATER_THAN if <T1> is greater than <T2>,
	        ord_EQUAL        if both terms are equal and 
	        ord_UNCOMPARABLE otherwise.
  EFFECT:     Variable bindings are considered.
  ASSUMPTION: All index variables of <T1> and <T2> are bound in
              <GlobalC1> and <GlobalCt2>, respectively
  CAUTION: If <VarIsConst> is set then variables are interpreted as constants
           with lowest precedence. They are ranked to each other using
           their variable index.
***************************************************************/
{
  ord_RESULT result;
  LIST       l1, l2, scan1, scan2;

  /* Don't apply bindings at top level, since that happened */
  /* in rpos_ContGreaterEqual */

  if (symbol_HasProperty(term_TopSymbol(T1), ORDRIGHT)) {
    l1 = list_Reverse(term_ArgumentList(T1)); /* Create new lists */
    l2 = list_Reverse(term_ArgumentList(T2));
  } else {
    l1 = term_ArgumentList(T1);
    l2 = term_ArgumentList(T2);
  }
  /* First ignore equal arguments */
  result = ord_Equal();
  for (scan1 = l1, scan2 = l2; !list_Empty(scan1);
       scan1 = list_Cdr(scan1), scan2 = list_Cdr(scan2)) {
    result = rpos_ContGreaterEqual(GlobalC1, TermC1, list_Car(scan1), 
                                   GlobalC2, TermC2, list_Car(scan2),
				   VarIsConst);
    if (!ord_IsEqual(result))
      break;
  }

  if (ord_IsEqual(result))  /* All arguments are equal, so the terms */
    /* empty */;            /* are equal with respect to RPOS */
  else if (ord_IsGreaterThan(result)) {
    /* Check if T1 > each remaining argument of T2 */
    for (scan2 = list_Cdr(scan2);
	 !list_Empty(scan2) && rpos_ContGreaterAux(GlobalC1, TermC1, T1, 
                                                GlobalC2, TermC2, list_Car(scan2),
						VarIsConst);
	 scan2 = list_Cdr(scan2)); /* Empty body */
    if (list_Empty(scan2))
      result = ord_GreaterThan();
    else
      result = ord_Uncomparable();
  }
  else {
    /* Argument of T1 was not >= argument of T2. */
    /* Try to find an argument of T1 that is >= T2 */
    for (scan1 = list_Cdr(scan1), result = ord_Uncomparable();
	 !list_Empty(scan1) && !ord_IsGreaterThan(result);
	 scan1 = list_Cdr(scan1)) {
      if (!ord_IsUncomparable(rpos_ContGreaterEqual(GlobalC1, TermC1,list_Car(scan1),
                                                    GlobalC2, TermC2,T2,
						    VarIsConst)))
	result = ord_GreaterThan();
    }
  }

  if (symbol_HasProperty(term_TopSymbol(T1), ORDRIGHT)) {
    list_Delete(l1);  /* Delete the lists create above */
    list_Delete(l2);
  }
  return result;
}


BOOL rpos_ContEqual(VARCONT GlobalC1, VARCONT TermC1, TERM T1, 
                    VARCONT GlobalC2, VARCONT TermC2, TERM T2)
/**************************************************************
  INPUT:      Two contexts and two terms.
  RETURNS:    TRUE, if <T1> is equal to <T2> and
              FALSE otherwise.
  EFFECT:     Variable bindings are considered.
  ASSUMPTION: All index variables of <T1> and <T2> are bound in
              <GlobalC1> and <GlobalCt2>, respectively
***************************************************************/
{
  LIST l1, l2;

  T1 = cont_Deref(GlobalC1, &TermC1, T1);
  T2 = cont_Deref(GlobalC2, &TermC2, T2);

  if (!term_EqualTopSymbols(T1, T2))
    return FALSE;
  else if (!term_IsComplex(T1))
    return TRUE;
  else {
    if (symbol_HasProperty(term_TopSymbol(T1), ORDMUL)) {
      l1 = rpos_ContMultisetDifference(GlobalC1, TermC1, T1,
                                       GlobalC2, TermC2, T2);
      if (list_Empty(l1))
	return TRUE;
      else {
	list_Delete(l1);
	return FALSE;
      }
    } else {   /* LEX case */
      for (l1 = term_ArgumentList(T1), l2 = term_ArgumentList(T2);
	   !list_Empty(l1) &&  rpos_ContEqual(GlobalC1, TermC1,list_Car(l1),
                                              GlobalC2, TermC2,list_Car(l2));
	   l1 = list_Cdr(l1), l2 = list_Cdr(l2)); /* empty body */
      return list_Empty(l1);  /* All arguments were equal */
    }
  }
}


ord_RESULT rpos_ContGreaterEqual(VARCONT GlobalC1, VARCONT TermC1, TERM T1,
                                 VARCONT GlobalC2, VARCONT TermC2, TERM T2,
				 BOOL VarIsConst)
/**************************************************************
  INPUT:     Two contexts and two terms.
  RETURNS:      ord_GREATER_THAN if <T1> is greater than <T2>
	        ord_EQUAL        if both terms are equal
	        ord_UNCOMPARABLE otherwise.
  EFFECT:     Variable bindings are considered.
  CAUTION:    The precedence from the order module is used to determine
              the precedence of symbols!
	      If <VarIsConst> is set then variables are interpreted as constants
              with lowest precedence. They are ranked to each other using
              their variable index.
  ASSUMPTION: All index variables of <T1> and <T2> are bound in
              <GlobalC1> and <GlobalCt2>, respectively
***************************************************************/
{
  LIST scan;

  T1 = cont_Deref(GlobalC1, &TermC1, T1);
  T2 = cont_Deref(GlobalC2, &TermC2, T2);

  if (term_IsVariable(T1)) {
    if (term_EqualTopSymbols(T1, T2))
      return ord_Equal();   /* T2 is the same variable */
    else
      /* A variable can't be greater than another term */
      return ord_Uncomparable();
  } else if (term_IsVariable(T2)) {   /* T1 isn't a variable */
    if (cont_TermContainsSymbol(GlobalC1, TermC1, T1, term_TopSymbol(T2)))
      return ord_GreaterThan();
    else
      return ord_Uncomparable();
  } else if (term_EqualTopSymbols(T1, T2)) {
    if (symbol_HasProperty(term_TopSymbol(T1), ORDMUL))
      return rpos_ContMulGreaterEqual(GlobalC1, TermC1, T1, 
                                      GlobalC2, TermC2, T2, VarIsConst);
    else
      return rpos_ContLexGreaterEqual(GlobalC1, TermC1, T1, 
                                      GlobalC2, TermC2, T2, VarIsConst);
  } else {
    if (symbol_PrecedenceGreater(ord_PRECEDENCE, term_TopSymbol(T1),
				 term_TopSymbol(T2))) {
      /* Different top symbols, symbol of T1 > symbol of T2. */
      /* Try if T1 > each argument of T2.                    */
      for (scan = term_ArgumentList(T2); !list_Empty(scan); scan = list_Cdr(scan))
	if (!rpos_ContGreaterAux(GlobalC1, TermC1, T1, 
                              GlobalC2, TermC2, list_Car(scan), VarIsConst))
	  return ord_Uncomparable();
      return ord_GreaterThan();
    } else {
      /* Try to find an argument of T1 that is >= T2 */
      for (scan = term_ArgumentList(T1); !list_Empty(scan); scan = list_Cdr(scan))
	if (!ord_IsUncomparable(rpos_ContGreaterEqual(GlobalC1, TermC1,list_Car(scan),
                                                      GlobalC2, TermC2,T2,
						      VarIsConst)))
	  return ord_GreaterThan();    /* Argument of T1 >= T2 */
      return ord_Uncomparable();
    }
  }
}

ord_RESULT rpos_ContCompareAux(VARCONT C1, TERM T1, VARCONT C2, TERM T2, BOOL VarIsConst)
/**************************************************************
  INPUT:   Two contexts and two terms.
  RETURNS: The relation between the two terms with respect to the
           RPOS ordering:
           ord_GREATER_THAN if <T1> is greater than <T2>,
	   ord_EQUAL        if both terms are equal,
           ord_SMALLER_THAN if <T2> is greater than <T1> and
	   ord_UNCOMPARABLE otherwise.
  EFFECT:  Variable bindings are considered.
           If VarIsConst is true variables are interpreted as constants
  CAUTION: The precedence from the order module is used to determine
           the precedence of symbols!
	   If <VarIsConst> is set then variables are interpreted as constants
           with lowest precedence. They are ranked to each other using
           their variable index.
***************************************************************/
{
  ord_RESULT result;
  VARCONT GlobalC1, GlobalC2;      
  
  GlobalC1 = C1;
  GlobalC2 = C2;
  
  T1 = cont_Deref(GlobalC1, &C1, T1);
  T2 = cont_Deref(GlobalC2, &C2, T2);

  result = rpos_ContGreaterEqual(GlobalC1, C1, T1, GlobalC2, C2, T2, VarIsConst);
  if (!ord_IsUncomparable(result))
    return result;
  else if (rpos_ContGreaterAux(GlobalC2, C2, T2, GlobalC1, C1, T1, VarIsConst))
    return ord_SmallerThan();
  else
    return ord_UNCOMPARABLE;
}

ord_RESULT rpos_ContCompare(VARCONT C1, TERM T1, VARCONT C2, TERM T2)
/**************************************************************
  INPUT:   Two contexts, two terms and a flag
  RETURNS: The relation between the two terms with respect to the
           RPOS ordering:
           ord_GREATER_THAN if <T1> is greater than <T2>,
	   ord_EQUAL        if both terms are equal,
           ord_SMALLER_THAN if <T2> is greater than <T1> and
	   ord_UNCOMPARABLE otherwise.
  EFFECT:  Variable bindings are considered.
  CAUTION: The precedence from the order module is used to determine
           the precedence of symbols!
***************************************************************/
{
        return rpos_ContCompareAux(C1, T1, C2, T2, FALSE);
}
