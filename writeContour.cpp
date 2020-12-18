
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


//  Places contour values in the CHRTR2 file(s).

void writeContour (MISC *misc, OPTIONS *options, int32_t count, double *cur_x, double *cur_y)
{
  try
    {
      misc->hd_contour.resize (misc->hd_contour_count + 1);
    }
  catch (std::bad_alloc&)
    {
      perror ("Allocating misc->hd_contour in writeContour.cpp");
      exit (-1);
    }
  misc->hd_contour[misc->hd_contour_count].count = 0;


  //  We need to figure out what the MBR of the contour is.

  NV_F64_XYMBR mbr;
  mbr.min_y = 999.0;
  mbr.min_x = 999.0;
  mbr.max_y = -999.0;
  mbr.max_x = -999.0;

  for (int32_t i = 0 ; i < count ; i++)
    {
      mbr.min_y = qMin (mbr.min_y, cur_y[i]);
      mbr.min_x = qMin (mbr.min_x, cur_x[i]);
      mbr.max_y = qMax (mbr.max_y, cur_y[i]);
      mbr.max_x = qMax (mbr.max_x, cur_x[i]);
    }


  //  Loop through all of the displayed CHRTR2 layers.

  for (int32_t chrtr2 = 0 ; chrtr2 < misc->chrtr2_share->chrtr2_count ; chrtr2++)
    {
      if (misc->contour_in_chrtr2[chrtr2] && misc->chrtr2_share->display_chrtr2[chrtr2] && !misc->chrtr2_share->lock_chrtr2[chrtr2])
	{
          //  Adjust bounds to nearest grid point for the CHRTR2 file.

          double min_y = misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat +
            (NINT ((mbr.min_y - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat) / misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees)) *
            misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees;
          double max_y = misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat + 
            (NINT ((mbr.max_y - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat) / misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees)) * 
            misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees;
          double min_x = misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon + 
            (NINT ((mbr.min_x - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon) / misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees)) *
            misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees;
          double max_x = misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon + 
            (NINT ((mbr.max_x - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon) / misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees)) *
            misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees;


          min_y = qMax (min_y, misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat);
          max_y = qMin (max_y, misc->chrtr2_share->chrtr2_header[chrtr2].mbr.nlat);
          min_x = qMax (min_x, misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon);
          max_x = qMin (max_x, misc->chrtr2_share->chrtr2_header[chrtr2].mbr.elon);


          //  We add one to the bounds to give us a bit of cushion.

          int32_t width = (NINT ((max_x - min_x) / misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees)) + 1;
          int32_t height = (NINT ((max_y - min_y) / misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees)) + 1;
          int32_t row = NINT ((min_y - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat) /
                               misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees) - 1;
          int32_t column = NINT ((min_x - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon) /
                                  misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees) - 1;

          if (column + width > misc->chrtr2_share->chrtr2_header[chrtr2].width) width = misc->chrtr2_share->chrtr2_header[chrtr2].width - column;
          
          if (row + height > misc->chrtr2_share->chrtr2_header[chrtr2].height) height = misc->chrtr2_share->chrtr2_header[chrtr2].height - row;


          //  Compute the minimum cell size in meters.  This will be our step along each contour segment so that we make sure
          //  that we place a point in all cells along the contour.

          double min_cell_size = qMin (misc->ss_cell_size_x[chrtr2], misc->ss_cell_size_y[chrtr2]);


          //  Loop through each segment of the contour starting with [0] and [1] (Note that the loop starts at 1).

	  for (int32_t i = 1 ; i < count ; i++)
	    {
              CHRTR2_RECORD chrtr2_record;


              //  Save the segment end points so that we can clip them to the overall MBR without destroying the original segment ends.

              double tmp_x[2] = {cur_x[i - 1], cur_x[i]}, tmp_y[2] = {cur_y[i - 1], cur_y[i]};


              //  Clip the segment to the overall MBR

              if (clip_lat_lon (&tmp_x[0], &tmp_y[0], &tmp_x[1], &tmp_y[1], misc->chrtr2_share->chrtr2_header[chrtr2].mbr))
                {
                  //  Compute the distance and azimuth from one end of the segment to the other.

                  double dist, az;
                  invgp (NV_A0, NV_B0, tmp_y[0], tmp_x[0], tmp_y[1], tmp_x[1], &dist, &az);


                  //  Figure out how many steps we'll have to use as we move along the segment so that we'll make sure that we
                  //  hit every intervening cell.

                  int32_t steps = (int32_t) (dist / min_cell_size);

                  NV_I32_COORD2 coord, prev_coord = {-1, -1};


                  //  Move along the segment in min_cell_size increments.

                  for (int32_t j = 0 ; j < steps ; j++)
                    {
                      //  Try to read the cell.

                      if (!chrtr2_get_coord (misc->chrtr2_handle[chrtr2], tmp_y[0], tmp_x[0], &coord))
                        {
                          //  If we haven't changed cells, move on.

                          if (prev_coord.x != coord.x || prev_coord.y != coord.y)
                            {
                              //  Read the record.

                              chrtr2_read_record (misc->chrtr2_handle[chrtr2], coord, &chrtr2_record);


                              //  We don't want to save points that fall in cells with previously existing real or digitized data.

                              if (!(chrtr2_record.status & (CHRTR2_REAL | CHRTR2_DIGITIZED_CONTOUR)))
                                {
                                  try
                                    {
                                      misc->hd_contour[misc->hd_contour_count].xyz.resize (misc->hd_contour[misc->hd_contour_count].count + 1);
                                    }
                                  catch (std::bad_alloc&)
                                    {
                                      perror ("Allocating misc->hd_contour[misc->hd_contour_count].xyz in writeContour.cpp");
                                      exit (-1);
                                    }

                                  misc->hd_contour[misc->hd_contour_count].xyz[misc->hd_contour[misc->hd_contour_count].count].x = tmp_x[0];
                                  misc->hd_contour[misc->hd_contour_count].xyz[misc->hd_contour[misc->hd_contour_count].count].y = tmp_y[0];
                                  misc->hd_contour[misc->hd_contour_count].xyz[misc->hd_contour[misc->hd_contour_count].count].z = 
                                    options->draw_contour_level / options->z_factor - options->z_offset;
                                  misc->hd_contour[misc->hd_contour_count].count++;
                                }
                            }


                          //  Add min_cell_size to the X and Y positions in the "az" direction (i.e. increment the position along the segment).

                          double lat, lon;
                          newgp (tmp_y[0], tmp_x[0], az, min_cell_size, &lat, &lon);


                          if (misc->dateline && lon < 0.0) lon += 360.0;


                          tmp_y[0] = lat;
                          tmp_x[0] = lon;


                          //  Save the previous coordinates.

                          prev_coord = coord;
                        }
                    }


                  //  Make sure we get the last cell of each segment (tmp_y[1],tmp_x[1]).

                  if (!chrtr2_get_coord (misc->chrtr2_handle[chrtr2], tmp_y[1], tmp_x[1], &coord))
                    {
                      //  If we haven't changed cells, move on.

                      if (prev_coord.x != coord.x || prev_coord.y != coord.y)
                        {
                          //  Read the record.

                          chrtr2_read_record (misc->chrtr2_handle[chrtr2], coord, &chrtr2_record);


                          //  We don't want to save points that fall in cells with previously existing real or digitized data.

                          if (!(chrtr2_record.status & (CHRTR2_REAL | CHRTR2_DIGITIZED_CONTOUR)))
                            {
                              try
                                {
                                  misc->hd_contour[misc->hd_contour_count].xyz.resize (misc->hd_contour[misc->hd_contour_count].count + 1);
                                }
                              catch (std::bad_alloc&)
                                {
                                  perror ("Allocating misc->hd_contour[misc->hd_contour_count].xyz in writeContour.cpp");
                                  exit (-1);
                                }

                              misc->hd_contour[misc->hd_contour_count].xyz[misc->hd_contour[misc->hd_contour_count].count].x = tmp_x[1];
                              misc->hd_contour[misc->hd_contour_count].xyz[misc->hd_contour[misc->hd_contour_count].count].y = tmp_y[1];
                              misc->hd_contour[misc->hd_contour_count].xyz[misc->hd_contour[misc->hd_contour_count].count].z =
                                options->draw_contour_level / options->z_factor - options->z_offset;
                              misc->hd_contour[misc->hd_contour_count].count++;
                            }
                        }
                    }
                }
            }
	}
    }


  //  If we added a contour, increment the counter.

  if (misc->hd_contour[misc->hd_contour_count].count) misc->hd_contour_count++;
}
