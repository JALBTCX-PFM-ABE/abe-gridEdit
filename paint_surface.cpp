
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


//  This function paints the surface (actual painting occurs in hatchr.cpp).

void paint_surface (MISC *misc, OPTIONS *options, nvMap *map, NVMAP_DEF *mapdef, uint8_t cov_area_defined)
{
  static float prev_min = CHRTRNULL, prev_max = -CHRTRNULL;


  float *next_attr = NULL;
  uint8_t *next_flags = NULL;

  
  uint8_t compute_layer_min_max (MISC *misc, OPTIONS *options);
  void hatchr (nvMap *map, OPTIONS *options, NV_F64_XYMBR mbr, float min_z, float max_z, float range,
               double x_bin_size, double y_bin_size, int32_t height, int32_t start_x, int32_t end_x, float ss_null, double cell_size_x,
               double cell_size_y, float attr_range, float attr_max, float *current_row, float *next_row,  float *current_attr,
               uint8_t *current_flags, uint8_t alpha, int32_t row, int32_t attribute);
  void geotiff (nvMap *map, MISC *misc, OPTIONS *options);
  void overlayData (nvMap *map, MISC *misc, OPTIONS *options);


  if (!cov_area_defined) return;


  misc->total_displayed_area.min_y = mapdef->bounds[mapdef->zoom_level].min_y;
  misc->total_displayed_area.min_x = mapdef->bounds[mapdef->zoom_level].min_x;
  misc->total_displayed_area.max_y = mapdef->bounds[mapdef->zoom_level].max_y;
  misc->total_displayed_area.max_x = mapdef->bounds[mapdef->zoom_level].max_x;


  if (misc->total_displayed_area.min_x > misc->total_displayed_area.max_x)
    {
      //  Only adjust the lons if the displayed area does not cross the dateline.
            
      if ((misc->total_displayed_area.max_x > 0.0 && misc->total_displayed_area.min_x > 0.0) || 
	  (misc->total_displayed_area.max_x < 0.0 && misc->total_displayed_area.min_x < 0.0))
	{
	  double temp = misc->total_displayed_area.min_x;
	  misc->total_displayed_area.min_x = misc->total_displayed_area.max_x;
	  misc->total_displayed_area.max_x = temp;
	}
    }


  //  Save the displayed area for other CHRTR2 programs.  Some key off of the editor/viewer area (displayed_area)
  //  and some key off of just the viewer area (viewer_displayed_area) so, at this point, we set them both.

  misc->chrtr2_share->displayed_area = misc->chrtr2_share->viewer_displayed_area = misc->total_displayed_area;


  //  Compute center point for sun angle arrow

  double center_map_x = misc->total_displayed_area.min_x + (misc->total_displayed_area.max_x - misc->total_displayed_area.min_x) / 2.0;
  double center_map_y = misc->total_displayed_area.min_y + (misc->total_displayed_area.max_y - misc->total_displayed_area.min_y) / 2.0;
  double dum_map_z;
  int32_t dum_z;
  map->map_to_screen (1, &center_map_x, &center_map_y, &dum_map_z, &misc->center_point.x, &misc->center_point.y, &dum_z);


  //  Compute the min and max for the displayed area (using values from all CHRTR2 layers).

  if (!compute_layer_min_max (misc, options)) return;


  //  Check to see if we want to force clearing because the min and/or max value changed.

  if (misc->color_by_attribute)
    {
      if (prev_min != misc->displayed_area_attr_min || prev_max != misc->displayed_area_attr_max) misc->clear = NVTrue;
      prev_min = misc->displayed_area_attr_min;
      prev_max = misc->displayed_area_attr_max;
    }
  else
    {
      if (prev_min != misc->displayed_area_min || prev_max != misc->displayed_area_max) misc->clear = NVTrue;
      prev_min = misc->displayed_area_min;
      prev_max = misc->displayed_area_max;
    }


  //  Fill the background with the background color so we don't have to paint the background in hatchr.
  //  Also so we can display the GeoTIFF under the data if requested.

  //  Whole area


  if (misc->clear)
    {
      map->fillRectangle (misc->total_displayed_area.min_x, misc->total_displayed_area.min_y,
                          misc->total_displayed_area.max_x, misc->total_displayed_area.max_y,
                          options->background_color, NVFalse);
    }


  //  Partial area (edited area)

  else
    {
      map->fillRectangle (misc->chrtr2_share->edit_area.min_x, misc->chrtr2_share->edit_area.min_y,
                          misc->chrtr2_share->edit_area.max_x, misc->chrtr2_share->edit_area.max_y,
                          options->background_color, NVFalse);
    }


  for (int32_t chrtr2 = misc->chrtr2_share->chrtr2_count - 1 ; chrtr2 >= 0 ; chrtr2--)
    {
      //  Only if we want to display it.

      if (misc->chrtr2_share->display_chrtr2[chrtr2])
        {
          //  Adjust bounds to nearest grid point

          adjust_bounds (misc, chrtr2);


          //  Redraw everything

          if (misc->clear)
            {
              misc->hatchr_start_x = 0;
              misc->hatchr_end_x = misc->displayed_area_width[chrtr2];
              misc->hatchr_start_y = 0;
              misc->hatchr_end_y = misc->displayed_area_height[chrtr2];
            }


          //  Redraw a portion (what we edited)

          else
            {
              NV_F64_XYMBR tmp_mbr = misc->chrtr2_share->edit_area;


              //  Adjust edit bounds to nearest grid point (with a border of 1 grid point to clean up the edges).

              tmp_mbr.min_y = misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat + 
                (NINT ((misc->chrtr2_share->edit_area.min_y - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat) /
                       misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees) - 1) *
                misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees;
              tmp_mbr.max_y = misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat + 
                (NINT ((misc->chrtr2_share->edit_area.max_y - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat) /
                       misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees) + 1) * 
                misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees;
              tmp_mbr.min_x = misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon + 
                (NINT ((misc->chrtr2_share->edit_area.min_x - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon) /
                       misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees) - 1) *
                misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees;
              tmp_mbr.max_x = misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon + 
                (NINT ((misc->chrtr2_share->edit_area.max_x - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon) /
                       misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees) + 1) *
                misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees;


              tmp_mbr.min_y = qMax (tmp_mbr.min_y, misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat);
              tmp_mbr.max_y = qMin (tmp_mbr.max_y, misc->chrtr2_share->chrtr2_header[chrtr2].mbr.nlat);
              tmp_mbr.min_x = qMax (tmp_mbr.min_x, misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon);
              tmp_mbr.max_x = qMin (tmp_mbr.max_x, misc->chrtr2_share->chrtr2_header[chrtr2].mbr.elon);



              misc->hatchr_start_x = NINT ((tmp_mbr.min_x - misc->displayed_area[chrtr2].min_x) /
                                           misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees);
              misc->hatchr_end_x = misc->hatchr_start_x + (NINT ((tmp_mbr.max_x - tmp_mbr.min_x) /
                                                                 misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees));

              misc->hatchr_start_y = NINT ((tmp_mbr.min_y - misc->displayed_area[chrtr2].min_y) /
                                           misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees);
              misc->hatchr_end_y = misc->hatchr_start_y + (NINT ((tmp_mbr.max_y - tmp_mbr.min_y) /
                                                                 misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees));
            }


          //  If the width or height is 0 we have asked for an area outside of the CHRTR2's MBR so we don't want to do anything.

          if (misc->displayed_area_width[chrtr2] > 0 && misc->displayed_area_height[chrtr2] > 0)
            {
              //  We only want to do attributes if we're coloring by attribute.

              int32_t attribute = 0;
              if (misc->color_by_attribute) attribute = misc->color_by_attribute;


              //  If misc->display_GeoTIFF is -1 we want to draw the GeoTIFF first.

              if (chrtr2 == misc->chrtr2_share->chrtr2_count - 1 && misc->display_GeoTIFF < 0) geotiff (map, misc, options);


              //  The only time we don't want to go through here is when we have an open GeoTIFF file, we are displaying
              //  the GeoTIFF surface, and the GeoTIFF alpha value (transparency) is 255.

              if (misc->display_GeoTIFF <= 0 || options->GeoTIFF_alpha < 255)
                {
                  //  Allocate the needed arrays.

                  int32_t size;
                  float *ar = NULL;

                  if (options->contour)
                    {
                      size = (misc->displayed_area_width[chrtr2] + 1) * (misc->displayed_area_height[chrtr2] + 1);

                      ar = (float *) calloc (size, sizeof (float));

                      if (ar == NULL)
                        {
                          perror (gridEdit::tr ("Allocating ar array in paint_surface").toLatin1 ());
                          exit (-1);
                        }
                    }


                  CHRTR2_RECORD *current_record = (CHRTR2_RECORD *) calloc (misc->displayed_area_width[chrtr2], sizeof (CHRTR2_RECORD));
                  if (current_record == NULL)
                    {
                      perror (gridEdit::tr ("Allocating current_record in paint_surface").toLatin1 ());
                      exit (-1);
                    }

                  misc->next_row = (float *) calloc (misc->displayed_area_width[chrtr2], sizeof (float));
                  if (misc->next_row == NULL)
                    {
                      perror (gridEdit::tr ("Allocating next_row in paint_surface").toLatin1 ());
                      exit (-1);
                    }

                  if (attribute)
                    {
                      next_attr = (float *) calloc (misc->displayed_area_width[chrtr2], sizeof (float));
                      if (next_attr == NULL)
                        {
                          perror (gridEdit::tr ("Allocating next_attr in paint_surface").toLatin1 ());
                          exit (-1);
                        }
                    }

                  next_flags = (uint8_t *) calloc (misc->displayed_area_width[chrtr2], sizeof (char));
                  if (next_flags == NULL)
                    {
                      perror (gridEdit::tr ("Allocating next_flags in paint_surface").toLatin1 ());
                      exit (-1);
                    }

                  misc->current_row = (float *) calloc (misc->displayed_area_width[chrtr2], sizeof (float));
                  if (misc->current_row == NULL)
                    {
                      perror (gridEdit::tr ("Allocating current_row in paint_surface").toLatin1 ());
                      exit (-1);
                    }

                  if (attribute)
                    {
                      misc->current_attr = (float *) calloc (misc->displayed_area_width[chrtr2], sizeof (float));
                      if (misc->current_attr == NULL)
                        {
                          perror (gridEdit::tr ("Allocating current_attr in paint_surface").toLatin1 ());
                          exit (-1);
                        }
                    }


                  misc->current_flags = (uint8_t *) calloc (misc->displayed_area_width[chrtr2], sizeof (char));
                  if (misc->current_flags == NULL)
                    {
                      perror (gridEdit::tr ("Allocating current_flags in paint_surface").toLatin1 ());
                      exit (-1);
                    }


                  //  Read input data row by row.  Note that hatchr_start_y and hatchr_end_y may not be the same as the
                  //  entire displayed area since we may only be redrawing a small edited portion of the display.

                  for (int32_t jj = misc->hatchr_start_y ; jj <= misc->hatchr_end_y ; jj++)
                    {
                      //  Read data from disk.

                      if (jj == misc->hatchr_start_y)
                        {
                          chrtr2_read_row (misc->chrtr2_handle[chrtr2], misc->displayed_area_row[chrtr2] + jj, misc->displayed_area_column[chrtr2],
                                           misc->displayed_area_width[chrtr2], current_record);


                          loadArrays (misc->displayed_area_width[chrtr2], current_record, misc->current_row, misc->current_attr, attribute,
                                      misc->current_flags, options->highlight, options->highlight_depth[0], options->highlight_depth[1],
                                      options->display_interpolated);

                          memcpy (misc->next_row, misc->current_row, misc->displayed_area_width[chrtr2] * sizeof (float));

                          if (attribute) memcpy (next_attr, misc->current_attr, misc->displayed_area_width[chrtr2] * sizeof (float));

                          memcpy (next_flags, misc->current_flags, misc->displayed_area_width[chrtr2] * sizeof (char));
                        }
                      else
                        {
                          memcpy (misc->current_row, misc->next_row, misc->displayed_area_width[chrtr2] * sizeof (float));

                          if (attribute) memcpy (misc->current_attr, next_attr, misc->displayed_area_width[chrtr2] * sizeof (float));

                          memcpy (misc->current_flags, next_flags, misc->displayed_area_width[chrtr2] * sizeof (char));


                          //  If not at top edge, read another row.

                          if (jj < misc->chrtr2_share->chrtr2_header[chrtr2].height) 
                            {
                              chrtr2_read_row (misc->chrtr2_handle[chrtr2], misc->displayed_area_row[chrtr2] + jj, misc->displayed_area_column[chrtr2],
                                               misc->displayed_area_width[chrtr2], current_record);


                              loadArrays (misc->displayed_area_width[chrtr2], current_record, misc->next_row, next_attr, attribute, next_flags,
                                          options->highlight, options->highlight_depth[0], options->highlight_depth[1], options->display_interpolated);
                            }
                        }


                      //  HSV fill and sunshade.

                      hatchr (map, options, misc->displayed_area[chrtr2], misc->displayed_area_min, misc->displayed_area_max, misc->displayed_area_range,
                              misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees, misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees,
                              misc->chrtr2_share->chrtr2_header[chrtr2].height, misc->hatchr_start_x, misc->hatchr_end_x, CHRTR2_NULL_Z_VALUE,
                              misc->ss_cell_size_x[chrtr2], misc->ss_cell_size_y[chrtr2], misc->displayed_area_attr_range, misc->displayed_area_attr_max,
                              misc->current_row, misc->next_row, misc->current_attr, misc->current_flags, misc->chrtr2_alpha[chrtr2], jj, attribute);


                      if (options->contour)
                        {
                          for (int32_t i = 0 ; i < misc->displayed_area_width[chrtr2] ; i++)
                            {
                              //  We use next_row for sunshading on all but the first row to send to scribe.

                              if (i)
                                {
                                  ar[(jj - misc->hatchr_start_y) * misc->displayed_area_width[chrtr2] + i] = misc->next_row[i] * 
                                    options->z_factor + options->z_offset;
                                }
                              else
                                {
                                  ar[(jj - misc->hatchr_start_y) * misc->displayed_area_width[chrtr2] + i] = misc->current_row[i] * 
                                    options->z_factor + options->z_offset;
                                }
                            }
                        }

                      if (qApp->hasPendingEvents ())
                        {
                          qApp->processEvents();
                          if (misc->drawing_canceled) break;
                        }
                    }


                  //  Flush all of the saved fillRectangles from hatchr to screen

                  map->flush ();


                  //  Free allocated memory.

                  free (current_record);
                  free (misc->next_row);
                  if (attribute) free (next_attr);
                  free (next_flags);
                  free (misc->current_row);
                  if (attribute) free (misc->current_attr);
                  free (misc->current_flags);


                  if (!misc->drawing_canceled)
                    {
                      //  If you asked for contours, do it.

                      if (options->contour)
                        {
                          if (misc->function == DRAW_CONTOUR)
                            {
                              scribe (map, options, misc, chrtr2, ar, options->draw_contour_level);
                            }
                          else
                            {
                              scribe (map, options, misc, chrtr2, ar, 99999.0);
                            }
                        }
                    }


                  //  Free allocated scribe memory.

                  if (options->contour) free (ar);
                }


              //  Issue a warning when the user is displaying more bins than pixels

              if (!misc->tposiafps && (misc->clear && (misc->displayed_area_width[chrtr2] > mapdef->draw_width ||
                                                       misc->displayed_area_height[chrtr2] > mapdef->draw_height)))
                {
                  QString warning_message = gridEdit::tr ("Number of bins displayed exceeds number of pixels.\n");
                  warning_message += gridEdit::tr ("Some data masking will occur.\n");
                  warning_message += gridEdit::tr ("Increase window size or decrease area size.\n\n");
                  warning_message +=
                    QString (gridEdit::tr ("Grid width  = %1  ,  Pixel width  = %2\n")).arg (misc->displayed_area_width[chrtr2]).arg (mapdef->draw_width);
                  warning_message +=
                    QString (gridEdit::tr ("Grid height = %1  ,  Pixel height = %2\n")).arg (misc->displayed_area_height[chrtr2]).arg (mapdef->draw_height);

                  QMessageBox msgBox (QMessageBox::Warning, gridEdit::tr ("gridEdit TPOSIAFPS"), warning_message);

                  msgBox.setStandardButtons (QMessageBox::Close);
                  msgBox.setDefaultButton (QMessageBox::Close);

                  QCheckBox dontShow (gridEdit::tr ("I understand that I can't see everything due to resolution but don't show this again!"), &msgBox);
                  msgBox.addButton (&dontShow, QMessageBox::HelpRole);
                  msgBox.exec ();
                  misc->tposiafps = ((bool) dontShow.checkState ());
                }
            }
        }
    }


  if (!misc->drawing_canceled)
    {
      //  Display GeoTIFF data if available.

      if (misc->display_GeoTIFF == 1) geotiff (map, misc, options);


      //  Overlay any flags that have been requested.

      overlayFlag (map, options, misc);


      //  Overlay any files that have been requested.

      overlayData (map, misc, options);


      //  Display filter masks if any are defined.

      for (int32_t i = 0 ; i < misc->poly_filter_mask_count ; i++)
        {
          if (!misc->poly_filter_mask[i].displayed)
            {
              //  Check to see if this mask (that we are going to redraw) overlaps another (that we weren't planning on redrawing).
              //  If so, set the flag to redraw the other one.

              for (int32_t j = 0 ; j < misc->poly_filter_mask_count ; j++)
                {
                  if (j != i && misc->poly_filter_mask[j].displayed)
                    {
                      if (polygon_collision2 (misc->poly_filter_mask[i].x, misc->poly_filter_mask[i].y, misc->poly_filter_mask[i].count,
                                              misc->poly_filter_mask[j].x, misc->poly_filter_mask[j].y, misc->poly_filter_mask[j].count))
                        misc->poly_filter_mask[j].displayed = NVFalse;
                    }
                }
            }
        }


      for (int32_t i = 0 ; i < misc->poly_filter_mask_count ; i++)
        {
          if (!misc->poly_filter_mask[i].displayed)
            {
              if (options->poly_filter_mask_color.alpha () < 255)
                {
                  map->fillPolygon (misc->poly_filter_mask[i].count, misc->poly_filter_mask[i].x, misc->poly_filter_mask[i].y, 
                                    options->poly_filter_mask_color, NVTrue);
                }
              else
                {
                  //  We don't have to worry about clipping this because moving the area discards the mask areas.

                  map->drawPolygon (misc->poly_filter_mask[i].count, misc->poly_filter_mask[i].x, misc->poly_filter_mask[i].y,
                                    options->poly_filter_mask_color, 2, NVTrue, Qt::SolidLine, NVTrue);
                }
              misc->poly_filter_mask[i].displayed = NVTrue;
            }
        }


      //  Flag minimum and maximum if enabled.

      if (options->display_minmax) displayMinMax (map, options, misc);
    }
}
