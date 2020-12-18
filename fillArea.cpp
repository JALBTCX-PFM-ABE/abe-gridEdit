
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


//  Fills an area with a set value, sets the area to CHRTR2_NULL (set value to NULL_Z_VALUE), or deletes 
//  hand-drawn contours within the polygon (set value to -99999.0).

void fillArea (MISC *misc, float value, double *mx, double *my, int32_t count)
{
  NV_I32_COORD2      coord;
  NV_F64_XYMBR       mbr;
  QString            string;
  CHRTR2_RECORD      chrtr2_record;


  //  Figure out the MBR of the area to be filled

  mbr.min_x = 999999.0;
  mbr.min_y = 999999.0;
  mbr.max_x = -999999.0;
  mbr.max_y = -999999.0;

  for (int32_t i = 0 ; i < count ; i++)
    {
      if (misc->dateline && mx[i] < 0.0) mx[i] += 360.0;

      if (mx[i] < mbr.min_x) mbr.min_x = mx[i];
      if (my[i] < mbr.min_y) mbr.min_y = my[i];
      if (mx[i] > mbr.max_x) mbr.max_x = mx[i];
      if (my[i] > mbr.max_y) mbr.max_y = my[i];
    }


  if (!create_undo (misc))
    {
      QMessageBox::critical (misc->map_widget, gridEdit::tr ("gridEdit"), gridEdit::tr ("Unable to create and/or open undo file!"));
      return;
    }


  for (int32_t chrtr2 = 0 ; chrtr2 < misc->chrtr2_share->chrtr2_count ; chrtr2++)
    {
      //  Only if we want to display it and it's not locked.

      if (!misc->chrtr2_share->display_chrtr2[chrtr2] || misc->chrtr2_share->lock_chrtr2[chrtr2])
        {
          start_undo_block (misc, chrtr2, 0, 0, 0, 0);
        }
      else
        {
	  double half_x = misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees * 0.5;
	  double half_y = misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees * 0.5;


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


          int32_t width = (NINT ((max_x - min_x) / misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees));
          int32_t height = (NINT ((max_y - min_y) / misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees));
          int32_t row = NINT ((min_y - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat) / misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees);
          int32_t column = NINT ((min_x - misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon) / misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees);

          if (column + width >= misc->chrtr2_share->chrtr2_header[chrtr2].width)
            {
              width = misc->chrtr2_share->chrtr2_header[chrtr2].width - column - 1;
              max_x = misc->chrtr2_share->chrtr2_header[chrtr2].mbr.elon;
            }
          
          if (row + height >= misc->chrtr2_share->chrtr2_header[chrtr2].height)
            {
              height = misc->chrtr2_share->chrtr2_header[chrtr2].height - row - 1;
              max_y = misc->chrtr2_share->chrtr2_header[chrtr2].mbr.nlat;
            }


          //  If width or height is less than 0 we're outside the MBR so we want to go to the next CHRTR2 file.

          if (width <= 0 || height <= 0)
            {
              start_undo_block (misc, chrtr2, 0, 0, 0, 0);
            }
          else
            {
              start_undo_block (misc, chrtr2, row, row + height - 1, column, column + width - 1);


              if (value == CHRTR2_NULL_Z_VALUE)
                {
                  string = gridEdit::tr (" Setting grid to CHRTR2_NULL for CHRTR2 %1 of %2").arg (chrtr2 + 1).arg (misc->chrtr2_share->chrtr2_count);
                }
              else if (value < -90000.0)
                {
                  string = gridEdit::tr (" Deleting hand-drawn contours in polygon for CHRTR2 %1 of %2").arg (chrtr2 + 1).arg
                    (misc->chrtr2_share->chrtr2_count);
                }
              else
                {
                  string = gridEdit::tr (" Setting grid to %1 for CHRTR2 %2 of %3").arg (value, 0, 'f', 2).arg (chrtr2 + 1).arg
                    (misc->chrtr2_share->chrtr2_count);
                }


              misc->statusProg->setRange (0, height);
              misc->statusProgLabel->setText (string);
              misc->statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::green);
              misc->statusProgLabel->setPalette (misc->statusProgPalette);
              misc->statusProg->setTextVisible (true);
              qApp->processEvents();


              //  Loop for the height of the displayed area.

              for (int32_t j = 0 ; j < height ; j++)
                {
                  misc->statusProg->setValue (j);
                  qApp->processEvents();

                  coord.y = row + j;


                  //  Compute the latitude of the center of the bin.

                  double lat = misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat + coord.y *
                    misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees + half_y;


                  //  Loop for the width of the displayed area.
    
                  for (int32_t i = 0 ; i < width ; i++)
                    {
                      coord.x = column + i;


                      //  Compute the longitude of the center of the bin.

                      double lon = misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon + coord.x *
                        misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees + half_x;


                      chrtr2_read_record_lat_lon (misc->chrtr2_handle[chrtr2], lat, lon, &chrtr2_record);


                      if (!append_undo (misc, chrtr2_record))
                        {
                          QMessageBox::critical (misc->map_widget, gridEdit::tr ("gridEdit"), gridEdit::tr ("Unable to write undo records!"));
                          remove_last_undo (misc);
                          return;
                        }


                      //  Check to see if the center of the bin is inside our polygon.

                      if (inside_polygon2 (mx, my, count, lon, lat))
                        {
                          if (value == CHRTR2_NULL_Z_VALUE)
                            {
                              memset (&chrtr2_record, 0, sizeof (CHRTR2_RECORD));

                              chrtr2_write_record_lat_lon (misc->chrtr2_handle[chrtr2], lat, lon, chrtr2_record);
                            }
                          else if (value < -90000.0)
                            {
                              if (chrtr2_record.status & CHRTR2_DIGITIZED_CONTOUR)
                                {
                                  memset (&chrtr2_record, 0, sizeof (CHRTR2_RECORD));

                                  chrtr2_write_record_lat_lon (misc->chrtr2_handle[chrtr2], lat, lon, chrtr2_record);
                                }
                            }
                          else
                            {
                              //  Don't fill areas with real data

                              if (!(chrtr2_record.status & CHRTR2_REAL))
                                {
                                  chrtr2_record.status = (chrtr2_record.status & CHRTR2_DIGITIZED_MASK) | CHRTR2_DIGITIZED_CONTOUR;
                                  chrtr2_record.z = value;

                                  chrtr2_write_record_lat_lon (misc->chrtr2_handle[chrtr2], lat, lon, chrtr2_record);
                                }
                            }
                        }
                    }
                }
            }
        }
    }


  misc->statusProg->reset ();
  misc->statusProg->setTextVisible (false);
  qApp->processEvents();
}
