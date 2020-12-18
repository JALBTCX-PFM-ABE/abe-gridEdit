
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


//  Computes the minimum and maximum X, Y, and Z for all of the displayed layers (CHRTR2 files).

uint8_t compute_layer_min_max (MISC *misc, OPTIONS *options)
{
  void adjust_bounds (MISC *misc, int32_t chrtr2);
  void setScale (float min_z, float max_z, float range, int32_t attribute, MISC *misc, OPTIONS *options, uint8_t min_lock, uint8_t max_lock);



  misc->displayed_area_min = 999999999.0;
  misc->displayed_area_max = -999999999.0;
  misc->displayed_area_attr_min = 999999999.0;
  misc->displayed_area_attr_max = -999999999.0;


  int32_t attribute = 0;
  uint8_t min_lock = NVFalse, max_lock = NVFalse;


  //  Pre-read the data to get the min and max (since most disk systems cache this isn't much of a performance hit).
  //  The loop runs from max to min so that we end up with the final bounds defined by CHRTR2 layer 0.

  for (int32_t chrtr2 = misc->chrtr2_share->chrtr2_count - 1 ; chrtr2 >= 0 ; chrtr2--)
    {
      //  Don't use it to compute min/max if we're not displaying it.

      if (misc->chrtr2_share->display_chrtr2[chrtr2])
        {
          //  Adjust input bounds to nearest grid point.

          adjust_bounds (misc, chrtr2);


	  //  If the width or height is 0 or negative we have asked for an area outside of the CHRTR2's MBR so we don't want to 
	  //  do anything.

	  if (misc->displayed_area_width[chrtr2] > 0 && misc->displayed_area_height[chrtr2] > 0)
	    {
	      //  We only want to do attributes if we're coloring by horizontal or vertical uncertainty.

	      if (misc->color_by_attribute) attribute = misc->color_by_attribute;


	      //  Allocate the needed arrays.

	      CHRTR2_RECORD *current_record = (CHRTR2_RECORD *) calloc (misc->displayed_area_width[chrtr2], sizeof (CHRTR2_RECORD));
	      if (current_record == NULL)
                {
                  perror (gridEdit::tr ("Allocating current_record in compute_layer_min_max").toLatin1 ());
                  exit (-1);
                }

	      misc->current_row = (float *) calloc (misc->displayed_area_width[chrtr2], sizeof (float));
	      if (misc->current_row == NULL)
                {
                  perror (gridEdit::tr ("Allocating current_row in compute_layer_min_max").toLatin1 ());
                  exit (-1);
                }

	      if (attribute)
                {
                  misc->current_attr = (float *) calloc (misc->displayed_area_width[chrtr2], sizeof (float));
                  if (misc->current_attr == NULL)
                    {
                      perror (gridEdit::tr ("Allocating current_attr in compute_layer_min_max").toLatin1 ());
                      exit (-1);
                    }
                }

	      misc->current_flags = (uint8_t *) calloc (misc->displayed_area_width[chrtr2], sizeof (char));
              if (misc->current_flags == NULL)
                {
                  perror (gridEdit::tr ("Allocating current_flags in compute_layer_min_max").toLatin1 ());
                  exit (-1);
                }


              misc->statusProg->setRange (0, misc->displayed_area_height[chrtr2]);
              QString title = gridEdit::tr (" Loading %1 of %2 : ").arg (misc->chrtr2_share->chrtr2_count - chrtr2).arg (misc->chrtr2_share->chrtr2_count) +
                QFileInfo (QString (misc->chrtr2_share->chrtr2_file[chrtr2])).fileName () + " ";
              misc->statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::green);
              misc->statusProgLabel->setPalette (misc->statusProgPalette);
              misc->statusProgLabel->setText (title);
              misc->statusProg->setTextVisible (true);
              qApp->processEvents();


              //  We only want to update the progress bar at about 20% increments.  This makes things go
              //  marginally faster.

              int32_t prog_inc = misc->displayed_area_height[chrtr2] / 5;
              if (!prog_inc) prog_inc = 1;


              for (int32_t i = 0 ; i < misc->displayed_area_height[chrtr2] ; i++)
                {
                  if (!(i % prog_inc))
                    {
                      misc->statusProg->setValue (i);
                      qApp->processEvents();
                    }


                  chrtr2_read_row (misc->chrtr2_handle[chrtr2], misc->displayed_area_row[chrtr2] + i, misc->displayed_area_column[chrtr2],
                                   misc->displayed_area_width[chrtr2], current_record);


                  loadArrays (misc->displayed_area_width[chrtr2], current_record, misc->current_row, misc->current_attr, attribute,
                              misc->current_flags, options->highlight, 0.0, 0.0, options->display_interpolated);


                  for (int32_t j = 0 ; j < misc->displayed_area_width[chrtr2] ; j++)
                    {
                      if (current_record[j].status)
                        {
                          if (misc->current_row[j] < CHRTR2_NULL_Z_VALUE)
                            {
                              //  Since the user may have turned on display of minimum and maximum we're going to save these so
                              //  we don't have to read through the data again to find the locations.

                              if (misc->current_row[j] < misc->displayed_area_min)
                                {
                                  misc->displayed_area_min = misc->current_row[j];
                                  misc->displayed_area_min_coord.x = misc->displayed_area_column[chrtr2] + j;
                                  misc->displayed_area_min_coord.y = misc->displayed_area_row[chrtr2] + i;
                                  misc->displayed_area_min_chrtr2 = chrtr2;
                                }

                              if (misc->current_row[j] > misc->displayed_area_max) 
                                {
                                  misc->displayed_area_max = misc->current_row[j];
                                  misc->displayed_area_max_coord.x = misc->displayed_area_column[chrtr2] + j;
                                  misc->displayed_area_max_coord.y = misc->displayed_area_row[chrtr2] + i;
                                  misc->displayed_area_max_chrtr2 = chrtr2;
                                }


                              if (attribute)
                                {
                                  misc->displayed_area_attr_min = qMin (misc->current_attr[j], misc->displayed_area_attr_min);
                                  misc->displayed_area_attr_max = qMax (misc->current_attr[j], misc->displayed_area_attr_max);
                                }
                            }
                        }
                    }
                }


              misc->statusProg->setValue (misc->displayed_area_height[chrtr2]);
              qApp->processEvents();


              //  If the min or max hsv lock is set we need to check to see if we want to use the locked value(s).

              float *min_z = &misc->displayed_area_min, *max_z = &misc->displayed_area_max;

              if (attribute)
                {
                  min_z = &misc->displayed_area_attr_min;
                  max_z = &misc->displayed_area_attr_max;
                }


              if (options->min_hsv_locked[attribute])
                {
                  if (*min_z < options->min_hsv_value[attribute])
                    {
                      *min_z = options->min_hsv_value[attribute];
                      min_lock = NVTrue;
                    }
                }

              if (options->max_hsv_locked[attribute])
                {
                  if (*max_z > options->max_hsv_value[attribute])
                    {
                      *max_z = options->max_hsv_value[attribute];
                      max_lock = NVTrue;
                    }
                }

	      misc->displayed_area_range = misc->displayed_area_max - misc->displayed_area_min;
	      if (misc->displayed_area_range == 0.0) misc->displayed_area_range = 1.0;


	      if (attribute)
                {
                  misc->displayed_area_attr_range = misc->displayed_area_attr_max - misc->displayed_area_attr_min;
                  if (misc->displayed_area_attr_range == 0.0) misc->displayed_area_attr_range = 1.0;
                }


	      //  Free allocated memory.

	      free (current_record);
	      free (misc->current_row);
	      if (attribute) free (misc->current_attr);
	      free (misc->current_flags);
            }
        }
    }


  //  Set the scale colors for the current range.

  if (attribute)
    {
      setScale (misc->displayed_area_attr_min, misc->displayed_area_attr_max, misc->displayed_area_attr_range, attribute, misc, options, min_lock, max_lock);
    }
  else
    {
      setScale (misc->displayed_area_min, misc->displayed_area_max, misc->displayed_area_range, attribute, misc, options, min_lock, max_lock);
    }


  misc->statusProg->reset ();
  misc->statusProg->setTextVisible (false);
  qApp->processEvents();


  return (NVTrue);
}
