/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2018 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public
* License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

/*!
 * \file tilemap.c
 * Tilemap manager
 */

#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#endif

#include <string.h>
#include <stdio.h>
#include "Tilengine.h"
#include "Tilemap.h"

typedef struct
{
	int x,y,w,h;
}
Rect;

/*!
 * \brief
 * Creates a new tilemap
 * 
 * \param rows
 * Number of rows (vertical dimension)
 * 
 * \param cols
 * Number of cols (horizontal dimension)
 * 
 * \param tiles
 * Array of tiles with data (see struct Tile)
 * 
 * \param bgcolor
 * Background color value (RGB32 packed)
 *
 * \param tileset
 * Optional reference to associated tileset, can be NULL
 *
 * \returns
 * Reference to the created tilemap, or NULL if error
 * 
 * \remarks
 * Make sure that the tiles[] array is has at least rows*cols items or application may crash
 * 
 * \see
 * TLN_DeleteTilemap(), struct Tile
 */
TLN_Tilemap TLN_CreateTilemap (int rows, int cols, TLN_Tile tiles, uint32_t bgcolor, TLN_Tileset tileset)
{
	TLN_Tilemap tilemap = NULL;
	int size = sizeof(struct Tilemap) + (rows * cols * sizeof(struct Tile));

	tilemap = CreateBaseObject (OT_TILEMAP, size);
	if (!tilemap)
	{
		TLN_SetLastError (TLN_ERR_OUT_OF_MEMORY);
		return NULL;
	}

	tilemap->rows = rows;
	tilemap->cols = cols;
	tilemap->bgcolor = bgcolor;
	tilemap->tileset = tileset;

	if (tiles)
		memcpy (tilemap->tiles, tiles, tilemap->size - sizeof(struct Tilemap));

	TLN_SetLastError (TLN_ERR_OK);
	return tilemap;
}

/*!
 * \brief
 * Creates a duplicate of the specified tilemap
 * 
 * \param src
 * Reference to the tilemap to clone
 * 
 * \returns
 * A reference to the newly cloned tilemap, or NULL if error
 *
 * \see
 * TLN_LoadTilemap()
 */
TLN_Tilemap TLN_CloneTilemap (TLN_Tilemap src)
{
	TLN_Tilemap tilemap;

	if (!CheckBaseObject (src, OT_TILEMAP))
	{
		TLN_SetLastError (TLN_ERR_REF_TILEMAP);
		return NULL;
	}

	tilemap = CloneBaseObject (src);
	if (tilemap)
	{
		TLN_SetLastError (TLN_ERR_OK);
		return tilemap;
	}
	else
	{
		TLN_SetLastError (TLN_ERR_OUT_OF_MEMORY);
		return NULL;
	}
}

/*!
 * \brief
 * Returns the number of vertical tiles in the tilemap
 * 
 * \param tilemap
 * Reference of the tilemap to get info
 * 
 * \see
 * TLN_GetTilemapCols()
 */
int TLN_GetTilemapRows (TLN_Tilemap tilemap)
{
	if (CheckBaseObject (tilemap, OT_TILEMAP))
	{
		TLN_SetLastError (TLN_ERR_OK);
		return tilemap->rows;
	}
	else
	{
		TLN_SetLastError (TLN_ERR_REF_TILEMAP);
		return 0;
	}
}

/*!
 * \brief
 * Returns the number of horizontal tiles in the tilemap
 * 
 * \param tilemap
 * Reference of the tilemap to get info
 * 
 * \see
 * TLN_GetTilemapCols()
 */
int TLN_GetTilemapCols (TLN_Tilemap tilemap)
{
	if (CheckBaseObject (tilemap, OT_TILEMAP))
	{
		TLN_SetLastError (TLN_ERR_OK);
		return tilemap->cols;
	}
	else
	{
		TLN_SetLastError (TLN_ERR_REF_TILEMAP);
		return 0;
	}
}

/*!
 * \brief
 * Returns the optional associated tileset to the specified tilemap
 * 
 * \param tilemap
 * Reference of the tilemap to get info
 * 
 * \see
 * TLN_CreateTilemap(), TLN_LoadTilemap()
 */
TLN_Tileset TLN_GetTilemapTileset (TLN_Tilemap tilemap)
{
	if (CheckBaseObject (tilemap, OT_TILEMAP))
	{
		TLN_SetLastError (TLN_ERR_OK);
		return tilemap->tileset;
	}
	else
	{
		TLN_SetLastError (TLN_ERR_REF_TILEMAP);
		return NULL;
	}
}

static TLN_Tile GetTilemapPtr (TLN_Tilemap tilemap, int row, int col)
{
	if (row<tilemap->rows && col<tilemap->cols)
		return &tilemap->tiles[row*tilemap->cols + col];
	else
		return NULL;
}

/*!
 * \brief
 * Gets data of a single tile inside a tilemap
 * 
 * \param tilemap
 * Reference of the tilemap to get the tile
 * 
 * \param row
 * Vertical location of the tile (0 <= row < rows)
 * 
 * \param col
 * Horizontal location of the tile (0 <= col < cols)
 *
 * \param tile
 * Reference to an application-allocated struct Tile that will get the data
 */
