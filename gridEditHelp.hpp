
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



QString openText = 
  gridEdit::tr ("<img source=\":/icons/fileopen.png\"> Click this to open one or more CHRTR2 files.  The last file opened will be the "
                "top layer (primary file).");
QString closeCHRTR2Text = 
  gridEdit::tr ("Selecting this menu item will close the current top layer CHRTR2 file.  All other "
                "available layers will be moved up by one level.");
QString geotiffText = 
  gridEdit::tr ("<img source=\":/icons/geotiffopen.png\"> Click this button to open a GeoTIFF file.");

QString editRect3DText = 
  gridEdit::tr ("<img source=\":/icons/edit_rect.png\"> Click this button to switch the default mode to 3D rectangle "
                "edit.<br><br>"
                "After clicking the button the cursor will change to the edit rectangle cursor "
                "<img source=\":/icons/edit_rect_cursor.png\">.  Click the left mouse button to define a starting point "
                "for a rectangle.  Move the mouse to define the edit bounds.  Left click again to begin the edit "
                "operation.  To abort the operation click the middle mouse button.");
QString editPoly3DText = 
  gridEdit::tr ("<img source=\":/icons/edit_poly.png\"> Click this button to switch the default mode to 3D polygon "
                "edit.<br><br>"
                "After clicking the button the cursor will change to the edit polygon cursor "
                "<img source=\":/icons/edit_poly_cursor.png\">.  Click the left mouse button to define a starting point "
                "for a polygon.  Moving the mouse will draw a line.  Left click to define the next vertex of the polygon.  "
                "Double click to define the last vertex of the polygon and begin the edit operation.  "
                "To abort the operation click the middle mouse button.");

QString deleteRectText = 
  gridEdit::tr ("<img source=\":/icons/delete_rect.png\"> Click this button to switch the default mode to delete rectangle mode.<br><br>"
                "After clicking the button the cursor will change to the delete rectangle cursor "
                "<img source=\":/icons/delete_rect_cursor.png\">.  Click the left mouse button to define a starting point "
                "for a rectangle.  Move the mouse to define the edit bounds.  Left click again to begin the delete "
                "operation.  To abort the operation click the middle mouse button.");
QString deletePolyText = 
  gridEdit::tr ("<img source=\":/icons/delete_poly.png\"> Click this button to switch the default mode to delete polygon mode.<br><br>"
                "After clicking the button the cursor will change to the delete polygon cursor "
                "<img source=\":/icons/delete_poly_cursor.png\">.  Click the left mouse button to define a starting point "
                "for a polygon.  Moving the mouse will draw a line.  Left click to define the next vertex of the polygon.  "
                "Double click to define the last vertex of the polygon and begin the delete operation.  "
                "To abort the operation click the middle mouse button.");
QString deleteContourPolyText = 
  gridEdit::tr ("<img source=\":/icons/delete_contour_poly.png\"> Click this button to switch the default mode to delete hand-drawn contours "
                "within polygon mode.<br><br>"
                "After clicking the button the cursor will change to the delete polygon contours cursor "
                "<img source=\":/icons/delete_contour_poly_cursor.png\">.  Click the left mouse button to define a starting point "
                "for a polygon.  Moving the mouse will draw a line.  Left click to define the next vertex of the polygon.  "
                "Double click to define the last vertex of the polygon and begin the delete operation.  "
                "To abort the operation click the middle mouse button.");
QString fillRectText = 
  gridEdit::tr ("<img source=\":/icons/fill_rect.png\"> Click this button to switch the default mode to fill rectangle mode.<br><br>"
                "After clicking the button the cursor will change to the fill rectangle cursor <img source=\":/icons/fill_rect_cursor.png\">.  "
                "Prior to clicking the left mouse button to begin the rectangle you can set the fill value by clicking the middle mouse "
                "button while the cursor is over the desired value or you can use the <b>Set depth for fill</b> option from the right "
                "click popup menu.  Click the left mouse button to define a starting point for a rectangle.  Move the mouse to define "
                "the edit bounds.  Left click again to begin the fill operation.  To abort the operation click the middle mouse button.");
