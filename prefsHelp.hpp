
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



QString bGrpText = 
  Prefs::tr ("Select the format in which you want all geographic positions to be displayed.");

QString contourIntText = 
  Prefs::tr ("Set the contour interval to be used by gridEdit and also gridPointEdit.  Setting this "
             "to 0.0 will cause the user defined contour levels to be used.  No contours will "
             "be plotted unless the plot contour button <img source=\":/icons/contour.png\"> has been "
             "toggled on.");

QString contourSmText = 
  Prefs::tr ("Set the contour smoothing factor by gridEdit and also gridPointEdit.  This value goes "
             "from 0 (no smoothing) to 10 (max smoothing).<br><br>"
             "<b>IMPORTANT NOTE: Smoothing contours can make the program run very slowly.  Normally you "
             "should leave this set to 0.</b>");

QString ZFactorText = 
  Prefs::tr ("Set the scaling factor for contours <b><i>and</i></b> all displayed depth values "
             "in gridEdit and gridPointEdit.  The actual values aren't changed just the displayed values.  "
             "This is useful for comparing depths to GeoTIFFs of scanned charts from NOAA (feet) "
             "or the US Navy (fathoms).");

QString offsetText = 
  Prefs::tr ("Set the offset value for contours <b><i>and</i></b> all displayed depth values "
             "in gridEdit and gridPointEdit.  The actual values aren't changed just the displayed values.  "
             "This is useful for applying estimated ellipsoid offsets prior to the final datum "
             "shift being applied to the data.<br><br>"
             "<b>IMPORTANT NOTE: The offset value is <i>added</i> to the Z value and is applied <i>after</i> "
             "scaling so the units are in scaled units.</b>");

QString WFactorText = 
  Prefs::tr ("Set the weighting factor for the MISP regridding function.  The general rule of thumb for this value "
             "is to set to 1 to grid randomly spaced point data, 2 to grid randomly spaced swath data, and 3 to grid "
             "near complete coverage swath data.  Set to the negative value to force the grid to be as close as "
             "possible to the original input values.  This option is only useful if you have generated a MISP surface "
             "for the CHRTR2.");

QString forceText = 
  Prefs::tr ("Select this if you wish to force the final output grid to use the original data values at the "
	     "location of those values.  This overrides the weight factor in the locations of the original "
	     "values.  The weight factor will still have some influence on the grid surface in areas near the "
	     "original values.  Generally speaking, you only want to use this option if you have set the "
	     "<b>Weight factor</b> to a value of 3.");

QString zeroText = 
  Prefs::tr ("Select this if you wish to restart the color range when you cross zero.  This can be somewhat confusing if "
             "you have large flat areas near zero.");

QString nonZeroLockText = 
  Prefs::tr ("Select this if you wish to have all layers except for the top layer locked by default on startup.  "
             "You will still have the ability to individually unlock the layers.  If you set this and change the top layer "
             "all of the other layers will be automatically locked.  Again, you can still unlock them manually.  The "
             "last file opened automatically becomes the top layer.");

QString WidthText = 
  Prefs::tr ("Set the contour line width/thickness in pixels.  Index contours (if set) will be twice this size.");

QString IndexText = 
  Prefs::tr ("Set the index contour interval.  The Nth contour will be drawn at double the normal thickness.  "
             "For example, if this is set to 5, every fifth contour will be double thick.  Set to 0 to disable "
             "index contour emphasis.");

QString filterText = 
  Prefs::tr ("Set the statistical filter options in this tab");

QString overlapText = 
  Prefs::tr ("Set the percentage that the window will be overlapped when the arrow keys are used to move the winow.  "
             "This is five percent by default.  The min and max values are 0 and 25 respectively.");

QString conText = 
  Prefs::tr ("Set contour and depth display options in this tab");

QString contourLevelsText = 
  Prefs::tr ("Set individual contour levels.  Pressing this button will set the standard contour "
             "interval to 0.0 and bring up a table dialog to allow the user to insert contour "
             "levels (one per line, ascending).  The default levels are IHO standard contour "
             "levels.");

QString contoursTableText = 
  Prefs::tr ("Use this table to set individual contour levels.  Levels should be set in ascending "
             "order.");

QString prefsGeotiffText = 
  Prefs::tr ("Click this button to pop up a dialog with tools to allow you to change the amount of "
             "transparency used for a GeoTIFF overlay.");

QString transparencyText = 
  Prefs::tr ("This dialog is used to set the amount of transparency used in the GeoTIFF overlay.  Move the "
             "slider to change the value.  When the slider is changed the picture will be redrawn.");

QString hotKeysText = 
  Prefs::tr ("Click this button to change the accelerator key sequences associated with some of "
             "the function buttons.");

