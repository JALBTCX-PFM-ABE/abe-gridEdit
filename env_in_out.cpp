
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


/*!
  These functions store and retrieve the program settings (environment) from a .ini file.  On both Linux and Windows
  the file will be called "misc->qsettings_app".ini (the name of the application with a .ini extension) and will be
  stored in a directory called ABE.config.  On Linux, the ABE.config directory will be stored in your $HOME directory.
  On Windows, it will be stored in your $USERPROFILE folder.  If you make a change to the way a variable is used and
  you want to force the defaults to be restored just change the settings_version to a newer number (I've been using
  the program version number from version.hpp - which you should be updating EVERY time you make a change to the
  program!).  You don't need to change the settings_version though unless you want to force the program to go back
  to the defaults (which can annoy your users).  So, the settings_version won't always match the program version.
*/

double settings_version = 1.06;


//  This function cleans up old undo files (in the event of a crash).

void cleanUndoFiles (MISC *misc, int32_t pid)
{
  QString tempDir = QDir::tempPath ();
  strcpy (misc->temp_dir, tempDir.toLatin1 ());

  QString PID;
  PID.sprintf ("_%05d_", pid);

  QDir files;
  files.setFilter (QDir::Files);

  if (files.cd (QDir::tempPath ()))
    {
      QFileInfoList flist = files.entryInfoList ();
      for (int32_t i = 0 ; i < flist.size () ; i++)
        {
          QString name = flist.at (i).fileName ();
          QFile path (flist.at (i).absoluteFilePath ());

          if (name.contains ("gridEdit_temp_") && name.contains (PID) && name.contains (".undo")) path.remove ();
        }
    }
}



