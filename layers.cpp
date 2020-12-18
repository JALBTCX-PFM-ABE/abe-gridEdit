
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



#include "layers.hpp"


//  CHRTR2 layer handling functions.

void swap_layers (MISC *misc, int32_t l1, int32_t l2)
{
  //  Save the information for the first layer

  CHRTR2_HEADER chrtr2_header = misc->chrtr2_share->chrtr2_header[l1];
  uint8_t display_chrtr2 = misc->chrtr2_share->display_chrtr2[l1];
  uint8_t lock_chrtr2 = misc->chrtr2_share->lock_chrtr2[l1];
  char chrtr2_file[512];
  strcpy (chrtr2_file, misc->chrtr2_share->chrtr2_file[l1]);
  int32_t chrtr2_handle = misc->chrtr2_handle[l1];

  double ss_cell_size_x = misc->ss_cell_size_x[l1];
  double ss_cell_size_y = misc->ss_cell_size_y[l1];
  int32_t chrtr2_alpha = misc->chrtr2_alpha[l1];


  //  Replace first layer with second layer

  misc->chrtr2_share->chrtr2_header[l1] = misc->chrtr2_share->chrtr2_header[l2];
  misc->chrtr2_share->display_chrtr2[l1] = misc->chrtr2_share->display_chrtr2[l2];
  misc->chrtr2_share->lock_chrtr2[l1] = misc->chrtr2_share->lock_chrtr2[l2];
  strcpy (misc->chrtr2_share->chrtr2_file[l1], misc->chrtr2_share->chrtr2_file[l2]);
  misc->chrtr2_handle[l1] = misc->chrtr2_handle[l2];

  misc->ss_cell_size_x[l1] = misc->ss_cell_size_x[l2];
  misc->ss_cell_size_y[l1] = misc->ss_cell_size_y[l2];
  misc->chrtr2_alpha[l1] = misc->chrtr2_alpha[l2];


  //  Replace second layer with saved first layer

  misc->chrtr2_share->chrtr2_header[l2] = chrtr2_header;
  misc->chrtr2_share->display_chrtr2[l2] = display_chrtr2;
  misc->chrtr2_share->lock_chrtr2[l2] = lock_chrtr2;
  strcpy (misc->chrtr2_share->chrtr2_file[l2], chrtr2_file);
  misc->chrtr2_handle[l2] = chrtr2_handle;

  misc->ss_cell_size_x[l2] = ss_cell_size_x;
  misc->ss_cell_size_y[l2] = ss_cell_size_y;
  misc->chrtr2_alpha[l2] = chrtr2_alpha;
}



void remove_layer (MISC *misc, int32_t l1)
{
  chrtr2_close_file (misc->chrtr2_handle[l1]);


  //  Move everything below the selected layer up one level

  for (int32_t chrtr2 = l1 ; chrtr2 < misc->chrtr2_share->chrtr2_count - 1 ; chrtr2++)
    {
      misc->chrtr2_share->chrtr2_header[chrtr2] = misc->chrtr2_share->chrtr2_header[chrtr2 + 1];
      misc->chrtr2_share->display_chrtr2[chrtr2] = misc->chrtr2_share->display_chrtr2[chrtr2 + 1];
      misc->chrtr2_share->lock_chrtr2[chrtr2] = misc->chrtr2_share->lock_chrtr2[chrtr2 + 1];
      strcpy (misc->chrtr2_share->chrtr2_file[chrtr2], misc->chrtr2_share->chrtr2_file[chrtr2 + 1]);
      misc->chrtr2_handle[chrtr2] = misc->chrtr2_handle[chrtr2 + 1];

      misc->ss_cell_size_x[chrtr2] = misc->ss_cell_size_x[chrtr2 + 1];
      misc->ss_cell_size_y[chrtr2] = misc->ss_cell_size_y[chrtr2 + 1];
      misc->chrtr2_alpha[chrtr2] = misc->chrtr2_alpha[chrtr2 + 1];
    }

  misc->chrtr2_share->chrtr2_count--;
}



