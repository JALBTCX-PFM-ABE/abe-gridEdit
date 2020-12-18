
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


//  Sets an area checked or unchecked.  That is, it sets or unsets the CHRTR2_CHECKED status flag.

void setAreaChecked (MISC *misc, int32_t type, uint8_t area)
{
  int32_t             i, j, width, height, row, column;
  NV_I32_COORD2       coord;
  QString             string;
  CHRTR2_RECORD       chrtr2_record;


  for (int32_t chrtr2 = 0 ; chrtr2 < misc->chrtr2_share->chrtr2_count ; chrtr2++)
    {
      //  Only if we want to display it and it's not locked.

      if (misc->chrtr2_share->display_chrtr2[chrtr2] && !misc->chrtr2_share->lock_chrtr2[chrtr2])
        {
          //  Adjust bounds to nearest grid point for setting displayed area, otherwise do entire file(s).

          if (area)
            {
              adjust_bounds (misc, chrtr2);
              row = misc->displayed_area_row[chrtr2];
              column = misc->displayed_area_column[chrtr2];
              width = misc->displayed_area_width[chrtr2];
              height = misc->displayed_area_height[chrtr2];
            }
          else
            {
              row = 0;
              column = 0;
              width = misc->chrtr2_share->chrtr2_header[chrtr2].width;            
              height = misc->chrtr2_share->chrtr2_header[chrtr2].height;             
            }


          switch (type)
            {
            case 0:
              string = gridEdit::tr (" Setting bin flags to CHECKED for CHRTR2 %1 of %2").arg (chrtr2 + 1).arg (misc->chrtr2_share->chrtr2_count);
              break;

            case 1:
              string = gridEdit::tr (" Setting bin flags to UNCHECKED for CHRTR2 %1 of %2").arg (chrtr2 + 1).arg (misc->chrtr2_share->chrtr2_count);
              break;
            }


          misc->statusProg->setRange (0, height);
          misc->statusProgLabel->setText (string);
          misc->statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::green);
          misc->statusProgLabel->setPalette (misc->statusProgPalette);
          misc->statusProg->setTextVisible (true);
          qApp->processEvents();


          //  Loop for the height of the displayed area.

          for (j = 0 ; j < height ; j++)
            {
              misc->statusProg->setValue (j);
              qApp->processEvents();

              coord.y = row + j;


              //  Loop for the width of the displayed area.
    
              for (i = 0 ; i < width ; i++)
                {
                  coord.x = column + i;


                  chrtr2_read_record (misc->chrtr2_handle[chrtr2], coord, &chrtr2_record);


                  //  To check or not to check, that is the question.

                  switch (type)
                    {
                    case 0:
                      chrtr2_record.status |= CHRTR2_CHECKED;
                      break;

                    case 1:
                      chrtr2_record.status &= ~CHRTR2_CHECKED;
                      break;
                    }


                  chrtr2_write_record (misc->chrtr2_handle[chrtr2], coord, chrtr2_record);
                }
            }
        }
    }


  misc->statusProg->reset ();
  misc->statusProg->setTextVisible (false);
  qApp->processEvents();
}
