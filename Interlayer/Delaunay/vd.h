/**********************************************************************/
/*                     Voronoi/Delaunay - Module                        */
/*                          header file                               */
/*                                                                    */
/*(c) 1993 by Wolfgang Stuerzlinger(wRZL)               */
/*     This code may be used only for non - commercial purposes !       */
/* This code may not be redistributed without the authors permission. */
/**********************************************************************/

 
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

#define EPSILON (1.0e-6)

#ifndef TRUE
#define TRUE 1==1
#define FALSE 0==1
#endif

/******************************************************************************/
/* Reference for Quad - edge data structure:

Leonidas Guibas and Jorge Stolfi, "Primitives for the manipulation of general
	 subdivisions and the computations of Voronoi diagrams",
	 ACM Transactions on Graphics 4, 74 - 123(1985).
*/
/******************************************************************************/
/**************************** Common data structures **************************/

typedef int SITE_PTR;
typedef int EDGE_PTR;
typedef int INFO_PTR;

// �Ͼ�, ������, �ж�
typedef enum DISCONTINUITY { DBDRY, D0, D1, D2, DINF } DISCONTINUITY;
//DBDRY�߽�
//D0��һ��ϲ� ��ʼ���յ㲻����
//D1�ڶ��ϲ� �յ�����
//D2�����ϲ� ��������
//
typedef struct VEC3							// 
{
	double x, y, z;
	BOOL operator == (VEC3 vec)
	{
		if( vec.x == x && vec.y == y && vec.z == z)
			return TRUE;
		else
			return FALSE;
	};
} VEC3;

typedef struct VEC2						// 
{
	double x, y;
	BOOL operator == (VEC2 vec)
	{
		if( vec.x == x && vec.y == y)
			return TRUE;
		else
			return FALSE;
	};
} VEC2;

struct SITE							// ��
{
	VEC3 vc;						// �����꼰��ֵ
	EDGE_PTR edge;					// ��Ӧ�ߵ�����
	int invisible;					// ���εģ���������
};


typedef struct TRIANGLE				// ���������ݽṹ
{
	int p[3];						// ������������ֵ
	int tri[3];						// ���߶�Ӧ���������������Ϊ-1���Ǳ߽�
									// p[0]-p[1],p[1]-p[2],p[2]-p[0]
}tagTRIANGLE;


struct EDGE_INFO					// ����Ϣ
{
	DISCONTINUITY disco;
};
 
typedef struct SURFACE				// ��
{
	int n_sites;					// ����
	int n_edges;					// ����
	int alloced_sites;				// �������
	struct SITE *sa;				// ������
	EDGE_PTR *next;					// 
	SITE_PTR *org;
	struct EDGE_INFO *ei;			// ����Ϣ
	EDGE_PTR One_bndry_edge;		// 
	VEC3 min, max;						/* bounding box */
	
	EDGE_PTR Last_locate_edge;
	EDGE_PTR next_edge;
	EDGE_PTR avail_edge;
} SURFACE;
 
typedef struct NEIGHBOURS				// �ھ�
{
	struct NEIGHBOURS *next;
	SITE_PTR site;
	DISCONTINUITY d;
} NEIGHBOURS;

/*
����Voronoiͼ���ֽ�̩ɭ����λ�Dirichletͼ��������һ�����������ڵ�ֱ�ߵ�
��ֱƽ������ɵ������������ɡ�N����ƽ����������ĵ㣬�������ڽ�ԭ��
��ƽ�棻ÿ��������������������������Delaunay����������������Voronoi��
���ι���һ���ߵ���ص����Ӷ��ɵ������Ρ�Delaunay�����ε����ԲԲ��������
������ص�Voronoi����ε�һ�����㡣Voronoi��������Delaunayͼ��żͼ��
*/

typedef struct VORONOIS 
{
	struct VORONOIS *next;				/* as we need a ring ! */
	struct VORONOIS *prev;				/* but it is split up later on ! */
	VEC3 from;
	VEC3 to;
	DISCONTINUITY d;
	int done;
} VORONOIS;

/******************************************************************************/
/********************** Macros for manipulation of quad - edges *****************/

