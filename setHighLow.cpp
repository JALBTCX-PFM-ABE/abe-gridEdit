
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


//  Outputs a point at the lowest or highest point in the supplied rectangle.

uint8_t setHighLow (double *mx, double *my, MISC *misc, OPTIONS *options, nvMap *map)
{
  NV_F64_XYMBR        bounds;


  bounds.min_x = 999999.0;
  bounds.min_y = 999999.0;
  bounds.max_x = -999999.0;
  bounds.max_y = -999999.0;

  for (int32_t i = 0 ; i < 4 ; i++)
    {
      if (misc->dateline && mx[i] < 0.0) mx[i] += 360.0;

      if (mx[i] < bounds.min_x) bounds.min_x = mx[i];
      if (my[i] < bounds.min_y) bounds.min_y = my[i];
      if (mx[i] > bounds.max_x) bounds.max_x = mx[i];
      if (my[i] > bounds.max_y) bounds.max_y = my[i];
    }


  int32_t width = (NINT ((bounds.max_x - bounds.min_x) / misc->chrtr2_share->chrtr2_header[0].lon_grid_size_degrees));
  int32_t height = (NINT ((bounds.max_y - bounds.min_y) / misc->chrtr2_share->chrtr2_header[0].lat_grid_size_degrees));
  int32_t row = NINT ((bounds.min_y - misc->chrtr2_share->chrtr2_header[0].mbr.slat) / misc->chrtr2_share->chrtr2_header[0].lat_grid_size_degrees);
  int32_t column = NINT ((bounds.min_x - misc->chrtr2_share->chrtr2_header[0].mbr.wlon) / misc->chrtr2_share->chrtr2_header[0].lon_grid_size_degrees);


  CHRTR2_RECORD chrtr2_record, min_bin, max_bin;

  memset (&min_bin, 0, sizeof (CHRTR2_RECORD));
  memset (&max_bin, 0, sizeof (CHRTR2_RECORD));


  //  Loop for the height of the rectangle.

  float min_val = 999999.0;
  float max_val = -999999.0;
  NV_I32_COORD2 min_coord = {0, 0}, max_coord = {0, 0}, coord = {0, 0};

  for (int32_t i = 0 ; i < height ; i++)
    {
      //  Loop for the width of the rectangle.
    
      for (int32_t j = 0 ; j < width ; j++)
        {
          chrtr2_read_record_row_col (misc->chrtr2_handle[0], row + i, column + j, &chrtr2_record);


          if (chrtr2_record.status & CHRTR2_REAL)
            {
              if (chrtr2_record.z < min_val)
                {
                  min_coord.x = column + j;
                  min_coord.y = row + i;
                  min_val = chrtr2_record.z;
                  min_bin = chrtr2_record;
                }

              if (chrtr2_record.z > max_val)
                {
                  max_coord.x = column + j;
                  max_coord.y = row + i;
                  max_val = chrtr2_record.z;
                  max_bin = chrtr2_record;
                }
            }
        }
    }


  QString question, depth_string;
  char type[20];


  switch (misc->function)
    {
    case SELECT_HIGH_POINT:
      chrtr2_record = min_bin;
      coord = min_coord;
      strcpy (type, "High");
      depth_string.sprintf ("High point :: %.2f ", min_bin.z * options->z_factor + options->z_offset);
      question = gridEdit::tr ("Do you wish to accept the high point?");
      break;

    case SELECT_LOW_POINT:
      chrtr2_record = max_bin;
      coord = max_coord;
      strcpy (type, "Low");
      depth_string.sprintf ("Low point :: %.2f ", max_bin.z * options->z_factor + options->z_offset);
      question = gridEdit::tr ("Do you wish to accept the low point?");
      break;
    }


  double lat, lon;

  chrtr2_get_lat_lon (misc->chrtr2_handle[0], &lat, &lon, coord);


  int32_t x, y, z;
  double dz = 0.0;
  int32_t mvc = -1, mvt = -1;
  map->map_to_screen (1, &lon, &lat, &dz, &x, &y, &z);
  map->setMovingCircle (&mvc, x, y, 12, 0.0, 360.0, LINE_WIDTH, options->contour_color, NVFalse, Qt::SolidLine);
  map->setMovingText (&mvt, depth_string, x + 6, y + 3, options->contour_color, 90.0, 12);
  map->flush ();


  QMessageBox msgBox (map);
  msgBox.setIcon (QMessageBox::Question);
  msgBox.setInformativeText (question);
  msgBox.setStandardButtons (QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton (QMessageBox::Yes);
  int32_t ret = msgBox.exec ();

  if (ret == QMessageBox::Yes)
    {
      //  If there was no high/low file, create one.

      QString name = misc->chrtr2_share->chrtr2_file[0];
      name += ".hilo";


      char filename[512];

      if (!QFileInfo (name).exists ())
        {
          QFileDialog fd (map, gridEdit::tr ("gridEdit Assign High/Low File"));
          fd.setViewMode (QFileDialog::List);
          fd.setDirectory (QFileInfo (QString (misc->chrtr2_share->chrtr2_file[0])).dir ());

          fd.setNameFilter (gridEdit::tr ("gridEdit High/Low file (*.hilo)"));
          fd.setFileMode (QFileDialog::AnyFile);
          fd.selectFile (name);


          QStringList files;
          QString file;

          if (fd.exec () == QDialog::Accepted)
            {
              files = fd.selectedFiles ();

              file = files.at (0);

              if (file.isEmpty()) return (NVFalse);


              if (!file.endsWith (".hilo")) file.append (".hilo");


              //  Check for a pre-existing file

              strcpy (filename, file.toLatin1 ());

              if ((misc->hilo_fp = fopen (filename, "r+")) == NULL)
                {
                  if ((misc->hilo_fp = fopen (filename, "w+")) == NULL)
                    {
                      QString msg = QString (strerror (errno));
                      QMessageBox::warning (map, gridEdit::tr ("gridEdit"), gridEdit::tr ("Unable to create high/low file\nReason: ") + msg);
                      map->closeMovingCircle (&mvc);
                      map->closeMovingText (&mvt);
                      return (NVFalse);
                    }
                }
            }
        }
      else
        {
          strcpy (filename, name.toLatin1 ());

          if ((misc->hilo_fp = fopen (filename, "r+")) == NULL)
            {
              if ((misc->hilo_fp = fopen (filename, "w+")) == NULL)
                {
                  QString msg = QString (strerror (errno));
                  QMessageBox::warning (map, gridEdit::tr ("gridEdit"), gridEdit::tr ("Unable to open high/low file\nReason: ") + msg);
                  map->closeMovingCircle (&mvc);
                  map->closeMovingText (&mvt);
                  return (NVFalse);
                }
            }
        }


      map->closeMovingCircle (&mvc);
      map->closeMovingText (&mvt);

      map->drawCircle (x, y, 12, 0.0, 360.0, LINE_WIDTH, options->contour_color, Qt::SolidLine, NVFalse);
      map->drawText (depth_string, x + 6, y + 3, options->contour_color, NVFalse);
      map->flush ();


      //  Append a record to the file.

      fseek (misc->hilo_fp, 0, SEEK_END);
      fprintf (misc->hilo_fp, "%.9f %.9f %.2f %s\n", lat, lon, chrtr2_record.z * options->z_factor + options->z_offset, type);
      fclose (misc->hilo_fp);
    }
  else
    {
      map->closeMovingCircle (&mvc);
      map->closeMovingText (&mvt);
      return (NVFalse);
    }

  return (NVTrue);
}
