
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
    document the API.  Dashes in these comment blocks are used to create bullet lists.  The
    lack of blank lines after a block of dash preceeded comments means that the next block
    of dash preceeded comments is a new, indented bullet list.  I've tried to keep the
    Doxygen formatting to a minimum but there are some other items (like <br> and <pre>)
    that need to be left alone.  If you see a comment that starts with / * ! or / / ! and
    there is something that looks a bit weird it is probably due to some arcane Doxygen
    syntax.  Be very careful modifying blocks of Doxygen comments.

*****************************************  IMPORTANT NOTE  **********************************/



#include "gridEdit.hpp"


//!  This function draws the overlay flags (at present, just hand-drawn contours).

void overlayFlag (nvMap *map, OPTIONS *options, MISC *misc)
{
  int32_t             i, j, x, y, z;
  double              wlon, elon, slat, nlat, dep;
  NV_I32_COORD2       coord;
  CHRTR2_RECORD       chrtr2_record;


  if (!misc->clear)
    {
      slat = misc->chrtr2_share->edit_area.min_y;
      nlat = misc->chrtr2_share->edit_area.max_y;
      wlon = misc->chrtr2_share->edit_area.min_x;
      elon = misc->chrtr2_share->edit_area.max_x;
    }
  else
    {
      slat = misc->total_displayed_area.min_y;
      nlat = misc->total_displayed_area.max_y;
      wlon = misc->total_displayed_area.min_x;
      elon = misc->total_displayed_area.max_x;
    }


  //  Flag cells with hand-drawn contour data.

  if (options->flag_contour)
    {
      for (int32_t chrtr2 = misc->chrtr2_share->chrtr2_count - 1 ; chrtr2 >= 0 ; chrtr2--)
        {
          for (i = 0 ; i < misc->displayed_area_height[chrtr2] ; i++)
            {
              coord.y = misc->displayed_area_row[chrtr2] + i;
              double lat = misc->displayed_area[chrtr2].min_y + ((double) i + 0.5) *
                misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees;

              for (j = 0 ; j < misc->displayed_area_width[chrtr2] ; j++)
                {
                  coord.x = misc->displayed_area_column[chrtr2] + j;
                  double lon = misc->displayed_area[chrtr2].min_x + ((double) j + 0.5) *
                    misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees;

                  if (chrtr2_read_record (misc->chrtr2_handle[chrtr2], coord, &chrtr2_record))
                    {
                      chrtr2_perror ();
                      exit (-1);
                    }


                  if (lat >= slat && lat <= nlat && lon >= wlon && lon <= elon)
                    {
                      if (chrtr2_record.status & CHRTR2_DIGITIZED_CONTOUR)
                        {
                          map->map_to_screen (1, &lon, &lat, &dep, &x, &y, &z);
                          map->drawLine (x - 3, y - 3, x + 3, y + 3, options->contour_highlight_color, LINE_WIDTH, NVFalse, Qt::SolidLine);
                          map->drawLine (x - 3, y + 3, x + 3, y - 3, options->contour_highlight_color, LINE_WIDTH, NVFalse, Qt::SolidLine);
                          map->drawLine (x - 6, y, x + 6, y, options->contour_highlight_color, LINE_WIDTH, NVFalse, Qt::SolidLine);
                          map->drawLine (x, y - 6, x, y + 6, options->contour_highlight_color, LINE_WIDTH, NVFalse, Qt::SolidLine);
                        }
                    }
                }

              if (qApp->hasPendingEvents ())
                {
                  qApp->processEvents();
                  if (misc->drawing_canceled) break;
                }
            }
        }
    }

  map->flush ();
}
