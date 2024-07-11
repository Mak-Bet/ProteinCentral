/* -*- mode: C -*-  */
/*
   IGraph library.
   Copyright (C) 2010-2012  Gabor Csardi <csardi.gabor@gmail.com>
   334 Harvard st, Cambridge MA, 02139 USA

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301 USA

*/

#include <igraph.h>

#include "test_utilities.h"

int main(void) {
    igraph_t g;
    igraph_bool_t simple;
    FILE *ifile;

    /* turn on attribute handling */
    igraph_set_attribute_table(&igraph_cattribute_table);

    ifile = fopen("pajek5.net", "r");
    IGRAPH_ASSERT(ifile != NULL);

    igraph_read_graph_pajek(&g, ifile);
    fclose(ifile);

    IGRAPH_ASSERT(igraph_vcount(&g) == 10);
    IGRAPH_ASSERT(igraph_ecount(&g) == 9);

    igraph_destroy(&g);

    ifile = fopen("pajek6.net", "r");
    IGRAPH_ASSERT(ifile != NULL);

    igraph_read_graph_pajek(&g, ifile);
    fclose(ifile);

    IGRAPH_ASSERT(igraph_vcount(&g) == 10);
    IGRAPH_ASSERT(igraph_ecount(&g) == 9);

    igraph_destroy(&g);

    /* This file starts with a UTF-8 BOM, which Pajek itself supports.
     * It also contains some custom attributes. */
    ifile = fopen("utf8_with_bom.net", "r");
    IGRAPH_ASSERT(ifile != NULL);

    igraph_read_graph_pajek(&g, ifile);
    fclose(ifile);

    IGRAPH_ASSERT(igraph_vcount(&g) == 10);
    IGRAPH_ASSERT(igraph_ecount(&g) == 6);

    igraph_destroy(&g);

    /* File in Arcslist format */
    ifile = fopen("pajek_arcslist.net", "r");
    IGRAPH_ASSERT(ifile != NULL);

    igraph_read_graph_pajek(&g, ifile);
    fclose(ifile);

    IGRAPH_ASSERT(igraph_vcount(&g) == 18);
    IGRAPH_ASSERT(igraph_ecount(&g) == 55);
    IGRAPH_ASSERT(igraph_is_directed(&g));

    igraph_is_simple(&g, &simple);
    IGRAPH_ASSERT(simple);

    igraph_destroy(&g);

    /* File in Edgeslist format */
    ifile = fopen("pajek_edgeslist.net", "r");
    IGRAPH_ASSERT(ifile != NULL);

    igraph_read_graph_pajek(&g, ifile);
    fclose(ifile);

    IGRAPH_ASSERT(igraph_vcount(&g) == 3);
    IGRAPH_ASSERT(igraph_ecount(&g) == 3);
    IGRAPH_ASSERT(! igraph_is_directed(&g));

    igraph_is_simple(&g, &simple);
    IGRAPH_ASSERT(simple);

    igraph_destroy(&g);

    VERIFY_FINALLY_STACK();

    return 0;
}