QString fillPolyText = 
  gridEdit::tr ("<img source=\":/icons/fill_poly.png\"> Click this button to switch the default mode to fill polygon mode.<br><br>"
                "After clicking the button the cursor will change to the fill polygon cursor <img source=\":/icons/fill_poly_cursor.png\">.  "
                "Prior to clicking the left mouse button to begin the polygon you can set the fill value by clicking the middle mouse "
                "button while the cursor is over the desired value or you can use the <b>Set depth for fill</b> option from the right "
                "click popup menu.  Click the left mouse button to define a starting point for a polygon.  Moving the mouse will draw a line.  "
                "Left click to define the next vertex of the polygon.  Double click to define the last vertex of the polygon and begin the "
                "fill operation.  To abort the operation click the middle mouse button.");
QString fillPolyPFMText = 
  gridEdit::tr ("<img source=\":/icons/fill_poly_pfm.png\"> Click this button to switch the default mode to fill polygon with PFM data mode.<br><br>"
                "After clicking the button the cursor will change to the fill polygon with PFM data cursor "
                "<img source=\":/icons/fill_poly_cursor_pfm.png\">.  Click the left mouse button to define a starting point for a polygon.  "
                "Moving the mouse will draw a line.  Left click to define the next vertex of the polygon.  Double click to define the last vertex "
                "of the polygon, select the PFM file from which to read data, and begin the fill operation.  To abort the operation click the middle "
                "mouse button.<br><br>"
                "<b>IMPORTANT NOTE: All valid data from the PFM that falls within the polygon will be placed into the relevant CHRTR2 file(s).  "
                "If you invalidated data in the PFM and you need to recover that you must first re-validate that data in the PFM before inserting "
                "it into the CHRTR2 file(s).  Also, the data is regridded (using MISP) prior to insertion of the data points.</b>");
QString filterRectText = 
  gridEdit::tr ("<img source=\":/icons/filter_rect.png\"> Click this button to switch the default mode to rectangle "
                "filter.<br><br>"
                "After clicking the button the cursor will change to the filter rectangle cursor "
                "<img source=\":/icons/filter_rect_cursor.png\">.  Click the left mouse button to define a starting point for a "
                "rectangle.  Moving the mouse will draw a rectangle.  Left click to end rectangle definition and begin the "
                "filter operation.  To abort the operation click the middle mouse button.  There are three values that affect "
                "the filter, they are the filter cutoff, slope, and length.  These values may be set in the preferences dialog "
                "<img source=\":/icons/prefs.png\"> or using the right click popup menu.<br><br>"
                "<b>IMPORTANT NOTE: You may mask areas using either the Rectangular Filter Mask "
                "<img source=\":/icons/filter_mask_rect.png\"> button or the Polygonal Filter Mask "
                "<img source=\":/icons/filter_mask_poly.png\"> button.");
QString filterPolyText = 
  gridEdit::tr ("<img source=\":/icons/filter_poly.png\"> Click this button to switch the default mode to polygon "
                "filter.<br><br>"
                "After clicking the button the cursor will change to the filter polygon cursor "
                "<img source=\":/icons/filter_poly_cursor.png\">.  Click the left mouse button to define a starting point for a "
                "polygon.  Moving the mouse will draw a line.  Left click to define the next vertex of the polygon.  "
                "Double click to define the last vertex of the polygon and begin the filter operation.  "
                "To abort the operation click the middle mouse button.  There are three values that affect "
                "the filter, they are the filter cutoff, slope, and length.  These values may be set in the preferences dialog "
                "<img source=\":/icons/prefs.png\"> or using the right click popup menu.<br><br>"
                "<b>IMPORTANT NOTE: You may mask areas using either the Rectangular Filter Mask "
                "<img source=\":/icons/filter_mask_rect.png\"> button or the Polygonal Filter Mask "
                "<img source=\":/icons/filter_mask_poly.png\"> button.");
QString filterRectMaskText = 
  gridEdit::tr ("<img source=\":/icons/filter_mask_rect.png\"> Click this button to define temporary areas to protect (mask) from "
                "subsequent filter operations <img source=\":/icons/filter_rect.png\">.<br><br>"
                "After clicking the button the cursor will change to the rectangle mask cursor "
                "<img source=\":/icons/filter_mask_rect_cursor.png\">.  "
                "Click the left mouse button to define a starting point for the rectangle.  Moving the mouse will draw a rectangle.  "
                "Left click to end rectangle definition and mask the area.  To abort the operation click the middle mouse button.<br><br>"
                "<b>IMPORTANT NOTE: The filter mask areas are only temporary.  If you change the area of the CHRTR2 that you "
                "are viewing these will be discarded.</b>");
