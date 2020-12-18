
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
#include "pfm.h"


//  Fills an area with all data from the same area within the selected PFM file.

void fillPFM (MISC *misc, OPTIONS *options, char *fname, double *mx, double *my, int32_t count)
{
  NV_I32_COORD2      coord;
  NV_F64_XYMBR       mbr;
  QString            string;
  PFM_OPEN_ARGS      open_args;
  int32_t            pfm_handle, recnum;
  DEPTH_RECORD       *depth;


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


  strcpy (open_args.list_path, fname);
  open_args.checkpoint = 0;
  if ((pfm_handle = open_existing_pfm_file (&open_args)) < 0)
    {
      QMessageBox::warning (0, gridEdit::tr ("gridEdit PFM Data Fill"), gridEdit::tr ("Unable to open PFM file ") +
                            QDir::toNativeSeparators (QString (fname)) + gridEdit::tr ("\nReason: %1").arg (QString (pfm_error_str (pfm_error))));
      return;
    }


  int32_t x_start = 0;
  int32_t y_start = 0;
  int32_t width = open_args.head.bin_width;
  int32_t height = open_args.head.bin_height;


  if (mbr.min_y > open_args.head.mbr.max_y || mbr.max_y < open_args.head.mbr.min_y ||
      mbr.min_x > open_args.head.mbr.max_x || mbr.max_x < open_args.head.mbr.min_x)
    {
      QMessageBox::warning (0, gridEdit::tr ("gridEdit"), gridEdit::tr ("Specified area is completely outside of the PFM bounds!"));
      close_pfm_file (pfm_handle);
      return;
    }


  //  Match to nearest cell.

  x_start = NINT ((mbr.min_x - open_args.head.mbr.min_x) / open_args.head.x_bin_size_degrees);
  y_start = NINT ((mbr.min_y - open_args.head.mbr.min_y) / open_args.head.y_bin_size_degrees);
  width = NINT ((mbr.max_x - mbr.min_x) / open_args.head.x_bin_size_degrees);
  height = NINT ((mbr.max_y - mbr.min_y) / open_args.head.y_bin_size_degrees);


  //  Adjust to PFM bounds if necessary.

  if (x_start < 0) x_start = 0;
  if (y_start < 0) y_start = 0;
  if (x_start + width > open_args.head.bin_width) width = open_args.head.bin_width - x_start;
  if (y_start + height > open_args.head.bin_height) height = open_args.head.bin_height - y_start;


  //  Redefine bounds.

  mbr.min_x = open_args.head.mbr.min_x + x_start * open_args.head.x_bin_size_degrees;
  mbr.min_y = open_args.head.mbr.min_y + y_start * open_args.head.y_bin_size_degrees;
  mbr.max_x = mbr.min_x + width * open_args.head.x_bin_size_degrees;
  mbr.max_y = mbr.min_y + height * open_args.head.y_bin_size_degrees;


  for (int32_t i = y_start ; i < y_start + height ; i++)
    {
      coord.y = i;
      for (int32_t j = x_start ; j < x_start + width ; j++)
        {
          coord.x = j;

          if (!read_depth_array_index (pfm_handle, coord, &depth, &recnum))
            {
              for (int32_t k = 0 ; k < recnum ; k++)
                {
                  if (!(depth[k].validity & (PFM_INVAL | PFM_DELETED | PFM_REFERENCE)))
                    {
                      if (inside_polygon2 (mx, my, count, depth[k].xyz.x, depth[k].xyz.y))
                        {
                          try
                            {
                              misc->pfm_data.resize (misc->pfm_data_count + 1);
                            }
                          catch (std::bad_alloc&)
                            {
                              QMessageBox::critical (0, gridEdit::tr ("gridEdit PFM Data Fill"),
                                                     gridEdit::tr ("Unable to allocate misc->pfm_data in fillPFM.cpp"));
                              exit (-1);
                            }

                          misc->pfm_data[misc->pfm_data_count] = depth[k].xyz;
                          misc->pfm_data_count++;
                        }
                    }
                }
            }
        }
    }

  close_pfm_file (pfm_handle);


  remisp (misc, options, &mbr, NVTrue);
}
