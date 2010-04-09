#include "misc.h"
#include "list_of_lists.h"
#include <Rdefines.h>
#undef isMatrix

//convert from C ListOfLists object to a list in R.
SEXP rph_listOfLists_to_SEXP(ListOfLists *lol) {
  int numprotect=0, listlen,lstSize, col, *intp, i, hasHeader=0;
  double *doublep;
  List *currlist;
  SEXPTYPE lstType;
  SEXP result, header, tempvec;

  listlen = lst_size(lol->lst);

  PROTECT(result = allocVector(VECSXP, listlen));
  numprotect++;

  if (lol->lstName != NULL) {
    for (i=0; i<listlen; i++) 
      if (lst_get_ptr(lol->lstName, i) !=NULL) {
	hasHeader=1;
	break;
      }
  }
  if (hasHeader) {
    PROTECT(header = allocVector(STRSXP, listlen));
    numprotect++;
  }
  for (col=0; col < listlen; col++) {
    lstType = lst_get_int(lol->lstType, col);
    if (lstType == LIST_LIST) {
      PROTECT(tempvec = 
	      rph_listOfLists_to_SEXP((ListOfLists*)lst_get_ptr(lol->lst, col)));
      numprotect++;
    } else {
      currlist = (List*)lst_get_ptr(lol->lst, col);
      lstSize = lst_size(currlist);
      if (lstType == INT_LIST) {
	PROTECT(tempvec = allocVector(INTSXP, lstSize));
	numprotect++;
	intp = INTEGER_POINTER(tempvec);
	for (i=0; i<lstSize; i++) {
	  intp[i] = lst_get_int(currlist, i);
	}
      } else if (lstType == DBL_LIST) {
	PROTECT(tempvec = allocVector(REALSXP, lstSize));
	numprotect++;
	doublep = NUMERIC_POINTER(tempvec);
	for (i=0; i < lstSize; i++) {
	  doublep[i] = lst_get_dbl(currlist, i);
	}
      } else if (lstType == CHAR_LIST) {
	PROTECT(tempvec = allocVector(STRSXP, lstSize));
	numprotect++;
	for (i=0; i < lstSize; i++) {
	  SET_STRING_ELT(tempvec, i, mkChar((char*)lst_get_ptr(currlist, i)));
	}
      } else {
	die("rph_listOfList_to_R unknown type %i\n", lstType);
      }
    }

    SET_VECTOR_ELT(result, col, tempvec);
    if (lol->lstName != NULL) 
      SET_STRING_ELT(header, col, mkChar((char*)lst_get_ptr(lol->lstName, col)));
  }

  if (lol->lstName != NULL)
    SET_NAMES(result, header);
  
  if (lol->isMatrix || lol->isDataFrame) {
    SEXP attrName, attrVal;
    PROTECT(attrName = allocVector(STRSXP, 1));
    SET_STRING_ELT(attrName, 0, lol->isMatrix ? mkChar("isMatrix") : mkChar("isDataFrame"));
    PROTECT(attrVal = allocVector(LGLSXP, 1));
    intp = LOGICAL_POINTER(attrVal);
    intp[0] = TRUE;
    SET_ATTR(result, attrName, attrVal);
    numprotect += 2;
  }

  UNPROTECT(numprotect);
  return result;
}