QString filterPolyMaskText = 
  gridEdit::tr ("<img source=\":/icons/filter_mask_poly.png\"> Click this button to define temporary areas to protect (mask) from "
                "subsequent filter operations <img source=\":/icons/filter_poly.png\">.<br><br>"
                "After clicking the button the cursor will change to the polygon mask cursor "
                "<img source=\":/icons/filter_mask_poly_cursor.png\">.  "
                "Click the left mouse button to define a starting point for a polygon.  Moving the mouse will draw a line.  "
                "Left click to define the next vertex of the polygon.  Double click to define the last vertex of the polygon.  "
                "To abort the operation click the middle mouse button.<br><br>"
                "<b>IMPORTANT NOTE: The filter mask areas are only temporary.  If you change the area of the CHRTR2 that you "
                "are viewing these will be discarded.</b>");
QString clearMasksText = 
  gridEdit::tr ("<img source=\":/icons/clear_filter_masks.png\"> Click this button to clear all currently defined filter masks.  "
                "The masks will also be cleared if you change the area you are viewing.");

QString clearText = 
  gridEdit::tr ("<img source=\":/icons/clear_drawn_contours.png\"> Click this button to remove all hand drawn contours.");

QString clearContourText = 
  gridEdit::tr ("<img source=\":/icons/clear_drawn_contour.png\"> Click this button to switch to clear hand drawn contour mode.  When pressed, "
                "the cursor will change to the clear hand drawn contour cursor <img source=\":/icons/clear_drawn_contour._cursor.png\">"
                "Moving the cursor in the map will cause the tracking marker to snap to the nearest drawn contour point.  Once the marker "
                "is on the desired contour just left click to clear it.  Click the middle mouse button to discard the operation and return "
                "to the last used mode.");

QString undoText = 
  gridEdit::tr ("<img source=\":/icons/undo.png\"> Click this button to undo the last edit operation.  You only have 50 levels of undo "
                "but hopefully that will be sufficient.<br><br>"
                "<b>IMPORTANT NOTE: If you open a new CHRTR2 file or close a CHRTR2 file after having edited the data <i>ALL</i> "
                "undo files will be discarded!</b>");

QString remispText = 
  gridEdit::tr ("<img source=\":/icons/remisp_icon.png\"> Click this button to switch the default mode to "
                "<b>regrid rectangular area</b>.<br><br>"
                "After clicking the button the cursor will change to the regrid cursor "
                "<img source=\":/icons/remisp_cursor.png\">.  Click the left mouse button to define a starting point for a "
                "rectangle.  Move the mouse to define the edit bounds.  Left click again to begin the regrid operation.  "
                "To abort the operation click the middle mouse button.");

QString drawContourText = 
  gridEdit::tr ("<img source=\":/icons/draw_contour.png\"> Click this button to switch the default mode to <b>draw "
                "contour</b>.  This will allow you to draw a contour which will then be placed into the CHRTR2 structure.  "
                "There are three ways to define the depth value that you wish to draw as a contour.  The easiest is to "
                "move the draw contour cursor <img source=\":/icons/draw_contour_cursor.png\"> near a contour and click "
                "the center mouse button to grab the value of the nearest contour.  You may also use the <b>Grab nearest "
                "contour depth for contour</b> menu entry from the right click popup menu to grab the nearest contour "
                "value.  The third method is to use the <b>Set depth for contour</b> menu entry from the right click popup "
                "menu to manually set the value.  Once you have set the contour value, contours of exactly that value that "
                "are displayed on the screen will be redrawn highlighted in the current display.  The chosen value will "
                "be displayed in the status bar at the bottom of the window.  Note that manually entering a value that "
                "is not an exact contour value will not cause any contour highlighting.  As long as you are in <b>draw "
                "contour</b> mode the chosen contour level will be highlighted whenever it is redrawn.<br><br>"
                "At this point you may move the cursor to the desired location and left click/release to begin drawing the "
                "contour.   To end the contour you may left click/release again.  If, at any time during the operation, "
                "you wish to discard the contour and abort the operation simply click the middle mouse button or right "
                "click and select the appropriate menu option.<br><br>"
                "<b>IMPORTANT NOTE : Drawn contours will not place data in cells containing already existing, valid "
                "data.  If you want to draw contours through existing, valid data you must first delete that data.</b>");

