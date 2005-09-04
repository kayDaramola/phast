/* $Id: indel_history.h,v 1.3 2005-09-04 21:27:56 acs Exp $
   Written by Adam Siepel, 2005
   Copyright 2005, Adam Siepel, University of California */

#ifndef IND_HIST
#define IND_HIST

#include <stdio.h>
#include <trees.h>
#include <msa.h>

#define NINDEL_CHARS 3
typedef enum {INS, DEL, BASE} indel_char; /* note: order is used in places */

typedef struct {
  indel_char type;
  int start;
  int len;
} Indel;

typedef struct {
  TreeNode *tree;
  int ncols;
  List **indels;
} CompactIndelHistory;

typedef struct {
  TreeNode *tree;
  int ncols;
  char **indel_strings;            /* make bin vector later */
} IndelHistory;

IndelHistory *ih_new(TreeNode *tree, int ncols);
void ih_free(IndelHistory *ih);
CompactIndelHistory *ih_new_compact(TreeNode *tree, int ncols);
void ih_free_compact(CompactIndelHistory *cih);
IndelHistory *ih_expand(CompactIndelHistory *cih);
CompactIndelHistory *ih_compact(IndelHistory *ih);
void ih_print(IndelHistory *ih, FILE *outf, char *msa_name, char *prog_name);
void ih_print_compact(CompactIndelHistory *cih, FILE *outf, char *msa_name, 
                      char *prog_name);
MSA *ih_as_alignment(IndelHistory *ih, MSA *msa);
CompactIndelHistory *ih_read_compact(FILE *inf);
IndelHistory *ih_new_from_file(FILE* inf);
IndelHistory *ih_extract_from_alignment(MSA *msa, TreeNode *tree);
IndelHistory *ih_reconstruct(MSA *msa, TreeNode *tree);
void ih_convert_ia_names(MSA *msa, TreeNode *tree);

#endif