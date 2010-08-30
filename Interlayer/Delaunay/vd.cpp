/**********************************************************************/
/*                     Voronoi/Delaunay - Module                        */
/*                      main interface module                         */
/*                                                                    */
/*(c) 1993 by Wolfgang Stuerzlinger(wRZL)               */
/*     This code may be used only for non - commercial purposes !       */
/* This code may not be redistributed without the authors permission. */
/**********************************************************************/

#include "stdafx.h"
#include "vd.h"
#include <float.h>
#include <math.h>

#define SITEALLOC 1000

#define ABS(x) ( (x) < 0 ? -(x) : (x) )

SURFACE *_surf;

static BOOLEAN v_locate(VEC3 site, EDGE_PTR *edge, SITE_PTR *oldsite);
static void incremental_delaunay(SITE_PTR site, EDGE_PTR edge);
static void fix_edge_right(EDGE_PTR *e);
static void triangulate_left(EDGE_PTR first);

/****************************************************************/
/*  Top - level Delaunay Triangulation Procedure					*/
/****************************************************************/

void init_surface(SURFACE *surf, double x0, double y0, double x1, double y1)
{
	double lenx, leny;
	EDGE_PTR a, b, c;
	
	_surf = surf;
	surf->alloced_sites = SITEALLOC;
	sef_alloc(surf->alloced_sites);
	delete_all_edges();
	surf->min.x = x0;
	surf->min.y = y0;
	surf->max.x = x1;
	surf->max.y = y1;
	surf->n_edges = 0;
	
	/******************** Generate Basic Mesh **************************/
	surf->n_sites = 4;				/* we generate a 10% bigger square */
	lenx = (x1 - x0) / 10.0;
	leny = (y1 - y0) / 10.0;
	X(0) = x0 - lenx;
	Y(0) = y0 - leny;
	Invis(0) = 1;
	X(1) = x1 + lenx;
	Y(1) = y0 - leny;
	Invis(1) = 1;
	X(2) = x1 + lenx;
	Y(2) = y1 + leny;
	Invis(2) = 1;
	X(3) = x0 - lenx;
	Y(3) = y1 + leny;
	Invis(3) = 1;
	a = makeedge(0, 3, DBDRY);
	b = makeedge(1, 2, DBDRY);
	c = connect_left(sym(b), a, DBDRY);
	surf->One_bndry_edge = sym(c);
	c = sym(connect_right(rprev(c), c, DINF));
	c = connect_left(b, sym(c), DBDRY);
	
	surf->Last_locate_edge = surf->One_bndry_edge;		/* init locate */
}

void delete_surface(SURFACE *surf)
{
	_surf = surf;
	sef_free(TRUE);
}

BOOLEAN insert_site(SURFACE *surf, double x, double y, double z, SITE_PTR *site)
{
	EDGE_PTR e;
	VEC3 xyz;
	int n;
	
	_surf = surf;
	if (x < _surf->min.x || x > _surf->max.x ||
		y < _surf->min.y || y > _surf->max.y)
	{
		fprintf(stderr, "SITE outside! %g,%g\n", x, y);
		*site = 0;
		return (FALSE);
	}

	xyz.x = x;
	xyz.y = y;
	xyz.z = z;
	if (v_locate(xyz, & e, site))
	{
		return (FALSE);
	}
	if (surf->n_sites == surf->alloced_sites)		/* we're full ! */
		sef_enlarge(SITEALLOC);
	
	n = surf->n_sites++;
	Site(n) = xyz;
	Invis(n) = 0;				/* it's visible */
	
	incremental_delaunay(n, e);
	*site = n;
	return (TRUE);
}

void locate_point(SURFACE *surf, double x, double y, double z, SITE_PTR sites[3])
{
	EDGE_PTR e;
	SITE_PTR site;
	VEC3 xyz;
	
	_surf = surf;
	if (x < _surf->min.x || x > _surf->max.x ||
		y < _surf->min.y || y > _surf->max.y)
	{
		fprintf(stderr, "POINT outside! %g,%g\n", x, y);
		return;
	}
	
	xyz.x = x;
	xyz.y = y;
	xyz.z = z;
	v_locate(xyz, & e, & site);
	//if (v_locate(xyz, & e, & site))			/* already there */
	//	;								/* don't    care */
	
	sites[0] = orig(e);
	sites[1] = dest(e);
	sites[2] = dest(lnext(e));
}

void get_coords(SURFACE *surf, SITE_PTR site, double *x, double *y, double *z)
{
	_surf = surf;
	*x = X(site);
	*y = Y(site);
	*z = Z(site);
}

void for_all_sites(SURFACE *surf, void *ptr, void(*siteproc)(SURFACE *surf, SITE_PTR s, void *ptr))
{
	SITE_PTR site;
	
	_surf = surf;
	for (site = 0; site < surf->n_sites; site++)
	{
		if (Invis(site))
			continue;
		(*siteproc)(surf, site, ptr);
	}
}