QString grabContourText = 
  gridEdit::tr ("<img source=\":/icons/grab_contour.png\"> Click this button to switch the default mode to <b>capture "
                "interpolated contour</b> mode.  This will allow you to define a polygon to capture the interpolated "
                "contours and add them to your hand-drawn contours.  You can use this to avoid having to hand draw "
                "contours through an area in which you feel that the interpolated contours are correct.<br><br>"
                "After clicking the button the cursor will change to the capture contour cursor "
                "<img source=\":/icons/grab_contour_cursor.png\">.  Click the left mouse button to define a starting point "
                "for a polygon.  Moving the mouse will draw a line.  Left click to define the next vertex of the polygon.  "
                "Double click to define the last vertex of the polygon and begin the capture operation.  "
                "To abort the operation click the middle mouse button.<br><br> "
                "<b>NOTE: This mode will remain active unless one of the other modes "
                "(like rectangle edit <img source=\":/icons/edit_rect.png\">) is selected.</b>");

QString interpolatedText = 
  gridEdit::tr ("<img source=\":/icons/display_interpolated.png\"> Click this button to toggle display of the interpolated "
                "portions of the surface on or off.");

QString zoomOutText = 
  gridEdit::tr ("<img source=\":/icons/icon_zoomout.png\"> Click this button to zoom out one level.");
QString zoomInText = 
  gridEdit::tr ("<img source=\":/icons/icon_zoomin.png\"> Click this button to define a rectangular zoom area.  "
                "After clicking the button the cursor will change to the zoomCursor "
                "<img source=\":/icons/zoom_cursor.png\">.  Click the left mouse button to define a starting point for a "
                "rectangle.  Move the mouse to define the zoom in bounds.  Left click again to finish the operation.  To "
                "abort the zoom in operation click the middle mouse button.<br><br> "
                "<b>NOTE : This is a momentary button.  After zooming in or clicking the middle mouse button "
                "the active function will revert to the last edit function such as rectangular edit mode "
                "<img source=\":/icons/edit_rect.png\"></b>");
QString redrawText = 
  gridEdit::tr ("<img source=\":/icons/redraw.png\"> Click this button to redraw the displayed area.");

QString redrawCoverageText = 
  gridEdit::tr ("<img source=\":/icons/redrawcoverage.png\"> Click this button to redraw the coverage map.");

QString setCheckedText = 
  gridEdit::tr ("<img source=\":/icons/setchecked.png\"> Click this button to access a popup menu allowing you to set the "
                "bins in the displayed area of all of the CHRTR2 layers to <b>checked</b> or <b>unchecked</b>.");

QString displayMinMaxText = 
  gridEdit::tr ("<img source=\":/icons/display_min_max.png\"> Click this button to flag the minimum and maximum "
                "bins.  Bins will be marked with a circle for the minimum or a square for the maximum.");

QString prefsText = 
  gridEdit::tr ("<img source=\":/icons/prefs.png\"> Click this button to change program preferences.  This "
                "includes colors, contour intervals, contour smoothing factor, scale factor, "
                "sunshading options, coastline options, and position display format, among others.");

QString stopText = 
  gridEdit::tr ("<img source=\":/icons/stop.png\"> Click this button to cancel drawing of the surface.");

