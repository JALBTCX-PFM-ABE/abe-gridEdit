
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



#include <stdlib.h>
#include "nvutility.h"
#include "chrtr2.h"
#include "gridEditDef.hpp"

/***************************************************************************\
*                                                                           *
*   Module Name:        loadArrays                                          *
*                                                                           *
*   Programmer:         Jan C. Depner                                       *
*                                                                           *
*   Date Written:       November 1999                                       *
*                                                                           *
*   Purpose:            Loads the selected field and the checked flag into  *
*                       the display arrays.                                 *
*                                                                           *
\***************************************************************************/

void loadArrays (int32_t count, CHRTR2_RECORD chrtr2_record[], float data[], float attr[], int32_t attr_num, uint8_t flags[], int32_t highlight,
                 float min_depth, float max_depth, uint8_t display_interpolated)
{
  for (int32_t i = 0 ; i < count ; i++)
    {
      //  Default to null attribute value.

      if (attr_num) attr[i] = 0.0;


      //  Do we have any values in the bin (if we don't want to display interpolated data we need to bypass this if the bin is interpolated).

      if (chrtr2_record[i].status && (display_interpolated || !(chrtr2_record[i].status & CHRTR2_INTERPOLATED)))
        {
          data[i] = chrtr2_record[i].z;


	  switch (attr_num)
	    {
	    case 1:
	      attr[i] = (float) chrtr2_record[i].number_of_points;
	      break;

	    case 2:
	      attr[i] = chrtr2_record[i].uncertainty;
	      break;

	    case 3:
	      attr[i] = chrtr2_record[i].horizontal_uncertainty;
	      break;

	    case 4:
	      attr[i] = chrtr2_record[i].vertical_uncertainty;
	      break;
	    }


          //  Fill the flags array for highlighting.

          flags[i] = 0;
          switch (highlight)
            {
            case H_ALL:
              flags[i] = 1;
              break;


            case H_01:
              if (chrtr2_record[i].status & CHRTR2_USER_01) flags[i] = 1;
              break;


            case H_02:
              if (chrtr2_record[i].status & CHRTR2_USER_02) flags[i] = 1;
              break;


            case H_03:
              if (chrtr2_record[i].status & CHRTR2_USER_03) flags[i] = 1;
              break;


            case H_04:
              if (chrtr2_record[i].status & CHRTR2_USER_04) flags[i] = 1;
              break;


            case H_LAND_MASK:
              if (chrtr2_record[i].status & CHRTR2_LAND_MASK) flags[i] = 1;
              break;


            case H_INT:
              if (chrtr2_record[i].status & CHRTR2_INTERPOLATED) flags[i] = 1;
              break;


            case H_INSIDE:
              if (data[i] >= min_depth && data[i] <= max_depth) flags[i] = 1;
              break;


            case H_OUTSIDE:
              if (data[i] <= min_depth || data[i] >= max_depth) flags[i] = 1;
              break;

            case H_CHECKED:
              if (chrtr2_record[i].status & CHRTR2_CHECKED) flags[i] = 1;
              break;
            }
        }


      //  Empty bins need the null value.

      else
        {
          data[i] = CHRTR2_NULL_Z_VALUE;
          flags[i] = 0;
        }
    }
}
