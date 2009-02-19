/***************************************************************************
 * PHAST: PHylogenetic Analysis with Space/Time models
 * Copyright (c) 2002-2005 University of California, 2006-2009 Cornell 
 * University.  All rights reserved.
 *
 * This source code is distributed under a BSD-style license.  See the
 * file LICENSE.txt for details.
 ***************************************************************************/

/* $Id: tree_model.h,v 1.20 2009-02-19 17:25:28 acs Exp $ */

#ifndef TREE_MODEL_H
#define TREE_MODEL_H

#include <vector.h>
#include <markov_matrix.h> 
#include <trees.h> 
#include <stringsplus.h> 
#include <msa.h> 
#include <misc.h>
#include <numerical_opt.h>
#include <assert.h>
#include <subst_mods.h>

#define MAX_ALPH_SIZE 100

#define TM_IMAG_EPS 1e-6           
                                /* in cases where a function of
                                   complex numbers is supposed to be
                                   real, we'll consider it close
                                   enough if the imaginary component is
                                   smaller in magnitude than this
                                   threshold  */

/* convergence thresholds for EM, expressed as portions of function
   value: (f(xold) - f(xnew)) / f(xnew) */
#define TM_EM_CONV_HIGH 1e-8
#define TM_EM_CONV_MED 5e-7
#define TM_EM_CONV_LOW 1e-5
#define TM_EM_CONV_CRUDE 1e-4

#define TM_EM_CONV(P) ( (P) == OPT_HIGH_PREC ? TM_EM_CONV_HIGH : ( (P) == OPT_MED_PREC ? TM_EM_CONV_MED : ( (P) == OPT_CRUDE_PREC ? TM_EM_CONV_CRUDE : TM_EM_CONV_LOW) ))

/* type of branch length estimation */
typedef enum { 
  TM_BRANCHLENS_ALL, 
  TM_SCALE_ONLY, 
  TM_BRANCHLENS_NONE 
} blen_estim_type;

/* used in bounded estimation of subtree scale */
typedef enum {
  NB,                           /* no bound */
  LB,                           /* lower bound -- subtree scale at
                                   least as large as scale of whole
                                   tree */
  UB                            /* upper bound -- subtree scale at
                                   most as large as scale of whole
                                   tree */
} scale_bound_type; 

struct tp_struct;

/* defines alternative substitution model for a particular branch */
typedef struct {
  subst_mod_type subst_mod;  
  Vector *backgd_freqs;
  MarkovMatrix *rate_matrix;
} AltSubstMod;

/** Tree model object */
struct tm_struct {
  TreeNode *tree;
  Vector *backgd_freqs;
  MarkovMatrix *rate_matrix;
  subst_mod_type subst_mod;
  int *msa_seq_idx;             /**< (optional) Mapping from leaf
                                   indices to sequence indices in a
                                   given MSA; used in likelihood
                                   computations */
  MSA *msa;                     /* (optional) MSA from which TreeModel
                                   was estimated; for use in tm_fit */
  int category;                 /* (optional) site category in MSA or -1 */
  int order;
  double alpha;                 /* for gamma or discrete-gamma rate
                                   variation; set to 0 for constant rate */
  int nratecats;                /* number of rate categories, discrete gamma */
  double lnL;                   /* log likelihood from training */
  struct tp_struct *tree_posteriors; 
                                /* (optional) associated
                                   TreePosteriors object */
  int use_conditionals;         /* (optional) compute likelihood using
                                   conditional probabilities at each
                                   column; only relevant when order >
                                   0 */
  MarkovMatrix ***P;            /* probability matrices for edges,
                                   indexed by node id and rate category */
  double *rK, *freqK;           /* rate constants and freqs */
  List **rate_matrix_param_row, **rate_matrix_param_col;
  int root_leaf_id;             /* indicates id of leaf node to be
                                   interpretted as the root.  Must be
                                   a child of the actual root of the
                                   tree.  Ordinarily will have a null
                                   value (-1), but if non-negative, the
                                   distance to its parent will be
                                   constrained to be zero.  */
  int allow_gaps;
  int allow_but_penalize_gaps;
  int inform_reqd;              /* if TRUE, only "informative" sites
                                   will be given non-zero probability */
  int estimate_backgd;          /* estimate backgd freqs as free
                                   parameters in the optimization */
  blen_estim_type estimate_branchlens; 
                                /* if value is TM_BRANCHLENS_ALL, then
                                   estimate all branch lengths; if
                                   value is TM_SCALE_ONLY, then
                                   estimate only a scale factor; and
                                   if value is TM_BRANCHLENS_NONE, do
                                   not estimate branch lengths */
  double scale;                 /* current scale factor */
  double scale_sub;             /* scale factor for subtree, when
                                   estimating separate scale factors
                                   for subtree and supertree */
  int *in_subtree;              /* array indicating whether each
                                   branch is in designated subtree */
  scale_bound_type scale_sub_bound;
                                /* bound on scale of subtree */
  TreeNode *subtree_root;       /* node defining subtree */
  int *ignore_branch;           /* if ignore_branch is non-NULL and
                                   ignore_branch[i] == TRUE, then
                                   branch i will be ignored (treated
                                   as infinitely long) in likelihood
                                   calculations */
  int empirical_rates;          /* indicates "empirical"
                                   (nonparameteric) model for
                                   rate-variation */
  int estimate_ratemat;         /* indicates whether rate-matrix
                                   parameters should be estimated */
  AltSubstMod **alt_subst_mods; /* optional alternative substitution
                                   models per branch of three; can be
                                   used for nonhomogeneous models.
                                   Typically set to NULL and
                                   ignored.  */
};