QString highlightText = 
  gridEdit::tr ("Click this button to highlight parts of the display depending on data type or various data conditions.  "
                "This button may display any of the following icons depending on the option chosen:"
                "<ul>"
                "<li><img source=\":/icons/highlight_none.png\">  No highlighting</li>"
                "<li><img source=\":/icons/highlight_all.png\">  Highlight all data (essentially a gray scale display)</li>"
                "<li><img source=\":/icons/highlight_checked.png\">  Highlight data that is set to checked</li>"
                "<li><img source=\":/icons/highlight01.png\">  Highlight cells containing CHRTR2_USER_01 flagged data</li>"
                "<li><img source=\":/icons/highlight02.png\">  Highlight cells containing CHRTR2_USER_02 flagged data</li>"
                "<li><img source=\":/icons/highlight03.png\">  Highlight cells containing CHRTR2_USER_03 flagged data</li>"
                "<li><img source=\":/icons/highlight04.png\">  Highlight cells containing CHRTR2_USER_04 flagged data</li>"
                "<li><img source=\":/icons/landmask.png\">  Highlight cells containing CHRTR2_LAND_MASK flagged data</li>"
                "<li><img source=\":/icons/highlight_int.png\">  Highlight cells that have only interpolated (MISP) data</li>"
                "<li><img source=\":/icons/highlight_inside.png\">  Highlight cells whose CHRTR2 surface value is inside the specified depth range</li>"
                "<li><img source=\":/icons/highlight_outside.png\">  Highlight cells whose CHRTR2 surface value is outside the specified depth range</li>"
                "</ul>");


QString contourText = 
  gridEdit::tr ("<img source=\":/icons/contour.png\"> Click this button to toggle drawing of contours.");

QString coastText = 
  gridEdit::tr ("<img source=\":/icons/auto_coast.png\"> Click this button to select the coastline type and resolution.  Auto coastline "
                "resolution will pick the best resolution based on the size of the area.");

QString geoText = 
  gridEdit::tr ("<img source=\":/icons/geo_off.png\"> This is a tristate button.  Click it to change the way GeoTIFFs "
                "are displayed.  The three states are:<br><br>"
                "<img source=\":/icons/geo_off.png\"><br><br>"
                "GeoTIFF will not be displayed.<br><br>"
                "<img source=\":/icons/geo.png\"><br><br>"
                "GeoTIFF will be displayed over the data with the transparency set in the preferences dialog.<br><br>"
                "<img source=\":/icons/geo_under.png\"><br><br>"
                "GeoTIFF will be displayed under the data.");


QString attrText = 
  gridEdit::tr ("Click this button to access a pulldown menu allowing you to color the displayed surface by the value of the "
                "associated attribute.  This button will either display the color icon <img source=\":/icons/color.png\"> "
                "when not coloring by attribute, the horizontal uncertainty icon <img source=\":/icons/horiz.png\"> when "
                "coloring by horizontal uncertainty or, the vertical uncertainty icon <img source=\":/icons/vert.png\"> "
                "when coloring by vertical uncertainty.");

QString flagContourText = 
  gridEdit::tr ("<img source=\":/icons/geo_off.png\"> Click this button to place an asterisk (*) over any cells that contain "
                "hand-drawn or digitized contours.");

QString covMenuText = 
  gridEdit::tr ("You may select an area to view by clicking and dragging on the Coverage Map with the left mouse "
                "button.  After defining the area, click the left mouse button again and the binned data will be "
                "displayed in the main window.  To discard the area click the middle mouse button or right click to "
                "select an option from the menu.");


QString covText = 
  gridEdit::tr ("This is the coverage map.  Areas that do not contain data will be displayed in the background color.  "
                "Areas with data marked as CHRTR2_REAL, CHRTR2_DIGITIZED_CONTOUR, or CHRTR2_INTERPOLATED will be "
                "displayed in the relevant color.  If the area is also marked as CHRTR2_CHECKED the color will be set "
                "to a highlighted (less saturated) version of the relevant color.<br><br>"
                "The CHRTR2 area boundary (which may not be a rectangle) will be displayed in semi-transparent green.  "
                "If you are displaying multiple CHRTR2 layers then only the top level CHRTR2 boundary will be displayed in "
                "semi-transparent green.  The other CHRTR2 boundaries will be displayed in semi-transparent red.<br><br>") +
                covMenuText;

QString chrtr2LabelText = 
  gridEdit::tr ("This is the name of the CHRTR2 layer that the cursor is moving over in the main window or the coverage "
                "map.  If multiple CHRTR2 layers are being displayed and there are overlapping areas then this will be "
                "the name of the topmost CHRTR2 layer.");

QString layerDisplayText = 
  gridEdit::tr ("When pressed this button will allow the named layer to be displayed.  When not depressed the named layer "
                "will be hidden and not used in any computations or editing operations.  When pressed the button will "
                "display the open eye icon <img source=\":/icons/display_layer.png\">.  When not depressed it will display "
                "the closed eye icon <img source=\":/icons/undisplay_layer.png\">.");

