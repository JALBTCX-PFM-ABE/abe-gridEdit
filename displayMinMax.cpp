
/*********************************************************************************************

    This is public domain software that was developed by or for the U.S. Naval Oceanographic
    Office and/or the U.S. Army Corps of Engineers.

    This is a work of the U.S. Government. In accordance with 17 USC 105, copyright protection
    is not available for any work of the U.S. Government.

    Neither the United States Government, nor any employees of the United States Government,
    nor the author, makes any warranty, express or implied, without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, or assumes any liability or
    responsibility for the accuracy, completeness, or usefulness of any information,
    apparatus, product, or process disclosed, or represents that its use would not infringe
    privately-owned rights. Reference herein to any specific commercial products, process,
    or service by trade name, trademark, manufacturer, or otherwise, does not necessarily
    constitute or imply its endorsement, recommendation, or favoring by the United States
    Government. The views and opinions of authors expressed herein do not necessarily state
    or reflect those of the United States Government, and shall not be used for advertising
    or product endorsement purposes.
*********************************************************************************************/


/****************************************  IMPORTANT NOTE  **********************************

    Comments in this file that start with / * ! or / / ! are being used by Doxygen to
    document the software.  Dashes in these comment blocks are used to create bullet lists.
    The lack of blank lines after a block of dash preceeded comments means that the next
    block of dash preceeded comments is a new, indented bullet list.  I've tried to keep the
    Doxygen formatting to a minimum but there are some other items (like <br> and <pre>)
    that need to be left alone.  If you see a comment that starts with / * ! or / / ! and
    there is something that looks a bit weird it is probably due to some arcane Doxygen
    syntax.  Be very careful modifying blocks of Doxygen comments.

*****************************************  IMPORTANT NOTE  **********************************/



#include "gridEdit.hpp"



//  This function places markers on the minimum and maximum values in the displayed area.
//  The actual values used are computed in compute_layer_min_max.cpp which is called from paint_surface.cpp.

void displayMinMax (nvMap *map, OPTIONS *options, MISC *misc)
{
  double              x[2], y[2];


  int32_t min_chrtr2 = misc->displayed_area_min_chrtr2;
  int32_t max_chrtr2 = misc->displayed_area_max_chrtr2;

  NV_I32_COORD2 min_coord = misc->displayed_area_min_coord;
  NV_I32_COORD2 max_coord = misc->displayed_area_max_coord;


  //  Minimum value.

  y[0] = misc->displayed_area[min_chrtr2].min_y + ((double) (min_coord.y - misc->displayed_area_row[min_chrtr2]) + 0.5) * 
    misc->chrtr2_share->chrtr2_header[min_chrtr2].lat_grid_size_degrees;
  x[0] = misc->displayed_area[min_chrtr2].min_x + ((double) (min_coord.x - misc->displayed_area_column[min_chrtr2])+ 0.5) * 
    misc->chrtr2_share->chrtr2_header[min_chrtr2].lon_grid_size_degrees;


  //  Maximum value.

  y[1] = misc->displayed_area[max_chrtr2].min_y + ((double) (max_coord.y - misc->displayed_area_row[max_chrtr2]) + 0.5) * 
    misc->chrtr2_share->chrtr2_header[max_chrtr2].lat_grid_size_degrees;
  x[1] = misc->displayed_area[max_chrtr2].min_x + ((double) (max_coord.x - misc->displayed_area_column[max_chrtr2]) + 0.5) * 
    misc->chrtr2_share->chrtr2_header[max_chrtr2].lon_grid_size_degrees;


  map->fillCircle (x[0], y[0], 15, 0.0, 360.0, options->contour_color, NVTrue);
  map->drawCircle (x[0], y[0], 16, 0.0, 360.0, 1, Qt::black, Qt::SolidLine, NVTrue);

  map->fillRectangle (x[1], y[1], 15, 15, options->contour_color, NVTrue);
  map->drawRectangle (x[1], y[1], 15, 15, Qt::black, 1, Qt::SolidLine, NVTrue);
}
