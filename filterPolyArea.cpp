
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

/***************************************************************************\

   Module Name:        filterPolyArea

   Programmer(s):      Jan C. Depner

   Date Written:       July 2010

   Purpose:            Filter a rectangular area of the CHRTR2 file.

\***************************************************************************/

void filterPolyArea (OPTIONS *options, MISC *misc, double *mx, double *my, int32_t count)
{
  //  Figure out the width, start row, start column, end row, and end column

  NV_F64_XYMBR mbr;

  mbr.min_x = 999999.0;
  mbr.min_y = 999999.0;
  mbr.max_x = -999999.0;
  mbr.max_y = -999999.0;
  for (int32_t i = 0 ; i < count ; i++)
    {
      if (misc->dateline && mx[i] < 0.0) mx[i] += 360.0;

      mbr.min_x = qMin (mbr.min_x, mx[i]);
      mbr.min_y = qMin (mbr.min_y, my[i]);
      mbr.max_x = qMax (mbr.max_x, mx[i]);
      mbr.max_y = qMax (mbr.max_y, my[i]);
    }


  if (!create_undo (misc))
    {
      QMessageBox::critical (misc->map_widget, gridEdit::tr ("gridEdit"), gridEdit::tr ("Unable to create and/or open undo file!"));
      return;
    }


  for (int32_t chrtr2 = misc->chrtr2_share->chrtr2_count - 1 ; chrtr2 >= 0 ; chrtr2--)
    {
      //  Don't use it to compute min/max if we're not displaying it or it's locked.

      if (!misc->chrtr2_share->display_chrtr2[chrtr2] || misc->chrtr2_share->lock_chrtr2[chrtr2])
        {
          start_undo_block (misc, chrtr2, 0, 0, 0, 0);
        }
      else
        {
          NV_F64_XYMBR bounds;


          //  Adjust input bounds to nearest grid point.

          bounds.min_y = misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat +
            (NINT ((mbr.min_y - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat) / misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees)) * 
            misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees;

          bounds.max_y = misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat + 
            (NINT ((mbr.max_y - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat) / misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees)) * 
            misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees;

          bounds.min_x = misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon +
            (NINT ((mbr.min_x - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon) / misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees)) * 
            misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees;

          bounds.max_x = misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon + 
            (NINT ((mbr.max_x - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon) / misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees)) * 
            misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees;


          bounds.min_y = qMax (bounds.min_y, misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat);
          bounds.max_y = qMin (bounds.max_y, misc->chrtr2_share->chrtr2_header[chrtr2].mbr.nlat);
          bounds.min_x = qMax (bounds.min_x, misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon);
          bounds.max_x = qMin (bounds.max_x, misc->chrtr2_share->chrtr2_header[chrtr2].mbr.elon);


          int32_t start_row = NINT ((bounds.min_y - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat) /
                                     misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees);
          int32_t start_column = NINT ((bounds.min_x - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon) /
                                        misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees);
          int32_t end_row = NINT ((bounds.max_y - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat) /
                                   misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees);
          int32_t end_column = NINT ((bounds.max_x - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon) /
                                      misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees);


          //  Add filter half width.

          start_row -= options->filter_length;
          start_column -= options->filter_length;
          end_row += options->filter_length;
          end_column += options->filter_length;


          //  Make sure that the bounds do not exceed the file bounds.

          if (start_row < 0) start_row = 0;
          if (start_column < 0) start_column = 0;
          if (end_row >= misc->chrtr2_share->chrtr2_header[chrtr2].height) end_row = misc->chrtr2_share->chrtr2_header[chrtr2].height - 1;
          if (end_column >= misc->chrtr2_share->chrtr2_header[chrtr2].width) end_column = misc->chrtr2_share->chrtr2_header[chrtr2].width - 1;


          //  Compute height and width.

          int32_t height = end_row - start_row + 1;
          int32_t width = end_column - start_column + 1;


          //  If width or height is less than 1 we're outside the MBR so we want to go to the next CHRTR2 file.

          if (width <= 1 || height <= 1)
            {
              start_undo_block (misc, chrtr2, 0, 0, 0, 0);
            }
          else
            {
              start_undo_block (misc, chrtr2, start_row, end_row, start_column, end_column);


              //  Allocate the needed arrays.

              CHRTR2_RECORD *chrtr2_record = (CHRTR2_RECORD *) malloc (width * sizeof (CHRTR2_RECORD));
              if (chrtr2_record == NULL)
                {
                  QMessageBox::critical (0, gridEdit::tr ("gridEdit Filter"), gridEdit::tr ("Unable to allocate chrtr2_record in filterPolyArea"));
                  exit (-1);
                }

              float **ar = (float **) malloc ((height + 1) * sizeof (float *));

              if (ar == NULL)
                {
                  QMessageBox::critical (0, gridEdit::tr ("gridEdit Filter"), gridEdit::tr ("Unable to allocate ar in filterPolyArea"));
                  exit (-1);
                }


              for (int32_t i = start_row ; i <= end_row ; i++)
                {
                  chrtr2_read_row (misc->chrtr2_handle[chrtr2], i, start_column, width, chrtr2_record);


                  if (!append_undo_row (misc, chrtr2, chrtr2_record))
                    {
                      QMessageBox::critical (misc->map_widget, gridEdit::tr ("gridEdit"), gridEdit::tr ("Unable to write undo records!"));
                      remove_last_undo (misc);
                      free (ar);
                      return;
                    }


                  int32_t n = i - start_row;


                  ar[n] = (float *) malloc ((width + 1) * sizeof (float *));

                  if (ar[n] == NULL)
                    {
                      QMessageBox::critical (0, gridEdit::tr ("gridEdit Filter"), gridEdit::tr ("Unable to allocate ar[n] in filterPolyArea"));
                      exit (-1);
                    }


                  for (int32_t j = start_column ; j <= end_column ; j++)
                    {
                      int32_t m = j - start_column;

                      if (!chrtr2_record[m].status)
                        {
                          QMessageBox::warning (0, gridEdit::tr ("gridEdit Filter"), gridEdit::tr ("NULL value in area to be filtered<br>"
                                                                                                   "Filtering not allowed."));

                          free (chrtr2_record);

                          for (int32_t k = 0 ; k < n ; k++) free (ar[k]);
                          free (ar);

                          remove_last_undo (misc);

                          return;
                        }

                      ar[n][m] = chrtr2_record[m].z;
                    }
                }

              free (chrtr2_record);


              //  Allocate memory.    */

              int32_t size = qMax (height, width);
              double *in_data = (double *) malloc (size * sizeof (double));

              if (in_data == NULL)
                {
                  QMessageBox::critical (0, gridEdit::tr ("gridEdit Filter"), gridEdit::tr ("Unable to allocate in_data in filterPolyArea"));
                  exit (-1);
                }

              double *out_data = (double *) malloc (size * sizeof (double));

              if (out_data == NULL)
                {
                  QMessageBox::critical (0, gridEdit::tr ("gridEdit Filter"), gridEdit::tr ("Unable to allocate out_data in filterPolyArea"));
                  exit (-1);
                }


              //  Filter rows.
    
              for (int32_t i = 0 ; i < height ; i++)
                {
                  for (int32_t j = 0 ; j < width ; j++) in_data[j] = ar[i][j];
    
                  convolve (in_data, out_data, width, options->filter_length, options->filter_weights);

                  for (int32_t j = 0 ; j < width ; j++) ar[i][j] = out_data[j];
                }


              //  Filter columns.
    
              for (int32_t j = 0 ; j < width ; j++)
                {
                  for (int32_t i = 0 ; i < height ; i++) in_data[i] = ar[i][j];
    
                  convolve (in_data, out_data, height, options->filter_length, options->filter_weights);

                  for (int32_t i = 0 ; i < height ; i++) ar[i][j] = out_data[i];
                }


              //  Subtract filter half width from area bounds.
    
              start_row += options->filter_length;
              start_column += options->filter_length;
              end_row -= options->filter_length;
              end_column -= options->filter_length;
              width = end_column - start_column + 1;


              chrtr2_record = (CHRTR2_RECORD *) malloc (width * sizeof (CHRTR2_RECORD));
              if (chrtr2_record == NULL)
                {
                  perror (gridEdit::tr ("Allocating chrtr2_record in filterPolyArea").toLatin1 ());
                  exit (-1);
                }


              double half_y = misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees * 0.5;
              double half_x = misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees * 0.5;


              //  Write out the data in the area.
    
              for (int32_t i = start_row ; i <= end_row ; i++)
                {
                  chrtr2_read_row (misc->chrtr2_handle[chrtr2], i, start_column, width, chrtr2_record);

                  int32_t n = i - start_row + options->filter_length;

                  double lat = misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat + (double) i *
                    misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees + half_y;


                  for (int32_t j = start_column ; j <= end_column ; j++)
                    {
                      int32_t k = j - start_column;
                      int32_t m = k + options->filter_length;


                      double lon = misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon + (double) j *
                        misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees + half_x;


                      //  Only write points that fall inside the original polygon

                      if (inside_polygon2 (mx, my, count, lon, lat))
                        {
                          //  Check for filter masks.  If the point is in a masked area we don't want to write the filtered version out.

                          uint8_t masked = NVFalse;
                          for (int32_t mask = 0 ; mask < misc->poly_filter_mask_count ; mask++)
                            {
                              if (inside_polygon2 (misc->poly_filter_mask[mask].x, misc->poly_filter_mask[mask].y, misc->poly_filter_mask[mask].count, lon, lat))
                                {
                                  masked = NVTrue;
                                  break;
                                }
                            }

                          if (!masked)
                            {
                              chrtr2_record[k].z = ar[n][m];

                              chrtr2_write_record_row_col (misc->chrtr2_handle[chrtr2], i, j, chrtr2_record[k]);
                            }
                        }
                    }
                }


              //  Free memory.
        
              free (in_data);
              free (out_data);
              free (chrtr2_record);

              for (int32_t i = 0 ; i < height ; i++) free (ar[i]);
              free (ar);
            }
        }
    }
}