QString layerLockText = 
  gridEdit::tr ("When pressed this button will lock the named layer to prevent its use in MISP computations or edit operations.  "
                "It may or may not be displayed depending on the setting of the show/hide layer button for the named layer.  "
                "If you use the 3D point editor on data in a locked layer it will be shown as reference data (usually "
                "white) in the editor and no operations will be allowed on the locked layer data.  When pressed the button will "
                "display the locked icon <img source=\":/icons/lock.png\">.  When not depressed it will display "
                "the unlocked icon <img source=\":/icons/unlock.png\">.");

QString mapText = 
  gridEdit::tr ("The gridEdit program is used to display and edit geographically binned data that is stored in a CHRTR2 file.  "
                "Loading a file is as simple as clicking on the open file button "
                "<img source=\":/icons/fileopen.png\"> and then selecting a CHRTR2 file.  "
                "After doing this the area coverage will be displayed in the Coverage Map.  You may select an area to view "
                "by clicking and dragging on the Coverage Map with the left mouse button.  After defining "
                "the area, click the left mouse button again and the binned data will be displayed in the main "
                "window.  You may change the area being viewed by either defining a new area in the coverage "
                "map or pressing one of the arrow/keypad keys.  If you use the arrow/keypad keys there will be a five "
                "percent overlap between the new window and the old window.  PgUp, PgDn, Home, and End move the map in the "
                "coresponding diagonal directions.  Context sensitive help is available for all actions by using the What's This button "
                "<img source=\":/icons/contextHelp.png\">.");

QString gridEditAboutText = 
  gridEdit::tr ("<center><b>gridEdit</b><br><br>"
                "Author : Jan C. Depner<br>"
                "Date : 20 July 2010<br><br><br>"
                "The History Of CHRTR2<br></center>"
                "Back in 1994 I realized that we needed a simple replacement for the old CHRTR grid format.  "
                "The major shortcomings of CHRTR were that it didn't support unequal lat and lon grid sizes "
                "and it couldn't handle anything smaller than about 2 meter grids (due to 32 bit floating "
                "point precision).  Instead of just sitting down and writing the damn thing I talked to a "
                "bunch of other people about it.  At which point I realized the truth of the saying 'the "
                "only thing with 20 legs, 20 arms, and 10 heads that doesn't have a brain is a committee'.  "
                "The finalized version of the new grid format was released some time around 2006.  It sits "
                "on top of HDF5 which is a gigantic container 'format'.  In order to use it you have to map "
                "in a huge library and use a bunch of arcane function calls.  In it's defense, it can hold "
                "anything and everything.  That's right, it's the world's largest crescent wrench.  I hate "
                "crescent wrenches...<br><br>"
                "The CHRTR2 format was designed in keeping with the KISS principle (Keep It Simple Stupid).  "
                "It is used to store and retrieve a simple latitude and longitude grid with status and "
                "optional horizontal and vertical uncertainty.  It has an ASCII header that you can read with "
                "your own beady little eyes by simply 'more'ing or 'less'ing or, God forbid, 'type /page'ing "
                "the file.  It is arranged south to north, west to east.  That is, the first row is the  "
                "southernmost row and the first point in that row is the westernmost point.  The data is "
                "stored as bit packed, scaled integers so the file has no 'endianness'.  As opposed to "
                "the 'designed by committee' format described above, CHRTR2 took about two days to write.  "
                "I should have done this in 1994!<br><br>") + mapText +
  gridEdit::tr ("<br><br><center>Jan Depner (area.based.editor@gmail.com)<br></center>");

QString miscLabelText = 
  gridEdit::tr ("This area displays the currently active mode or information about the operation in progress.");

QString statusBarText = 
  gridEdit::tr ("The status bar is used to display the progress of the current operation.");

QString progStatusText = 
  gridEdit::tr ("The status bar is used to display the progress of the current operation.");

QString fileCheckedText = 
  gridEdit::tr ("This menu entry sets the <b><i>entire file for each displayed CHRTR2 layer</i></b> to CHECKED.  For a large file or files this "
                "can take a fairly long time.");