void for_all_edges(SURFACE *surf, void *ptr, 
	void(*edgeproc)(SURFACE *surf, SITE_PTR s1, SITE_PTR s2, DISCONTINUITY d, void *ptr))
{
	int i;
	EDGE_PTR e;
	
	_surf = surf;
	if (_surf->avail_edge != -1)
	{
		fprintf(stderr, "Aw, Shit not all edges are valid !\n");
		return;
	}
	e = 0;
	for (i = 0; i < _surf->n_edges; i++, e += 4)
	{
		if (Invis(orig(e)) || Invis(dest(e)))
			continue;
		(*edgeproc)(surf, orig(e), dest(e), disco(e), ptr);
	}
}

void for_all_triangles(SURFACE *surf, void *ptr, 
		void(*triproc)(SURFACE *surf, SITE_PTR s1, SITE_PTR s2, SITE_PTR s3, void *ptr))
{
	EDGE_PTR e, t;
	SITE_PTR site;
	
	_surf = surf;
	for (site = 0; site < surf->n_sites; site++)
	{
		if (Invis(site))
			continue;
		e = Edge(site);
		t = e;
		do 
		{
			if (! Invis(dest(e)) && ! Invis(dest(onext(e))) &&
				Y(site) < Y(dest(onext(e))))
			{
				if (Y(site) < Y(dest(e)) ||(Y(site) == Y(dest(e)) &&
					X(site) < X(dest(e))))
					(*triproc)(surf, site, dest(e), dest(onext(e)), ptr);
			}
			e = oprev(e);
		} while (t != e);
	}
}

void flip_flipable_edges(SURFACE *surf, void *ptr)
{
	int i;
	double dist1, dist2;
	EDGE_PTR e;
	SITE_PTR s1, s2, s3, s4;
	
	_surf = surf;
	if (_surf->avail_edge != -1)
	{
		fprintf(stderr, "Aw, Shit not all edges are valid !\n");
		return;
	}
	e = 0;
	for (i = 0; i < _surf->n_edges; i++, e += 4)
	{
		s1 = orig(e);
		s2 = dest(e);
		if (disco_edge(e) || Invis(s1) || Invis(s2))
			continue;
		s3 = dest(oprev(e));		/* point on right */
		s4 = dest(onext(e));		/* point on left */
		dist1 = distcircle(s1, s3, s2, s4);
		dist2 = distcircle(s3, s2, s4, s1);
		if (dist1 >= 0.0 && dist2 >= 0.0)
		{
			/* both are inside */
			swapedge(e);
		}
	}
}

/****************************************************************/
/*  Incremental Delaunay Triangulation Procedure				*/
/****************************************************************/

/* find an edge where site lies left or on ! */

static BOOLEAN v_locate(VEC3 site, EDGE_PTR *edge, SITE_PTR *oldsite)
{
	register EDGE_PTR e;
	int i;
	i = 0;
	e = _surf->Last_locate_edge;
	while (1) 
	{
		i++;
		if (site.x == X(orig(e)) && site.y == Y(orig(e)))
		{
			*edge = e;
			*oldsite = orig(e);
			
			return (TRUE);
		}
		else if (site.x == X(dest(e)) && site.y == Y(dest(e)))
		{
			*edge = e;
			*oldsite = dest(e);
			
			return (TRUE);
		}
		else if (v_rightof(site, e))
			e = sym(e);
		else if (v_leftoron(site, onext(e)))
			e = onext(e);
		else if (v_leftoron(site, dprev(e)))
			e = dprev(e);
		else
			break;
	}
	/* As Gordon said: Check it! */
	assert(v_leftoron(site, e));
	assert(v_leftof(site, lnext(e)));
	assert(v_leftof(site, lprev(e)));
	assert(v_rightoron(site, sym(e)));
	assert(v_rightof(site, dprev(e)));
	assert(v_rightof(site, onext(e)));
	assert(dest(lnext(e)) == orig(lprev(e)));
	assert(dest(onext(e)) == orig(dprev(e)));
	assert(dest(lnext(e)) == orig(dprev(e)));
	*edge = e;
	*oldsite = 0;
	return (FALSE);
}

/****************************************************************/
/*  Incremental Edge Insertion									*/
/*  with a lot of ideas by Dani Lischinski						*/
/****************************************************************/

/* this one does not flip unflippable edges ! */

static void incremental_delaunay(SITE_PTR site, EDGE_PTR edge)
{
	SITE_PTR first;
	EDGE_PTR e, t, base;
	EDGE_PTR a, b, c, d;
	
	e = edge;						/* site lies left or on edge ! */
	
	if (onedge(site, e))
	{
		/* we are on an existing edge */
		a = oprev(e);
		b = dnext(e);
		c = lnext(e);
		d = lprev(e);
		splitedge(e, site);
		connect_left(e, lprev(e), DINF);
		connect_left(oprev(e), sym(e), DINF);
		fix_edge_right(&a);
		fix_edge_right(&b);
		fix_edge_right(&c);
		fix_edge_right(&d);
		return;
	}
	first = orig(e);
	base = makeedge(first, site, DINF);
	splice(base, e);
	do 
	{
		base = connect_left(e, sym(base), DINF);
		e = oprev(base);
	} while (dest(e) != first);
	
	while (1) 
	{
		t = oprev(e);
		if (flipable(e) && rightof(dest(t), e) &&
			incircle(orig(e), dest(t), dest(e), site))
		{
			swapedge(e);
			e = oprev(e);
			continue;
		}
		else if (orig(e) == first)
			break;
		e = lprev(onext(e));
	}
}

