
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

//  This function grabs the contour closest to the cursor and then highlights that contour level wherever it is displayed.
//  The contour level will then be used for hand drawing contours to fill gaps in the data coverage.  This is only used
//  if you have an interpolated surface instead of the normal average surface.

void highlight_contour (MISC *misc, OPTIONS *options, double lat, double lon, nvMap *map)
{
  //  Save the MISP depth for drawing contours (depending on the type of MISP surface)

  NV_F64_COORD2 xy;
  CHRTR2_RECORD chrtr2_record;

  xy.y = lat;
  xy.x = lon;


  //  Try to find the highest layer with a valid value to grab the contour level.

  for (int32_t chrtr2 = 0 ; chrtr2 < misc->chrtr2_share->chrtr2_count ; chrtr2++)
    {
      if (inside_mbr_coord2 (&misc->chrtr2_share->chrtr2_header[chrtr2].mbr, xy))
	{
          chrtr2_read_record_lat_lon (misc->chrtr2_handle[chrtr2], lat, lon, &chrtr2_record);


	  //  Check the validity.

	  if (chrtr2_record.status) break;
	}
    }


  float save_level = (float) (NINT (chrtr2_record.z));


  //  Custom contour levels...

  if (options->cint == 0.0)
    {
      float mindiff = 999999999.0;
      int32_t j = 0;
      for (int32_t i = 0 ; i < options->num_levels ; i++)
	{
	  float diff = fabs (save_level - options->contour_levels[i]);
	  if (diff < mindiff)
	    {
	      diff = mindiff;
	      j = i;
	    }
	}

      options->draw_contour_level = options->contour_levels[j];
    }


  //  Normal contour interval...

  else
    {
      int32_t mult = NINT ((save_level * options->z_factor + options->z_offset) / options->cint);
      options->draw_contour_level = options->cint * (float) mult;
    }


  //  Draw the current contours in highlight color so the "artist" can see where he can connect
  //  his/her drawn contour

  if (options->contour)
    {
      for (int32_t chrtr2 = misc->chrtr2_share->chrtr2_count - 1 ; chrtr2 >= 0 ; chrtr2--)
	{
	  if (misc->chrtr2_share->display_chrtr2[chrtr2])
	    {
	      //  Adjust bounds to nearest grid point

	      adjust_bounds (misc, chrtr2);


	      misc->hatchr_start_x = 0;
	      misc->hatchr_end_x = misc->displayed_area_width[chrtr2];
	      misc->hatchr_start_y = 0;
	      misc->hatchr_end_y = misc->displayed_area_height[chrtr2];


              //  If the width or height is less than 0 (computed by adjust_bounds) then we're outside the CHRTR2 MBR and we don't want to do anything.

              if (misc->displayed_area_width[chrtr2] > 0 && misc->displayed_area_height[chrtr2] > 0)
                {
                  //  Allocate the needed arrays.

                  int32_t size;
                  float *ar = NULL;

                  size = misc->displayed_area_width[chrtr2] * misc->displayed_area_height[chrtr2];
                  ar = (float *) calloc (size, sizeof (float));

                  if (ar == NULL)
                    {
                      perror (gridEdit::tr ("Allocating ar array in highlight_contour").toLatin1 ());
                      exit (-1);
                    }

                  CHRTR2_RECORD *current_record = (CHRTR2_RECORD *) calloc (misc->displayed_area_width[chrtr2], sizeof (CHRTR2_RECORD));

                  if (current_record == NULL)
                    {
                      perror (gridEdit::tr ("Allocating current_record array in highlight_contour").toLatin1 ());
                      exit (-1);
                    }

                  misc->current_row = (float *) calloc (misc->displayed_area_width[chrtr2], sizeof (float));
                  misc->current_flags = (uint8_t *) calloc (misc->displayed_area_width[chrtr2], sizeof (char));


                  //  If the calloc failed, error out.

                  if (misc->current_flags == NULL)
                    {
                      perror (gridEdit::tr (" Unable to allocate current_flags in highlight_contour.\n").toLatin1 ());
                      exit (-1);
                    }


                  //  Read input data row by row and allot data to appropriate
                  //  subplot files until current row of subplots is complete.

                  for (int32_t jj = misc->hatchr_start_y ; jj < misc->hatchr_end_y ; jj++)
                    {
                      //  Read data from disk.

                      chrtr2_read_row (misc->chrtr2_handle[chrtr2], misc->displayed_area_row[chrtr2] + jj, misc->displayed_area_column[chrtr2],
                                       misc->displayed_area_width[chrtr2], current_record);


                      loadArrays (misc->displayed_area_width[chrtr2], current_record, misc->current_row, NULL, 0, misc->current_flags, options->highlight,
                                  options->highlight_depth[0], options->highlight_depth[1], options->display_interpolated);


                      for (int32_t i = 0 ; i < misc->displayed_area_width[chrtr2] ; i++)
                        ar[(jj - misc->hatchr_start_y) * misc->displayed_area_width[chrtr2] + i] = misc->current_row[i] *
                          options->z_factor + options->z_offset;
                    }
                  free (current_record);
                  free (misc->current_row);
                  free (misc->current_flags);

                  scribe (map, options, misc, chrtr2, ar, options->draw_contour_level);

                  free (ar);
                }
            }
	}
    }
}
