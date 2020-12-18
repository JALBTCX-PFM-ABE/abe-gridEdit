
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



#ifndef _GRID_EDIT_DEF_H_
#define _GRID_EDIT_DEF_H_


#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#include "nvutility.h"
#include "nvutility.hpp"


#ifdef NVWIN3X
    #include "windows_getuid.h"
#endif

#include <gdal.h>
#include <gdal_priv.h>
#include <cpl_string.h>
#include <ogr_spatialref.h>
#include <gdalwarper.h>
#include <ogr_spatialref.h>

#include "chrtr2.h"
#include "chrtr2_shared.h"

#include <QtCore>
#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#include <QSharedMemory>


using namespace std;  // Windoze bullshit - God forbid they should follow a standard


#define         MAX_UNDO                    50


//  Pointer interaction functions.

#define         RECT_EDIT_AREA_3D           0   //  Define a rectangular area to edit in 3D
#define         POLY_EDIT_AREA_3D           1   //  Define a polygonal area to edit in 3D
#define         RECT_DELETE_AREA            2   //  Delete a rectangular area
#define         POLY_DELETE_AREA            3   //  Delete a polygonal area
#define         RECT_FILL_AREA              4   //  Fill a rectangular area
#define         POLY_FILL_AREA              5   //  Fill a polygonal area
#define         ZOOM_IN_AREA                6   //  Define a rectangular area to zoom in to
#define         OUTPUT_POINTS               7   //  Select data points to output to an ASCII file
#define         DEFINE_RECT_AREA            8   //  Define a rectangular area to save as a generic or Army Corps area file
#define         DEFINE_POLY_AREA            9   //  Define a polygonal area to save as a generic or Army Corps area file
#define         DRAW_CONTOUR                10  //  Draw a contour to be used for redefining the MISP surface
#define         REMISP_AREA                 11  //  Define a rectangular area to regrid with MISP
#define         RECT_FILTER_AREA            12  //  Define a rectangular area to filter (statistically)
#define         POLY_FILTER_AREA            13  //  Define a polygonal area to filter (statistically)
#define         POLY_FILTER_MASK            14  //  Define a polygonal area to mask from filtering
#define         RECT_FILTER_MASK            15  //  Define a rectangular area to mask from filtering
#define         SELECT_HIGH_POINT           16  //  Define a rectangular area in which to mark a high (minimum Z) point
#define         SELECT_LOW_POINT            17  //  Define a rectangular area in which to mark a low (maximum Z) point
#define         CLEAR_CONTOUR               18  //  Remove a single hand drawn contour from memory
#define         GRAB_CONTOUR                19  //  Define a polygonal area and insert interpolated contours into the grid cells
#define         POLY_DELETE_CONTOUR         20  //  Define a polygonal area and delete interpolated contours
#define         POLY_FILL_PFM               21  //  Fill a polygonal with data from a PFM file


//  Coverage area pointer interaction functions.

#define         COV_START_DRAW_RECTANGLE    0   //  Start defining a rectangular area
#define         COV_DRAW_RECTANGLE          1   //  Define a rectangular area to view
#define         COV_START_DRAG_RECTANGLE    2   //  Start the process of dragging the rectangle
#define         COV_DRAG_RECTANGLE          3   //  Drag the current rectangular area to a new position


//  Highlight types.

#define         H_NONE                      0   //  No highlighting
#define         H_ALL                       1   //  Highlight everything (essentially gray shade)
#define         H_CHECKED                   2   //  Highlight checked data
#define         H_01                        3   //  Highlight bins containing CHRTR2_USER_01 marked data
#define         H_02                        4   //  Highlight bins containing CHRTR2_USER_02 marked data
#define         H_03                        5   //  Highlight bins containing CHRTR2_USER_03 marked data
#define         H_04                        6   //  Highlight bins containing CHRTR2_USER_04 marked data
#define         H_LAND_MASK                 7   //  Highlight bins containing CHRTR2_USER_05 marked data
#define         H_INT                       8   //  Highlight interpolated (MISP) bins
#define         H_INSIDE                    9   //  Highlight data inside of the user defined depth bounds

//  IMPORTANT NOTE:  Always leave H_OUTSIDE as the last of the highlight types.  It is used in the code.