bool TLN_GetTilemapTile (TLN_Tilemap tilemap, int row, int col, TLN_Tile tile)
{
	if (CheckBaseObject (tilemap, OT_TILEMAP) && tile)
	{
		TLN_Tile srctile = GetTilemapPtr (tilemap, row, col);
		if (srctile)
		{
			tile->flags = srctile->flags;
			tile->index = srctile->index;
			TLN_SetLastError (TLN_ERR_OK);
			return true;
		}
		else
		{
			TLN_SetLastError (TLN_ERR_WRONG_SIZE);
			return false;
		}
	}
	else
	{
		TLN_SetLastError (TLN_ERR_REF_TILEMAP);
		return false;
	}
}

/*!
 * \brief
 * Sets a tile of a tilemap
 * 
 * \param tilemap
 * Reference to the tilemap
 * 
 * \param row
 * Row (vertical position) of the tile [0 - num_rows - 1]
 * 
 * \param col
 * Column (horizontal position) of the tile [0 - num_cols - 1]
 * 
 * \param tile
 * Reference to the tile to set
 * 
 * \returns
 * true (success) or false (error)
 */
bool TLN_SetTilemapTile (TLN_Tilemap tilemap, int row, int col, TLN_Tile tile)
{
	if (CheckBaseObject (tilemap, OT_TILEMAP) && tile)
	{
		TLN_Tile dsttile = GetTilemapPtr (tilemap, row, col);
		if (dsttile)
		{
			dsttile->flags = tile->flags;
			dsttile->index = tile->index;
			if (tilemap->maxindex < tile->index)
				tilemap->maxindex = tile->index;

			TLN_SetLastError (TLN_ERR_OK);
			return true;
		}
		else
		{
			TLN_SetLastError (TLN_ERR_WRONG_SIZE);
			return false;
		}
	}
	else
	{
		TLN_SetLastError (TLN_ERR_REF_TILEMAP);
		return false;
	}
}

/*!
 * \brief
 * Deletes the specified tilemap and frees memory
 * 
 * \param tilemap
 * Reference to the tilemap to delete
 * 
 * \remarks
 * Don't delete a tilemap currently attached to a layer!
 * 
 * \see
 * TLN_LoadTilemap(), TLN_CloneTilemap()
 */
bool TLN_DeleteTilemap (TLN_Tilemap tilemap)
{
	if (CheckBaseObject (tilemap, OT_TILEMAP))
	{
		if (ObjectOwner (tilemap))
			DeleteBaseObject (tilemap->tileset);
		DeleteBaseObject (tilemap);
		TLN_SetLastError (TLN_ERR_OK);
		return true;
	}
	else
	{
		TLN_SetLastError (TLN_ERR_REF_TILEMAP);
		return false;
	}
}

static void ClipRect (Rect* src, Rect* dst)
{
	if (src->x + src->w >= dst->w)
		src->w = dst->w - src->x;
	if (src->y + src->h >= dst->h)
		src->h = dst->h - src->y;
}

/*!
 * \brief
 * Copies blocks of tiles between two tilemaps
 * 
 * \param src
 * Reference to the source tilemap
 * 
 * \param srcrow
 * Starting row (vertical position) inside the source tilemap
 * 
 * \param srccol
 * Starting column (horizontal position) inside the source tilemap
 * 
 * \param rows
 * Number of rows to copy
 * 
 * \param cols
 * Number of columns to copy
 * 
 * \param dst
 * Reference to the target tilemap
 * 
 * \param dstrow
 * Starting row (vertical position) inside the target tilemap
 * 
 * \param dstcol
 * Starting column (horizontal position) inside the target tilemap
 * 
 * \remarks
 * Use this function to implement tile streaming
 */
bool TLN_CopyTiles (TLN_Tilemap src, int srcrow, int srccol, int rows, int cols, TLN_Tilemap dst, int dstrow, int dstcol)
{
	int y, size;

	if (!CheckBaseObject (src, OT_TILEMAP))
	{
		TLN_SetLastError (TLN_ERR_REF_TILEMAP);
		return false;
	}
	if (!CheckBaseObject (dst, OT_TILEMAP))
	{
		TLN_SetLastError (TLN_ERR_REF_TILEMAP);
		return false;
	}

	/* setup rects */
	{
		Rect tgtrect = {srccol,srcrow, cols,rows};	/* area a copiar */
		Rect srcrect = {0,0, src->rows,src->cols};	/* tilemap de origen */
		Rect dstrect = {0,0, dst->rows,dst->cols};	/* tilemap de destino */

		/* clipping */
		ClipRect (&tgtrect, &srcrect);
		ClipRect (&tgtrect, &dstrect);

		size = tgtrect.w * sizeof(Tile);
		for (y=0; y<tgtrect.h; y++)
		{
			Tile* srctile = GetTilemapPtr (src, y + srcrow, srccol);
			Tile* dsttile = GetTilemapPtr (dst, y + dstrow, dstcol);
			if (srctile && dsttile)
				memcpy (dsttile, srctile, size);
			else
			{
				TLN_SetLastError (TLN_ERR_WRONG_SIZE);
				return false;
			}
		}
	}

	TLN_SetLastError (TLN_ERR_OK);
	return true;
}