uint8_t envin (OPTIONS *options, MISC *misc, QMainWindow *mainWindow)
{
  double saved_version = 0.0;
  QString string;
  QString buttonHotKey[HOTKEYS];


  //  Get the INI file name

#ifdef NVWIN3X
  QString ini_file = QString (getenv ("USERPROFILE")) + "/ABE.config/" + QString (misc->qsettings_app) + ".ini";
#else
  QString ini_file = QString (getenv ("HOME")) + "/ABE.config/" + QString (misc->qsettings_app) + ".ini";
#endif

  QSettings settings (ini_file, QSettings::IniFormat);
  settings.beginGroup (misc->qsettings_app);

  saved_version = settings.value (gridEdit::tr ("settings version"), saved_version).toDouble ();


  //  Recent files, directories, and hot keys should never be affected by the settings version so we want to load them even if the settings version
  //  has changed.

  options->recent_file_count = settings.value (gridEdit::tr ("recent file count"), options->recent_file_count).toInt ();

  for (int32_t i = 0 ; i < options->recent_file_count ; i++)
    {
      string.sprintf (gridEdit::tr ("recent file %d").toLatin1 (), i);
      options->recentFile[i] = settings.value (string, options->recentFile[i]).toString ();
    }


  options->overlay_dir = settings.value (gridEdit::tr ("overlay directory"), options->overlay_dir).toString ();
  options->output_area_dir = settings.value (gridEdit::tr ("output area directory"), options->output_area_dir).toString ();
  options->output_points_dir = settings.value (gridEdit::tr ("output points directory"), options->output_points_dir).toString ();
  options->input_chrtr2_dir = settings.value (gridEdit::tr ("input chrtr2 directory"), options->input_chrtr2_dir).toString ();
  options->geotiff_dir = settings.value (gridEdit::tr ("geotiff directory"), options->geotiff_dir).toString ();
  options->area_dir = settings.value (gridEdit::tr ("area directory"), options->area_dir).toString ();


  //  We're trying to save the hot keys since these are a pain for the user to change back if they've made major changes 
  //  to them.  We want to use whatever the user had saved even if we change the settings version.  We will still have to
  //  check for hot key conflicts though since there might be new keys with new deefault values.  To do this we're going
  //  to save the default hot keys (set in set_defaults.cpp) and then do a comparison.  If there are conflicts we'll replace 
  //  the user's setting with the default and then issue a warning.

  uint8_t hotkey_conflict = NVFalse;

  for (int32_t i = 0 ; i < HOTKEYS ; i++)
    {
      //  Save the default so we can check it later.

      buttonHotKey[i] = options->buttonAccel[i];


      QString string = misc->buttonText[i] + gridEdit::tr (" hot key");
      options->buttonAccel[i] = settings.value (string, options->buttonAccel[i]).toString ();
    }


  //  Make sure we have no hotkey duplications.

  for (int32_t i = 0 ; i < HOTKEYS ; i++)
    {
      for (int32_t j = i + 1 ; j < HOTKEYS ; j++)
        {
          if (options->buttonAccel[i].toUpper () == options->buttonAccel[j].toUpper ())
            {
              hotkey_conflict = NVTrue;
              options->buttonAccel[i] = buttonHotKey[i];
              options->buttonAccel[j] = buttonHotKey[j];
            }
        }
    }


  if (hotkey_conflict)
    {
      QMessageBox::warning (0, gridEdit::tr ("gridEdit settings"),
                            gridEdit::tr ("Hot key conflicts have been detected when reading program settings!<br><br>") +
                            gridEdit::tr ("Some of your hotkeys have been reset to default values!"));
    }


  //  If the settings version has changed we need to leave the values at the new defaults (from set_defaults.cpp) since they may have changed.

  if (settings_version != saved_version) return (NVFalse);;


  options->contour = settings.value (gridEdit::tr ("contour flag"), options->contour).toBool ();

  options->contour_width = settings.value (gridEdit::tr ("contour width"), options->contour_width).toInt ();

  options->contour_index = settings.value (gridEdit::tr ("contour index"), options->contour_index).toInt ();


  options->display_interpolated = settings.value (gridEdit::tr ("display interpolated flag"), options->display_interpolated).toBool ();

  options->flag_contour = settings.value (gridEdit::tr ("flag hand-drawn contour cells"), options->flag_contour).toBool ();

  options->coast = settings.value (gridEdit::tr ("coast flag"), options->coast).toInt ();
  options->GeoTIFF_alpha = settings.value (gridEdit::tr ("geotiff alpha"), options->GeoTIFF_alpha).toInt ();


  options->misp_weight = settings.value (gridEdit::tr ("misp weight"), options->misp_weight).toInt ();
  options->misp_force_original = settings.value (gridEdit::tr ("misp force original flag"), options->misp_force_original).toBool ();

  options->position_form = settings.value (gridEdit::tr ("position form"), options->position_form).toInt ();

  int32_t red = settings.value (gridEdit::tr ("contour color/red"), options->contour_color.red ()).toInt ();
  int32_t green = settings.value (gridEdit::tr ("contour color/green"), options->contour_color.green ()).toInt ();
  int32_t blue = settings.value (gridEdit::tr ("contour color/blue"), options->contour_color.blue ()).toInt ();
  int32_t alpha = settings.value (gridEdit::tr ("contour color/alpha"), options->contour_color.alpha ()).toInt ();
  options->contour_color.setRgb (red, green, blue, alpha);


  red = settings.value (gridEdit::tr ("contour highlight color/red"), options->contour_highlight_color.red ()).toInt ();
  green = settings.value (gridEdit::tr ("contour highlight color/green"), options->contour_highlight_color.green ()).toInt ();
  blue = settings.value (gridEdit::tr ("contour highlight color/blue"), options->contour_highlight_color.blue ()).toInt ();
  alpha = settings.value (gridEdit::tr ("contour highlight color/alpha"), options->contour_highlight_color.alpha ()).toInt ();
  options->contour_highlight_color.setRgb (red, green, blue, alpha);


  red = settings.value (gridEdit::tr ("coast color/red"), options->coast_color.red ()).toInt ();
  green = settings.value (gridEdit::tr ("coast color/green"), options->coast_color.green ()).toInt ();
  blue = settings.value (gridEdit::tr ("coast color/blue"), options->coast_color.blue ()).toInt ();
  alpha = settings.value (gridEdit::tr ("coast color/alpha"), options->coast_color.alpha ()).toInt ();
  options->coast_color.setRgb (red, green, blue, alpha);


  red = settings.value (gridEdit::tr ("background color/red"), options->background_color.red ()).toInt ();
  green = settings.value (gridEdit::tr ("background color/green"), options->background_color.green ()).toInt ();
  blue = settings.value (gridEdit::tr ("background color/blue"), options->background_color.blue ()).toInt ();
  alpha = settings.value (gridEdit::tr ("background color/alpha"), options->background_color.alpha ()).toInt ();
  options->background_color.setRgb (red, green, blue, alpha);


  red = settings.value (gridEdit::tr ("filter mask color/red"), options->poly_filter_mask_color.red ()).toInt ();
  green = settings.value (gridEdit::tr ("filter mask color/green"), options->poly_filter_mask_color.green ()).toInt ();
  blue = settings.value (gridEdit::tr ("filter mask color/blue"), options->poly_filter_mask_color.blue ()).toInt ();
  alpha = settings.value (gridEdit::tr ("filter mask color/alpha"), options->poly_filter_mask_color.alpha ()).toInt ();
  options->poly_filter_mask_color.setRgb (red, green, blue, alpha);

  
  red = settings.value (gridEdit::tr ("coverage real color/red"), options->cov_real_color.red ()).toInt ();
  green = settings.value (gridEdit::tr ("coverage real color/green"), options->cov_real_color.green ()).toInt ();
  blue = settings.value (gridEdit::tr ("coverage real color/blue"), options->cov_real_color.blue ()).toInt ();
  alpha = settings.value (gridEdit::tr ("coverage real color/alpha"), options->cov_real_color.alpha ()).toInt ();
  options->cov_real_color.setRgb (red, green, blue, alpha);


  red = settings.value (gridEdit::tr ("coverage digitized color/red"), options->cov_digitized_color.red ()).toInt ();
  green = settings.value (gridEdit::tr ("coverage digitized color/green"), options->cov_digitized_color.green ()).toInt ();
  blue = settings.value (gridEdit::tr ("coverage digitized color/blue"), options->cov_digitized_color.blue ()).toInt ();
  alpha = settings.value (gridEdit::tr ("coverage digitized color/alpha"), options->cov_digitized_color.alpha ()).toInt ();
  options->cov_digitized_color.setRgb (red, green, blue, alpha);


  red = settings.value (gridEdit::tr ("coverage interpolated color/red"), options->cov_interpolated_color.red ()).toInt ();
  green = settings.value (gridEdit::tr ("coverage interpolated color/green"), options->cov_interpolated_color.green ()).toInt ();
  blue = settings.value (gridEdit::tr ("coverage interpolated color/blue"), options->cov_interpolated_color.blue ()).toInt ();
  alpha = settings.value (gridEdit::tr ("coverage interpolated color/alpha"), options->cov_interpolated_color.alpha ()).toInt ();
  options->cov_interpolated_color.setRgb (red, green, blue, alpha);


  red = settings.value (gridEdit::tr ("coverage background color/red"), options->cov_background_color.red ()).toInt ();
  green = settings.value (gridEdit::tr ("coverage background color/green"), options->cov_background_color.green ()).toInt ();
  blue = settings.value (gridEdit::tr ("coverage background color/blue"), options->cov_background_color.blue ()).toInt ();
  alpha = settings.value (gridEdit::tr ("coverage background color/alpha"), options->cov_background_color.alpha ()).toInt ();
  options->cov_background_color.setRgb (red, green, blue, alpha);


  for (int32_t i = 0 ; i < NUM_HSV ; i++)
    {
      string = gridEdit::tr ("minimum hsv color value %1").arg (i);
      options->min_hsv_color[i] = settings.value (string, options->min_hsv_color[i]).toInt ();
      string = gridEdit::tr ("maximum hsv color value %1").arg (i);
      options->max_hsv_color[i] = settings.value (string, options->max_hsv_color[i]).toInt ();
      string = gridEdit::tr ("minimum hsv locked flag %1").arg (i);
      options->min_hsv_locked[i] = settings.value (string, options->min_hsv_locked[i]).toBool ();
      string = gridEdit::tr ("maximum hsv locked flag %1").arg (i);
      options->max_hsv_locked[i] = settings.value (string, options->max_hsv_locked[i]).toBool ();
      string = gridEdit::tr ("minimum hsv locked value %1").arg (i);
      options->min_hsv_value[i] = (float) settings.value (string, (double) options->min_hsv_value[i]).toDouble ();
      string = gridEdit::tr ("maximum hsv locked value %1").arg (i);
      options->max_hsv_value[i] = (float) settings.value (string, (double) options->max_hsv_value[i]).toDouble ();
    }


  QString en = settings.value (gridEdit::tr ("3D editor name"), QString (options->edit_name_3D)).toString ();
  strcpy (options->edit_name_3D, en.toLatin1 ());


  options->sunopts.sun.x = settings.value (gridEdit::tr ("sunopts sun x"), options->sunopts.sun.x).toDouble ();

  options->sunopts.sun.y = settings.value (gridEdit::tr ("sunopts sun y"), options->sunopts.sun.y).toDouble ();

  options->sunopts.sun.z = settings.value (gridEdit::tr ("sunopts sun y"), options->sunopts.sun.y).toDouble ();

  options->sunopts.azimuth = settings.value (gridEdit::tr ("sunopts azimuth"), options->sunopts.azimuth).toDouble ();

  options->sunopts.elevation = settings.value (gridEdit::tr ("sunopts elevation"), options->sunopts.elevation).toDouble ();

  options->sunopts.exag = settings.value (gridEdit::tr ("sunopts exag"), options->sunopts.exag).toDouble ();

  options->sunopts.power_cos = settings.value (gridEdit::tr ("sunopts power cosine"), options->sunopts.power_cos).toDouble ();

  options->sunopts.num_shades = settings.value (gridEdit::tr ("sunopts number of shades"), options->sunopts.num_shades).toInt ();


  options->display_minmax = settings.value (gridEdit::tr ("view min and max"), options->display_minmax).toBool ();


  options->edit_mode = settings.value (gridEdit::tr ("edit mode [0,1]"), options->edit_mode).toInt ();


  options->smoothing_factor = settings.value (gridEdit::tr ("contour smoothing factor"), options->smoothing_factor).toInt ();

  options->z_factor = (float) settings.value (gridEdit::tr ("depth scaling factor"), (double) options->z_factor).toDouble ();

  options->z_offset = (float) settings.value (gridEdit::tr ("depth offset value"), (double) options->z_offset).toDouble ();

  options->zero_turnover = settings.value (gridEdit::tr ("zero turnover flag"), options->zero_turnover).toBool ();

  options->non_top_layers_locked = settings.value (gridEdit::tr ("lock non-zero layers"), options->non_top_layers_locked).toBool ();

  options->highlight = settings.value (gridEdit::tr ("highlight"), options->highlight).toInt ();
  options->highlight_depth[0] = (float) settings.value (gridEdit::tr ("highlight depth range 0"), (double) options->highlight_depth[0]).toDouble ();
  options->highlight_depth[1] = (float) settings.value (gridEdit::tr ("highlight depth range 1"), (double) options->highlight_depth[1]).toDouble ();


  options->cint = (float) settings.value (gridEdit::tr ("contour interval"), (double) options->cint).toDouble ();

  options->draw_contour_level = (float) settings.value (gridEdit::tr ("draw contour and fill value"), (double) options->draw_contour_level).toDouble ();

  options->filter_slope = settings.value (gridEdit::tr ("filter slope"), options->filter_slope).toDouble ();
  options->filter_cutoff = settings.value (gridEdit::tr ("filter cutoff"), options->filter_cutoff).toInt ();
  options->filter_length = settings.value (gridEdit::tr ("filter length"), options->filter_length).toInt ();
  options->filter_type = settings.value (gridEdit::tr ("filter type"), options->filter_type).toInt ();

  options->overlap_percent = settings.value (gridEdit::tr ("window overlap percentage"), options->overlap_percent).toInt ();

  options->num_levels = settings.value (gridEdit::tr ("contour levels"), options->num_levels).toInt ();

  for (int32_t i = 0 ; i < options->num_levels ; i++)
    {
      string.sprintf (gridEdit::tr ("contour level %d").toLatin1 (), i);
      options->contour_levels[i] = (float) settings.value (string, (double) options->contour_levels[i]).toDouble ();
    }


  options->new_feature = settings.value (gridEdit::tr ("new feature counter"), options->new_feature).toInt ();
  options->startup_message = settings.value (gridEdit::tr ("startup message flag"), options->startup_message).toBool ();


  mainWindow->restoreState (settings.value (gridEdit::tr ("main window state")).toByteArray (), (int32_t) (settings_version * 100.0));

  mainWindow->restoreGeometry (settings.value (gridEdit::tr ("main window geometry")).toByteArray ());


  //  Get the previous PID and cleanup any undo files that were left from the last iteration (in case we crashed).

  int32_t prevPID = settings.value (gridEdit::tr ("previous PID"), options->new_feature).toInt ();

  cleanUndoFiles (misc, prevPID);


  //  Save the previous ID now so that if we crash we'll have it when we restart.

  settings.setValue (gridEdit::tr ("previous PID"), misc->process_id);


  settings.endGroup ();


  //  We need to get the font from the global settings since it will be used in (hopefully) all of the ABE map GUI applications.

#ifdef NVWIN3X
  QString ini_file2 = QString (getenv ("USERPROFILE")) + "/ABE.config/" + "globalABE.ini";
#else
  QString ini_file2 = QString (getenv ("HOME")) + "/ABE.config/" + "globalABE.ini";
#endif

  QSettings settings2 (ini_file2, QSettings::IniFormat);
  settings2.beginGroup ("globalABE");


  QString defaultFont = options->font.toString ();
  QString fontString = settings2.value (QString ("ABE map GUI font"), defaultFont).toString ();
  options->font.fromString (fontString);


  settings2.endGroup ();


  return (NVTrue);
}