#define         H_OUTSIDE                   10  //  Highlight data outside of the user defined depth bounds



#define         NUMSHADES                   256    //  Number of shades to be used for sunshading
#define         NUMHUES                     128    //  Number of hues to be used for coloring the surface
#define         NUM_SCALE_LEVELS            16     //  Number of color scale boxes
#define         CONTOUR_POINTS              1000   //  Max number of points in a drawn contour
#define         DEFAULT_SEGMENT_LENGTH      0.25   //  Default segment length for contours in scribe.cpp
#define         NUM_OVERLAYS                10     //  Max number of overlays
#define         LINE_WIDTH                  2      //  Standard line width
#define         FAT_LINE_WIDTH              4      //  Fat line width
#define         POLYGON_POINTS              NVMAP_POLYGON_POINTS   //  Maximum number of points in a polygon
#define         NUM_TOOLBARS                3      //  Number of toolbars defined
#define         NUM_BUTTONS                 4      //  Buttons with accelerators#
#define         MARKER_W                    15     //  Marker (cursor point tracker) width
#define         MARKER_H                    10     //  Marker height
#define         MAX_RECENT                  10     //  Maximum number of files in the Open Recent... file list
#define         COV_CHECKED_SATURATION      64     //  Amount of saturation (in HSV) for the checked versions of the coverage colors
#define         NUM_HSV                     5      //  Number of color attributes (by depth, by horizontal uncert, vertical uncert)


//  Button hotkeys that are editable by the user.  These are used as indexes into the options.buttonAccel, misc.buttonText,
//  misc.button, and misc.buttonIcon arrays.  To add or remove from this list you need to change it here, in set_defaults.cpp
//  and in gridEdit.cpp (look for the tool buttons that have one of these defined names attached).

#define         REDRAW_KEY                  0
#define         REDRAW_COVERAGE_KEY         1
#define         TOGGLE_CONTOUR_KEY          2
#define         TOGGLE_INTERPOLATED_KEY     3

#define         HOTKEYS                     4      //  Number of hot keys defined


//  Overlay types

#define         ISS60_AREA                  1
#define         GENERIC_AREA                2
#define         ISS60_ZONE                  3
#define         GENERIC_ZONE                4
#define         ISS60_SURVEY                5
#define         TRACK                       6
#define         GENERIC_YXZ                 7
#define         SHAPE                       8
#define         ACE_AREA                    9


//  The OPTIONS structure contains all those variables that can be saved to the
//  users gridEdit QSettings.


