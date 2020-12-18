
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

//  Checks the file against the recent files list, adds it or rearranges the list, returns NVTrue on success or NVFalse on error.

uint8_t check_recent (QWidget *parent, OPTIONS *options, MISC *misc, QString file, QActionGroup *recentGrp, QMenu *recentMenu, QAction **recentFileAction)
{
  //  Check to see if we already have the file in the list.

  int32_t hit = -1;
  for (int32_t i = 0 ; i < options->recent_file_count ; i++)
    {
      if (file == options->recentFile[i])
        {
          hit = i;
          break;
        }
    }


  //  We want to try to open the file and find out where it is in relation to the currently opened CHRTR2(s).
  //  If it's too far away we will issue a warning.  Note that we don't need to check if there are no files open at this point.

  if (misc->chrtr2_share->chrtr2_count)
    {
      //  First, make sure that we recompute the total MBR every time we add a file.

      compute_total_mbr (misc);


      CHRTR2_HEADER chrtr2_header;
      int32_t chrtr2_handle;
      char temp_file[512];

      strcpy (temp_file, file.toLatin1 ());

      if ((chrtr2_handle = chrtr2_open_file (temp_file, &chrtr2_header, CHRTR2_UPDATE)) < 0)
        {
          QMessageBox::warning (parent, gridEdit::tr ("gridEdit Open CHRTR2 File"), gridEdit::tr ("The file ") + 
                                QDir::toNativeSeparators (file) + 
                                gridEdit::tr (" is not a CHRTR2 file or there was an error reading the file.") +
                                gridEdit::tr ("  The error message returned was:\n\n") + QString (chrtr2_strerror ()));

          return (NVFalse);
        }

      if (chrtr2_header.z_units != CHRTR2_METERS)
        {
          QMessageBox::warning (parent, gridEdit::tr ("Open CHRTR2 File"), gridEdit::tr ("The file ") + 
                                QDir::toNativeSeparators (QString (temp_file)) + 
                                gridEdit::tr (" does not use CHRTR2_METERS units.  This is not allowed."));
          chrtr2_close_file (chrtr2_handle);
          return (NVFalse);
        }

      double min_dist = 9999999999.0, dist, az;

      
      //  Check the four corners against each of the MBR corners to find the point of closest approach.

      invgp (NV_A0, NV_B0, chrtr2_header.mbr.slat, chrtr2_header.mbr.wlon, misc->total_mbr.min_y, misc->total_mbr.min_x, &dist, &az);
      min_dist = qMin (min_dist, dist);
      invgp (NV_A0, NV_B0, chrtr2_header.mbr.nlat, chrtr2_header.mbr.wlon, misc->total_mbr.min_y, misc->total_mbr.min_x, &dist, &az);
      min_dist = qMin (min_dist, dist);
      invgp (NV_A0, NV_B0, chrtr2_header.mbr.nlat, chrtr2_header.mbr.elon, misc->total_mbr.min_y, misc->total_mbr.min_x, &dist, &az);
      min_dist = qMin (min_dist, dist);
      invgp (NV_A0, NV_B0, chrtr2_header.mbr.slat, chrtr2_header.mbr.elon, misc->total_mbr.min_y, misc->total_mbr.min_x, &dist, &az);
      min_dist = qMin (min_dist, dist);


      chrtr2_close_file (chrtr2_handle);


      //  Check the distance.  If it's more that 100 kilometers away then we probably don't want to open it.

      if (min_dist > 100000.0)
        {
          QMessageBox::warning (parent, gridEdit::tr ("gridEdit Open CHRTR2 File"), gridEdit::tr ("The file ") + QString (gen_basename (temp_file)) + 
                                gridEdit::tr (" is more than 100 kilometers away from the currently displayed area.\n\n") +
                                gridEdit::tr ("If you wish to open this file you must close the currently opened CHRTR2(s)."));

          return (NVFalse);
        }
    }


  //  If it was already in the list we want to move it to the top of the list.

  if (hit >= 0)
    {
      //  No point in moving it up if it's already at the top.

      if (!hit) return (NVTrue);


      QString saveName = options->recentFile[hit];

      for (int32_t i = hit ; i > 0 ; i--)
        {
          options->recentFile[i] = options->recentFile[i - 1];
        }
      options->recentFile[0] = saveName;
    }
  else
    {
      //  Move everything down one slot.

      for (int32_t i = MAX_RECENT - 2 ; i >= 0 ; i--)
        {
          options->recentFile[i + 1] = options->recentFile[i];
        }


      options->recentFile[0] = file;


      options->recent_file_count = qMin (MAX_RECENT, options->recent_file_count + 1);
    }


  //  Reset the Open Recent... menu entries.

  for (int32_t i = 0 ; i < MAX_RECENT ; i++) recentGrp->removeAction (recentFileAction[i]);
  recentMenu->clear ();

  for (int32_t i = 0 ; i < MAX_RECENT ; i++)
    {
      recentFileAction[i] = recentMenu->addAction (options->recentFile[i]);
      recentGrp->addAction (recentFileAction[i]);
      if (i < options->recent_file_count)
        {
          recentFileAction[i]->setVisible (true);
        }
      else
        {
          recentFileAction[i]->setVisible (false);
        }
    }


  return (NVTrue);
}
