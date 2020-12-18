
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



/*  gridEdit class definitions.  */

#ifndef GRID_EDIT_H
#define GRID_EDIT_H

#include "gridEditDef.hpp"
#include "version.hpp"
#include "startupMessage.hpp"
#include "prefs.hpp"
#include "manageOverlays.hpp"
#include "manageLayers.hpp"
#include "displayHeader.hpp"
#include "layers.hpp"
#include "lockValue.hpp"
#include "undo.hpp"
#include "remisp.hpp"


void displayMinMax (nvMap *map, OPTIONS *options, MISC *misc);
void overlayFlag (nvMap *map, OPTIONS *options, MISC *misc);
void scribe (nvMap *map, OPTIONS *options, MISC *misc, int32_t chrtr2, float *ar, float highlight);
void loadArrays (int32_t count, CHRTR2_RECORD chrtr2_record[], float data[], float attr[], int32_t attr_num, uint8_t flags[], int32_t highlight,
                 float min_depth, float max_depth, uint8_t display_interpolated);
void compute_total_mbr (MISC *misc);
void adjust_bounds (MISC *misc, int32_t chrtr2);
void fillArea (MISC *misc, float value, double *mx, double *my, int32_t count);
void fillPFM (MISC *misc, OPTIONS *options, char *fname, double *mx, double *my, int32_t count);


class gridEdit:public QMainWindow
{
  Q_OBJECT 


public:

  gridEdit (int *argc = 0, char **argv = 0, QWidget *parent = 0);
  ~gridEdit ();

  void moveMap (int32_t direction);
  void zoomIn (NV_F64_XYMBR bounds, uint8_t chrtr23D);
  void zoomOut ();
  void redrawMap (uint8_t clear, uint8_t chrtr23D);
  void editFeatureNum (int32_t feature_number);
  void commandLineFileCheck ();


protected:

  Prefs           *prefs_dialog;

  manageOverlays  *manageOverlays_dialog;

  manageLayers    *manageLayers_dialog;

  displayHeader   *displayHeader_dialog;

  QMenu           *layerMenu, *editMenu, *utilitiesMenu, *popupMenu, *covPopupMenu, *recentMenu;

  QTimer          *trackCursor;

  QTabWidget      *notebook;

  QLabel          *xSizeName, *ySizeName, *attrName[NUM_HSV], *layerName[MAX_CHRTR2_FILES], *latLabel, *lonLabel, *numLabel, *uncLabel, *zLabel,
                  *hvLabel, *tLabel, *chrtr2Label, *coordXLabel, *coordYLabel, *xSizeLabel, *ySizeLabel, *miscLabel, *attrLabel[NUM_HSV];

  QPushButton     *layerDisplay[MAX_CHRTR2_FILES], *layerLock[MAX_CHRTR2_FILES];

  QPainterPath    marker, arrow;

  nvMap           *map, *cov;

  QAction         *fileChecked, *fileUnchecked, *fileCloseAction, *geotiffOpenAction, *definePolygons, *exportImage, *setChecked, *setUnchecked,
                  *highlightNone, *highlightAll, *highlightChecked, *highlightUser01, *highlightUser02, *highlightUser03, *highlightUser04, *highlightLandMask,
                  *highlightInt, *highlightInside, *highlightOutside, *attr[NUM_HSV], *layer[MAX_CHRTR2_FILES], *start3D, *recentFileAction[MAX_RECENT],
                  *bHelp, *popup0, *popup1, *popup2, *popup3, *popup4, *popupHelp, *covPopup0, *covPopup1, *covPopup2, *noCoast, *autoCoast,
                  *ngaCoast, *wvsfCoast;

  QActionGroup    *recentGrp;

  QIcon           highlightIcon[H_OUTSIDE + 1], attrIcon[NUM_HSV];

  int32_t         menu_cursor_x, menu_cursor_y, cov_menu_cursor_x, cov_menu_cursor_y, gridPointEditMod, gridPointEditMask, abe_register_group,
                  prev_icon_size, recent_file_flag, mv_marker, mv_tracker, mv_arrow, mv_rectangle, rb_rectangle,
                  rb_polygon, cov_rb_rectangle, cov_mv_rectangle, redraw_count, nearest_drawn_contour, new_feature;

  float           nearest_value;

  uint8_t         grid_edit_active, popup_active, cov_popup_active, gridPointEdit_stopped, cov_area_defined, logo_first, double_click,
                  threeD_edit, need_redraw, command_file_flag, process_running, startup;

  double          ss_cell_size_x, ss_cell_size_y, menu_cursor_lon, menu_cursor_lat, cov_menu_cursor_lon, cov_menu_cursor_lat, prev_poly_lat, prev_poly_lon,
                  cov_start_drag_lon, cov_start_drag_lat;

  NV_F64_XYMBR    cov_drag_bounds;

  NVMAP_DEF       mapdef, covdef;

  QString         program, commandFile;

  QString         acknowledgmentsText;

  OPTIONS         options;

  MISC            misc;

  QProcess        *editProc, *threeDProc;

  QStatusBar      *covStatus, *mapStatus;

  QPalette        zPalette;

  QCursor         editRectCursor, editPolyCursor, deleteRectCursor, deletePolyCursor, fillRectCursor, fillPolyCursor, zoomCursor,
                  filterRectCursor, filterPolyCursor, filterMaskRectCursor, filterMaskPolyCursor, filterMaskPointCursor, remispCursor, drawContourCursor,
                  pencilCursor, highCursor, lowCursor, clearContourCursor, grabContourCursor, deleteContourPolyCursor, fillPolyPFMCursor;