typedef struct
{
  uint8_t       contour;                    //  To contour or not to contour...
  int32_t       coast;                      //  Coastline type
  int32_t       position_form;              //  Position format number
  int32_t       contour_width;              //  Contour line width/thickness in pixels
  int32_t       contour_index;              //  Contour index (Nth contour emphasized)
  QColor        contour_color;              //  Color to be used for contours
  QColor        contour_highlight_color;    //  Color to be used for highlighted contours
  QColor        coast_color;                //  Color to be used for coast
  QColor        background_color;           //  Color to be used for background
  QColor        poly_filter_mask_color;     //  Color to be used for filter masks
  QColor        cov_real_color;             //  Color to be used for real data in the coverage map
  QColor        cov_digitized_color;        //  Color to be used for digitized data in the coverage map
  QColor        cov_interpolated_color;     //  Color to be used for interpolated data in the coverage map
  QColor        cov_checked_r_color;        //  Color to be used for checked real data in the coverage map
  QColor        cov_checked_d_color;        //  Color to be used for checked digitized data in the coverage map
  QColor        cov_checked_i_color;        //  Color to be used for checked interpolated data in the coverage map
  QColor        cov_background_color;       //  Color to be used for background in the coverage map
  QFont         font;                       //  Font used for all ABE map GUI applications
  uint16_t      min_hsv_color[NUM_HSV];     //  HSV color wheel index for color to be used for minimum values in surface (default 0)
  uint16_t      max_hsv_color[NUM_HSV];     //  HSV color wheel index for color to be used for maximum values in surface (default 315)
  float         min_hsv_value[NUM_HSV];     //  Locked minimum data value for minimum HSV color
  float         max_hsv_value[NUM_HSV];     //  Locked maximum data value for maximum HSV color
  uint8_t       min_hsv_locked[NUM_HSV];    //  Set if min HSV is locked to a value
  uint8_t       max_hsv_locked[NUM_HSV];    //  Set if max HSV is locked to a value
  char          edit_name_3D[256];          //  3D Editor name
  SUN_OPT       sunopts;                    //  Sun shading options
  uint8_t       display_minmax;             //  Flag to show min and max points
  uint8_t       display_interpolated;       //  Flag to show interpolated data
  uint8_t       flag_contour;               //  If set, display asterisks (*) over cells with hand-drawn contours
  int32_t       edit_mode;                  //  0 - rectangle, 1 - polygon
  int32_t       smoothing_factor;           //  Contour smoothing factor (0-10)
  float         z_factor;                   //  Coversion factor for Z values. May be used to convert m to ft...
  float         z_offset;                   //  Offset value for Z values.
  int32_t       highlight;                  //  See H_NONE and others above for definitions
  float         highlight_depth[2];         //  Highlight inside, outside depth values.
  QString       buttonAccel[NUM_BUTTONS];   //  Accelerator key sequences for some of the buttons
  int32_t       misp_weight;                //  Weight value for misp_init in remisp
  uint8_t       misp_force_original;        //  Force original flag for remisp
  int32_t       overlap_percent;            //  Window overlap percentage when moving with arrow keys
  int32_t       GeoTIFF_alpha;              //  25 - almost invisible, 255 - opaque, range in between - some transparency
  QString       threeD_prog;                //  3D viewer program.
  QString       threeD_hotkey;              //  3D viewer hot keys.
  QString       threeD_actkey;              //  3D viewer hot keys.
  int32_t       new_feature;                //  Stupid counter so I can force the startup message when I add a new feature
  uint8_t       startup_message;            //  Display the startup message
  QString       overlay_dir;                //  Last directory searched for overlays
  QString       output_area_dir;            //  Last directory in which an area file was saved
  QString       output_points_dir;          //  Last directory in which an output data points file was saved
  QString       input_chrtr2_dir;           //  Last directory searched for CHRTR2 files
  QString       geotiff_dir;                //  Last directory searched for GeoTIFF files
  QString       area_dir;                   //  Last directory searched for area files
  QString       pfm_dir;                    //  Last directory searched for PFM files
  QString       recentFile[MAX_RECENT];     //  Saved file names for Open Recent...
  int32_t       recent_file_count;          //  Number of files in Open Recent... list
  float         cint;                       //  Contour interval (same as misc.chrtr2_share->cint)
  int32_t       num_levels;                 //  Number of contour levels (same as misc.chrtr2_share->num_levels)
  float         contour_levels[200];        //  User defined contour levels (same as misc.chrtr2_share->contour_levels)
  int32_t       main_button_icon_size;      //  Main button icon size (16, 24, 32)
  QColor        color_array[2][NUMHUES + 1][NUMSHADES]; //  arrays of surface colors
  uint8_t       zero_turnover;              //  Set if we restart the colors at zero
  float         draw_contour_level;         //  Value inserted when drawing contours
  int32_t       filter_cutoff;              //  Filter cutoff frequency
  double        filter_slope;               //  Filter slope
  int32_t       filter_length;              //  Filter segment length
  int32_t       filter_type;                //  Filter type
  double        filter_weights[90];         //  Filter weights
  uint8_t       non_top_layers_locked;      //  Set if you want layers other than the top layer to be write locked
} OPTIONS;


//  Overlay structure.

typedef struct
{
  int32_t       file_type;                  //  File type (see #define ISS60_AREA above)
  uint8_t       display;                    //  Set to NVTrue if it is to be displayed
  QColor        color;                      //  Color to draw the overlay
  char          filename[512];              //  Overlay file name
} OVERLAY;


//  Drawn contour structure.

typedef struct
{
  std::vector<NV_F64_COORD3> xyz;
  int32_t                    count;
} HD_CONTOUR;


//  Filter mask structure.

typedef struct
{
  double        x[POLYGON_POINTS];          //  Array of polygon X values
  double        y[POLYGON_POINTS];          //  Array of polygon Y values
  int32_t       count;                      //  Number of points in the mask polygon
  uint8_t       displayed;                  //  Set to NVTrue if this mask has been displayed since last redraw
} FILTER_MASK;