void move_layer_to_top (MISC *misc, int32_t l1)
{
  //  Save the information for the layer we selected

  CHRTR2_HEADER chrtr2_header = misc->chrtr2_share->chrtr2_header[l1];
  uint8_t display_chrtr2 = misc->chrtr2_share->display_chrtr2[l1];
  uint8_t lock_chrtr2 = misc->chrtr2_share->lock_chrtr2[l1];
  char chrtr2_file[512];
  strcpy (chrtr2_file, misc->chrtr2_share->chrtr2_file[l1]);
  int32_t chrtr2_handle = misc->chrtr2_handle[l1];

  double ss_cell_size_x = misc->ss_cell_size_x[l1];
  double ss_cell_size_y = misc->ss_cell_size_y[l1];
  int32_t chrtr2_alpha = misc->chrtr2_alpha[l1];


  //  Move everything above the selected layer down one level

  for (int32_t chrtr2 = l1 ; chrtr2 > 0 ; chrtr2--)
    {
      misc->chrtr2_share->chrtr2_header[chrtr2] = misc->chrtr2_share->chrtr2_header[chrtr2 - 1];
      misc->chrtr2_share->display_chrtr2[chrtr2] = misc->chrtr2_share->display_chrtr2[chrtr2 - 1];
      misc->chrtr2_share->lock_chrtr2[chrtr2] = misc->chrtr2_share->lock_chrtr2[chrtr2 - 1];
      strcpy (misc->chrtr2_share->chrtr2_file[chrtr2], misc->chrtr2_share->chrtr2_file[chrtr2 - 1]);
      misc->chrtr2_handle[chrtr2] = misc->chrtr2_handle[chrtr2 - 1];

      misc->ss_cell_size_x[chrtr2] = misc->ss_cell_size_x[chrtr2 - 1];
      misc->ss_cell_size_y[chrtr2] = misc->ss_cell_size_y[chrtr2 - 1];
      misc->chrtr2_alpha[chrtr2] = misc->chrtr2_alpha[chrtr2 - 1];
    }


  //  Set the 0 layer to be the selected layer

  misc->chrtr2_share->chrtr2_header[0] = chrtr2_header;
  misc->chrtr2_share->display_chrtr2[0] = display_chrtr2;
  misc->chrtr2_share->lock_chrtr2[0] = lock_chrtr2;
  strcpy (misc->chrtr2_share->chrtr2_file[0], chrtr2_file);
  misc->chrtr2_handle[0] = chrtr2_handle;

  misc->ss_cell_size_x[0] = ss_cell_size_x;
  misc->ss_cell_size_y[0] = ss_cell_size_y;
  misc->chrtr2_alpha[0] = chrtr2_alpha;
}



void move_layer_to_bottom (MISC *misc, int32_t l1)
{
  //  Save the information for the layer we selected

  CHRTR2_HEADER chrtr2_header = misc->chrtr2_share->chrtr2_header[l1];
  uint8_t display_chrtr2 = misc->chrtr2_share->display_chrtr2[l1];
  uint8_t lock_chrtr2 = misc->chrtr2_share->lock_chrtr2[l1];
  char chrtr2_file[512];
  strcpy (chrtr2_file, misc->chrtr2_share->chrtr2_file[l1]);
  int32_t chrtr2_handle = misc->chrtr2_handle[l1];

  double ss_cell_size_x = misc->ss_cell_size_x[l1];
  double ss_cell_size_y = misc->ss_cell_size_y[l1];
  int32_t chrtr2_alpha = misc->chrtr2_alpha[l1];


  //  Move everything below the selected layer up one level

  for (int32_t chrtr2 = l1 ; chrtr2 < misc->chrtr2_share->chrtr2_count - 1 ; chrtr2++)
    {
      misc->chrtr2_share->chrtr2_header[chrtr2] = misc->chrtr2_share->chrtr2_header[chrtr2 + 1];
      misc->chrtr2_share->display_chrtr2[chrtr2] = misc->chrtr2_share->display_chrtr2[chrtr2 + 1];
      misc->chrtr2_share->lock_chrtr2[chrtr2] = misc->chrtr2_share->lock_chrtr2[chrtr2 + 1];
      strcpy (misc->chrtr2_share->chrtr2_file[chrtr2], misc->chrtr2_share->chrtr2_file[chrtr2 + 1]);
      misc->chrtr2_handle[chrtr2] = misc->chrtr2_handle[chrtr2 + 1];

      misc->ss_cell_size_x[chrtr2] = misc->ss_cell_size_x[chrtr2 + 1];
      misc->ss_cell_size_y[chrtr2] = misc->ss_cell_size_y[chrtr2 + 1];
      misc->chrtr2_alpha[chrtr2] = misc->chrtr2_alpha[chrtr2 + 1];
    }


  //  Set the bottom [misc->abe_share->chrtr2_count - 1] layer to be the selected layer

  misc->chrtr2_share->chrtr2_header[misc->chrtr2_share->chrtr2_count - 1] = chrtr2_header;
  misc->chrtr2_share->display_chrtr2[misc->chrtr2_share->chrtr2_count - 1] = display_chrtr2;
  misc->chrtr2_share->lock_chrtr2[misc->chrtr2_share->chrtr2_count - 1] = lock_chrtr2;
  strcpy (misc->chrtr2_share->chrtr2_file[misc->chrtr2_share->chrtr2_count - 1], chrtr2_file);
  misc->chrtr2_handle[misc->chrtr2_share->chrtr2_count - 1] = chrtr2_handle;

  misc->ss_cell_size_x[misc->chrtr2_share->chrtr2_count - 1] = ss_cell_size_x;
  misc->ss_cell_size_y[misc->chrtr2_share->chrtr2_count - 1] = ss_cell_size_y;
  misc->chrtr2_alpha[misc->chrtr2_share->chrtr2_count - 1] = chrtr2_alpha;
}