  QToolBar        *toolBar[NUM_TOOLBARS];

  QToolButton     *bOpen, *bZoomOut, *bZoomIn, *bPrefs,  *bEditRect3D, *bEditPoly3D, *bFilterRect, *bFilterPoly, *bFilterRectMask,
                  *bDeleteRect, *bDeletePoly, *bDeleteContourPoly, *bFillRect, *bFillPoly, *bFilterPolyMask, *bClearMasks, *bClearContour, *bClear, *bUndo,
                  *bOpenGeotiff, *bDisplayMinMax, *bSetChecked, *bRedraw, *bRedrawCoverage, *bRemisp, *bDrawContour, *bLink, *bHigh, *bLow, *bStop,
                  *bHighlight, *bContour, *bInterpolated, *bCoast, *bGeotiff, *bAttr, *bGrabContour, *bFlagContour, *bFillPolyPFM;


  void redrawCovBounds ();
  void editCommand (double *mx, double *my, int32_t count);
  void setMainButtons (uint8_t enabled);
  void discardMovableObjects ();
  void initializeMaps ();
  void closeEvent (QCloseEvent *event);
  void setFunctionCursor (int32_t function);
  void clearFilterMasks ();

  void midMouse (int32_t mouse_x, int32_t mouse_y, double lon, double lat);
  void leftMouse (int32_t mouse_x, int32_t mouse_y, double lon, double lat);
  void rightMouse (int32_t mouse_x, int32_t mouse_y, double lon, double lat);

  void covMidMouse (int32_t mouse_x, int32_t mouse_y, double lon, double lat);
  void covLeftMouse (int32_t mouse_x, int32_t mouse_y, double lon, double lat);
  void covRightMouse (int32_t mouse_x, int32_t mouse_y, double lon, double lat);

protected slots:

  void slotCheckMenu (QAction *action);
  void slotCoastMenu (QAction *action);

  void slotPopupMenu0 ();
  void slotPopupMenu1 ();
  void slotPopupMenu2 ();
  void slotPopupMenu3 ();
  void slotPopupMenu4 ();
  void slotPopupHelp ();

  void slotMouseDoubleClick (QMouseEvent *e, double lon, double lat);
  void slotMousePress (QMouseEvent *e, double lon, double lat);
  void slotPreliminaryMousePress (QMouseEvent *e);
  void slotMouseRelease (QMouseEvent *e, double lon, double lat);
  void slotMouseMove (QMouseEvent *e, double lon, double lat);
  void slotResize (QResizeEvent *e);
  void slotClose (QCloseEvent *e);
  void slotPreRedraw (NVMAP_DEF mapdef);

  void slotLink ();
  void slotRegisterABEKeySet (int32_t key);
  void slotTrackCursor ();

  void slotCovPopupMenu0 ();
  void slotCovPopupMenu1 ();
  void slotCovPopupMenu2 ();
  void slotCovPopupHelp ();

  void slotCovMousePress (QMouseEvent *e, double lon, double lat);
  void slotCovMouseRelease (QMouseEvent *e, double lon, double lat);
  void slotCovMouseMove (QMouseEvent *e, double lon, double lat);
  void slotCovPreRedraw (NVMAP_DEF mapdef);
  void slotCovPostRedraw (NVMAP_DEF mapdef);

  void slotQuit ();
  void slotPostRedraw (NVMAP_DEF mapdef);

  void slotOpen ();
  void slotOpenRecent (QAction *action);
  void slotCloseCHRTR2 ();
  void slotOpenGeotiff ();
  void slotEditMode (int id);
  void slotZoomIn ();
  void slotZoomOut ();
  void slotClearFilterMasks ();
  void slotClearContours ();
  void slotUndo ();
  void slotRedraw ();
  void slotRedrawCoverage (); 
  void slotDisplayMinMax ();

  void slotStop ();

  void slotHighlightMenu (QAction *action);

  void slotLockValueDone (uint8_t accepted);
  void slotMinScalePressed (QMouseEvent *e);
  void slotMaxScalePressed (QMouseEvent *e);

  void slotDisplayLayerClicked (int id);
  void slotLockLayerClicked (int id);
  void slotContour ();
  void slotFlagContour ();
  void slotInterpolated ();
  void slotGeotiff ();
  void slotAttrMenu (QAction *action);

  void slotEditDone (int exitCode, QProcess::ExitStatus exitStatus);
  void slotEditError (QProcess::ProcessError error);
  void slotEditReadyReadStandardError ();
  void slotEditReadyReadStandardOutput ();

  void slotLayerGrpTriggered (QAction *action);
  void slotLayers ();
  void slotManageLayersDataChanged ();

  void slotToolbarHelp ();

  void slotStart3DViewer ();
  void slot3DError (QProcess::ProcessError error);
  void slot3DDone (int exitCode, QProcess::ExitStatus exitStatus);
  void slot3DReadyReadStandardError ();

  void slotExportImage ();

  void about ();
  void slotAcknowledgments ();
  void aboutQt ();

  void slotPrefs ();
  void slotPrefDataChanged (uint8_t lockChanged);
  void slotPrefHotKeyChanged (int32_t i);

  void slotOverlays ();
  void slotManageOverlaysDataChanged ();

  void slotDisplayHeader ();

  void slotOutputDataPoints ();

  void slotZoomToArea ();

  void slotDefineRectArea ();
  void slotDefinePolyArea ();


private:

  void keyPressEvent (QKeyEvent *e);

};

#endif