/* recursive edge flipping to the right of an edge */

static void fix_edge_right(EDGE_PTR *e)
{
	EDGE_PTR f, g;
	
	if (disco_edge(*e))
		return;
	f = oprev(*e);
	g = dnext(*e);
	if (incircle(dest(*e), dest(onext(*e)), orig(*e), dest(f)))
	{
		swapedge(*e);
		fix_edge_right(&f);
		fix_edge_right(&g);
	}
}

/* triangulate anew the loop to the left of first */
/* ??? maybe this only works if all vertices lie to the left of last */
/* i.e. the edge preceeding first ??? */

static void triangulate_left(EDGE_PTR first)
{
	EDGE_PTR last, a, b, ea, t1, t2;
	
	fflush(stdout);
	last = lprev(first);
	while (lnext(lnext(first)) != last) 
	{		/* still > triangle */
		ea = lnext(first);
		t1 = first;
		while (ea != last) 
		{
			alive("T");
			t2 = lnext(ea);
			if (t2 == last && t1 == first)
				break;
			if (leftof(dest(ea), t1))
			{
				if (t1 == first)
				{
					first = sym(connect_left(ea, t1, DINF));
					t1 = first;
				}
				else
					t1 = sym(connect_left(ea, t1, DINF));
				a = oprev(t1);
				b = dnext(t1);
				fix_edge_right(&a);
				fix_edge_right(&b);
				ea = t2;
			}
			else 
			{
				t1 = ea;
				ea = t2;
			}
		}
	}
	a = lnext(last);
	b = lprev(last);
	fix_edge_right(&a);
	fix_edge_right(&b);
	fix_edge_right(&last);
}

void insert_edge(SURFACE *surf, SITE_PTR a, SITE_PTR b,
				DISCONTINUITY discontinuity)
{
	EDGE_PTR ea, eb, t, ne;
	SITE_PTR last, cross;
	VEC3 inter;
	
	_surf = surf;
	ea = Edge(a);				/* get edge with a as orig */
	eb = Edge(b);				/* get edge with b as orig */
	
	alive(".");
	last = orig(eb);
	while (orig(ea) != last) 
	{
		alive("e");
		while (rightof(b, ea) || leftoron(b, onext(ea)))
			ea = onext(ea);
		assert(dest(onext(ea)) != last);
		assert(! online(dest(onext(ea)), a, b));
		
		if (online(dest(ea), a, b))
		{
			disco(ea) = discontinuity;
			ea = sym(ea);
			continue;
		}
		t = ea;
		while (1) 
		{
			alive("E");
			if (online(dest(lnext(t)), a, b))
			{
				if (lnext(lnext(lnext(t))) == ea)
				{
					disco(lnext(lnext(t))) = discontinuity;
					ea = sym(lnext(t));
					break;
				}
				else 
				{
					ne = connect_left(lnext(t), ea, discontinuity);
					ea = sym(lnext(t));
					triangulate_left(lnext(ne));
					triangulate_left(oprev(ne));
					break;
				}
			}
			if (leftofline(dest(lnext(t)), a, b))
			{
				/* left not on! */
				if (flipable(lnext(t)))
					deleteedge(lnext(t));
				else 
				{							/* 2 x - ing disco edges */
					assert(vintersect(origv(lnext(t)), destv(lnext(t)),
						Site(a), Site(b), & inter));
					assert(inter.x != X(orig(lnext(t))) ||
						inter.y != Y(orig(lnext(t))));
					/* orig not on edge but cross == dest */
					assert(inter.x != X(dest(lnext(t))) ||
						inter.y != Y(dest(lnext(t))));
					/* dest not on edge but cross = dest */
					if (surf->n_sites == surf->alloced_sites)	/* full */
						sef_enlarge(SITEALLOC);
					cross = surf->n_sites++;
					Site(cross) = inter;
					Invis(cross) = 0;
					
					splitedge(lnext(t), cross);
					ne = connect_left(lnext(t), ea, discontinuity);
					ea = sym(lnext(t));
					triangulate_left(lnext(ne));
					triangulate_left(oprev(ne));
					break;
				}
			}
			else
				t = lnext(t);
		}
	}
}

