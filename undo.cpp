
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



#include "undo.hpp"

uint8_t create_undo (MISC *misc)
{
  static int32_t file_count = 0;

  if (misc->undo_count == MAX_UNDO)
    {
      fclose (misc->undo[0].fp);
      remove (misc->undo[0].filename);


      for (int32_t i = 0 ; i < misc->undo_count ; i++)
        {
          strcpy (misc->undo[i].filename, misc->undo[i + 1].filename);
          misc->undo[i].fp  = misc->undo[i + 1].fp;

          for (int32_t j = 0 ; j < misc->chrtr2_share->chrtr2_count ; j++)
            {
              misc->undo[i].start[j].x = misc->undo[i + 1].start[j].x;
              misc->undo[i].start[j].y = misc->undo[i + 1].start[j].y;
              misc->undo[i].end[j].x = misc->undo[i + 1].end[j].x;
              misc->undo[i].end[j].y = misc->undo[i + 1].end[j].y;
              misc->undo[i].height[j] = misc->undo[i + 1].height[j];
              misc->undo[i].width[j] = misc->undo[i + 1].width[j];
            }
        }

      misc->undo_count--;
    }


  sprintf (misc->undo[misc->undo_count].filename, "%s%1cgridEdit_temp_%05d_%05d.undo", misc->temp_dir, SEPARATOR, misc->process_id, file_count);


  if ((misc->undo[misc->undo_count].fp = fopen (misc->undo[misc->undo_count].filename, "wb+")) == NULL) return (NVFalse);


  misc->undo_count++;

  file_count++;

  return (NVTrue);
}



void start_undo_block (MISC *misc, int32_t chrtr2, int32_t start_row, int32_t end_row, int32_t start_column, int32_t end_column)
{
  misc->undo[misc->undo_count - 1].start[chrtr2].x = start_column;
  misc->undo[misc->undo_count - 1].start[chrtr2].y = start_row;
  misc->undo[misc->undo_count - 1].end[chrtr2].x = end_column;
  misc->undo[misc->undo_count - 1].end[chrtr2].y = end_row;
  misc->undo[misc->undo_count - 1].height[chrtr2] = end_row - start_row + 1;
  misc->undo[misc->undo_count - 1].width[chrtr2] = end_column - start_column + 1;
}



void delete_undo (MISC *misc)
{
  for (int32_t i = 0 ; i < misc->undo_count ; i++)
    {
      fclose (misc->undo[i].fp);
      remove (misc->undo[i].filename);
    }

  misc->undo_count = 0;
}



void remove_last_undo (MISC *misc)
{
  fclose (misc->undo[misc->undo_count - 1].fp);
  remove (misc->undo[misc->undo_count - 1].filename);


  misc->undo_count--;
}



uint8_t append_undo (MISC *misc, CHRTR2_RECORD chrtr2_record)
{
  if (!fwrite (&chrtr2_record, sizeof (CHRTR2_RECORD), 1, misc->undo[misc->undo_count - 1].fp)) return (NVFalse);

  return NVTrue;
}



uint8_t append_undo_row (MISC *misc, int32_t chrtr2, CHRTR2_RECORD *chrtr2_record)
{
  if (!fwrite (chrtr2_record, misc->undo[misc->undo_count - 1].width[chrtr2] * sizeof (CHRTR2_RECORD), 1, misc->undo[misc->undo_count - 1].fp)) return (NVFalse);

  return NVTrue;
}



void undo (MISC *misc)
{
  if (!misc->undo_count) return;


  int32_t ndx = misc->undo_count - 1;


  fseek (misc->undo[ndx].fp, 0, SEEK_SET);

  for (int32_t chrtr2 = 0 ; chrtr2 < misc->chrtr2_share->chrtr2_count ; chrtr2++)
    {
      //  The CHRTR2 file wasn't being displayed at the time the edit was performed (i.e. it wasn't edited).

      if (misc->undo[ndx].start[chrtr2].y != misc->undo[ndx].end[chrtr2].y)
        {
          CHRTR2_RECORD *chrtr2_record = (CHRTR2_RECORD *) malloc (misc->undo[ndx].width[chrtr2] * sizeof (CHRTR2_RECORD));
          if (chrtr2_record == NULL)
            {
              fprintf(stderr, "%s %s %d %d\n",__FILE__,__FUNCTION__,__LINE__,misc->undo[ndx].width[chrtr2]);
              perror ("Allocating chrtr2_record in undo");
              exit (-1);
            }


          for (int32_t i = misc->undo[ndx].start[chrtr2].y ; i <= misc->undo[ndx].end[chrtr2].y ; i++)
            {
              if (!fread (chrtr2_record, misc->undo[ndx].width[chrtr2] * sizeof (CHRTR2_RECORD), 1, misc->undo[ndx].fp))
		{
		  fprintf (stderr, "Unable to read undo data in file %s, function %s, line %d\n", __FILE__, __FUNCTION__, __LINE__);
		}

              if (chrtr2_write_row (misc->chrtr2_handle[chrtr2], i, misc->undo[ndx].start[chrtr2].x, misc->undo[ndx].width[chrtr2], chrtr2_record))
                chrtr2_perror ();
            }

          free (chrtr2_record);
        }
    }


  fclose (misc->undo[ndx].fp);
  remove (misc->undo[ndx].filename);

  misc->undo_count--;
}
