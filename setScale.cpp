
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

void setScale (float min_z, float max_z, float range, int32_t attribute, MISC *misc, OPTIONS *options, uint8_t min_lock, uint8_t max_lock)
{
  //  Set the scale colors for the current range.

  int32_t numhues = NUMHUES - 1;
  float inc = 1.0;

  inc = range / (float) (NUM_SCALE_LEVELS - 1);


  for (int32_t i = 0 ; i < NUM_SCALE_LEVELS ; i++)
    {
      float value = min_z + (float) i * inc;
      int32_t h_index = 0;


      //  Set the HSV fill color.

      if (attribute)
        {
          h_index = (int32_t) (numhues - fabs ((value - max_z) / range * numhues));
        }
      else
        {
          if (options->zero_turnover && min_z < 0.0 && max_z >= 0.0)
            {
              if (value <= 0.0)
                {
                  h_index = (int32_t) (numhues - fabs ((value - min_z) / (-min_z) * numhues));
                }
              else
                {
                  h_index = (int32_t) (numhues - fabs (value / max_z * numhues));
                }
            }
          else
            {
              h_index = (int32_t) (numhues - fabs ((value - min_z) / range * numhues));
            }
        }


      //  If we've locked the first or last scaleBox, set the flag to draw the lock image.

      int32_t lock = 0;

      if (!i)
        {
          if (min_lock)
            {
              lock = 1;
            }
          else
            {
              lock = -1;
            }
        }

      if (i == NUM_SCALE_LEVELS - 1)
        {
          if (max_lock)
            {
              lock = 1;
            }
          else
            {
              lock = -1;
            }
        }


      if (attribute)
        {
          misc->scale[(NUM_SCALE_LEVELS - 1) - i]->setContents (options->color_array[0][h_index][NUMSHADES - 1], value, lock);
        }
      else
        {
          misc->scale[i]->setContents (options->color_array[0][h_index][NUMSHADES - 1], value, lock);
        }
    }
}