/****************************************************************/
/*	Quad - edge manipulation primitives */
/****************************************************************/
EDGE_PTR makeedge(SITE_PTR origin, SITE_PTR destination, DISCONTINUITY d)
{
	register EDGE_PTR temp, ans;
	
	assert(origin != destination);
	assert(X(origin) != X(destination) || Y(origin) != Y(destination));
	
	temp = alloc_edge();
	ans = temp;
	disco(ans) = d;
	onext(temp) = ans;
	orig(temp) = origin;
	onext(++temp) = ans + 3;
	onext(++temp) = ans + 2;
	orig(temp) = destination;
	onext(++temp) = ans + 1;
	Edge(origin) = ans;
	Edge(destination) = sym(ans);
	return (ans);
}

void splice(EDGE_PTR a, EDGE_PTR b)
{
	EDGE_PTR alpha, beta, temp;
	
	alpha = rot(onext(a));
	beta = rot(onext(b));
	temp = onext(alpha);
	onext(alpha) = onext(beta);
	onext(beta) = temp;
	temp = onext(a);
	onext(a) = onext(b);
	onext(b) = temp;
	Edge(orig(a)) = a;
	Edge(orig(b)) = b;
}

void swapedge(EDGE_PTR e)
{
	EDGE_PTR a, b, syme;
	
	a = oprev(e);
	syme = sym(e);
	b = oprev(syme);
	splice(e, a);
	splice(syme, b);
	splice(e, lnext(a));
	splice(syme, lnext(b));
	orig(e) = dest(a);
	Edge(orig(a)) = a;		/* correct point references */
	dest(e) = dest(b);
	Edge(orig(b)) = b;
}

EDGE_PTR connect_left(EDGE_PTR a, EDGE_PTR b, DISCONTINUITY d)
{
	register EDGE_PTR ans;
	
	ans = makeedge(dest(a), orig(b), d);
	splice(ans, lnext(a));
	splice(sym(ans), b);
	return (ans);
}

EDGE_PTR connect_right(EDGE_PTR a, EDGE_PTR b, DISCONTINUITY d)
{
	register EDGE_PTR ans;
	
	ans = makeedge(dest(a), orig(b), d);
	splice(ans, sym(a));
	splice(sym(ans), oprev(b));
	return (ans);
}

void deleteedge(EDGE_PTR e)
	/* disconnects e from the rest of the structure and destroys it. */
{
	if (e == _surf->Last_locate_edge || e == sym(_surf->Last_locate_edge))
		_surf->Last_locate_edge = onext(e);
	Edge(orig(e)) = oprev(e);	/* reset point references */
	Edge(dest(e)) = lnext(e);
	splice(e, oprev(e));
	splice(sym(e), oprev(sym(e)));
	free_edge(e);
}

/* copied from code by Dani Lischinski */

void splitedge(EDGE_PTR e, SITE_PTR site)
{
	EDGE_PTR t, newe;
	
	t = lnext(e);
	splice(sym(e), t);
	Edge(orig(t)) = t;		/* to make sure it's ok ! */
	dest(e) = site;
	newe = connect_left(e, t, disco(e));
	Edge(site) = newe;
}

/****************************************************************/
/*	Quad - edge storage allocation */
/****************************************************************/

/******************** Overall storage allocation ****************/

void sef_alloc(int n)
{
	_surf->sa = new SITE[n];
	_surf->org = new SITE_PTR[12*n];
	_surf->next = new EDGE_PTR[12*n];
	_surf->ei = new EDGE_INFO[3*n];
}

void sef_enlarge(int additional)
{
	int n;
	SURFACE old_surf, *temp;
	
	old_surf = *_surf;
	n = _surf->alloced_sites;
	_surf->alloced_sites += additional;
	sef_alloc(_surf->alloced_sites);
	memcpy(_surf->sa, old_surf.sa, sizeof(struct SITE) * n);
	memcpy(_surf->org, old_surf.org, sizeof(SITE_PTR) * 12 * n);
	memcpy(_surf->next, old_surf.next, sizeof(EDGE_PTR) * 12 * n);
	memcpy(_surf->ei, old_surf.ei, sizeof(struct EDGE_INFO) * 3 * n);
	
	temp = _surf;
	_surf = &old_surf;
	sef_free(FALSE);
	_surf = temp;
}

void sef_free(BOOLEAN bFlag)
{
	delete []_surf->sa;
	delete []_surf->org;
	delete []_surf->next;
	delete []_surf->ei;
}

/*************************** Edge storage management *************************/

#define NYL -1

void delete_all_edges(void)
{
	_surf->next_edge = 0;
	_surf->avail_edge = NYL;
	_surf->n_edges = 0;
}

EDGE_PTR alloc_edge(void)
{
	EDGE_PTR ans;
	
	_surf->n_edges++;
	if (_surf->avail_edge == NYL)
		ans = _surf->next_edge, _surf->next_edge += 4;
	else
		ans = _surf->avail_edge, _surf->avail_edge = onext(_surf->avail_edge);
	return (ans);
}

void free_edge(EDGE_PTR e)
{
	e ^= e & 3;
	onext(e) = _surf->avail_edge;
	_surf->avail_edge = e;
	_surf->n_edges--;
}

/****************************************************************/
/*	Geometric primitives */
/****************************************************************/

