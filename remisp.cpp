
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
#include "misp.h"


#define         FILTER 9


//  This function runs MISP on the selected area.

remisp::remisp (MISC *mi, OPTIONS *op, NV_F64_XYMBR *m, uint8_t undo)
{
  std::vector<NV_F64_COORD3> xyz_array;
  int32_t            out_count, misp_weight, hd_count = 0, pfm_count = 0;
  CHRTR2_RECORD      chrtr2_record;
  NV_F64_COORD3      xyz;
  NV_F64_XYMBR       grid_mbr[MAX_CHRTR2_FILES];
  int32_t            gridcols[MAX_CHRTR2_FILES], gridrows[MAX_CHRTR2_FILES], col_filter[MAX_CHRTR2_FILES], row_filter[MAX_CHRTR2_FILES];
  double             half_x = 0.0, half_y = 0.0;
  std::vector<NV_I32_COORD3> track_hd_contour, track_pfm_data;
  QString            progText;
  gridThread         grid_thread;



  misc = mi;
  options = op;
  mbr = m;

  misp_weight = options->misp_weight;
  complete = NVFalse;


  //  If the undo flag is set to NVFalse we don't want to write the undo blocks.  This is only done if we are remisping after
  //  running gridPointEdit or gridPointEdit3D.

  if (undo)
    {
      if (!create_undo (misc))
        {
          QMessageBox::critical (misc->map_widget, gridEdit::tr ("gridEdit"), gridEdit::tr ("Unable to create and/or open undo file!"));
          return;
        }
    }


  out_count = 0;


  //  First we retrieve all valid data points from all available CHRTR2 layers.

  for (int32_t chrtr2 = 0 ; chrtr2 < misc->chrtr2_share->chrtr2_count ; chrtr2++)
    {
      //  Only if it's displayed and not locked.

      if (!misc->chrtr2_share->display_chrtr2[chrtr2] || misc->chrtr2_share->lock_chrtr2[chrtr2])
        {
          if (undo) start_undo_block (misc, chrtr2, 0, 0, 0, 0);
        }
      else
	{
	  //  Adjust the MBR to the nearest grid point.  Note that part or all of the MBR may be outside
	  //  of the CHRTR2 bounds if we are handling more than one layer.

	  grid_mbr[chrtr2].min_y = misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat +
            (NINT ((mbr->min_y - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat) / misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees)) *
	    misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees;

	  grid_mbr[chrtr2].max_y = misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat + 
	    (NINT ((mbr->max_y - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat) / misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees)) *
	    misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees;

	  grid_mbr[chrtr2].min_x = misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon + 
	    (NINT ((mbr->min_x - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon) / misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees)) *
	    misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees;

	  grid_mbr[chrtr2].max_x = misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon +
            (NINT ((mbr->max_x - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon) / misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees)) *
	    misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees;


	  //  Add the filter border to the MBR

	  grid_mbr[chrtr2].min_x -= ((double) FILTER * misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees);
	  grid_mbr[chrtr2].min_y -= ((double) FILTER * misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees);
	  grid_mbr[chrtr2].max_x += ((double) FILTER * misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees);
	  grid_mbr[chrtr2].max_y += ((double) FILTER * misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees);


	  //  Number of rows and columns in the area

	  gridcols[chrtr2] = NINT ((grid_mbr[chrtr2].max_x - grid_mbr[chrtr2].min_x) / misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees);
	  gridrows[chrtr2] = NINT ((grid_mbr[chrtr2].max_y - grid_mbr[chrtr2].min_y) / misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees);


          row_filter[chrtr2] = gridrows[chrtr2] - FILTER;
          col_filter[chrtr2] = gridcols[chrtr2] - FILTER;


          //  Compute the undo block extents.

          double min_y = qMax (grid_mbr[chrtr2].min_y, misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat);
          double max_y = qMin (grid_mbr[chrtr2].max_y, misc->chrtr2_share->chrtr2_header[chrtr2].mbr.nlat);
          double min_x = qMax (grid_mbr[chrtr2].min_x, misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon);
          double max_x = qMin (grid_mbr[chrtr2].max_x, misc->chrtr2_share->chrtr2_header[chrtr2].mbr.elon);


          int32_t width = NINT ((max_x - min_x) / misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees);
          int32_t height = NINT ((max_y - min_y) / misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees);
          int32_t row = NINT ((min_y - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat) / misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees);
          int32_t column = NINT ((min_x - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon) / misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees);

          if (column < 0) column = 0;
          if (column + width >= misc->chrtr2_share->chrtr2_header[chrtr2].width) width = misc->chrtr2_share->chrtr2_header[chrtr2].width - column - 1;
          if (row < 0) row = 0;
          if (row + height >= misc->chrtr2_share->chrtr2_header[chrtr2].height) height = misc->chrtr2_share->chrtr2_header[chrtr2].height - row - 1;


          //  If width or height is less than 0 we're outside the MBR so we want to go to the next CHRTR2 file.

          if (width <= 0 || height <= 0)
            {
              if (undo) start_undo_block (misc, chrtr2, 0, 0, 0, 0);
            }
          else
            {
              if (undo)
                {
                  start_undo_block (misc, chrtr2, row, row + height - 1, column, column + width - 1);


                  //  Unfortunately, it is too damn hard to deal with edge conditions during undo so we're reading the data twice (I got lazy).
                  //  Fortunately, most modern systems cache disk I/O so it will probably not be read from memory the second time ;-)

                  for (int32_t i = row ; i < row + height ; i++)
                    {
                      for (int32_t j = column ; j < column + width ; j++)
                        {
                          chrtr2_read_record_row_col (misc->chrtr2_handle[chrtr2], i, j, &chrtr2_record);

                          if (!append_undo (misc, chrtr2_record))
                            {
                              QMessageBox::critical (misc->map_widget, gridEdit::tr ("gridEdit"), gridEdit::tr ("Unable to write undo records!"));
                              remove_last_undo (misc);
                              return;
                            }
                        }
                    }
                }


              misc->statusProg->setRange (0, gridrows[chrtr2]);
              progText = gridEdit::tr (" Reading CHRTR2 data from ") +
                QFileInfo (QString (misc->chrtr2_share->chrtr2_file[chrtr2])).fileName ().remove (".ch2") + " ";
              misc->statusProgLabel->setText (progText);
              misc->statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::green);
              misc->statusProgLabel->setPalette (misc->statusProgPalette);
              misc->statusProg->setTextVisible (true);
              qApp->processEvents();


              half_x = misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees * 0.5;
              half_y = misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees * 0.5;


              //  Get the data that falls in the MBR from the current CHRTR2 and save it to memory.

              for (int32_t i = 0 ; i < gridrows[chrtr2] ; i++)
                {
                  //  Compute the latitude of the center of the bin.

                  NV_F64_COORD2 xy;
                  xy.y = grid_mbr[chrtr2].min_y + i * misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees + half_y;

                  misc->statusProg->setValue (i);
                  qApp->processEvents();

                  for (int32_t j = 0 ; j < gridcols[chrtr2] ; j++)
                    {
                      //  Compute the longitude of the center of the bin.

                      xy.x = grid_mbr[chrtr2].min_x + j * misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees + half_x;


                      //  Make sure that we are inside of the CHRTR2

                      if (inside_mbr_coord2 (&misc->chrtr2_share->chrtr2_header[chrtr2].mbr, xy))
                        {
                          chrtr2_read_record_lat_lon (misc->chrtr2_handle[chrtr2], xy.y, xy.x, &chrtr2_record);


                          xyz.x = xy.x;
                          xyz.y = xy.y;
                          xyz.z = chrtr2_record.z;


                          //  If we have data in the bin, go get it (we want to interpolate over already interpolated data
                          //  so we only load real or drawn data except in the filter border).

                          if (chrtr2_record.status & (CHRTR2_REAL | CHRTR2_DIGITIZED_CONTOUR))
                            {
                              add_point (&xyz_array, xyz, &out_count);
                            }
                          else
                            {
                              //  If there was no depth data in the border area (filter size) we want to load previously
                              //  interpolated (MISP gridded) data in order to smoothly edge match to existing interpolated data
                              //  outside of the MBR.

                              if (chrtr2_record.status & CHRTR2_INTERPOLATED)
                                {
                                  if (i < FILTER || j < FILTER || i >= row_filter[chrtr2] || j >= col_filter[chrtr2])
                                    {
                                      add_point (&xyz_array, xyz, &out_count);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }


  //  Don't process if we didn't have any input data (xyz would not have been allocated so we don't need to free it).

  if (!out_count)
    {
      remove_last_undo (misc);
      return;
    }


  //  We have all of the input data (from any visible CHRTR2) in memory so now we run MISP for each CHRTR2 layer.

  for (int32_t chrtr2 = 0 ; chrtr2 < misc->chrtr2_share->chrtr2_count ; chrtr2++)
    {
      //  Only deal with those that are being displayed and aren't locked.

      if (misc->chrtr2_share->display_chrtr2[chrtr2] && !misc->chrtr2_share->lock_chrtr2[chrtr2])
        {
          //  We're going to let MISP handle everything in zero based units of the bin size.  That is, we subtract off the
          //  west lon from longitudes then divide by the grid size in the X direction.  We do the same with the latitude using
          //  the south latitude.  This will give us values that range from 0.0 to gridcols[chrtr2] in longitude and 0.0 to
          //  gridrows[chrtr2] in latitude.

          NV_F64_XYMBR new_mbr;
          new_mbr.min_x = 0.0;
          new_mbr.min_y = 0.0;
          new_mbr.max_x = (double) gridcols[chrtr2];
          new_mbr.max_y = (double) gridrows[chrtr2];


          //  Initialize the MISP engine.

          if (options->misp_force_original) misp_weight = -misp_weight;
          misp_init (1.0, 1.0, 0.05, 4, 20.0, 20, 999999.0, -999999.0, misp_weight, new_mbr);

          progText = gridEdit::tr (" Loading data into MISP for ") +
            QFileInfo (QString (misc->chrtr2_share->chrtr2_file[chrtr2])).fileName ().remove (".ch2") + " ";


          misc->statusProgLabel->setText (progText);
          misc->statusProg->setRange (0, out_count);
          qApp->processEvents ();


          //  Don't allow the progress bar to slow things down too much.

          int32_t inc = out_count / 20;
          if (!inc) inc = 1;


          for (int32_t i = 0 ; i < out_count ; i++)
            {
              if (!(out_count % inc))
                {
                  misc->statusProg->setValue (i);
                  qApp->processEvents();
                }


              //  Load the points.
              //
              //  IMPORTANT NOTE:  MISP (by default) grids using corner posts.  That is, the data in a bin is assigned to the 
              //  lower left corner of the bin.  Normal gridding/binning systems use the center of the bin.  Because of this we need
              //  to lie to MISP and tell them that the point is really half a bin lower and to the left.  This is extremely
              //  confusing but it works ;-)

              xyz.x = (xyz_array[i].x - grid_mbr[chrtr2].min_x) / misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees - 0.5;
              xyz.y = (xyz_array[i].y - grid_mbr[chrtr2].min_y) / misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees - 0.5;
              xyz.z = xyz_array[i].z;
              misp_load (xyz);
            }


          //  Load any hand drawn contour points in the area.  Since no HD contour exists in a cell that had real data
          //  we want to keep track of what cells in what CHRTR2 get HD data so we can flag the cells after the remisp.

          for (int32_t i = 0 ; i < misc->hd_contour_count ; i++)
            {
              for (int32_t j = 0 ; j < misc->hd_contour[i].count ; j++)
                {
                  //  IMPORTANT NOTE:  MISP (by default) grids using corner posts.  That is, the data in a bin is assigned to the 
                  //  lower left corner of the bin.  Normal gridding/binning systems use the center of the bin.  Because of this we need
                  //  to lie to MISP and tell them that the point is really half a bin lower and to the left.  This is extremely
                  //  confusing but it works ;-)

                  xyz.x = (misc->hd_contour[i].xyz[j].x - grid_mbr[chrtr2].min_x) / misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees - 0.5;
                  xyz.y = (misc->hd_contour[i].xyz[j].y - grid_mbr[chrtr2].min_y) / misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees - 0.5;
                  xyz.z = misc->hd_contour[i].xyz[j].z;
                  misp_load (xyz);

                  try
                    {
                      track_hd_contour.resize (hd_count + 1);
                    }
                  catch (std::bad_alloc&)
                    {
                      perror ("Allocating track_hd_contour memory in remisp.cpp");
                      exit (-1);
                    }

                  track_hd_contour[hd_count].z = chrtr2;
                  NV_I32_COORD2 hdxy;
                  chrtr2_get_coord (misc->chrtr2_handle[chrtr2], misc->hd_contour[i].xyz[j].y, misc->hd_contour[i].xyz[j].x, &hdxy);
                  track_hd_contour[hd_count].x = hdxy.x;
                  track_hd_contour[hd_count].y = hdxy.y;
                  hd_count++;
                }
            }


          //  Load any PFM data we may be inserting.

          if (misc->pfm_data_count)
            {
              for (int32_t i = 0 ; i < misc->pfm_data_count ; i++)
                {
                  //  IMPORTANT NOTE:  MISP (by default) grids using corner posts.  That is, the data in a bin is assigned to the 
                  //  lower left corner of the bin.  Normal gridding/binning systems use the center of the bin.  Because of this we need
                  //  to lie to MISP and tell them that the point is really half a bin lower and to the left.  This is extremely
                  //  confusing but it works ;-)

                  xyz.x = (misc->pfm_data[i].x - grid_mbr[chrtr2].min_x) / misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees - 0.5;
                  xyz.y = (misc->pfm_data[i].y - grid_mbr[chrtr2].min_y) / misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees - 0.5;
                  xyz.z = misc->pfm_data[i].z;
                  misp_load (xyz);

                  try
                    {
                      track_pfm_data.resize (pfm_count + 1);
                    }
                  catch (std::bad_alloc&)
                    {
                      perror ("Allocating track_pfm_data memory in remisp.cpp");
                      exit (-1);
                    }

                  track_pfm_data[pfm_count].z = chrtr2;
                  NV_I32_COORD2 pfmxy;
                  chrtr2_get_coord (misc->chrtr2_handle[chrtr2], misc->pfm_data[i].y, misc->pfm_data[i].x, &pfmxy);
                  track_pfm_data[pfm_count].x = pfmxy.x;
                  track_pfm_data[pfm_count].y = pfmxy.y;
                  pfm_count++;
                }

              misc->pfm_data.clear ();
              misc->pfm_data_count = 0;
            }


          progText = gridEdit::tr (" Computing MISP surface for ") +
            QFileInfo (QString (misc->chrtr2_share->chrtr2_file[chrtr2])).fileName ().remove (".ch2") + " ";

          misc->statusProgLabel->setText (progText);
	  misc->statusProg->setRange (0, 0);
	  qApp->processEvents();


          //  We're starting the grid processing concurrently using a thread.  Note that we're using the Qt::DirectConnection type
          //  for the signal/slot connections.  This causes all of the signals emitted from the thread to be serviced immediately.
          //  Why are we running misp_proc in a thread???  Because it's the only way to get the stupid progress bar to update so
          //  that the user will know that the damn program is still running.  Sheesh!

          complete = NVFalse;
          connect (&grid_thread, SIGNAL (completed ()), this, SLOT (slotGridCompleted ()), Qt::DirectConnection);

          grid_thread.grid (misc->chrtr2_share->chrtr2_header[chrtr2].grid_type);


          //  We can't move on until the thread is complete but we want to keep our progress bar updated.  This is a bit tricky 
          //  because you can't update the progress bar from within slots connected to thread signals.  Those slots are considered part
          //  of the mispThread and not part of the GUI thread.  When the thread is finished we move on to the retrieval step.

          while (!complete)
            {
#ifdef NVWIN3X
              Sleep (50);
#else
              usleep (50000);
#endif

              qApp->processEvents ();
            }


          progText = gridEdit::tr (" Retrieving MISP data for ") +
            QFileInfo (QString (misc->chrtr2_share->chrtr2_file[chrtr2])).fileName ().remove (".ch2") + " ";


          misc->statusProgLabel->setText (progText);
	  misc->statusProg->setRange (0, gridrows[chrtr2]);
	  qApp->processEvents();


          //  Allocating one more than gridcols[chrtr2] due to constraints of old chrtr (see comments in misp_funcs.c)

          float *array = (float *) malloc ((gridcols[chrtr2] + 1) * sizeof (float));

          if (array == NULL)
            {
              perror ("Allocating array in remisp");
              exit (-1);
            }


          //  This is where we stuff the new interpolated surface back in to the CHRTR2.

          for (int32_t i = 0 ; i < gridrows[chrtr2] ; i++)
            {
              misc->statusProg->setValue (i);
              qApp->processEvents();


              if (!misp_rtrv (array)) break;


              //  Only use data that aren't in the filter border

              if (i >= FILTER && i < row_filter[chrtr2])
                {
                  //  Compute the latitude of the center of the bin.

                  NV_F64_COORD2 xy;
                  xy.y = grid_mbr[chrtr2].min_y + i * misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees + half_y;


                  for (int32_t j = 0 ; j < gridcols[chrtr2] ; j++)
                    {
                      //  Only use data that aren't in the filter border

                      if (j >= FILTER && j < col_filter[chrtr2])
                        {
                          //  Compute the longitude of the center of the bin.

                          xy.x = grid_mbr[chrtr2].min_x + j * misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees + half_x;


                          //  Make sure we're inside the CHRTR2 bounds.

                          if (inside_mbr_coord2 (&misc->chrtr2_share->chrtr2_header[chrtr2].mbr, xy))
                            {
                              //  Read the record.

                              chrtr2_read_record_lat_lon (misc->chrtr2_handle[chrtr2], xy.y, xy.x, &chrtr2_record);


                              //  We don't want to replace real, digitized, or land masked bins.

                              if (!(chrtr2_record.status & (CHRTR2_REAL | CHRTR2_DIGITIZED_CONTOUR | CHRTR2_LAND_MASK)))
                                {
                                  //  Mark the record as interpolated unless it had an input point (which means it was newly drawn).

                                  if (bit_test (array[j], 0))
                                    {
                                      chrtr2_record.status = (chrtr2_record.status & CHRTR2_DIGITIZED_MASK) | CHRTR2_DIGITIZED_CONTOUR;
                                    }
                                  else
                                    {
                                      chrtr2_record.status = (chrtr2_record.status & CHRTR2_INTERPOLATED_MASK) | CHRTR2_INTERPOLATED;
                                    }


                                  //  If we exceeded the CHRTR2 limits we have to set it to CHRTR2_NULL (everything zeroed).

                                  if (array[j] <= misc->chrtr2_share->chrtr2_header[chrtr2].max_z &&
                                      array[j] >= misc->chrtr2_share->chrtr2_header[chrtr2].min_z)
                                    {
                                      chrtr2_record.z = array[j];
                                    }
                                  else
                                    {
                                      memset (&chrtr2_record, 0, sizeof (CHRTR2_RECORD));
                                    }


                                  //  Write the record back out.

                                  chrtr2_write_record_lat_lon (misc->chrtr2_handle[chrtr2], xy.y, xy.x, chrtr2_record);
                                }
                            }
                        }
                    }
                }
            }

          free (array);
	}
    }


  misc->statusProg->reset ();
  misc->statusProg->setTextVisible (false);
  qApp->processEvents();


  xyz_array.clear ();


  //  If we had any hand drawn contours we need to clean them up.

  if (misc->hd_contour_count)
    {
      //  Check each point against all (displayed) CHRTR2 files to see if it was loaded.

      for (int32_t i = 0 ; i < misc->hd_contour_count ; i++)
        {
          for (int32_t j = 0 ; j < misc->hd_contour[i].count ; j++)
            {
              for (int32_t chrtr2 = 0 ; chrtr2 < misc->chrtr2_share->chrtr2_count ; chrtr2++)
                {
                  if (misc->chrtr2_share->display_chrtr2[chrtr2] && !misc->chrtr2_share->lock_chrtr2[chrtr2])
                    {
                      //  If we're inside the remisp area mark this point for deletion.

                      if (inside_xymbr_coord3 (mbr, misc->hd_contour[i].xyz[j])) misc->hd_contour[i].xyz[j].z = CHRTRNULL;
                    }
                }
            }
        }


      //  Now clear out all of the points that we loaded and remove unneeded memory.

      for (int32_t i = 0 ; i < misc->hd_contour_count ; i++)
        {
          int32_t new_point_count = 0;
          for (int32_t j = 0 ; j < misc->hd_contour[i].count ; j++)
            {
              if (misc->hd_contour[i].xyz[j].z < CHRTRNULL)
                {
                  misc->hd_contour[i].xyz[new_point_count] = misc->hd_contour[i].xyz[j];
                  new_point_count++;
                }
            }


          misc->hd_contour[i].count = new_point_count;


          //  We still have points so we need to resize the array.

          if (misc->hd_contour[i].count)
            {
              try
                {
                  misc->hd_contour[i].xyz.resize (misc->hd_contour[i].count);
                }
              catch (std::bad_alloc&)
                {
                  perror ("Allocating misc->hd_contour[i].xyz in remisp.cpp");
                  exit (-1);
                }
            }
          else
            {
              misc->hd_contour[i].xyz.clear ();
            }
        }


      //  Now we need to get rid of any contours that we have completely emptied out.

      int32_t new_contour_count = 0;
      for (int32_t i = 0 ; i < misc->hd_contour_count ; i++)
        {
          //  The contour still has data.

          if (misc->hd_contour[i].count)
            {
              misc->hd_contour[new_contour_count].count = misc->hd_contour[i].count;
              misc->hd_contour[new_contour_count].xyz = misc->hd_contour[i].xyz;
              new_contour_count++;
            }
        }

      misc->hd_contour_count = new_contour_count;

      if (misc->hd_contour_count)
        {
          try
            {
              misc->hd_contour.resize (misc->hd_contour_count);
            }
          catch (std::bad_alloc&)
            {
              perror ("Allocating misc->hd_contour in remisp.cpp");
              exit (-1);
            }
        }
      else
        {
          misc->hd_contour.clear ();
        }


      //  If we actually stored any HD contour points we need to set the status of those bins as CHRTR2_DIGITIZED_CONTOUR.

      if (hd_count)
        {
          for (int32_t i = 0 ; i < hd_count ; i++)
            {
              chrtr2_read_record_row_col (misc->chrtr2_handle[track_hd_contour[i].z], track_hd_contour[i].y, track_hd_contour[i].x, &chrtr2_record);

              if (chrtr2_record.status && !(chrtr2_record.status & CHRTR2_REAL))
                {
                  chrtr2_record.status = (chrtr2_record.status & CHRTR2_DIGITIZED_MASK) | CHRTR2_DIGITIZED_CONTOUR;
                  chrtr2_write_record_row_col (misc->chrtr2_handle[track_hd_contour[i].z], track_hd_contour[i].y, track_hd_contour[i].x, chrtr2_record);
                }
            }

          track_hd_contour.clear ();
          hd_count = 0;
        }
    }


  //  If we actually stored any PFM data points we need to set the status of those bins as CHRTR2_REAL.

  if (pfm_count)
    {
      for (int32_t i = 0 ; i < pfm_count ; i++)
        {
          chrtr2_read_record_row_col (misc->chrtr2_handle[track_pfm_data[i].z], track_pfm_data[i].y, track_pfm_data[i].x, &chrtr2_record);

          if (chrtr2_record.status && !(chrtr2_record.status & CHRTR2_REAL))
            {
              chrtr2_record.status = (chrtr2_record.status & CHRTR2_REAL_MASK) | CHRTR2_REAL;
              chrtr2_write_record_row_col (misc->chrtr2_handle[track_pfm_data[i].z], track_pfm_data[i].y, track_pfm_data[i].x, chrtr2_record);
            }
        }

      track_pfm_data.clear ();
      pfm_count = 0;
    }
}



remisp::~remisp ()
{
}



void 
remisp::add_point (std::vector<NV_F64_COORD3> *xyz_array, NV_F64_COORD3 xyz, int32_t *count)
{
  try
    {
      xyz_array->resize (*count + 1);
    }
  catch (std::bad_alloc&)
    {
      perror (gridEdit::tr ("Allocating xyz_array in remisp.").toLatin1 ());
      exit (-1);
    }


  (*xyz_array)[*count] = xyz;

  (*count)++;
}



void remisp::slotGridCompleted ()
{
  complete = NVTrue;
}
