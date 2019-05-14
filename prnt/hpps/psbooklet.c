/* psbooklet.c
 * Copyright (C) Angus J. C. Duggan 1991-1995
 * See file LICENSE for details.
 *
 * rearrange pages in conforming PS file for printing in signatures
 *
 * Usage:
 *       psbook [-q] [-s<signature>] [infile [outfile]]
 */
#include <stdio.h>
#include <stdlib.h>
#include "psutil.h"
#include "psspec.h"
#include "pserror.h"
#include "patchlev.h"
#include "hppsfilter.h"


#define MIN(x,y) ((x) > (y) ? (y) : (x))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

/* Global variables */
char *program;
int pages;
int verbose;
char pagelabel[BUFSIZ];
int pageno;
FILE *infile = NULL;
FILE *outfile= NULL;

/* Structure of a node for boklet page order list*/
struct node {
    int data[4];          // Data 
    struct node *next; // Address 
}*head;
struct node *newNode=NULL;
struct node *temp=NULL;
int *data=NULL;

/* Function prototypes */
void createPageList(int maxpages, int *data, int bookletMaker);
void PageListcopy();
void freeList(struct node* head);
void PS_Booklet(char *tempfile, char *bookletfile, char *nupfile,int order, int nup, char* pagesize, int bookletMaker);
void PS_Booklet_Order_Wrapper(char *inputfile, char *outputfile, int order, int bookletMaker);
void PS_Nup_Wrapper(char *inputfile, char *outputfile, char *papersize, int nupnumber);

/* return next larger exact divisor of number, or 0 if none. There is probably
 * a much more efficient method of doing this, but the numbers involved are
 * small, so it's not a big loss. */
static int nextdiv(int n, int m)
{
   while (++n <= m) {
      if (m%n == 0)
	 return (n);
   }
   return (0);
}


void PS_Booklet(char *tempfile, char *bookletfile, char *nupfile,int order, int nup, char* pagesize, int bookletMaker)
{

   // 1. Perform the PS booklwet order
   PS_Booklet_Order_Wrapper(tempfile,bookletfile,order, bookletMaker);
   // 2. Perform the PS Nup and Duplex 
   PS_Nup_Wrapper(bookletfile,nupfile,pagesize,nup);
}

void createPageList(int maxpages, int *data, int bookletMaker)
{
    int i=0;
    int j=0;
    int k=0;
    int n = maxpages/4;
    struct node *tpnode;
    head = (struct node *)malloc(sizeof(struct node));

    /*If unable to allocate memory for head node*/
    if(head == NULL)
    {
        message(FATAL, "\nUnable to allocate memory for head node.\n");
    }
    else
    {
        /* Input data of node from the user */
        for(k=0;k<4;k++)
        {
           head->data[k] = data[j]; // Link data field with data
           j= j+1;
        }
        head->next = NULL; // Link address field to NULL

        temp = head;

        /*Create n-1 nodes and adds to linked list*/
        for(i=2; i<=n; i++)
        {
            temp=head;
            newNode = (struct node *)malloc(sizeof(struct node));

            /* If memory is not allocated for newNode */
            if(newNode == NULL)
            {
                message(FATAL, "\nUnable to allocate memory for new node.\n");
                break;
            }
            else
            {
                for(k=0;k<4;k++)
                {
                   newNode->data[k] = data[j]; // Link data field with data
                   j= j+1;
                }
                if(bookletMaker){
                    newNode->next = head; // Link address field of newNode at the start
                    head = newNode; 
                }
                else{
                    while(temp->next !=NULL)
                        temp= temp->next;

                    newNode->next = NULL; // Link address field of newNode at the end
                    temp->next = newNode; 
                }
 
            }
            
        }

    }
}