BOOLEAN incircle(SITE_PTR a, SITE_PTR b, SITE_PTR c, SITE_PTR d)
{
	double az, bz, cz, dz, result;
	
	az = X(a) * X(a) + Y(a) * Y(a);
	bz = X(b) * X(b) + Y(b) * Y(b);
	cz = X(c) * X(c) + Y(c) * Y(c);
	dz = X(d) * X(d) + Y(d) * Y(d);
	result = (Y(a) *(bz * X(c) + cz * X(d) + X(b) * dz) +
		az *(X(b) * Y(c) + Y(b) * X(d) + X(c) * Y(d)) +
		X(a) *(Y(b) * cz + bz * Y(d) + Y(c) * dz) +
		bz * Y(c) * X(d) + X(b) * cz * Y(d) + Y(b) * X(c) * dz) -
		(az *(Y(b) * X(c) + Y(c) * X(d) + X(b) * Y(d)) +
		X(a) *(bz * Y(c) + cz * Y(d) + Y(b) * dz) +
		Y(a) *(X(b) * cz + X(c) * dz + bz * X(d)) +
		Y(b) * cz * X(d) + bz * X(c) * Y(d) + X(b) * Y(c) * dz);
	return (result > 0.0 ? TRUE : FALSE);
	
	/*  as in the Guibas - Stolfi paper.
	register double adx, ady, bdx, bdy, cdx, cdy, dx, dy, nad, nbd, ncd;
	dx = X(d);
	dy = Y(d);
	adx = X(a) - dx;
	ady = Y(a) - dy;	
	bdx = X(b) - dx;
	bdy = Y(b) - dy;	
	cdx = X(c) - dx;
	cdy = Y(c) - dy;	
	nad = adx*adx + ady*ady;
	nbd = bdx*bdx + bdy*bdy;
	ncd = cdx*cdx + cdy*cdy;
	return ((0.0 <(nad *(bdx * cdy - bdy * cdx) +
	nbd *(cdx * ady - cdy * adx) +
	ncd *(adx * bdy - ady * bdx))) ? TRUE : FALSE); */
}

double distcircle(SITE_PTR a, SITE_PTR b, SITE_PTR c, SITE_PTR d)
{
	double az, bz, cz, dz, result;
	
	az = X(a) * X(a) + Y(a) * Y(a);
	bz = X(b) * X(b) + Y(b) * Y(b);
	cz = X(c) * X(c) + Y(c) * Y(c);
	dz = X(d) * X(d) + Y(d) * Y(d);
	result = (Y(a) *(bz * X(c) + cz * X(d) + X(b) * dz) +
		az *(X(b) * Y(c) + Y(b) * X(d) + X(c) * Y(d)) +
		X(a) *(Y(b) * cz + bz * Y(d) + Y(c) * dz) +
		bz * Y(c) * X(d) + X(b) * cz * Y(d) + Y(b) * X(c) * dz) -
		(az *(Y(b) * X(c) + Y(c) * X(d) + X(b) * Y(d)) +
		X(a) *(bz * Y(c) + cz * Y(d) + Y(b) * dz) +
		Y(a) *(X(b) * cz + X(c) * dz + bz * X(d)) +
		Y(b) * cz * X(d) + bz * X(c) * Y(d) + X(b) * Y(c) * dz);
	return (result);
}

/*#define calc_ccw(a, b, c)((X(a) * Y(b) + Y(a) * X(c) + X(b) * Y(c)) - \
(Y(a) * X(b) + Y(b) * X(c) + X(a) * Y(c))) */
#define calc_ccw(a,b,c) \
((X(a) - X(c)) *(Y(b) - Y(c)) -(X(b) - X(c)) *(Y(a) - Y(c)))

double ccw(SITE_PTR a, SITE_PTR b, SITE_PTR c)
	/* > 0.0 iff A, B, C form a counterclockwise oriented triangle */
{
	double result;
	
	if (a == b || a == c || c == b)
		return (0.0);
	result = calc_ccw(a, b, c);
	if (fabs(result) < 1E-10)
	{
		result += calc_ccw(a, c, b);
		result += calc_ccw(b, a, c);
		result += calc_ccw(b, c, a);
		result += calc_ccw(c, a, b);
		result += calc_ccw(c, b, a);
		result /= 6.0;
	}
	return (result);
}

#define v_calc_ccw(a,b,c) \
((a.x - c.x) *(b.y - c.y) -(b.x - c.x) *(a.y - c.y))

double v_ccw(VEC3 a, VEC3 b, VEC3 c)
{
	double result;
	
	if ((a.x == b.x && a.y == b.y) ||(a.x == c.x && a.y == c.y) ||
		(b.x == c.x && b.y == c.y))
		return (0.0);
	result = v_calc_ccw(a, b, c);
	if (fabs(result) < 1E-10)
	{
		result += v_calc_ccw(a, c, b);
		result += v_calc_ccw(b, a, c);
		result += v_calc_ccw(b, c, a);
		result += v_calc_ccw(c, a, b);
		result += v_calc_ccw(c, b, a);
		result /= 6.0;
	}
	return (result);
}

