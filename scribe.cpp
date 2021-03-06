
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


#define  CONTOUR_LABEL_HEIGHT               12  //  in pixels
#define  HALF_CONTOUR_LABEL_HEIGHT          CONTOUR_LABEL_HEIGHT / 2


//  The ever popular scribe contour drawing package.  Most of the functions called here are in the nvutility library
//  because they're pretty much stock functions.  Only the scribe function itself ever really gets modified (based
//  on what you're trying to draw contours for and what you want to draw them to).

void scribe (nvMap *map, OPTIONS *options, MISC *misc, int32_t chrtr2, float *ar, float highlight)
{
  int32_t                 index_contour, num_points, num_interp, bytes, width, ncc, nrr, digits, cont_index_1, cont_index_2, win_index_1 = 0,
                          win_index_2, label_len_chars, label_x = 0, label_y = 0;
  double                  dcontour_x[CONTOUR_POINTS], dcontour_y[CONTOUR_POINTS], dcontour_z[CONTOUR_POINTS], dx, dy, cell_diag_length, xorig, yorig,
                          segment_length, x[2], y[2], z[2], *xyz_x, *xyz_y, *xyz_z, dist;
  float                   level, half_gridx, half_gridy, *contour_x, *contour_y, save_draw_contour_level = 0.0;
  QString                 label = "";
  uint8_t                 capture = NVFalse;


  void contourMinMax (float, float);
  void contourEmphasis (int32_t);
  void contourMaxDensity (int32_t);
  void contourMaxPoints (int32_t);
  void contourLevels (int32_t numLevels, float *);
  int32_t initContour (float, int32_t, int32_t, float *);
  int32_t getContour (float *, int32_t *, int32_t *, float *, float *);
  void smooth_contour (int32_t, int32_t *, double *, double *);
  void writeContour (MISC *misc, OPTIONS *options, int32_t count, double *cur_x, double *cur_y);


  //  If we're capturing the contours we need to save the original draw contour level.  We also 
  //  need to lie to the writeContour function and tell it that the contour is in ALL CHRTR2's.
  //  writeContour will sort it out anyway.

  if (misc->function == GRAB_CONTOUR && misc->poly_count)
    {
      save_draw_contour_level = options->draw_contour_level;
      for (int32_t gchrtr2 = 0 ; gchrtr2 < misc->chrtr2_share->chrtr2_count ; gchrtr2++) misc->contour_in_chrtr2[gchrtr2] = NVTrue;
      capture = NVTrue;
    }


  ncc = misc->displayed_area_width[chrtr2];
  nrr = misc->displayed_area_height[chrtr2];
  xorig = misc->displayed_area[chrtr2].min_x;
  yorig = misc->displayed_area[chrtr2].min_y + (misc->hatchr_start_y * misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees);


  //  Check the smoothing factor range and get the number of interpolation 
  //  points per unsmoothed contour segment

  if (options->smoothing_factor < 0) options->smoothing_factor = 0;
  if (options->smoothing_factor > 10) options->smoothing_factor = 10;


  dcontour_x[0] = xorig + (misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees * ncc) * 0.5;
  dcontour_y[0] = yorig + (misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees * nrr) * 0.5;
  dcontour_x[1] = dcontour_x[0] + misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees;
  dcontour_y[1] = dcontour_y[0] + misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees;

  map->map_to_screen (2, dcontour_x, dcontour_y, dcontour_z, x, y, z);

  dx = x[1] - x[0];
  dy = y[1] - y[0];

  cell_diag_length = sqrt (pow (dx, 2) + pow (dy, 2));

  segment_length = DEFAULT_SEGMENT_LENGTH / options->smoothing_factor;

  if (cell_diag_length > segment_length)
    {
      num_interp = (int32_t) ((cell_diag_length / segment_length) + 0.5);
    }
  else
    {
      num_interp = 1;
    }

       
  /* allocate memory for contour arrays */

  if (options->smoothing_factor > 0)
    {
      bytes = (((num_interp * (CONTOUR_POINTS - 1)) + 1) * sizeof (double));
    }
  else
    {
      bytes = (CONTOUR_POINTS * sizeof (double));
    }

  contour_x = (float *) malloc (bytes / 2);
  contour_y = (float *) malloc (bytes / 2);
  xyz_x = (double *) malloc (bytes);
  xyz_y = (double *) malloc (bytes);
  if ((xyz_z = (double *) malloc (bytes)) == NULL)
    {
      perror (__FILE__);
      exit (-1);
    }


  //  Set the min and max contours, the index contour interval, the maximum contour density, and the number of points to be returned
  //  by the package.

  contourMinMax (misc->chrtr2_share->chrtr2_header[chrtr2].min_z * options->z_factor + options->z_offset, 
                 misc->chrtr2_share->chrtr2_header[chrtr2].max_z * options->z_factor + options->z_offset);
  contourMaxDensity (misc->maxd);
  contourMaxPoints (CONTOUR_POINTS);


  //  If the contour interval is set to 0.0 use the user defined levels.

  if (options->cint == 0.0) 
    {
      contourLevels (options->num_levels, options->contour_levels);
      contourEmphasis (9999999);
    }
  else
    {
      if (options->contour_index)
        {
          contourEmphasis (options->contour_index);
        }
      else
        {
          contourEmphasis (9999999);
        }
    }


  //  Pass the grid array (arranged 1D) to the contouring package.

  initContour (options->cint, nrr, ncc, ar);


  //  Compute half of a grid cell in degrees.

  half_gridx = misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees * 0.5;
  half_gridy = misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees * 0.5;


  //  Get the contours from the package until all are drawn.

  while (getContour (&level, &index_contour, &num_points, contour_x, contour_y))
    {
      //  Convert from grid points (returned contours) to position.

      for (int32_t i = 0 ; i < num_points ; i++)
        {
          dcontour_x[i] = xorig + (contour_x[i] * misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees) + half_gridx;
          dcontour_y[i] = yorig + (contour_y[i] * misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees) + half_gridy; 
        }


      //  Convert from position to pixels.

      map->map_to_screen (num_points, dcontour_x, dcontour_y, dcontour_z, xyz_x, xyz_y, xyz_z);


      //  smooth out the contour (don't do this sooner for the sake of efficiency -- smooth in xy plot space)

      if (options->smoothing_factor > 0) smooth_contour (num_interp, &num_points, xyz_x, xyz_y);


      QColor cnt = options->contour_color;
      if (level == highlight) cnt = options->contour_highlight_color;


      //  Label index contours

      if (index_contour)
        {
          width = options->contour_width * 2;

          cont_index_1 = 0;
          cont_index_2 = num_points;
          label_x = 0;


          // determine start/end indices. If < 80 pts in contour line, skip labeling

          //if (num_points <= 80)


          // Cheating here - I don't want to do contour labeling at present but I may do it in the future so we're
          // looking for an impossible answer.  See line above for the proper check.

          if (num_points <= 1000000000)
            {
              win_index_2 = 1;
            }
          else
            {
              // initalize window indices to contour indices

              win_index_1 = (cont_index_1 + cont_index_2 + 1) / 2;


              // determine length of label and angle at which to plot it.
              // length of label is number of digits to left of decimal plus number of decimal places desired

              digits = NINT (log10f (level + 0.5));
              label_len_chars = digits;	// number of chars

              double wln = (double) label_len_chars * CONTOUR_LABEL_HEIGHT;

              double label_len = wln * 1.50;

              win_index_2 = win_index_1 + 1;

              if (win_index_2 >= cont_index_2)
                {
                  win_index_2 = cont_index_1;
                }
              else
                {
                  do
                    {
                      double index_diff_x = xyz_x[win_index_1] - xyz_x[win_index_2];
                      double index_diff_y = xyz_y[win_index_1] - xyz_y[win_index_2];
                      dist = sqrt ((index_diff_x * index_diff_x) + (index_diff_y * index_diff_y));


                      win_index_2++;


                      //  if at end of contour and haven't reached correct dist yet break out and draw contour
                      //  without a label

                      if (win_index_2 > num_points) break;

                    } while (dist < label_len);


                  if (win_index_2 > num_points)
                    {
                      win_index_2 = 1;
                    }
                  else
                    {
                      //  plot first part of contour

                      for (int32_t i = 1 ; i < win_index_1 ; i++)
                        {
                          map->drawLine (NINT (xyz_x[i - 1]), NINT (xyz_y[i - 1]), NINT (xyz_x[i]), NINT (xyz_y[i]), cnt, width, NVFalse, Qt::SolidLine);
                        }


                      //  calculate x/y positions for label

                      label_x = NINT (xyz_x[win_index_1] + (xyz_x[win_index_2] - xyz_x[win_index_1]) / 2.0 - label_len / 2.0);
                      label_y = NINT (xyz_y[win_index_1] + (xyz_y[win_index_2] - xyz_y[win_index_1]) / 2.0 + HALF_CONTOUR_LABEL_HEIGHT);


                      //  plot label

                      char format[20];

                      sprintf (format, " %%%dd ", digits);

                      label.sprintf (format, NINT (level));
                    }
                }
            }


          for (int32_t i = win_index_2 ; i < num_points ; i++)
            {
              map->drawLine (NINT (xyz_x[i - 1]), NINT (xyz_y[i - 1]), NINT (xyz_x[i]), NINT (xyz_y[i]), cnt, width, NVFalse, Qt::SolidLine);
            }


          if (label_x) map->drawText (label, label_x, label_y, 90.0, CONTOUR_LABEL_HEIGHT, options->contour_color, NVFalse);
        }
      else
        {
          width = options->contour_width;


          //  Draw the contours.

          for (int32_t i = 1 ; i < num_points ; i++)
            {
              map->drawLine (NINT (xyz_x[i - 1]), NINT (xyz_y[i - 1]), NINT (xyz_x[i]), NINT (xyz_y[i]), cnt, width, NVFalse, Qt::SolidLine);
            }
        }


      //  If we're capturing the contours we have to convert them back to positions and then check them against the 
      //  bounding polygon.

      if (capture)
        {
          map->screen_to_map (num_points, dcontour_x, dcontour_y, dcontour_z, xyz_x, xyz_y, xyz_z);

          double new_x[CONTOUR_POINTS], new_y[CONTOUR_POINTS];
          uint8_t started = NVFalse;
          int32_t cntr = 0;

          options->draw_contour_level = level;

          for (int32_t i = 0 ; i < num_points ; i++)
            {
              if (inside_polygon2 (misc->polygon_x, misc->polygon_y, misc->poly_count, dcontour_x[i], dcontour_y[i]))
                {
                  new_x[cntr] = dcontour_x[i];
                  new_y[cntr] = dcontour_y[i];
                  cntr++;
                  started = NVTrue;

                  if (i == num_points - 1 && cntr > 1)
                    {
                      writeContour (misc, options, cntr, new_x, new_y);
                    }
                }
              else
                {
                  if (started)
                    {
                      if (cntr > 1)
                        {
                          writeContour (misc, options, cntr, new_x, new_y);
                        }

                      started = NVFalse;
                      cntr = 0;
                    }
                }
            }
        }


      //  Check the event queue to see if the user wants to interrupt the contouring.

      if (qApp->hasPendingEvents ())
        {
          qApp->processEvents ();
          if (misc->drawing_canceled) break;
        }
    }

  free (contour_x);
  free (contour_y);
  free (xyz_x);
  free (xyz_y);
  free (xyz_z);


  //  If we were capturing the contours we need to restore the original draw contour level.

  if (capture)
    {
      options->draw_contour_level = save_draw_contour_level;
      for (int32_t gchrtr2 = 0 ; gchrtr2 < misc->chrtr2_share->chrtr2_count ; gchrtr2++) misc->contour_in_chrtr2[gchrtr2] = NVFalse;
    }


  map->flush ();
}