void PageListcopy()
{
    struct node *tempnode;
    int i=0;

    /* If the list is empty i.e. head = NULL */
    if(head == NULL)
    {
       message(FATAL, "\nHead of the List is empty.\n");
    }
    else
    {
        tempnode = head;

        while(tempnode != NULL)
        {
            for(i=0;i<4;i++)
            {   
                if (tempnode->data[i] < pages)
                {
	           writepage(tempnode->data[i]);
                }
                else
                {
	           writeemptypage();
                }
            }
            tempnode = tempnode->next;                 // Move to next node
        }
    }
}

void freeList(struct node* head)
{
   struct node* tmp;

   while (head != NULL)
   {
       tmp = head;
       head = head->next;
       free(tmp);
   }
   newNode=NULL;
   temp=NULL;

}


void PS_Booklet_Order_Wrapper(char *inputfile, char *outputfile, int order, int bookletMaker)
{
   int signature = 0;
   int currentpg = 0;
   int maxpage = 0;
   int index=0;

   if ((infile = fopen(inputfile, "r")) == NULL)
   {
      message(FATAL, "can't open input file %s\n", infile);
   }

   if ((outfile = fopen(outputfile, "w")) == NULL)
   {
      message(FATAL, "can't open output file %s\n", outfile);
   }

   if ((infile=seekable(infile))==NULL)
   {
      message(FATAL, "can't seek input\n");
   }

   scanpages();

   if (!signature)
      signature = maxpage = pages+(4-pages%4)%4;
   else
      maxpage = pages+(signature-pages%signature)%signature;

   /* rearrange pages */
   writeheader(maxpage);
   writeprolog();
   writesetup();

   data = (int *)malloc(sizeof(int) * maxpage);
   if(data == NULL)
   {
        message(FATAL, "\ncan't create dynamic memory for booklet page ordering\n");
   }
   for (currentpg = 0; currentpg < maxpage; currentpg++) {
      int actualpg = currentpg - currentpg%signature;
      switch(currentpg%4) {
      case 0:
      case 3:
	 actualpg += signature-1-(currentpg%signature)/2;
	 break;
      case 1:
      case 2:
	 actualpg += (currentpg%signature)/2;
	 break;
      }
     
      data[index++]=actualpg;
   }

   createPageList(maxpage, data, bookletMaker);
   PageListcopy();

   free(data);
   freeList(head);

   writetrailer();
   fclose(infile);
   fclose(outfile);
}