BOOLEAN intersect_test(VEC3 from, VEC3 to, EDGE_PTR e)
{
	if (v_leftoron(from, e) &&					/* from lies left or on */
		v_rightof(to, e) &&						/* to   lies right */
		v_rightoronline(origv(e), from, to) &&	/* orig lies right or on */
		v_leftoronline(destv(e), from, to))		/* dest lies left or on */
		return (TRUE);
	return (FALSE);
}

/* use intersect_test before that proc to test if the lines really intersect */
/* the test is disabled because intersect_test is much more reliable as test */

BOOLEAN vintersect(VEC3 p1, VEC3 p2, VEC3 p3, VEC3 p4, VEC3 *inter)
{
	double a1, a2, b1, b2, c1, c2;
	double denom, num;
	
	a1 = p2.y - p1.y;					/* calc: a1 x + b1 y + c1 = 0 */
	b1 = p1.x - p2.x;
	c1 = p2.x * p1.y - p1.x * p2.y;
	a2 = p4.y - p3.y;					/* calc: a2 x + b2 y + c2 = 0 */
	b2 = p3.x - p4.x;
	c2 = p4.x * p3.y - p3.x * p4.y;
	denom = a1 * b2 - a2 * b1;
	if (denom == 0.0)					/* collinear :-(*/
		return (FALSE);
	num = b1 * c2 - b2 * c1;
	inter->x = num / denom;
	num = a2 * c1 - a1 * c2;
	inter->y = num / denom;
	return (TRUE);
}

#ifdef WITH_INTERSECT_TEST
BOOLEAN vintersect(VEC3 p1, VEC3 p2, VEC3 p3, VEC3 p4, VEC3 *inter)
{
	double a1, a2, b1, b2, c1, c2;
	double denom, num;
	double r1, r2, r3, r4;
	
	a1 = p2.y - p1.y;
	b1 = p1.x - p2.x;
	c1 = p2.x * p1.y - p1.x * p2.y;
	r3 = a1 * p3.x + b1 * p3.y + c1;
	r4 = a1 * p4.x + b1 * p4.y + c1;
	if (r3 != 0.0 && r4 != 0.0 && ((r3 > 0.0 && r4 > 0.0) ||
		(r3 < 0.0 && r4 < 0.0)))
		return (FALSE);
	a2 = p4.y - p3.y;
	b2 = p3.x - p4.x;
	c2 = p4.x * p3.y - p3.x * p4.y;
	r1 = a2 * p1.x + b2 * p1.y + c2;
	r2 = a2 * p2.x + b2 * p2.y + c2;
	if (r1 != 0.0 && r2 != 0.0 && ((r1 > 0.0 && r2 > 0.0) ||
		(r1 < 0.0 && r2 < 0.0)))
		return (FALSE);
	denom = a1 * b2 - a2 * b1;
	if (denom == 0.0)
		return (FALSE);
	num = b1 * c2 - b2 * c1;
	inter->x = num / denom;
	num = a2 * c1 - a1 * c2;
	inter->y = num / denom;
	return (TRUE);
}
#endif

void circle_center(VEC3 a, VEC3 b, VEC3 c, VEC3 *corner)
	/* computes the center of the circle passing through A, B & C. */
{
	VEC3 ac, bc;
	register double ac2, bc2, area;
	
	ac = V3_sub(a, c);
	ac2 = V3_dotq(ac, ac);
	bc = V3_sub(b, c);
	bc2 = V3_dotq(bc, bc);
	area = V3_cprodq(ac, bc);
	
	ac = V3_times(bc2, ac);
	bc = V3_times(ac2, bc);
	
	*corner = V3_sum(c, V3_times(-0.5 / area, V3_cross(V3_sub(ac, bc))));
}

/****************************************************************/
/*  Vector Routines.											*/
/*  From CMU vision library.									*/
/*  They are used only for the VD, not the DT.				  */
/*  They are slow because of large call - by - value parameters.	*/
/****************************************************************/

/* V3_cprod: forms triple scalar product of[u, v, k] */
/*(returns the magnitude of u cross v in space) or *(u x v) in the plane */

double V3_cprod(VEC3 u, VEC3 v)
{
	return (u.x * v.y - u.y * v.x);
}

/* V3_dot: vector dot product */

double V3_dot(VEC3 u, VEC3 v)
{
	return (u.x * v.x + u.y * v.y);
}

/* V3_times: multiply a vector by a scalar */

VEC3 V3_times(double c, VEC3 v)
{
	VEC3 ans;
	
	ans.x = c * v.x;
	ans.y = c * v.y;
	return (ans);
}

/* V3_sum, V3_sub: Vector addition and subtraction */

VEC3 V3_sum(VEC3 u, VEC3 v)
{
	VEC3 ans;
	
	ans.x = u.x + v.x;
	ans.y = u.y + v.y;
	return (ans);
}

