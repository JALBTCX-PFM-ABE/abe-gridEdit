
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


//  Set defaults for all of the program's user defined options.  This called before envin and may be called from 
//  prefs.cpp if requested.

void set_defaults (MISC *misc, OPTIONS *options, uint8_t restore)
{
  //  Restore is only NVFalse if we were caled from gridEdit (prior to envin).  If we're called from prefs.cpp
  //  this is set to NVTrue (we only want to reset the user's options in that case).

  if (!restore)
    {
      misc->coverage_drawing = NVFalse;
      misc->poly_count = 0;
      misc->maxd = 10;
      strcpy (misc->GeoTIFF_name, "");
      misc->GeoTIFF_init = NVTrue;
      misc->GeoTIFF_open = NVFalse;
      misc->drawing_canceled = NVFalse;
      misc->drawing = NVFalse;
      misc->display_GeoTIFF = NVFalse;
      misc->output_point_count = 0;
      misc->clear = NVTrue;
      misc->cov_clear = NVTrue;
      misc->prev_total_displayed_area.min_x = 999.0;
      misc->nearest_chrtr2 = 0;
      misc->linked = NVFalse;
      misc->undo_count = 0;
      misc->cov_function = COV_START_DRAW_RECTANGLE;


      //  Tooltip text for the buttons that have editable accelerators

      misc->buttonText[REDRAW_KEY] = gridEdit::tr ("Redraw");
      misc->buttonText[REDRAW_COVERAGE_KEY] = gridEdit::tr ("Redraw Coverage Map");
      misc->buttonText[TOGGLE_CONTOUR_KEY] = gridEdit::tr ("Toggle contour drawing");
      misc->buttonText[TOGGLE_INTERPOLATED_KEY] = gridEdit::tr ("Toggle interpolated display");


      //  ToolButton icons for the buttons that have editable accelerators

      misc->buttonIcon[REDRAW_KEY] = QIcon (":/icons/redraw.png");
      misc->buttonIcon[REDRAW_COVERAGE_KEY] = QIcon (":/icons/redrawcoverage.png");
      misc->buttonIcon[TOGGLE_CONTOUR_KEY] = QIcon (":/icons/contour.png");
      misc->buttonIcon[TOGGLE_INTERPOLATED_KEY] = QIcon (":/icons/display_interpolated.png");


      for (int32_t i = 0 ; i < NUM_OVERLAYS ; i++) 
        {
          misc->overlays[i].display = NVFalse;
          misc->overlays[i].file_type = 0;
        }


      //  Set up the sine and cosine values for arrow drawing.

      for (int32_t i = 0 ; i < 3600 ; i++)
        {
          misc->sin_array[i] = sinf (((float) i / 10.0) * NV_DEG_TO_RAD);
          misc->cos_array[i] = cosf (((float) i / 10.0) * NV_DEG_TO_RAD);
        }

      misc->qsettings_app = gridEdit::tr ("gridEdit");

      misc->poly_filter_mask_count = 0;

      misc->width = 1240;
      misc->height = 960;
      misc->window_x = 0;
      misc->window_y = 0;

      misc->cov_width = 275;
      misc->cov_height = 275;

      misc->dateline = NVFalse;

      misc->tposiafps = NVFalse;

      misc->process_id = getpid ();

      misc->hilo_file = "NONE";
      misc->hilo_fp = NULL;

      misc->color_by_attribute = 0;

      misc->hd_contour_count = 0;
      misc->pfm_data_count = 0;


      //  Special case.  We don't want to show the startup message again just because we reset the defaults.

      options->startup_message = NVTrue;
      options->recent_file_count = 0;
    }


  options->coast = NVFalse;

  options->position_form = 0;
  options->contour = NVTrue;
  options->contour_width = 2;
  options->contour_index = 0;
  options->display_minmax = NVFalse;
  options->display_interpolated = NVTrue;
  options->flag_contour = NVFalse;
  strcpy (options->edit_name_3D, "gridPointEdit3D");
  options->threeD_prog = "grid3D";
  options->threeD_hotkey = '3';
  options->threeD_actkey = "";
  options->sunopts.azimuth = 30.0;
  options->sunopts.elevation = 30.0;
  options->sunopts.exag = 1.0;
  options->sunopts.power_cos = 1.0;
  options->sunopts.num_shades = NUMSHADES;
  options->edit_mode = RECT_EDIT_AREA_3D;
  options->smoothing_factor = 0;
  options->z_factor = 1.0;
  options->z_offset = 0.0;
  options->contour_color = QColor (255, 255, 255);
  options->contour_highlight_color = QColor (255, 0, 0);
  options->coast_color = QColor (255, 255, 0);
  options->background_color = QColor (255, 255, 255);
  options->poly_filter_mask_color = QColor (255, 255, 255, 96);
  options->cov_real_color = QColor (0, 0, 255);
  options->cov_digitized_color = QColor (255, 0, 0);
  options->cov_interpolated_color = QColor (0, 255, 255);
  options->cov_background_color = QColor (255, 255, 255);
  options->font = QApplication::font ();
  for (int32_t i = 0 ; i < NUM_HSV ; i++)
    {
      options->min_hsv_color[i] = 0;
      options->max_hsv_color[i] = 315;
      options->min_hsv_locked[i] = NVFalse;
      options->max_hsv_locked[i] = NVFalse;
      options->min_hsv_value[i] = 0.0;
      options->max_hsv_value[i] = 0.0;
    }
  options->highlight = H_INT;
  options->highlight_depth[0] = 0.0;
  options->highlight_depth[1] = 20.0;
  options->misp_weight = 2;
  options->misp_force_original = NVTrue;
  options->overlap_percent = 5;
  options->GeoTIFF_alpha = 64;
  options->new_feature = 0;
  options->zero_turnover = NVTrue;
  options->draw_contour_level = 0.0;
  options->filter_cutoff = 10;
  options->filter_slope = 0.08;
  options->filter_length = 10;
  options->filter_type = 1;
  options->non_top_layers_locked = NVFalse;

  options->overlay_dir = ".";
  options->output_area_dir = ".";
  options->output_points_dir = ".";
  options->input_chrtr2_dir = ".";
  options->geotiff_dir = ".";
  options->area_dir = ".";
  options->pfm_dir = ".";
  options->main_button_icon_size = 24;


  options->cint = 5.0;


  //  Default contour levels

  options->num_levels = 52;
  options->contour_levels[0] = -30;
  options->contour_levels[1] = -20;
  options->contour_levels[2] = -10;
  options->contour_levels[3] = -5;
  options->contour_levels[4] = -2;
  options->contour_levels[5] = 0;
  options->contour_levels[6] = 2;
  options->contour_levels[7] = 5;
  options->contour_levels[8] = 10;
  options->contour_levels[9] = 20;
  options->contour_levels[10] = 30;
  options->contour_levels[11] = 40;
  options->contour_levels[12] = 50;
  options->contour_levels[13] = 60;
  options->contour_levels[14] = 70;
  options->contour_levels[15] = 80;
  options->contour_levels[16] = 90;
  options->contour_levels[17] = 100;
  options->contour_levels[18] = 120;
  options->contour_levels[19] = 130;
  options->contour_levels[20] = 140;
  options->contour_levels[21] = 160;
  options->contour_levels[22] = 180;
  options->contour_levels[23] = 200;
  options->contour_levels[24] = 250;
  options->contour_levels[25] = 300;
  options->contour_levels[26] = 350;
  options->contour_levels[27] = 400;
  options->contour_levels[28] = 450;
  options->contour_levels[29] = 500;
  options->contour_levels[30] = 600;
  options->contour_levels[31] = 700;
  options->contour_levels[32] = 800;
  options->contour_levels[33] = 900;
  options->contour_levels[34] = 1000;
  options->contour_levels[35] = 1500;
  options->contour_levels[36] = 2000;
  options->contour_levels[37] = 2500;
  options->contour_levels[38] = 3000;
  options->contour_levels[39] = 3500;
  options->contour_levels[40] = 4000;
  options->contour_levels[41] = 4500;
  options->contour_levels[42] = 5000;
  options->contour_levels[43] = 5500;
  options->contour_levels[44] = 6000;
  options->contour_levels[45] = 6500;
  options->contour_levels[46] = 7000;
  options->contour_levels[47] = 7500;
  options->contour_levels[48] = 8000;
  options->contour_levels[49] = 8500;
  options->contour_levels[50] = 9000;
  options->contour_levels[51] = 10000;


  options->buttonAccel[REDRAW_KEY] = "Shift+r";
  options->buttonAccel[REDRAW_COVERAGE_KEY] = "Shift+m";
  options->buttonAccel[TOGGLE_CONTOUR_KEY] = "Shift+c";
  options->buttonAccel[TOGGLE_INTERPOLATED_KEY] = "Shift+i";
}