//          e->dest /
//   ------------- * ----------
//                 |  
//                 | 
//   o e->left     ^ e     o e->right
//                 |
//                 | e->org
//   ------------- *----------------
//                / 
//               /
//
//
//////////////////////////////////////////////////////////////////////////
//      ��
// 
//   ��ʱ�� next = CCW 
//                  /
//   lnext --<---- * <--- dnext
//                 |
//                 | 
//                 ^ e 
//                 |
//                 |
//   onext --<---- *-----<---- rnext
//                /
//               /
//
//
//////////////////////////////////////////////////////////////////////////
//
//     ˳ʱ��  prev = cw
//
//                  /
//   dprev -->---- * >--- rprev
//                 |
//                 | 
//                 ^ e 
//                 |
//                 |
//   lprev -->---- *----->---- oprev
//                /
//               /
//
//
//////////////////////////////////////////////////////////////////////////
//
//         *  dest                                 * dest 
//        /  \                                    /| \
//       /     \                                 / |   \ 
//      /        \        make face Edge        /  |     \   
//     *           *     --------------->      *   |        * 
//     |   o f    /      <--------------       |o f^ o fnew/  
//     |         /        kill face edge       |   |      /   
//     *        /                              *   |    /
//      \      /                               \   |   /
//       \    /                                 \  |  /
//        \  /                                   \ | / 
//         * org                                   * org 
//
//////////////////////////////////////////////////////////////////////////
//
//           *
//           |
//           V e->Sym
//           |
// e->rotinv |                               *-->--* primal edge
//  o ..>... |  .....<.... o                 o..>..o  dual Edge
//           |   e->rot
//           |
//           ^ e
//           |
//
//
//////////////////////////////////////////////////////////////////////////
//                                          
//                                                   \ |  /        
//                                                    \| /         
//                                                     * v new
//       \ |  /            make vertex edge            |    
//        \| /             ---------->       o left    ^e    o right   
// o left  * v  o right    <-------------              |
//        / \              kill vertex edge            * v
//       /    \                                       / \   
//                                                  /    \
//                                           
//                                           
//////////////////////////////////////////////////////////////////////////

/*  SITE_PTR s  */
#define X(s) _surf->sa[s].vc.x							//
#define Y(s) _surf->sa[s].vc.y
#define Z(s) _surf->sa[s].vc.z
#define Edge(s) _surf->sa[s].edge						// ��
#define Invis(s) _surf->sa[s].invisible					// �Ƿ����
#define Site(s) _surf->sa[s].vc							// 

/*  EDGE_PTR a  */
#define sym(a) ((a) ^ 2)								// ����
#define rot(a) ( (((a) + 1) & 3) | ((a) & ~3) )			// 
#define rotinv(a) ( (((a) + 3) & 3) | ((a) & ~3) )		// 

#define onext(a) _surf->next[a]
#define oprev(a) rot(onext(rot(a)))
#define lnext(a) rot(onext(rotinv(a)))
#define lprev(a) sym(onext(a))
#define rnext(a) rotinv(onext(rot(a)))
#define rprev(a) onext(sym(a))
#define dnext(a) sym(onext(sym(a)))
#define dprev(a) rotinv(onext(rotinv(a)))

#define orig(a) _surf->org[a]
#define dest(a) orig(sym(a))
#define left(a) orig(rotinv(a))
#define right(a) orig(rot(a))

#define origv(a) _surf->sa[orig(a)].vc
#define destv(a) _surf->sa[dest(a)].vc
#define leftv(a) _surf->fa[left(a)].vc
#define rightv(a) _surf->fa[right(a)].vc

#define info(a) _surf->ei[(a)>>2]
#define disco(a) (info(a).disco)

/******************************************************************************/
/******************************* General macros *******************************/

#ifdef DEBUG
#define alive(x) printf(x)
#else
#define alive(x)
#endif

#define V3_dotq(u,v) ((u).x*(v).x + (u).y*(v).y)
#define V3_cprodq(u,v) ((u).x*(v).y - (u).y*(v).x)
#define V3_magnq(u) sqrt((u).x*(u).x + (u).y*(u).y)

/********************************* MISC ***************************************/

#define flipable(e) (disco(e)>D2)
#define disco_edge(e) (disco(e)<=D2)

#define rightofline(X,a,b)		(ccw(X,a,b)< 0.0)
#define rightoronline(X,a,b)	(ccw(X,a,b)<=0.0)
#define leftofline(X,a,b)		(ccw(X,a,b)> 0.0)
#define leftoronline(X,a,b)		(ccw(X,a,b)>=0.0)
#define online(X,a,b)			(ccw(X,a,b)==0.0)

#define rightof(X,e)			(ccw(X,orig(e),dest(e))< 0.0)
#define rightoron(X,e)			(ccw(X,orig(e),dest(e))<=0.0)
#define leftof(X,e)				(ccw(X,orig(e),dest(e))> 0.0)
#define leftoron(X,e)			(ccw(X,orig(e),dest(e))>=0.0)
#define onedge(X,e)				(ccw(X,orig(e),dest(e))==0.0)

#define v_rightofline(X,a,b)	(v_ccw(X,a,b)< 0.0)
#define v_rightoronline(X,a,b)	(v_ccw(X,a,b)<=0.0)
#define v_leftofline(X,a,b)		(v_ccw(X,a,b)> 0.0)
#define v_leftoronline(X,a,b)	(v_ccw(X,a,b)>=0.0)
#define v_online(X,a,b)			(v_ccw(X,a,b)==0.0)