VEC3 V3_sub(VEC3 u, VEC3 v)
{
	VEC3 ans;
	ans.x = u.x - v.x;
	ans.y = u.y - v.y;
	return (ans);
}

/* V3_magn: magnitude of vector */

double V3_magn(VEC3 u)
{
	return (sqrt(u.x * u.x + u.y * u.y));
}

/* returns *v = k x v(cross product). this is a vector perpendicular to v */
/* but beware: it points to the right ! */

VEC3 V3_cross(VEC3 v)
{
	VEC3 ans;
	
	ans.x = v.y;
	ans.y = -v.x;
	return (ans);
}

/* this one points to the left, as god intended it to be :-) */

VEC3 V3_crossl(VEC3 v)
{
	VEC3 ans;
	
	ans.x = -v.y;
	ans.y = v.x;
	return (ans);
}

VEC3 V3_normalize(VEC3 v)
{
	VEC3 ans;
	double len;
	
	len = V3_magn(v);
	ans = V3_times(1.0 / len, v);
	return (ans);
}

VEC3 V3_midpoint(VEC3 v, VEC3 v1)
{
	VEC3 ans;
	
	ans = V3_sum(v, v1);
	ans = V3_times(0.5, ans);
	return (ans);
}

//*****************************************************************************
//zj add

BOOLEAN	 intriangle(SURFACE *surf, double x, double y, SITE_PTR s[3])
{
	EDGE_PTR e, t;
	SITE_PTR site;
	int side1, side2, side3;
	SITE_PTR p1, p2, p3;
	
	_surf = surf;
	for (site = 0; site < surf->n_sites; site++)
	{
		if (Invis(site))
			continue;
		e = Edge(site);
		t = e;
		do 
		{
			if (! Invis(dest(e)) && ! Invis(dest(onext(e))) &&
				Y(site) < Y(dest(onext(e))))
			{
				if (Y(site) < Y(dest(e)) ||(Y(site) == Y(dest(e)) &&
					X(site) < X(dest(e))))
				{					
					p1 = site;
					p2 = dest(e);
					p3 = dest(onext(e));
					
					side1 = whichside(x, y, X(p1), Y(p1), X(p2), Y(p2));
					side2 = whichside(x, y, X(p2), Y(p2), X(p3), Y(p3));
					side3 = whichside(x, y, X(p3), Y(p3), X(p1), Y(p1));
					
					if (side1 == 0 && side2 == 0)
					{
						s[0] = p1;
						s[1] = p2;
						s[2] = p3;
						return (TRUE);
					}
					if (side1 == 0 && side3 == 0)
					{
						s[0] = p1;
						s[1] = p2;
						s[2] = p3;
						return (TRUE);
					}
					if (side2 == 0 && side3 == 0)
					{
						s[0] = p1;
						s[1] = p2;
						s[2] = p3;
						return (TRUE);
					}
					
					if (side1 == 0 && (side2 == side3))
					{
						s[0] = p1;
						s[1] = p2;
						s[2] = p3;
						return (TRUE);
					}
					if (side2 == 0 && (side1 == side3))
					{
						s[0] = p1;
						s[1] = p2;
						s[2] = p3;
						return (TRUE);
					}
					if (side3 == 0 && (side1 == side2))
					{
						s[0] = p1;
						s[1] = p2;
						s[2] = p3;
						return (TRUE);
					}
					
					if ((side1 == side2) && (side2 == side3))
					{
						s[0] = p1;
						s[1] = p2;
						s[2] = p3;
						return (TRUE);					
					}
				}
			}
			e = oprev(e);
		} while (t != e);
	}
	
	return (FALSE);
}

/*****************************************************************************
Determines which side of a line the point(x, y) lies.
The line goes from(x1, y1) to(x2, y2)

  Return codes are 	-1 for points to the left
  0 for points on the line
  +1 for points to the right
*/
int whichside(double x, double y, double x1, double y1, double x2, double y2)
{
    double dist;
	
    dist = (y - y1) *(x2 - x1) -(y2 - y1) *(x - x1);
    if (dist > 0)
		return (-1);
    else if (dist < 0)
		return (1);
    else
		return (0);
}

/*****************************************************************************
* The line AB intersects the line CD at O.
* Line AB equation :A1*X + B1*Y + C1 = 0
* Line CD equation :A2*X + B2*Y + C2 = 0
* 
*****************************************************************************/
BOOLEAN intersect(VEC2 p1[2], VEC2 p2[2], VEC2 *p)
{
	double A1, B1, C1, A2, B2, C2, D;

	A1 = p1[0].y - p1[1].y;
	B1 = p1[1].x - p1[0].x;
	C1 = p1[1].y*p1[0].x - p1[1].x*p1[0].y;
	
	A2 = p2[0].y - p2[1].y;
	B2 = p2[1].x - p2[0].x;
	C2 = p2[1].y*p2[0].x - p2[1].x*p2[0].y;

	D = B2*A1 - B1*A2;

	p->x = (B1*C2 - B2*C1) / D;
	p->y = (A2*C1 - A1*C2) / D;

	if( p->x > min(p1[0].x, p1[1].x ) && p->x < max(p1[0].x, p1[1].x ) && 
		p->y > min(p1[0].y, p1[1].y ) && p->y < max(p1[0].y, p1[1].y ) && 
		p->x > min(p2[0].x, p2[1].x ) && p->x < max(p2[0].x, p2[1].x ) && 
		p->y > min(p2[0].y, p2[1].y ) && p->y < max(p2[0].y, p2[1].y )
		)
		return TRUE;
	else
		return FALSE;
}