typedef struct tm_struct TreeModel;

TreeModel *tm_new(TreeNode *tree, MarkovMatrix *rate_matrix, 
                  Vector *backgd_freqs, subst_mod_type subst_mod, 
                  char *alphabet, int nratecats, double alpha,
                  List *rate_consts, int root_leaf_id);

void tm_reinit(TreeModel *tm, subst_mod_type subst_mod,
               int new_nratecats, double new_alpha, 
               List *new_rate_consts, List *new_rate_weights);

TreeModel *tm_new_from_file(FILE *F);

void tm_init_rmp(TreeModel *tm);

void tm_free_rmp(TreeModel *tm);

void tm_free(TreeModel *tm);

void tm_print(FILE *F, TreeModel *tm);

void tm_cpy(TreeModel *dest, TreeModel *src);

TreeModel *tm_create_copy(TreeModel *src);

void tm_set_subst_matrices(TreeModel *tm);

void tm_set_subst_matrix(TreeModel *tm, MarkovMatrix *P, double t);

void tm_scale(TreeModel *tm, double scale_const, int reset_subst_mats);

int tm_fit(TreeModel *mod, MSA *msa, Vector *params, int cat, 
           opt_precision_type precision, FILE *logf);

void tm_unpack_params(TreeModel *mod, Vector *params, int idx_offset);

double tm_scale_rate_matrix(TreeModel *mod);

void tm_scale_params(TreeModel *mod, Vector *params, double scale_factor);

Vector *tm_params_init(TreeModel *mod, double branchlen, double kappa,
                           double alpha);

Vector *tm_params_init_random(TreeModel *mod);

Vector *tm_params_new_init_from_model(TreeModel *mod);

void tm_params_init_from_model(TreeModel *mod, Vector *params,
                                      int start_idx);

int tm_get_nparams(TreeModel *mod);

int tm_get_neqfreqparams(TreeModel *mod);

int tm_get_nratevarparams(TreeModel *mod);

int tm_is_reversible(int subst_mod);

int tm_get_nbranchlenparams(TreeModel *mod);

MSA *tm_generate_msa(int ncolumns, HMM *hmm, 
                     TreeModel **classmods, int *labels);

TreeModel *tm_induced_aa(TreeModel *codon_mod);

void tm_build_seq_idx(TreeModel *mod, MSA *msa);

void tm_prune(TreeModel *mod, MSA *msa, List *names);

double tm_extrapolate_and_prune(TreeModel *mod, TreeNode *extrapolate_tree, 
                                MSA *msa, List *pruned_names);

void tm_reset_tree(TreeModel *mod, TreeNode *newtree);

void tm_set_ignore_branches(TreeModel *mod, List *ignore_branches);

AltSubstMod* tm_new_alt_subst_mod(subst_mod_type subst_mod,
                                  Vector *backgd_freqs, 
                                  MarkovMatrix *rate_matrix);

void tm_free_alt_subst_mod(AltSubstMod *am); 

#endif