void PS_Nup_Wrapper(char *inputfile, char *outputfile, char *papersize, int nupnumber)
{
   int horiz, vert, rotate, column, flip, leftright, topbottom;
   int nup = 1;
   double draw = 0;				/* draw page borders */
   double xscale, yscale, scale;				/* page scale */
   double uscale = 0;				/* user supplied scale */
   double ppwid, pphgt;				/* paper dimensions */
   double margin, border;			/* paper & page margins */
   double vshift, hshift;			/* page centring shifts */
   double iwidth, iheight ;			/* input paper size */
   double tolerance = 100000;			/* layout tolerance */
   Paper *paper;

   margin = border = vshift = hshift = column = flip = 0;
   leftright = topbottom = 1;
   iwidth = iheight = -1 ;
   verbose = 1;

   if ((infile = fopen(inputfile, "r")) == NULL)
   {
      message(FATAL, "can't open input file %s\n", infile);
   }

   if ((outfile = fopen(outputfile, "w")) == NULL)
   {
      message(FATAL, "can't open output file %s\n", outfile);
   }

   if ((infile=seekable(infile))==NULL)
   {
      message(FATAL, "can't seek input\n");
   }

   if ((paper = findpaper(papersize)) != (Paper *)0 )
   {
	width = (double)PaperWidth(paper);
	height = (double)PaperHeight(paper);
   }

   nup= nupnumber;
   margin = 10;
   /* subtract paper margins from height & width */
   ppwid = width - margin*2;
   pphgt = height - margin*2;

   if (ppwid <= 0 || pphgt <= 0)
      message(FATAL, "paper margins are too large\n");

   /* set default values of input height & width */
   if ( iwidth > 0 )
     width = iwidth ;
   if ( iheight > 0 )
     height = iheight ;

   /* Finding the best layout is an optimisation problem. We try all of the
    * combinations of width*height in both normal and rotated form, and
    * minimise the wasted space. */
   {
      double best = tolerance;
      int hor;
      for (hor = 1; hor; hor = nextdiv(hor, nup)) {
	 int ver = nup/hor;
	 /* try normal orientation first */
	 double scl = MIN(pphgt/(height*ver), ppwid/(width*hor));
	 double optim = (ppwid-scl*width*hor)*(ppwid-scl*width*hor) +
	    (pphgt-scl*height*ver)*(pphgt-scl*height*ver);
	 if (optim < best) {
	    best = optim;
	    /* recalculate scale to allow for internal borders */
	    yscale = MIN((pphgt-2*border*ver)/(height*ver),
			(ppwid-2*border*hor)/(width*hor));
            xscale = yscale;
	    hshift = (ppwid/hor - width*xscale)/2;
	    vshift = (pphgt/ver - height*yscale)/2;
	    horiz = hor; vert = ver;
	    rotate = flip;
	 }
	 /* try rotated orientation */
	 scl = MIN(pphgt/(width*hor), ppwid/(height*ver));
	 optim = (pphgt-scl*width*hor)*(pphgt-scl*width*hor) +
	    (ppwid-scl*height*ver)*(ppwid-scl*height*ver);
	 if (optim < best) {
	    best = optim;
	    /* recalculate scale to allow for internal borders */
            
	    yscale = MIN((pphgt-2*border*hor)/(width*hor),
			(ppwid-2*border*ver)/(height*ver));
            xscale = yscale;
	    hshift = (ppwid/ver - height*yscale)/2;
	    vshift = (pphgt/hor - width*xscale)/2;
	    horiz = ver; vert = hor;
	    rotate = !flip;
	 }
      }

      /* fail if nothing better than worst tolerance was found */
      if (best == tolerance)
	 message(FATAL, "can't find acceptable layout for %d-up\n", nup);
   }

   if (flip) {	/* swap width & height for clipping */
      double tmp = width;
      width = height;
      height = tmp;
   }

   if (rotate) {	/* rotate leftright and topbottom orders */
      int tmp = topbottom;
      topbottom = !leftright;
      leftright = tmp;
      column = !column;
   }

   /* now construct specification list and run page rearrangement procedure */
   {
      int page = 0;
      PageSpec *specs, *tail;

      tail = specs = newspec();

      while (page < nup) {
	 int up, across;		/* page index */

	 if (column) {
	    if (leftright)		/* left to right */
	       across = page/vert;
	    else			/* right to left */
	       across = horiz-1-page/vert;
	    if (topbottom)		/* top to bottom */
	       up = vert-1-page%vert;
	    else			/* bottom to top */
	       up = page%vert;
	 } else {
	    if (leftright)		/* left to right */
	       across = page%horiz;
	    else			/* right to left */
	       across = horiz-1-page%horiz;
	    if (topbottom)		/* top to bottom */
	       up = vert-1-page/horiz;
	    else			/* bottom to top */
	       up = page/horiz;
	 }
	 if (rotate) {
	    tail->xoff = margin + (across+1)*ppwid/horiz - hshift;
	    tail->rotate = 90;
	    tail->flags |= ROTATE;
	 } else {
	    tail->xoff = margin + across*ppwid/horiz + hshift;
	 }
	 tail->pageno = page;
	 if (uscale > 0)
	    tail->scale = uscale;
	 else
	    tail->xscale = xscale;
            tail->yscale = yscale;
	 tail->flags |= SCALE;
	 tail->yoff = margin + up*pphgt/vert + vshift;
	 tail->flags |= OFFSET;
	 if (++page < nup) {
	    tail->flags |= ADD_NEXT;
	    tail->next = newspec();
	    tail = tail->next;
	 }
      }
      
      pstops(nup, 1, 0, specs, draw);		/* do page rearrangement */
   }
   fclose(infile);
   fclose(outfile);
}