QString hotKeyTableText = 
  Prefs::tr ("Accelerator key sequences (hot keys) for some of the buttons in the main window may be changed "
             "in this dialog.  Up to four comma separated key values with modifiers may be used.  The format is "
             "illustrated by the following examples:\n\n"
             "    Q\n"
             "    Shift+q\n"
             "    Ctrl+q\n"
             "    Ctrl+Alt+q\n"
             "    Ctrl+x,Ctrl+c\n\n");

QString closeHotKeyText = 
             Prefs::tr ("Click this button to close the hot key dialog.");

QString closeContoursText = 
             Prefs::tr ("This button closes the contour level dialog.");

QString sunText = 
  Prefs::tr ("Set sun shading options in this tab");

QString sunAzText = 
  Prefs::tr ("Set the sun angle.  Sun angle is the direction from which the sun appears to be "
             "illuminating the sun shaded surface.  The default value is 30 degrees which would be "
             "from the upper right of the display.  It is much easier to do this using the Sun Angle "
             "button <img source=\":/icons/sunangle.png\"> in the tool bar.");

QString sunElText = 
  Prefs::tr ("Set the sun elevation.  Sun elevation is the apparent elevation of the sun above the "
             "horizon for the sun shaded surface.  The default value is 30 degrees.  Acceptable "
             "values are from 0.0 (totally dark) to 90.0 (directly overhead).");

QString sunExText = 
  Prefs::tr ("Set the sun shading Z exaggeration.  This is the apparent exaggeration of the surface "
             "in the Z direction.  The default is 1.0 (no exaggeration).");

QString iconSizeText = 
  Prefs::tr ("Set the size of the main window buttons.  The options are 16, 20, 24, 28, or 32 pixels.<br><br>"
             "<b>IMPORTANT NOTE: The icons size will change when you click the <i>Apply</i> button but "
             "the locations will not be entirely correct until you have exited and restarted the program.</b>");

QString miscText = 
  Prefs::tr ("This tab contains settings for a number of miscellaneous options including MISP surface parameters.");

QString colorText = 
  Prefs::tr ("Set color options in this tab.");

QString backgroundColorText = 
  Prefs::tr ("Click this button to change the color that is used as NULL background.  After "
             "selecting a color the map will be redrawn.");

QString coastColorText = 
  Prefs::tr ("Click this button to change the color that is used to plot the coasts.  After "
             "selecting a color the map will be redrawn.");

QString contourColorText = 
  Prefs::tr ("Click this button to change the color that is used to plot contours.  "
             "After selecting a color the map will be redrawn.");

QString contourHighlightColorText = 
  Prefs::tr ("Click this button to change the color that is used to highlight the selected "
             "contour interval when drawing contours.  "
             "After selecting a color the map will be redrawn.");

QString maskColorText = 
  Prefs::tr ("Click this button to change the color that is used to draw the polygon filter mask areas.  "
             "If you set the alpha value to anything other than 255 the area will be drawn as a filled "
             "polygon with some amount of transparency.  If you set it to 255 the area will be drawn as "
             "an empty polygon.  After selecting a color the map will be redrawn.");

QString covRealColorText = 
  Prefs::tr ("Click this button to change the color that is used to plot data marked as CHRTR2_REAL.  These are bins "
             "that were created from actual file data.  After selecting a color the map will be redrawn.");

QString covDigitizedColorText = 
  Prefs::tr ("Click this button to change the color that is used to plot data marked as CHRTR2_DIGITIZED_CONTOUR.  These are bins "
             "that were created from hand drawn or digitized contours.  After selecting a color the map will be redrawn.");

QString covInterpolatedColorText = 
  Prefs::tr ("Click this button to change the color that is used to plot data marked as CHRTR2_INTERPOLATED.  These are bins "
             "that were created using a cubic spline interpolation routine to fill empty bins.  After selecting a color the map will be redrawn.");

QString covBackgroundColorText = 
  Prefs::tr ("Click this button to change the color that is used to plot the background (data marked as CHRTR2_NULL).  These are bins "
             "that have no data of any kind in them.  After selecting a color the map will be redrawn.");

QString restoreDefaultsText = 
  Prefs::tr ("Click this button to restore all of the preferences to the original default values.  "
             "This includes <b><i>all</i></b> of the system preferences, not just those in the "
             "preferences dialog.  This includes things like the highlight option, surface viewed, "
             "contours viewed, contour levels, etc.");

QString closePrefsText = 
  Prefs::tr ("Click this button to discard all preference changes and close the dialog.  The main map and "
             "coverage map will not be redrawn.");

QString applyPrefsText = 
  Prefs::tr ("Click this button to apply all changes and close the dialog.  The main map and coverage map will "
             "be redrawn.");