//  Undo structure.  IMPORTANT NOTE: If misc.chrtr2_share->chrtr2_count changes (open or close a CHRTR2 file) you MUST
//  discard ALL undo information.

typedef struct
{
  char          filename[1024];             //  File name
  FILE          *fp;                        //  File pointer
  NV_I32_COORD2 start[MAX_CHRTR2_FILES];    //  Start row and column
  NV_I32_COORD2 end[MAX_CHRTR2_FILES];      //  End row and column
  int32_t       height[MAX_CHRTR2_FILES];   //  Height
  int32_t       width[MAX_CHRTR2_FILES];    //  Width
} UNDO;


//  General stuff.

typedef struct
{
  int32_t       function;                   //  Active edit or zoom function
  int32_t       save_function;              //  Save last function
  int32_t       last_edit_function;         //  Last edit function (so we can always come up in edit mode).
  int32_t       cov_function;               //  Active coverage map function
  uint8_t       coverage_drawing;           //  Set if coverage is drawing
  int32_t       poly_count;                 //  Polygon point count
  double        polygon_x[POLYGON_POINTS];  //  Polygon X
  double        polygon_y[POLYGON_POINTS];  //  Polygon Y
  int32_t       poly_filter_mask_count;     //  Number of polygonal filter mask areas defined
  std::vector<FILTER_MASK> poly_filter_mask; //  Pointer to array of polygonal filter mask areas.
  int32_t       maxd;                       //  Maximum contour density per grid cell
  char          GeoTIFF_name[512];          //  GeoTIFF file name
  uint8_t       GeoTIFF_open;               //  Set if GeoTIFF file has been opened.
  uint8_t       GeoTIFF_init;               //  Set if GeoTIFF is new
  int32_t       display_GeoTIFF;            //  0 - no display, 1 - display on top, -1 display underneath data
  int32_t       color_by_attribute;         //  0 for depth, 1 for horizontal uncertainty, 2 vertical uncertainty.
  uint8_t       drawing;                    //  set if we are drawing the surface
  uint8_t       drawing_canceled;           //  set to cancel drawing
  uint8_t       clear;                      //  clear entire area flag
  uint8_t       cov_clear;                  //  clear entire coverage area flag
  NV_F64_XYMBR  displayed_area[MAX_CHRTR2_FILES]; //  displayed area for each of the CHRTR2(s)
  NV_F64_XYMBR  total_displayed_area;       //  displayed area for all of the displayed CHRTR2s
  NV_F64_XYMBR  prev_total_displayed_area;  //  previously displayed total area
  int32_t       displayed_area_width[MAX_CHRTR2_FILES]; //  displayed area width in columns
  int32_t       displayed_area_height[MAX_CHRTR2_FILES]; //  displayed area height in rows
  int32_t       displayed_area_column[MAX_CHRTR2_FILES]; //  displayed area start column
  int32_t       displayed_area_row[MAX_CHRTR2_FILES]; //  displayed area start row
  float         displayed_area_min;         //  min Z value for displayed area
  NV_I32_COORD2 displayed_area_min_coord;   //  CHRTR2 coordinates of the min value in the displayed area
  int32_t       displayed_area_min_chrtr2;  //  CHRTR2 layer of the min value in the displayed area
  float         displayed_area_max;         //  max Z value for displayed area
  NV_I32_COORD2 displayed_area_max_coord;   //  CHRTR2 coordinates of the max value in the displayed area
  int32_t       displayed_area_max_chrtr2;  //  CHRTR2 layer of the max value in the displayed area
  float         displayed_area_range;       //  Z range for displayed area
  float         displayed_area_attr_min;    //  Minimum CHRTR2 attribute value (if coloring by attribute)
  float         displayed_area_attr_max;    //  Maximum CHRTR2 attribute value (if coloring by attribute)
  float         displayed_area_attr_range;  //  CHRTR2 attribute range (if coloring by attribute)
  int32_t       hatchr_start_x;             //  start column for hatchr
  int32_t       hatchr_end_x;               //  end column for hatchr
  int32_t       hatchr_start_y;             //  start row for hatchr
  int32_t       hatchr_end_y;               //  end row for hatchr
  float         *current_row;               //  One D array of data to pass to contour package
  float         *current_attr;              //  One D array of attribute data to pass to contour package
  uint8_t       *current_flags;             //  One D array of edit flags to pass to render function
  float         *next_row;                  //  One D array of data for sunshading
  OVERLAY       overlays[NUM_OVERLAYS];     //  overlay file information
  std::vector<NV_F64_COORD3> output_points; //  output data points
  int32_t       output_point_count;         //  number of points for OUTPUT_POINTS
  NV_I32_COORD2 center_point;               //  center of drawing area in pixels
  float         sin_array[3600];            //  Pre-computed sine values at 1/10 degree
  float         cos_array[3600];            //  Pre-computed cosine values at 1/10 degree
  QIcon         buttonIcon[HOTKEYS];        //  Tool button icons for tools having accelerators
  QString       buttonText[NUM_BUTTONS];    //  Tooltip text for the buttons that have editable accelerators
  QToolButton   *button[NUM_BUTTONS];       //  Buttons that have editable accelerators
  int32_t       nearest_chrtr2;
  QColor        widgetBackgroundColor;      //  The normal widget background color.
  QSharedMemory *chrtr2Share;               //  CHRTR2's shared memory pointer.
  QSharedMemory *abeRegister;               //  ABE's process register
  CHRTR2_SHARE  *chrtr2_share;              //  Pointer to the CHRTR2_SHARE structure in shared memory.
  ABE_REGISTER *abe_register;               //  Pointer to the ABE_REGISTER structure in shared memory.
  uint8_t       linked;                     //  Set if we have linked to another ABE application.
  QString       qsettings_app;              //  Application name for QSettings
  QWidget       *map_widget;                //  Map widget;
  int32_t       cov_width;                  //  Coverage window width
  int32_t       cov_height;                 //  Coverage window height
  int32_t       width;                      //  Main window width
  int32_t       height;                     //  Main window height
  int32_t       window_x;                   //  Main window x position
  int32_t       window_y;                   //  Main window y position
  uint8_t       dateline;                   //  Set to NVTrue if any of the CHRTR2s cross the dateline.
  NV_F64_COORD3 *filt_contour;              //  Contours drawn for filtering a MISP surface
  int32_t       filt_contour_count;         //  Number of filter contours drawn
  int32_t       process_id;                 //  This program's process ID
  QStatusBar    *progStatus;                //  Progress (and message) status bar
  QProgressBar  *statusProg;                //  Generic progress bar in status area
  QLabel        *statusProgLabel;           //  Generic progress bar label
  QPalette      statusProgPalette;          //  Background color palette for the progress bar label
  uint8_t       tposiafps;                  //  If set, don't show the famous TPOSIAFPS message again this session.
  scaleBox      *scale[NUM_SCALE_LEVELS];   //  Scale color boxes
  UNDO          undo[MAX_UNDO];             //  Undo structures
  int32_t       undo_count;                 //  Number of active undo files
  char          temp_dir[512];              //  Path to the temporary directory (for undo files)
  QString       hilo_file;                  //  High/Low point file name
  FILE          *hilo_fp;                   //  High/Low file pointer
  std::vector<HD_CONTOUR> hd_contour;       //  Hand drawn contours
  int32_t       hd_contour_count;           //  Number of hand drawn contours
  std::vector<NV_F64_COORD3> pfm_data;      //  Data being brought in from a PFM
  int32_t       pfm_data_count;             //  The number of points of PFM data brought in


  //  The following concern CHRTR2s as layers.

  NV_F64_XYMBR  total_mbr;                 //  MBR of all of the displayed CHRTR2s
  double        ss_cell_size_x[MAX_CHRTR2_FILES]; //  Sunshading X cell size
  double        ss_cell_size_y[MAX_CHRTR2_FILES]; //  Sunshading Y cell size
  int32_t       chrtr2_handle[MAX_CHRTR2_FILES];   //  CHRTR2 file handle
  uint8_t       chrtr2_alpha[MAX_CHRTR2_FILES];
  uint8_t       contour_in_chrtr2[MAX_CHRTR2_FILES]; //  NVTrue if a drawn contour enters the CHRTR2 (temporary use)
} MISC;


#endif