void envout (OPTIONS *options, MISC *misc, QMainWindow *mainWindow)
{
  QString string;


  //  Get the INI file name

#ifdef NVWIN3X
  QString ini_file = QString (getenv ("USERPROFILE")) + "/ABE.config/" + QString (misc->qsettings_app) + ".ini";
#else
  QString ini_file = QString (getenv ("HOME")) + "/ABE.config/" + QString (misc->qsettings_app) + ".ini";
#endif

  QSettings settings (ini_file, QSettings::IniFormat);
  settings.beginGroup (misc->qsettings_app);


  for (int32_t i = 0 ; i < HOTKEYS ; i++)
    {
      string = misc->buttonText[i] + gridEdit::tr (" hot key");
      settings.setValue (string, options->buttonAccel[i]);
    }


  settings.setValue (gridEdit::tr ("settings version"), settings_version);

  settings.setValue (gridEdit::tr ("contour flag"), options->contour);

  settings.setValue (gridEdit::tr ("contour width"), options->contour_width);

  settings.setValue (gridEdit::tr ("contour index"), options->contour_index);


  settings.setValue (gridEdit::tr ("display interpolated flag"), options->display_interpolated);

  settings.setValue (gridEdit::tr ("flag hand-drawn contour cells"), options->flag_contour);

  settings.setValue (gridEdit::tr ("coast flag"), options->coast);
  settings.setValue (gridEdit::tr ("geotiff alpha"), options->GeoTIFF_alpha);

  settings.setValue (gridEdit::tr ("main button icon size"), options->main_button_icon_size);

  settings.setValue (gridEdit::tr ("misp weight"), options->misp_weight);
  settings.setValue (gridEdit::tr ("misp force original flag"), options->misp_force_original);

  settings.setValue (gridEdit::tr ("position form"), options->position_form);


  settings.setValue (gridEdit::tr ("contour color/red"), options->contour_color.red ());
  settings.setValue (gridEdit::tr ("contour color/green"), options->contour_color.green ());
  settings.setValue (gridEdit::tr ("contour color/blue"), options->contour_color.blue ());
  settings.setValue (gridEdit::tr ("contour color/alpha"), options->contour_color.alpha ());


  settings.setValue (gridEdit::tr ("contour highlight color/red"), options->contour_highlight_color.red ());
  settings.setValue (gridEdit::tr ("contour highlight color/green"), options->contour_highlight_color.green ());
  settings.setValue (gridEdit::tr ("contour highlight color/blue"), options->contour_highlight_color.blue ());
  settings.setValue (gridEdit::tr ("contour highlight color/alpha"), options->contour_highlight_color.alpha ());


  settings.setValue (gridEdit::tr ("coast color/red"), options->coast_color.red ());
  settings.setValue (gridEdit::tr ("coast color/green"), options->coast_color.green ());
  settings.setValue (gridEdit::tr ("coast color/blue"), options->coast_color.blue ());
  settings.setValue (gridEdit::tr ("coast color/alpha"), options->coast_color.alpha ());


  settings.setValue (gridEdit::tr ("background color/red"), options->background_color.red ());
  settings.setValue (gridEdit::tr ("background color/green"), options->background_color.green ());
  settings.setValue (gridEdit::tr ("background color/blue"), options->background_color.blue ());
  settings.setValue (gridEdit::tr ("background color/alpha"), options->background_color.alpha ());


  settings.setValue (gridEdit::tr ("coverage real color/red"), options->cov_real_color.red ());
  settings.setValue (gridEdit::tr ("coverage real color/green"), options->cov_real_color.green ());
  settings.setValue (gridEdit::tr ("coverage real color/blue"), options->cov_real_color.blue ());
  settings.setValue (gridEdit::tr ("coverage real color/alpha"), options->cov_real_color.alpha ());


  settings.setValue (gridEdit::tr ("coverage digitized color/red"), options->cov_digitized_color.red ());
  settings.setValue (gridEdit::tr ("coverage digitized color/green"), options->cov_digitized_color.green ());
  settings.setValue (gridEdit::tr ("coverage digitized color/blue"), options->cov_digitized_color.blue ());
  settings.setValue (gridEdit::tr ("coverage digitized color/alpha"), options->cov_digitized_color.alpha ());


  settings.setValue (gridEdit::tr ("coverage interpolated color/red"), options->cov_interpolated_color.red ());
  settings.setValue (gridEdit::tr ("coverage interpolated color/green"), options->cov_interpolated_color.green ());
  settings.setValue (gridEdit::tr ("coverage interpolated color/blue"), options->cov_interpolated_color.blue ());
  settings.setValue (gridEdit::tr ("coverage interpolated color/alpha"), options->cov_interpolated_color.alpha ());


  settings.setValue (gridEdit::tr ("coverage background color/red"), options->cov_background_color.red ());
  settings.setValue (gridEdit::tr ("coverage background color/green"), options->cov_background_color.green ());
  settings.setValue (gridEdit::tr ("coverage background color/blue"), options->cov_background_color.blue ());
  settings.setValue (gridEdit::tr ("coverage background color/alpha"), options->cov_background_color.alpha ());


  settings.setValue (gridEdit::tr ("filter mask color/red"), options->poly_filter_mask_color.red ());
  settings.setValue (gridEdit::tr ("filter mask color/green"), options->poly_filter_mask_color.green ());
  settings.setValue (gridEdit::tr ("filter mask color/blue"), options->poly_filter_mask_color.blue ());
  settings.setValue (gridEdit::tr ("filter mask color/alpha"), options->poly_filter_mask_color.alpha ());


  settings.setValue (gridEdit::tr ("3D editor name"), QString (options->edit_name_3D));


  for (int32_t i = 0 ; i < NUM_HSV ; i++)
    {
      string = gridEdit::tr ("minimum hsv color value %1").arg (i);
      settings.setValue (string, options->min_hsv_color[i]);
      string = gridEdit::tr ("maximum hsv color value %1").arg (i);
      settings.setValue (string, options->max_hsv_color[i]);
      string = gridEdit::tr ("minimum hsv locked flag %1").arg (i);
      settings.setValue (string, options->min_hsv_locked[i]);
      string = gridEdit::tr ("maximum hsv locked flag %1").arg (i);
      settings.setValue (string, options->max_hsv_locked[i]);
      string = gridEdit::tr ("minimum hsv locked value %1").arg (i);
      settings.setValue (string, (double) options->min_hsv_value[i]);
      string = gridEdit::tr ("maximum hsv locked value %1").arg (i);
      settings.setValue (string, (double) options->max_hsv_value[i]);
    }


  settings.setValue (gridEdit::tr ("sunopts sun x"), options->sunopts.sun.x);
  settings.setValue (gridEdit::tr ("sunopts sun y"), options->sunopts.sun.y);
  settings.setValue (gridEdit::tr ("sunopts sun y"), options->sunopts.sun.y);
  settings.setValue (gridEdit::tr ("sunopts azimuth"), options->sunopts.azimuth);
  settings.setValue (gridEdit::tr ("sunopts elevation"), options->sunopts.elevation);
  settings.setValue (gridEdit::tr ("sunopts exag"), options->sunopts.exag);
  settings.setValue (gridEdit::tr ("sunopts power cosine"), options->sunopts.power_cos);
  settings.setValue (gridEdit::tr ("sunopts number of shades"), options->sunopts.num_shades);


  settings.setValue (gridEdit::tr ("view min and max"), options->display_minmax);

  settings.setValue (gridEdit::tr ("edit mode [0,1]"), options->edit_mode);


  settings.setValue (gridEdit::tr ("contour smoothing factor"), options->smoothing_factor);

  settings.setValue (gridEdit::tr ("depth scaling factor"), (double) options->z_factor);

  settings.setValue (gridEdit::tr ("depth offset value"), (double) options->z_offset);

  settings.setValue (gridEdit::tr ("zero turnover flag"), options->zero_turnover);

  settings.setValue (gridEdit::tr ("lock non-zero layers"), options->non_top_layers_locked);

  settings.setValue (gridEdit::tr ("highlight"), options->highlight);
  settings.setValue (gridEdit::tr ("highlight depth range 0"), (double) options->highlight_depth[0]);
  settings.setValue (gridEdit::tr ("highlight depth range 1"), (double) options->highlight_depth[1]);

  settings.setValue (gridEdit::tr ("contour interval"), options->cint);

  settings.setValue (gridEdit::tr ("draw contour and fill value"), (double) options->draw_contour_level);

  settings.setValue (gridEdit::tr ("filter slope"), options->filter_slope);
  settings.setValue (gridEdit::tr ("filter cutoff"), options->filter_cutoff);
  settings.setValue (gridEdit::tr ("filter length"), options->filter_length);
  settings.setValue (gridEdit::tr ("filter type"), options->filter_type);

  settings.setValue (gridEdit::tr ("window overlap percentage"), options->overlap_percent);


  settings.setValue (gridEdit::tr ("contour levels"), options->num_levels);

  for (int32_t i = 0 ; i < options->num_levels ; i++)
    {
      string.sprintf (gridEdit::tr ("contour level %d").toLatin1 (), i);
      settings.setValue (string, options->contour_levels[i]);
    }


  settings.setValue (gridEdit::tr ("overlay directory"), options->overlay_dir);
  settings.setValue (gridEdit::tr ("output area directory"), options->output_area_dir);
  settings.setValue (gridEdit::tr ("output points directory"), options->output_points_dir);
  settings.setValue (gridEdit::tr ("input chrtr2 directory"), options->input_chrtr2_dir);
  settings.setValue (gridEdit::tr ("geotiff directory"), options->geotiff_dir);
  settings.setValue (gridEdit::tr ("area directory"), options->area_dir);


  settings.setValue (gridEdit::tr ("recent file count"), options->recent_file_count);

  for (int32_t i = 0 ; i < options->recent_file_count ; i++)
    {
      string.sprintf (gridEdit::tr ("recent file %d").toLatin1 (), i);
      settings.setValue (string, options->recentFile[i]);
    }


  settings.setValue (gridEdit::tr ("new feature counter"), options->new_feature);
  settings.setValue (gridEdit::tr ("startup message flag"), options->startup_message);


  settings.setValue (gridEdit::tr ("main window state"), mainWindow->saveState ((int32_t) (settings_version * 100.0)));

  settings.setValue (gridEdit::tr ("main window geometry"), mainWindow->saveGeometry ());


  settings.endGroup ();
}