#define v_rightof(X,e)			(v_ccw(X,origv(e),destv(e))< 0.0)
#define v_rightoron(X,e)		(v_ccw(X,origv(e),destv(e))<=0.0)
#define v_leftof(X,e)			(v_ccw(X,origv(e),destv(e))> 0.0)
#define v_leftoron(X,e)			(v_ccw(X,origv(e),destv(e))>=0.0)
#define v_onedge(X,e)			(v_ccw(X,origv(e),destv(e))==0.0)

/* calc center of triangle to the left! of e */
#define cntr(e,corner) circle_center(origv(e),destv(e),destv(onext(e)),corner)

/****************************** PROTOTYPES ***********************************/

/* vd1.c */
void    init_surface(SURFACE *surf, double x0, double y0, double x1, double y1);
void    delete_surface(SURFACE *surf);
BOOLEAN insert_site(SURFACE *surf, double x, double y, double z, SITE_PTR *site);
void    locate_point(SURFACE *surf, double x, double y, double z, SITE_PTR sites[3]);
void    get_coords(SURFACE *surf, SITE_PTR site, double *x, double *y,double *z);

void    for_all_sites(SURFACE *surf, void *ptr, void(*siteproc)(SURFACE *surf, SITE_PTR s, void *ptr ));
void    for_all_edges(SURFACE *surf, void *ptr, 
        void(*edgeproc)(SURFACE *surf, SITE_PTR s1, SITE_PTR s2, DISCONTINUITY d, void *ptr));
void    for_all_triangles(SURFACE *surf, void *ptr, 
        void(*triproc)(SURFACE *surf, SITE_PTR s1, SITE_PTR s2, SITE_PTR s3, void *ptr));
void    flip_flipable_edges(SURFACE *surf, void *ptr);
void    insert_edge(SURFACE *surf, SITE_PTR a, SITE_PTR b,
        DISCONTINUITY discontinuity);


/* vd3.c: INTERNAL ROUTINES !*/

EDGE_PTR    makeedge(SITE_PTR origin, SITE_PTR destination, DISCONTINUITY d);
void        splice(EDGE_PTR a, EDGE_PTR b);
void        swapedge(EDGE_PTR e);
EDGE_PTR    connect_left(EDGE_PTR a, EDGE_PTR b, DISCONTINUITY d);
EDGE_PTR    connect_right(EDGE_PTR a, EDGE_PTR b, DISCONTINUITY d);
void        deleteedge(EDGE_PTR e);
void        splitedge(EDGE_PTR e, SITE_PTR site);
void        push_edge(EDGE_PTR edge);
EDGE_PTR    pop_edge(void);
void        zero_seen(void);
void        zero_seen_int(void);
void        push_sym_ring(EDGE_PTR edge);
void        sef_alloc(int n);
void        sef_enlarge(int additional);
void        sef_free(BOOLEAN bFlag);
void        delete_all_edges(void);
EDGE_PTR    alloc_edge(void);
void        free_edge(EDGE_PTR e);
void        delete_all_faces(void);
BOOLEAN     incircle(SITE_PTR a, SITE_PTR b, SITE_PTR c, SITE_PTR d);
double      distcircle(SITE_PTR a, SITE_PTR b, SITE_PTR c, SITE_PTR d);
double      ccw(SITE_PTR a, SITE_PTR b, SITE_PTR c);
double      v_ccw(VEC3 a, VEC3 b, VEC3 c);
BOOLEAN     intersect_test(VEC3 from, VEC3 to, EDGE_PTR e);
BOOLEAN     vintersect(VEC3 a, VEC3 b, VEC3 a1, VEC3 b1, VEC3 *inter);
void        circle_center(VEC3 a, VEC3 b, VEC3 c, VEC3 *corner);
double      V3_cprod(VEC3 u, VEC3 v);
double      V3_dot(VEC3 u, VEC3 v);
VEC3        V3_times(double c, VEC3 v);
VEC3		V3_sum(VEC3 u, VEC3 v);
VEC3		V3_sub(VEC3 u,VEC3 v);
double      V3_magn(VEC3 u);
VEC3        V3_cross(VEC3 v);
VEC3        V3_crossl(VEC3 v);
VEC3        V3_normalize(VEC3 v);
VEC3        V3_midpoint(VEC3 v, VEC3 v1);

//*****************************************************************************
//zj add
//

BOOLEAN	 intriangle(SURFACE *surf,double x, double y, SITE_PTR s[3]);
int whichside(double x, double y, double x1, double y1, double x2, double y2);
BOOLEAN intersect(VEC2 p1[2], VEC2 p2[2], VEC2 *p);
void incenter_of_tri(VEC3 p1, VEC3 p2, VEC3 p3, VEC3 *p);
int agaus(double *a, double *b, int n);
double LineInterpolation(double xp,double yp,VEC3 p1,VEC3 p2,VEC3 p3);