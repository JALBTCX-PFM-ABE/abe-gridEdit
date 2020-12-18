
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



#include "gridEditDef.hpp"


//  Computes the minimum bounding rectangle (MBR) for all of the CHRTR2 areas combined.

void compute_total_mbr (MISC *misc)
{
  //  Check for dateline crossing.

  for (int32_t chrtr2 = 0 ; chrtr2 < misc->chrtr2_share->chrtr2_count ; chrtr2++)
    {
      if (misc->chrtr2_share->chrtr2_header[chrtr2].mbr.elon > 180.0 && misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon < 180.0)
        {
          misc->dateline = NVTrue;
          break;
        }
    }


  misc->total_mbr.min_y = 999.0;
  misc->total_mbr.min_x = 999.0;
  misc->total_mbr.max_y = -999.0;
  misc->total_mbr.max_x = -999.0;

  int32_t max_x_bin_chrtr2 = 0, max_y_bin_chrtr2 = 0;
  double max_x_bin = 0.0, max_y_bin = 0.0;

  for (int32_t chrtr2 = 0 ; chrtr2 < misc->chrtr2_share->chrtr2_count ; chrtr2++)
    {
      if (misc->dateline && misc->chrtr2_share->chrtr2_header[chrtr2].mbr.elon < 0.0) misc->chrtr2_share->chrtr2_header[chrtr2].mbr.elon += 360.0;

      misc->total_mbr.min_y = qMin (misc->total_mbr.min_y, misc->chrtr2_share->chrtr2_header[chrtr2].mbr.slat);
      misc->total_mbr.min_x = qMin (misc->total_mbr.min_x, misc->chrtr2_share->chrtr2_header[chrtr2].mbr.wlon);
      misc->total_mbr.max_y = qMax (misc->total_mbr.max_y, misc->chrtr2_share->chrtr2_header[chrtr2].mbr.nlat);
      misc->total_mbr.max_x = qMax (misc->total_mbr.max_x, misc->chrtr2_share->chrtr2_header[chrtr2].mbr.elon);

      if (max_x_bin < misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees)
	{
	  max_x_bin_chrtr2 = chrtr2;
	  max_x_bin = misc->chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees;
	}

      if (max_y_bin < misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees)
	{
	  max_y_bin_chrtr2 = chrtr2;
	  max_y_bin = misc->chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees;
	}
    }


  //  Add 5 grid points as a border around the total MBR.  This gives us a bit of leeway on the borders.

  misc->total_mbr.min_y -= 5.0 * misc->chrtr2_share->chrtr2_header[max_y_bin_chrtr2].lat_grid_size_degrees;
  misc->total_mbr.min_x -= 5.0 * misc->chrtr2_share->chrtr2_header[max_x_bin_chrtr2].lon_grid_size_degrees;
  misc->total_mbr.max_y += 5.0 * misc->chrtr2_share->chrtr2_header[max_y_bin_chrtr2].lat_grid_size_degrees;
  misc->total_mbr.max_x += 5.0 * misc->chrtr2_share->chrtr2_header[max_x_bin_chrtr2].lon_grid_size_degrees;


  //  Adjust the displayed area if it's outside the total MBR bounds.

  if (misc->total_displayed_area.min_y < misc->total_mbr.min_y) misc->total_displayed_area.min_y = misc->total_mbr.min_y;
  if (misc->total_displayed_area.min_x < misc->total_mbr.min_x) misc->total_displayed_area.min_x = misc->total_mbr.min_x;
  if (misc->total_displayed_area.max_y > misc->total_mbr.max_y) misc->total_displayed_area.max_y = misc->total_mbr.max_y;
  if (misc->total_displayed_area.max_x > misc->total_mbr.max_x) misc->total_displayed_area.max_x = misc->total_mbr.max_x;
}