QString fileUncheckedText = 
  gridEdit::tr ("This menu entry sets the <b><i>entire file for each displayed CHRTR2 layer</i></b> to UNCHECKED.  For a large file or files "
                "this can take a fairly long time.");

QString selectOverlaysText = 
  gridEdit::tr ("This menu entry pops up the Select Overlays dialog.  The dialog allows you to select "
                "files for overlaying on the main display.  The following file extensions are "
                "recognized:"
                "<ul>"
                "<li>.ARE      -      ISS60 area file format</li>"
                "<li>.are      -      generic area file format</li>"
                "<li>.afs      -      Army Corps area file format (lon,lat)</li>"
                "<li>.ZNE      -      ISS60 tide or SV zone file format</li>"
                "<li>.zne      -      ISS60 tide or SV zone file format</li>"
                "<li>.tdz      -      generic tide zone file format</li>"
                "<li>.srv      -      ISS60 survey plan file format</li>"
                "<li>.trk      -      decimated track file format</li>"
                "<li>.pts      -      generic bin point format</li>"
                "</ul>"
                "The ISS60 file formats are documented in the ISS60 User Manual.  The generic file "
                "formats conform to the following:<br><br>"
                "Geographic positions are entered as a lat, lon pair separated by a comma.  A lat "
                "or lon may be in any of the following formats (degrees, minutes, and seconds must "
                "be separated by a space or tab):"
                "<ul>"
                "<li>Hemisphere Degrees decimal                 : S 28.4532</li>"
                "<li>Hemisphere Degrees minutes decimal         : S 28 27.192</li>"
                "<li>Hemisphere Degrees minutes seconds decimal : S 28 27 11.52</li>"
                "<li>Signed Degrees decimal                 : -28.4532</li>"
                "<li>Signed Degrees minutes decimal         : -28 27.192</li>"
                "<li>Signed Degrees minutes seconds decimal : -28 27 11.52</li>"
                "</ul>"
                "West longitude and south latitude are negative :<br><br>"
                "Ex. : -28 27 11.52 = S28 27 11.52 = s 28 27.192<br><br>"
                "The generic bin point format contains the following comma-separated fields:"
                "<ul>"
                "<li>Latitude</li>"
                "<li>Longitude</li>"
                "<li>Description</li>"
                "</ul>"
                "The decimated track file format is as follows:<br><br>"
                "file #,ping #,lat,lon,azimuth,distance<br><br>"
                "For the purposes of this program azimuth and distance are ignored.  Example:<br><br>"
                "3,9103,30.165371600,-88.749665800,353.748880,30.254914<br><br>"
                "Prior to each change of file number is a line containing the file name in this "
                "format:<br><br>"
                "FILE ### = /whatever/whatever/blah/blah/yackity/smackity");

QString start3DText = 
  gridEdit::tr ("This menu option allows you to start the 3D CHRTR2 bin surface viewer application.");

QString displayCHRTR2HeaderText = 
  gridEdit::tr ("This menu option pops up the Display CHRTR2 Header dialog.  This dialog displays the "
                "ASCII text CHRTR2 structure header.");

QString outputDataPointsText = 
  gridEdit::tr ("This menu option allows the user to select points to be output to an ASCII file.  "
                "When selected the cursor will become the cross hair cursor.  Left clicking on "
                "a location will collect the position and depths of that location.  Up to 200 points may be "
                "selected.  To select the last point double click on a location.  At that time a file "
                "selection dialog will pop up.  Enter a file name or use a preexisting file.  Files "
                "must have a .pts extension.  If you do not put one on the file name it will be appended "
                "automatically.  If, at any time during the operation you wish to discard the points and "
                "abort the operation simply click the middle mouse button.  The format of the output "
                "file is:<br><br>"
                "latitude, longitude, Point_#N average_depth minimum_depth maximum_depth<br>");

QString zoomToAreaText = 
  gridEdit::tr ("This menu option allows the user to select an area file and have the view zoom to the "
                "area file boundaries.  This is handy if you would like to compare sections of two different "
                "CHRTR2 files.  Otherwise it's kind of pointless ;-)");

