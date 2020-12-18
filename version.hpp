
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




#ifndef VERSION

#define     VERSION     "PFM Software - gridEdit V1.33 - 08/07/19"

#endif

/*! <pre>


    Version 1.00
    Jan C. Depner
    07/20/10

    First version.


    Version 1.01
    Jan C. Depner
    09/08/10

    Changes to handle Qt 4.6.3.


    Version 1.02
    Jan C. Depner
    09/24/10

    Now generates all possible cell points along a hand-drawn contour.
    Added button to toggle display of interpolated data. Fixed prefs bug.  The offset field wasn't showing up.


    Version 1.03
    Jan C. Depner
    11/22/10

    Attempting to fix edge mismatch on regrid of area when editing.


    Version 1.04
    Jan C. Depner
    12/01/10

    Actually fixed the edge mismatch on regrid when editing.  Also fixed the contour drawing and regridding.


    Version 1.05
    Jan C. Depner
    12/08/10

    Cleaned up the hotkey and accelerator handling quite a bit.  Added error checks on create_undo and append_undo calls.


    Version 1.06
    Jan C. Depner
    12/22/10

    Allow top and bottom scale values to be locked for any "color by" option.


    Version 1.07
    Jan C. Depner
    01/05/11

    Added hotkey check in the preferences dialog to prevent conflicts.  I really should have done this a long time
    ago but it was way down on the priority list ;-)


    Version 1.08
    Jan C. Depner
    01/06/11

    Correct problem with the way I was instantiating the main widget.  This caused an intermittent error on Windows7/XP.


    Version 1.09
    Jan C. Depner
    01/16/11

    Positioned non-modal popup dialogs in the center of the screen.


    Version 1.10
    Jan C. Depner
    01/20/11

    Bug fix.  Wasn't deleting land masked data.  Filling land masked data isn't allowed but deleting it is.  Also, added
    ability to select type of coastline to draw.


    Version 1.11
    Jan C. Depner
    01/21/11

    Added an experimental land mask flood fill function called maskMap.  I haven't implemented it yet due to time constraints
    but I will get to it very soon.


    Version 1.12
    Jan C. Depner
    03/28/11

    Try to save the user's hot key definitions even across setting version changes.


    Version 1.13
    Jan C. Depner
    04/15/11

    Fixed the geoTIFF reading by switching to using GDAL instead of Qt.  Hopefully Qt will get fixed eventually.


    Version 1.14
    Jan C. Depner
    06/15/11

    Now supports .yxz and .txt as overlays in addition to the .pts extension.


    Version 1.15
    Jan C. Depner
    06/27/11

    Save all directories used by the QFileDialogs.  Add current working directory to the sidebar for all QFileDialogs.
    Removed the . on the command line option since the current working directory is in the sidebar.


    Version 1.16
    Jan C. Depner
    07/22/11

    Using setSidebarUrls function from nvutility to make sure that current working directory (.) and
    last used directory are in the sidebar URL list of QFileDialogs.


    Version 1.17
    Jan C. Depner
    11/30/11

    Converted .xpm icons to .png icons.


    Version 1.18
    Jan C. Depner
    01/06/12

    Added contour capture mode.  This allows the user to define a polygonal area inside
    of which the interpolated contours will be captured as hand-drawn contours.


    Version 1.19
    Jan C. Depner
    01/20/12

    Now checks for CHRTR2_METERS units.  If the chrtr2 file is not in CHRTR2_METERS we abort.


    Version 1.20
    Jan C. Depner
    01/24/12

    Added ability to flag cells with hand-drawn contours.


    Version 1.21
    Jan C. Depner
    02/03/12

    - Added delete hand-drawn contours in polygon button.
    - Added insert data from PFM button.
    - Added non_zero_layers_locked preference (zero layer is top layer).
    - Added write and MISP lock/unlock buttons to layers tab.


    Version 1.22
    Jan C. Depner (PFM Software)
    12/09/13

    Switched to using .ini file in $HOME (Linux) or $USERPROFILE (Windows) in the ABE.config directory.  Now
    the applications qsettings will not end up in unknown places like ~/.config/navo.navy.mil/blah_blah_blah on
    Linux or, in the registry (shudder) on Windows.


    Version 1.23
    Jan C. Depner (PFM Software)
    12/17/13

    Removed the landmask code.  Now that we have a really good coastline based on SWBD (and a mask built from it)
    we don't really need to draw the landmask anymore.  If you want to see where the landmask will be, just look
    at the coastline.


    Version 1.24
    Jan C. Depner (PFM Software)
    02/26/14

    Cleaned up "Set but not used" variables that show up using the 4.8.2 version of gcc.


    Version 1.25
    Jan C. Depner (PFM Software)
    02/28/14

    Reverted to using Qt to read geoTIFFs.  It quit working in Qt 4.7.2 but is working again.  It's
    much faster than using GDAL and it's probably a lot more bulletproof.


    Version 1.26
    Jan C. Depner (PFM Software)
    03/19/14

    - Straightened up the Open Source acknowledgments.


    Version 1.27
    Jan C. Depner (PFM Software)
    05/07/14

    Fixed fread return checks.


    Version 1.28
    Jan C. Depner (PFM Software)
    05/27/14

    - Added the new LGPL licensed GSF library to the acknowledgments.


    Version 1.29
    Jan C. Depner (PFM Software)
    07/23/14

    - Switched from using the old NV_INT64 and NV_U_INT32 type definitions to the C99 standard stdint.h and
      inttypes.h sized data types (e.g. int64_t and uint32_t).


    Version 1.30
    Jan C. Depner (PFM Software)
    07/29/14

    - Fixed errors discovered by cppcheck.


    Version 1.31
    Jan C. Depner (PFM Software)
    08/09/16

    - Now gets font from globalABE.ini as set in Preferences by pfmView.
    - To avoid possible confusion, removed translation setup from QSettings in env_in_out.cpp.


    Version 1.32
    Jan C. Depner (PFM Software)
    09/02/16

    - Removed 2D point cloud editor option.


    Version 1.33
    Jan C. Depner (PFM Software)
    08/07/19

    - Added support for SHAPE files (via the utility library).

</pre>*/
