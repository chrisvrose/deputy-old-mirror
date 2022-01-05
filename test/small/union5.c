//KEEP baseline: success

//Example from the spec95 Lisp interpreter: change a union tag to
// an equivalent one without zeroing the union.

#include "harness.h"
#include <string.h> //memset

typedef struct file FILE;

/* node types */
#define FREE	0
#define SUBR	1
#define FSUBR	2
#define LIST	3
#define SYM	4
#define INT	5
#define STR	6
#define OBJ	7
#define FPTR	8
#define FLOAT	9
#define VECT	10

/* node flags */
#define MARK	1
#define LEFT	2

/* string types */
#define DYNAMIC	0
#define STATIC	1

/* new node access macros */
#define ntype(x)	((x)->n_type)

/* type predicates */
#define atom(x)		((x) == NIL || (x)->n_type != LIST)
#define null(x)		((x) == NIL)
#define listp(x)	((x) == NIL || (x)->n_type == LIST)
#define consp(x)	((x) && (x)->n_type == LIST)
#define subrp(x)	((x) && (x)->n_type == SUBR)
#define fsubrp(x)	((x) && (x)->n_type == FSUBR)
#define stringp(x)	((x) && (x)->n_type == STR)
#define symbolp(x)	((x) && (x)->n_type == SYM)
#define filep(x)	((x) && (x)->n_type == FPTR)
#define objectp(x)	((x) && (x)->n_type == OBJ)
#define fixp(x)		((x) && (x)->n_type == INT)
#define floatp(x)	((x) && (x)->n_type == FLOAT)
#define vectorp(x)	((x) && (x)->n_type == VECT)

typedef struct node {
    char n_type;		/* type of node */
    char n_flags;		/* flag bits */
    union {			/* value */
	struct xsubr {		/* subr/fsubr node */
            /* WEIMER struct node *(*xsu_subr)();	pointer to an internal routine */
            struct node *(*xsu_subr)(struct node *);	
	} n_xsubr  WHEN(n_type == SUBR || n_type == FSUBR);
	struct xlist {			/* list node (cons) or symbol, too */
	    struct node *xl_car;	/* the car pointer */
	    struct node *xl_cdr;	/* the cdr pointer */
	} n_xlist  WHEN(n_type == LIST || n_type == SYM || n_type == FREE);
	struct xint {			/* integer node */
	    long xi_int;		/* integer value */
	} n_xint  WHEN(n_type == INT);
	struct xfloat {			/* float node */
	    double xf_float;		/* float value */
	} n_xfloat  WHEN(n_type == FLOAT
                         || n_type == VECT   //KEEP notexclusive: error = Warning: Setting this tag makes two fields active
                         );
	struct xstr {			/* string node */
	    int xst_type;		/* string type */
	    char * NTS xst_str;		/* string pointer */
	} n_xstr  WHEN(n_type == STR);
	struct xfptr {			/* file pointer node */
	    FILE *xf_fp;		/* the file pointer */
	    int xf_savech;		/* lookahead character for input files */
	} n_xfptr  WHEN(n_type == FPTR);
	struct xvect {			/* vector node */
	    int xv_size;		/* vector size */
	    struct node ** COUNT(xv_size) xv_data;	/* vector data */
	} n_xvect  WHEN(n_type == VECT || n_type == OBJ);
    } n_info ; 
} NODE;

/* newnode - allocate a new node */
NODE *newnode(int type)
{
  NODE * nnode = malloc(sizeof(NODE));

  /* initialize the new node */
  memset(&nnode->n_info, 0, sizeof(nnode->n_info));
  nnode->n_type = type;
  
  /* return the new node */
  return (nnode);
}

//Test for union in a struct.
int main() {
  NODE * obj = newnode(VECT);
  NODE ** data =  malloc(4 * sizeof(NODE*));
  obj->n_info.n_xvect.xv_size = 4;
  obj->n_info.n_xvect.xv_data = data;

  //Now change the type to obj:
  obj->n_type = OBJ;    //KEEP difftag: success

  obj->n_type = STR;    //KEEP wrongtag: error = Assertion failed
  
  if (obj->n_info.n_xvect.xv_data != data) E(1);

  //And change it back:
  obj->n_type = VECT+256; //the +256 goes away because n_type is a char.
  
  if (obj->n_info.n_xvect.xv_data != data) E(2);
  return 0;
}