QString defineRectAreaText = 
  gridEdit::tr ("This menu option allows the user to define a rectangular area and output it to a "
                "generic area file (*.are).  When selected the cursor will become the diagonal sizing "
                "cursor.  Left clicking on a location will cause that point to be the rectangle anchor "
                "point.  Moving the cursor will cause a rectangle to appear.  To finalize the rectangle "
                "left click again.  At that time a file selection dialog will pop up.  Enter a "
                "file name or use a preexisting file.  Files must have a .are extension.  If you do not "
                "put one on the file name it will be appended automatically.  If, at any time during the "
                "operation you wish to discard the rectangle and abort the operation simply click the "
                "middle mouse button.");

QString definePolyAreaText = 
  gridEdit::tr ("This menu option allows the user to define a polygonal area and output it to a generic "
                "area file (*.are).  When selected the cursor will become the arrow cursor.  Left "
                "clicking on a location will cause that point to be the first polygon vertex.  Moving the "
                "cursor will cause a line to appear.  Select the next vertex by left clicking on a "
                "location again.  Select as many points as needed.  To select the last point double click "
                "on its location.  At that time a file selection dialog will pop up.  Enter a file name "
                "or use a preexisting file.  Files must have a .are extension.  If you do not put one on "
                "the file name it will be appended automatically.  If, at any time during the operation "
                "you wish to discard the polygon and abort the operation simply click the middle mouse "
                "button.");

QString exportImageText = 
  gridEdit::tr ("Export the displayed view to an image file.  Everything displayed on the screen with the exception "
                "of moving objects (marker, cursors, rubberband rectangels, etc.) will be saved to the image file.  The "
                "available formats are:<br><br>"
                "<ul>"
                "<li>JPG</li>"
                "<li>PNG</li>"
                "<li>GeoTIFF</li>"
                "<li>BMP</li>"
                "<li>XPM</li>"
                "</ul>"
                "<br><br><b>Important note: If you type in a name and place one of the extensions on the name (.jpg, "
                ".png, .tif, .bmp, or .png) or if you select a preexisting file with one of those extensions, that "
                "format will be used.  If you don't place an extension on the file name, the extension selected in the "
                "<i>Files of type:</i> menu will be appended and used.</b>");

QString linkText = 
  gridEdit::tr ("<img source=\":/icons/unlink.png\"> Click this button to choose from other ABE or geographical GUI "
                "programs to connect to.  At present the only possible applications are pfmView, gridEdit, fileEdit3D, and "
                "areaCheck.  Hopefully, in the near future, we will be adding CNILE to the list.  The connection "
                "allows cursor tracking and limited data exchange between the applications.  If there are no "
                "available ABE groups to link to you can create a new group and add yourself to it so that other "
                "applications can link to this one.");

QString unlinkText = 
  gridEdit::tr ("<img source=\":/icons/link.png\"> Click this button to disconnect from another ABE or "
                "geographical GUI program.  This option is not available unless you have already linked to another "
                "program.");

QString highText = 
  gridEdit::tr ("<img source=\":/icons/high.png\"> Click this button to select the highest (shoalest) point inside a user defined "
                "rectangle.  The point will be added to an ASCII output file.");

QString lowText = 
  gridEdit::tr ("<img source=\":/icons/low.png\"> Click this button to select the lowest (deepest) point inside a user defined "
                "rectangle.  The point will be added to an ASCII output file.");

QString layerText = 
  gridEdit::tr ("Selecting one of the layers will cause it to be the top level layer.");

QString layerPrefsText = 
  gridEdit::tr ("Selecting this menu item will cause the layer preferences dialog to appear.  In this dialog you can "
                "turn layers on or off, set their transparency level (alpha value), set their coverage color, remove "
                "the layer, and reorder the layers.");

QString toolbarText = 
  gridEdit::tr ("There are three available tool bars in the gridEdit program.  They are<br><br>"
                "<ul>"
                "<li>Edit tool bar - contains buttons to set editing modes</li>"
                "<li>View tool bar - contains buttons to change the view</li>"
                "<li>Utilities tool bar - contains buttons to modify the preferences and get context sensitive help</li>"
                "</ul>"
                "The tool bars may be turned on or off and relocated to any location on the screen.  You may click and "
                "drag the tool bars using the handle to the left of (or above) the tool bar.  Right clicking in the tool "
                "bar or in the menu bar will pop up a menu allowing you to hide or show the individual tool bars.  The "
                "location, orientation, and visibility of the tool bars will be saved on exit.");
