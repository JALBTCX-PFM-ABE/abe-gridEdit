
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


//  Hatchr paints the surface.  I've tried to optimize the painting where possible.

void hatchr (nvMap *map, OPTIONS *options, NV_F64_XYMBR mbr, float min_z, float max_z, float range,
             double x_bin_size, double y_bin_size, int32_t height, int32_t start_x, int32_t end_x, float ss_null, double cell_size_x,
             double cell_size_y, float attr_range, float attr_max, float *current_row, float *next_row, float *current_attr,
             uint8_t *current_flags, uint8_t alpha, int32_t row, int32_t attribute)
{
  double              cur_y[2] = {0.0, 0.0}, cur_x[2] = {0.0, 0.0};
  float               shade_factor;
  static int32_t      i, h_index = 0, s_index = 0, numhues = NUMHUES - 1;
  QColor              temp, prev;
  int32_t             ptr;
  uint8_t             first;


  //  Skip the last row for sunshading.

  if (row >= height) return;


  first = NVTrue;


  cur_y[0] = mbr.min_y + (row - 1) * y_bin_size;
  cur_y[1] = cur_y[0] + y_bin_size;


  //  Loop for the width of the displayed/edited area.

  for (i = start_x ; i < end_x ; i++)
    {
      //  Set the HSV fill color.

      if (attribute && current_attr != NULL)
	{
	  h_index = (int32_t) (numhues - fabs ((current_attr[i] - attr_max) / attr_range * numhues));
	}
      else
	{
          if (options->zero_turnover && min_z < 0.0 && max_z >= 0.0)
            {
              if (current_row[i] <= 0.0)
                {
                  h_index = (int32_t) (numhues - fabs ((current_row[i] - min_z) / (-min_z) * numhues));
                }
              else
                {
                  h_index = (int32_t) (numhues - fabs (current_row[i] / max_z * numhues));
                }
            }
          else
            {
              h_index = (int32_t) (numhues - fabs ((current_row[i] - min_z) / range * numhues));
            }


          //  Check for out of range data.

          if (current_row[i] < min_z)
            {
              h_index = numhues - 1;
            }
          else if (current_row[i] > max_z)
            {
              h_index = 0;
            }
        }


      //  Clear nulls

      if (current_row[i] == ss_null) h_index = -2; 


      //  Highlight if the bin is marked as CHRTR2_USER_?? or CHRTR2_CHECKED or CHRTR2_INTERPOLATED or whatever.

      ptr = 0;
      if (h_index >= 0 && options->highlight && current_flags != NULL && current_flags[i]) ptr = 1;


      //  Shade the cell.

      s_index = NUMSHADES - 1;
      if (h_index >= 0)
        {
          shade_factor = sunshade_null (current_row[i], next_row[i], next_row[i + 1], ss_null, &options->sunopts, cell_size_x, cell_size_y);


          //  The shade_factor will sometimes come back just slightly larger than 1.0.

          if (shade_factor < 0.0) shade_factor = 0.0;

          if (shade_factor > 1.0) shade_factor = 1.0;

          s_index = (int32_t) ((NUMSHADES - 1) * shade_factor + 0.5);
        }


      //  Select the color if not null.

      if (h_index >= 0)
        {
          temp = options->color_array[ptr][h_index][s_index];
	  temp.setAlpha (alpha);
        }
      else
        {
          //  Set to background for null depth.  Really doesn't matter since we plot these transparently anyway.

          temp = options->background_color;
	  temp.setAlpha (0);
        }


      if (prev != temp || i == end_x - 1)
        {

          //  Compute longitude.
    
          if (first) 
            {
              cur_x[0] = mbr.min_x + i * x_bin_size;
              cur_x[1] = cur_x[0];
            }
          else
            {
              cur_x[1] = mbr.min_x + (i + 1) * x_bin_size;
            }


          if (first)
            {
              prev = temp;

              first = NVFalse;
            }
          else
            {
              map->fillRectangle (cur_x[0], cur_y[0], cur_x[1], cur_y[1], prev, NVFalse);

              prev = temp;


              //  Compute x position.

              cur_x[0] = mbr.min_x + i * x_bin_size;
            }
        }
    }
}