//*****************************************************************************
//求三角形内切圆心坐标
//
// 三角形内心坐标公式：△ABC 的三边为 a、b、c ，则内心坐标为 。
//
//  x = (a*Xa+b*Xb+c*Xb)/(a+b+c)
//  y = (a*Ya+b*Yb+c*Yc)/(a+b+c)
//
//

void incenter_of_tri(VEC3 p1, VEC3 p2, VEC3 p3, VEC3 *p)
{
	double a,b,c;//三边长度
	//               
	//                 .p1
	//                /|\
	//            \  / | \
	//            q2*  |  \
	//             /   .p  \
	//            /    |    \
	//         p3.-----*-----.p2
	//                 q1
	//
	a = sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
	b = sqrt((p2.x-p3.x)*(p2.x-p3.x)+(p2.y-p3.y)*(p2.y-p3.y));
	c = sqrt((p1.x-p3.x)*(p1.x-p3.x)+(p1.y-p3.y)*(p1.y-p3.y));

	p->x = (a*p1.x + b*p2.x + c*p3.x) / (a+b+c);
	p->y = (a*p1.y + b*p2.y + c*p3.y) / (a+b+c);

	return ;
}

//*****************************************************************************
//全选主元高斯消去法解方程组AX=B
//

int agaus(double *a, double *b, int n)//
{ 
	int *js, l, k, i, j, is, p, q;
	double d, t;
	js = new int [n];
	l = 1;
	for (k = 0; k <= n - 2; k++)
	{
		d = 0.0;
		for (i = k; i <= n - 1; i++)
			for (j = k; j <= n - 1; j++)
			{
				t = fabs(a[i*n + j]);
				if (t>d)
				{
					d = t;
					js[k] = j;
					is = i;
				}
			}
			if (d + 1.0 == 1.0)
				l = 0;
			else
			{
				if (js[k] != k)
					for (i = 0; i <= n - 1; i++)
					{
						p = i*n + k;
						q = i*n + js[k];
						t = a[p];
						a[p] = a[q];
						a[q] = t;
					}
					if (is != k)
					{
						for (j = k; j <= n - 1; j++)
						{
							p = k*n + j;
							q = is*n + j;
							t = a[p];
							a[p] = a[q];
							a[q] = t;
						}
						t = b[k];
						b[k] = b[is];
						b[is] = t;
					}
			}
			if (l == 0)
			{
				delete []js;				
				return (0);
			}
			d = a[k*n + k];
			for (j = k + 1; j <= n - 1; j++)
			{
				p = k*n + j;
				a[p] = a[p]/d;
			}
			b[k] = b[k]/d;
			for (i = k + 1; i <= n - 1; i++)
			{
				for (j = k + 1; j <= n - 1; j++)
				{
					p = i*n + j;
					a[p] = a[p] - a[i*n + k]*a[k*n + j];
				}
				b[i] = b[i] - a[i*n + k]*b[k];
			}
	}
	d = a[(n - 1)*n + n - 1];
	if (fabs(d) + 1.0 == 1.0)
	{
		delete [] js;
		return (0);
	}
	b[n - 1] = b[n - 1]/d;
	for (i = n - 2; i >= 0; i--)
	{
		t = 0.0;
		for (j = i + 1; j <= n - 1; j++)
			t = t + a[i*n + j]*b[j];
		b[i] = b[i] - t;
	}
	js[n - 1] = n - 1;
	for (k = n - 1; k >= 0; k--)
		if (js[k] != k)
		{
			t = b[k];
			b[k] = b[js[k]];
			b[js[k]] = t;
		}
	delete []js;
	return (1);
}

//*****************************************************************************
// 线性插值

double LineInterpolation(double xp,double yp,VEC3 p1,VEC3 p2,VEC3 p3)
{
	double a,b,c,d;
	
	a = p1.y * (p2.z - p3.z) + p2.y * (p3.z - p1.z) + p3.y * (p1.z - p2.z);
	b = p1.z * (p2.x - p3.x) + p2.z * (p3.x - p1.x) + p3.z * (p1.x - p2.x);
	c = p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y);
	d = - p1.x * (p2.y * p3.z - p3.y * p2.z) 
		- p2.x * (p3.y * p1.z - p1.y * p3.z) 
		- p3.x * (p1.y * p2.z - p2.y * p1.z);
	
	if (ABS(c) > EPSILON)
		return(- (a * xp + b * yp + d) / c);
	else
		return(0.0);
}
