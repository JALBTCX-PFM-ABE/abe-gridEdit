
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



//  gridEdit class.

/***************************************************************************\

    Module :        gridEdit

    Prorammer :     Jan C. Depner

    Date :          07/19/10

    Purpose :       CHRTR2 grid editor.  Replaces CHRTR grid editor (grid_edit).

\***************************************************************************/
    
#include "gridEdit.hpp"
#include "gridEditHelp.hpp"

#include <getopt.h>


gridEdit::gridEdit (int *argc, char **argv, QWidget *parent):
  QMainWindow (parent, 0)
{
  uint8_t envin (OPTIONS *options, MISC *misc, QMainWindow *mainWindow);
  void set_defaults (MISC *misc, OPTIONS *options, uint8_t restore);
  void open_chrtr2_files (MISC *misc, char *file, NV_F64_XYMBR *bounds);


  QResource::registerResource ("/icons.rcc");


  //  Read the acknowledgments file for the Acknowledgments Help button.  This way I only have
  //  to change one file and copy it to the other programs icon folders to change the Acknowledgments
  //  Help text instead of changing it in every single program I use it in.

  QFile *aDataFile = new QFile (":/icons/ACKNOWLEDGMENTS");

  if (aDataFile->open (QIODevice::ReadOnly))
    {
      char string[256];

      while (aDataFile->readLine (string, sizeof (string)) > 0)
        {
	  acknowledgmentsText.append (string);
        }
      aDataFile->close ();
    }


  //  Have to set the focus policy or keypress events don't work properly at first in Focus Follows Mouse mode

  setFocusPolicy (Qt::WheelFocus);


  //  Set a few defaults for startup

  grid_edit_active = NVFalse;
  threeD_edit = NVFalse;
  cov = NULL;
  threeDProc = NULL;
  prefs_dialog = NULL;
  prev_poly_lat = -91.0;
  prev_poly_lon = -181.0;
  popup_active = NVFalse;
  cov_popup_active = NVFalse;
  double_click = NVFalse;
  gridPointEditMod = 0;
  gridPointEditMask = 0;
  mv_marker = -1;
  mv_tracker = -1;
  mv_arrow = -1;
  mv_rectangle = -1;
  rb_rectangle = -1;
  rb_polygon = -1;
  cov_rb_rectangle = -1;
  cov_mv_rectangle = -1;
  need_redraw = NVFalse;
  gridPointEdit_stopped = NVFalse;
  logo_first = NVTrue;
  recent_file_flag = 0;
  command_file_flag = NVFalse;
  process_running = NVFalse;
  nearest_value = 0.0;
  redraw_count = 0;
  nearest_drawn_contour = -1;


  //  Make the "marker" cursor painter path.

  marker = QPainterPath ();

  marker.moveTo (0, 0);
  marker.lineTo (30, 0);
  marker.lineTo (30, 20);
  marker.lineTo (0, 20);
  marker.lineTo (0, 0);

  marker.moveTo (0, 10);
  marker.lineTo (12, 10);

  marker.moveTo (30, 10);
  marker.lineTo (18, 10);

  marker.moveTo (15, 0);
  marker.lineTo (15, 6);

  marker.moveTo (15, 20);
  marker.lineTo (15, 14);


  //  Make the "arrow" painter path.

  arrow = QPainterPath ();

  arrow.moveTo (0, 0);
  arrow.moveTo (5, 25);
  arrow.lineTo (5, -15);
  arrow.lineTo (10, -15);
  arrow.lineTo (0, -25);
  arrow.lineTo (-10, -15);
  arrow.lineTo (-5, -15);
  arrow.lineTo (-5, 25);
  arrow.lineTo (5, 25);


  //  Set the main icon

  setWindowIcon (QIcon (":/icons/gridEditIcon.jpg"));


  //  Set up the cursors.

  editRectCursor = QCursor (QPixmap (":/icons/edit_rect_cursor.png"), 1, 1);
  editPolyCursor = QCursor (QPixmap (":/icons/edit_poly_cursor.png"), 1, 1);
  deleteRectCursor = QCursor (QPixmap (":/icons/delete_rect_cursor.png"), 1, 1);
  deletePolyCursor = QCursor (QPixmap (":/icons/delete_poly_cursor.png"), 1, 1);
  fillRectCursor = QCursor (QPixmap (":/icons/fill_rect_cursor.png"), 11, 0);
  fillPolyCursor = QCursor (QPixmap (":/icons/fill_poly_cursor.png"), 11, 0);
  fillPolyPFMCursor = QCursor (QPixmap (":/icons/fill_poly_cursor_pfm.png"), 11, 0);
  zoomCursor = QCursor (QPixmap (":/icons/zoom_cursor.png"), 11, 11);
  filterRectCursor = QCursor (QPixmap (":/icons/filter_rect_cursor.png"), 1, 1);
  filterPolyCursor = QCursor (QPixmap (":/icons/filter_poly_cursor.png"), 1, 1);
  filterMaskRectCursor = QCursor (QPixmap (":/icons/filter_mask_rect_cursor.png"), 1, 1);
  filterMaskPolyCursor = QCursor (QPixmap (":/icons/filter_mask_poly_cursor.png"), 1, 1);
  remispCursor = QCursor (QPixmap (":/icons/remisp_cursor.png"), 1, 1);
  drawContourCursor = QCursor (QPixmap (":/icons/draw_contour_cursor.png"), 1, 1);
  clearContourCursor = QCursor (QPixmap (":/icons/clear_drawn_contour_cursor.png"), 9, 10);
  pencilCursor = QCursor (QPixmap (":/icons/pencil_cursor.png"), 10, 26);
  highCursor = QCursor (QPixmap (":/icons/high_cursor.png"), 1, 1);
  lowCursor = QCursor (QPixmap (":/icons/low_cursor.png"), 1, 1);
  grabContourCursor = QCursor (QPixmap (":/icons/grab_contour_cursor.png"), 5, 1);
  deleteContourPolyCursor = QCursor (QPixmap (":/icons/delete_contour_poly_cursor.png"), 1, 1);


  //  Set all of the defaults

  set_defaults (&misc, &options, NVFalse);


  //  We have to get the icon size out of sequence (the rest of the options are read in env_in_out.cpp)
  //  so that we'll have the proper sized icons for the toolbars.  Otherwise, they won't be placed correctly.

#ifdef NVWIN3X
  QString ini_file = QString (getenv ("USERPROFILE")) + "/ABE.config/" + QString (misc.qsettings_app) + ".ini";
#else
  QString ini_file = QString (getenv ("HOME")) + "/ABE.config/" + QString (misc.qsettings_app) + ".ini";
#endif


  QSettings settings (ini_file, QSettings::IniFormat);
  settings.beginGroup (misc.qsettings_app);
  options.main_button_icon_size = settings.value (tr ("main button icon size"), options.main_button_icon_size).toInt ();
  settings.endGroup ();


  //  Create the Edit toolbar.  Hopefully, all of this toolbar setup stuff is intuitively obvious to the most
  //  casual observer ;-)

  toolBar[0] = new QToolBar (tr ("Edit tool bar"));
  toolBar[0]->setToolTip (tr ("Edit tool bar"));
  addToolBar (toolBar[0]);
  toolBar[0]->setObjectName (tr ("gridEdit Edit tool bar"));

  bOpen = new QToolButton (this);
  bOpen->setIcon (QIcon (":/icons/fileopen.png"));
  bOpen->setToolTip (tr ("Open CHRTR2 file"));
  bOpen->setWhatsThis (openText);
  connect (bOpen, SIGNAL (clicked ()), this, SLOT (slotOpen ()));
  toolBar[0]->addWidget (bOpen);


  toolBar[0]->addSeparator ();
  toolBar[0]->addSeparator ();


  QButtonGroup *editModeGrp = new QButtonGroup (this);
  connect (editModeGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotEditMode (int)));
  editModeGrp->setExclusive (true);


  bEditRect3D = new QToolButton (this);
  bEditRect3D->setIcon (QIcon (":/icons/edit_rect3D.png"));
  bEditRect3D->setToolTip (tr ("Set 3D rectangle edit mode"));
  bEditRect3D->setWhatsThis (editRect3DText);
  editModeGrp->addButton (bEditRect3D, RECT_EDIT_AREA_3D);
  bEditRect3D->setCheckable (true);
  toolBar[0]->addWidget (bEditRect3D);

  bEditPoly3D = new QToolButton (this);
  bEditPoly3D->setIcon (QIcon (":/icons/edit_poly3D.png"));
  bEditPoly3D->setToolTip (tr ("Set 3D polygon edit mode"));
  bEditPoly3D->setWhatsThis (editPoly3DText);
  editModeGrp->addButton (bEditPoly3D, POLY_EDIT_AREA_3D);
  bEditPoly3D->setCheckable (true);
  toolBar[0]->addWidget (bEditPoly3D);


  toolBar[0]->addSeparator ();
  toolBar[0]->addSeparator ();


  bDeleteRect = new QToolButton (this);
  bDeleteRect->setIcon (QIcon (":/icons/delete_rect.png"));
  bDeleteRect->setToolTip (tr ("Set delete rectangle mode"));
  bDeleteRect->setWhatsThis (deleteRectText);
  editModeGrp->addButton (bDeleteRect, RECT_DELETE_AREA);
  bDeleteRect->setCheckable (true);
  toolBar[0]->addWidget (bDeleteRect);

  bDeletePoly = new QToolButton (this);
  bDeletePoly->setIcon (QIcon (":/icons/delete_poly.png"));
  bDeletePoly->setToolTip (tr ("Set polygon delete mode"));
  bDeletePoly->setWhatsThis (deletePolyText);
  editModeGrp->addButton (bDeletePoly, POLY_DELETE_AREA);
  bDeletePoly->setCheckable (true);
  toolBar[0]->addWidget (bDeletePoly);

  bDeleteContourPoly = new QToolButton (this);
  bDeleteContourPoly->setIcon (QIcon (":/icons/delete_contour_poly.png"));
  bDeleteContourPoly->setToolTip (tr ("Set polygon HD contour delete mode"));
  bDeleteContourPoly->setWhatsThis (deleteContourPolyText);
  editModeGrp->addButton (bDeleteContourPoly, POLY_DELETE_CONTOUR);
  bDeleteContourPoly->setCheckable (true);
  toolBar[0]->addWidget (bDeleteContourPoly);


  toolBar[0]->addSeparator ();
  toolBar[0]->addSeparator ();


  QString tip;


  bFillRect = new QToolButton (this);
  bFillRect->setIcon (QIcon (":/icons/fill_rect.png"));
  bFillRect->setWhatsThis (fillRectText);
  editModeGrp->addButton (bFillRect, RECT_FILL_AREA);
  bFillRect->setCheckable (true);
  toolBar[0]->addWidget (bFillRect);

  bFillPoly = new QToolButton (this);
  bFillPoly->setIcon (QIcon (":/icons/fill_poly.png"));
  bFillPoly->setWhatsThis (fillPolyText);
  editModeGrp->addButton (bFillPoly, POLY_FILL_AREA);
  bFillPoly->setCheckable (true);
  toolBar[0]->addWidget (bFillPoly);

  bFillPolyPFM = new QToolButton (this);
  bFillPolyPFM->setIcon (QIcon (":/icons/fill_poly_pfm.png"));
  bFillPolyPFM->setWhatsThis (fillPolyText);
  bFillPolyPFM->setToolTip (tr ("Set polygon fill from PFM mode"));
  bFillPolyPFM->setWhatsThis (fillPolyPFMText);
  editModeGrp->addButton (bFillPolyPFM, POLY_FILL_PFM);
  bFillPolyPFM->setCheckable (true);
  toolBar[0]->addWidget (bFillPolyPFM);


  toolBar[0]->addSeparator ();
  toolBar[0]->addSeparator ();


  bFilterRect = new QToolButton (this);
  bFilterRect->setIcon (QIcon (":/icons/filter_rect.png"));
  bFilterRect->setToolTip (tr ("Filter a rectangular area"));
  bFilterRect->setWhatsThis (filterRectText);
  editModeGrp->addButton (bFilterRect, RECT_FILTER_AREA);
  bFilterRect->setCheckable (true);
  toolBar[0]->addWidget (bFilterRect);

  bFilterPoly = new QToolButton (this);
  bFilterPoly->setIcon (QIcon (":/icons/filter_poly.png"));
  bFilterPoly->setToolTip (tr ("Filter a polygonal area"));
  bFilterPoly->setWhatsThis (filterPolyText);
  editModeGrp->addButton (bFilterPoly, POLY_FILTER_AREA);
  bFilterPoly->setCheckable (true);
  toolBar[0]->addWidget (bFilterPoly);

  bFilterRectMask = new QToolButton (this);
  bFilterRectMask->setIcon (QIcon (":/icons/filter_mask_rect.png"));
  bFilterRectMask->setToolTip (tr ("Mask a rectangular area from the filter"));
  bFilterRectMask->setWhatsThis (filterRectMaskText);
  editModeGrp->addButton (bFilterRectMask, RECT_FILTER_MASK);
  bFilterRectMask->setCheckable (true);
  toolBar[0]->addWidget (bFilterRectMask);

  bFilterPolyMask = new QToolButton (this);
  bFilterPolyMask->setIcon (QIcon (":/icons/filter_mask_poly.png"));
  bFilterPolyMask->setToolTip (tr ("Mask a polygonal area from the filter"));
  bFilterPolyMask->setWhatsThis (filterPolyMaskText);
  editModeGrp->addButton (bFilterPolyMask, POLY_FILTER_MASK);
  bFilterPolyMask->setCheckable (true);
  toolBar[0]->addWidget (bFilterPolyMask);


  bClearMasks = new QToolButton (this);
  bClearMasks->setIcon (QIcon (":/icons/clear_filter_masks.png"));
  bClearMasks->setToolTip (tr ("Clear filter masks"));
  bClearMasks->setWhatsThis (clearMasksText);
  connect (bClearMasks, SIGNAL (clicked ()), this, SLOT (slotClearFilterMasks ()));
  toolBar[0]->addWidget (bClearMasks);


  toolBar[0]->addSeparator ();
  toolBar[0]->addSeparator ();


  QMenu *checkMenu = new QMenu (this);

  QActionGroup *checkGrp = new QActionGroup (this);
  connect (checkGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotCheckMenu (QAction *)));

  setChecked = checkMenu->addAction (tr ("Set displayed area checked"));
  setUnchecked = checkMenu->addAction (tr ("Set displayed area unchecked"));
  checkGrp->addAction (setChecked);
  checkGrp->addAction (setUnchecked);

  bSetChecked = new QToolButton (this);
  bSetChecked->setIcon (QIcon (":/icons/setchecked.png"));
  bSetChecked->setToolTip (tr ("Set displayed area (un)checked"));
  bSetChecked->setWhatsThis (setCheckedText);
  bSetChecked->setPopupMode (QToolButton::InstantPopup);
  bSetChecked->setMenu (checkMenu);
  toolBar[0]->addWidget (bSetChecked);


  toolBar[0]->addSeparator ();
  toolBar[0]->addSeparator ();


  bDrawContour = new QToolButton (this);
  bDrawContour->setIcon (QIcon (":/icons/draw_contour.png"));
  bDrawContour->setWhatsThis (drawContourText);
  editModeGrp->addButton (bDrawContour, DRAW_CONTOUR);
  bDrawContour->setCheckable (true);
  bDrawContour->setEnabled (false);
  toolBar[0]->addWidget (bDrawContour);

  bGrabContour = new QToolButton (this);
  bGrabContour->setIcon (QIcon (":/icons/grab_contour.png"));
  bGrabContour->setToolTip (tr ("Capture and insert interpolated contours from a polygon"));
  bGrabContour->setWhatsThis (grabContourText);
  editModeGrp->addButton (bGrabContour, GRAB_CONTOUR);
  bGrabContour->setCheckable (true);
  bGrabContour->setEnabled (false);
  toolBar[0]->addWidget (bGrabContour);

  bRemisp = new QToolButton (this);
  bRemisp->setIcon (QIcon (":/icons/remisp_icon.png"));
  bRemisp->setToolTip (tr ("Regrid a rectangular area"));
  bRemisp->setWhatsThis (remispText);
  editModeGrp->addButton (bRemisp, REMISP_AREA);
  bRemisp->setCheckable (true);
  bRemisp->setEnabled (false);
  toolBar[0]->addWidget (bRemisp);


  toolBar[0]->addSeparator ();
  toolBar[0]->addSeparator ();


  bClearContour = new QToolButton (this);
  bClearContour->setIcon (QIcon (":/icons/clear_drawn_contour.png"));
  bClearContour->setToolTip (tr ("Clear a single hand drawn contour"));
  bClearContour->setWhatsThis (clearContourText);
  editModeGrp->addButton (bClearContour, CLEAR_CONTOUR);
  bClearContour->setCheckable (true);
  bClearContour->setEnabled (false);
  toolBar[0]->addWidget (bClearContour);

  bClear = new QToolButton (this);
  bClear->setIcon (QIcon (":/icons/clear_drawn_contours.png"));
  bClear->setToolTip (tr ("Clear all hand drawn contours"));
  bClear->setWhatsThis (clearText);
  bClear->setEnabled (false);
  connect (bClear, SIGNAL (clicked ()), this, SLOT (slotClearContours ()));
  toolBar[0]->addWidget (bClear);

  toolBar[0]->addSeparator ();
  toolBar[0]->addSeparator ();

  bUndo = new QToolButton (this);
  bUndo->setIcon (QIcon (":/icons/undo.png"));
  bUndo->setToolTip (tr ("Undo last edit operation [Ctrl+z]"));
  bUndo->setWhatsThis (undoText);
  bUndo->setShortcut (tr ("Ctrl+Z"));
  connect (bUndo, SIGNAL (clicked ()), this, SLOT (slotUndo ()));
  toolBar[0]->addWidget (bUndo);


  //  Create the View toolbar

  toolBar[1] = new QToolBar (tr ("View tool bar"));
  toolBar[1]->setToolTip (tr ("View tool bar"));
  addToolBar (toolBar[1]);
  toolBar[1]->setObjectName (tr ("gridEdit View tool bar"));

  bStop = new QToolButton (this);
  bStop->setIcon (QIcon (":/icons/stop.png"));
  bStop->setToolTip (tr ("Stop drawing"));
  bStop->setWhatsThis (stopText);
  bStop->setEnabled (false);
  connect (bStop, SIGNAL (clicked ()), this, SLOT (slotStop ()));
  toolBar[1]->addWidget (bStop);


  toolBar[1]->addSeparator ();
  toolBar[1]->addSeparator ();


  bZoomIn = new QToolButton (this);
  bZoomIn->setIcon (QIcon (":/icons/icon_zoomin.png"));
  bZoomIn->setToolTip (tr ("Zoom in"));
  bZoomIn->setWhatsThis (zoomInText);
  connect (bZoomIn, SIGNAL (clicked ()), this, SLOT (slotZoomIn ()));
  toolBar[1]->addWidget (bZoomIn);

  bZoomOut = new QToolButton (this);
  bZoomOut->setIcon (QIcon (":/icons/icon_zoomout.png"));
  bZoomOut->setToolTip (tr ("Zoom out"));
  bZoomOut->setWhatsThis (zoomOutText);
  connect (bZoomOut, SIGNAL (clicked ()), this, SLOT (slotZoomOut ()));
  toolBar[1]->addWidget (bZoomOut);


  toolBar[1]->addSeparator ();
  toolBar[1]->addSeparator ();


  bRedraw = new QToolButton (this);
  misc.button[REDRAW_KEY] = bRedraw;
  bRedraw->setIcon (misc.buttonIcon[REDRAW_KEY]);
  bRedraw->setWhatsThis (redrawText);
  connect (bRedraw, SIGNAL (clicked ()), this, SLOT (slotRedraw ()));
  toolBar[1]->addWidget (bRedraw);

  bRedrawCoverage = new QToolButton (this);
  misc.button[REDRAW_COVERAGE_KEY] = bRedrawCoverage;
  bRedrawCoverage->setIcon (misc.buttonIcon[REDRAW_COVERAGE_KEY]);
  bRedrawCoverage->setWhatsThis (redrawCoverageText);
  connect (bRedrawCoverage, SIGNAL (clicked ()), this, SLOT (slotRedrawCoverage ()));
  toolBar[1]->addWidget (bRedrawCoverage);


  toolBar[1]->addSeparator ();
  toolBar[1]->addSeparator ();


  QMenu *highlightMenu = new QMenu (this);

  QActionGroup *highlightGrp = new QActionGroup (this);
  connect (highlightGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotHighlightMenu (QAction *)));

  for (int32_t i = 0 ; i <= H_OUTSIDE ; i++)
    {
      switch (i)
        {
        case H_NONE:
          highlightIcon[i] = QIcon (":/icons/highlight_none.png");
          break;

        case H_ALL:
          highlightIcon[i] = QIcon (":/icons/highlight_all.png");
          break;

        case H_CHECKED:
          highlightIcon[i] = QIcon (":/icons/highlight_checked.png");
          break;

        case H_01:
          highlightIcon[i] = QIcon (":/icons/highlight01.png");
          break;

        case H_02:
          highlightIcon[i] = QIcon (":/icons/highlight02.png");
          break;

        case H_03:
          highlightIcon[i] = QIcon (":/icons/highlight03.png");
          break;

        case H_04:
          highlightIcon[i] = QIcon (":/icons/highlight04.png");
          break;

        case H_LAND_MASK:
          highlightIcon[i] = QIcon (":/icons/landmask.png");
          break;

        case H_INT:
          highlightIcon[i] = QIcon (":/icons/highlight_int.png");
          break;

        case H_INSIDE:
          highlightIcon[i] = QIcon (":/icons/highlight_inside.png");
          break;

        case H_OUTSIDE:
          highlightIcon[i] = QIcon (":/icons/highlight_outside.png");
          break;
        }
    }

  highlightNone = highlightMenu->addAction (tr ("No highlighting"));
  highlightNone->setIcon (highlightIcon[H_NONE]);
  highlightAll = highlightMenu->addAction (tr ("Highlight all"));
  highlightAll->setIcon (highlightIcon[H_ALL]);
  highlightChecked = highlightMenu->addAction (tr ("Highlight checked/verified data"));
  highlightChecked->setIcon (highlightIcon[H_CHECKED]);
  highlightUser01 = highlightMenu->addAction (tr ("Highlight CHRTR2_USER_01 data"));
  highlightUser01->setIcon (highlightIcon[H_01]);
  highlightUser02 = highlightMenu->addAction (tr ("Highlight CHRTR2_USER_02 data"));
  highlightUser02->setIcon (highlightIcon[H_02]);
  highlightUser03 = highlightMenu->addAction (tr ("Highlight CHRTR2_USER_03 data"));
  highlightUser03->setIcon (highlightIcon[H_03]);
  highlightUser04 = highlightMenu->addAction (tr ("Highlight CHRTR2_USER_04 data"));
  highlightUser04->setIcon (highlightIcon[H_04]);
  highlightLandMask = highlightMenu->addAction (tr ("Highlight CHRTR2_LAND_MASK data"));
  highlightLandMask->setIcon (highlightIcon[H_LAND_MASK]);
  highlightInt = highlightMenu->addAction (tr ("Highlight interpolated (MISP) data"));
  highlightInt->setIcon (highlightIcon[H_INT]);
  highlightInside = highlightMenu->addAction (tr ("Highlight data inside depth range (0.0 to 20.0)"));
  highlightInside->setIcon (highlightIcon[H_INSIDE]);
  highlightOutside = highlightMenu->addAction (tr ("Highlight data outside depth range (0.0 to 20.0)"));
  highlightOutside->setIcon (highlightIcon[H_OUTSIDE]);
  highlightGrp->addAction (highlightNone);
  highlightGrp->addAction (highlightAll);
  highlightGrp->addAction (highlightChecked);
  highlightGrp->addAction (highlightUser01);
  highlightGrp->addAction (highlightUser02);
  highlightGrp->addAction (highlightUser03);
  highlightGrp->addAction (highlightUser04);
  highlightGrp->addAction (highlightLandMask);
  highlightGrp->addAction (highlightInt);
  highlightGrp->addAction (highlightInside);
  highlightGrp->addAction (highlightOutside);

  bHighlight = new QToolButton (this);
  bHighlight->setIcon (highlightIcon[H_NONE]);
  bHighlight->setToolTip (tr ("Toggle highlight options"));
  bHighlight->setWhatsThis (highlightText);
  bHighlight->setPopupMode (QToolButton::InstantPopup);
  bHighlight->setMenu (highlightMenu);
  toolBar[1]->addWidget (bHighlight);


  QMenu *attrMenu = new QMenu (this);

  QActionGroup *attributeGrp = new QActionGroup (this);
  connect (attributeGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotAttrMenu (QAction *)));


  attr[0] = attrMenu->addAction (tr ("Color by depth"));
  attributeGrp->addAction (attr[0]);
  attrIcon[0] = QIcon (":/icons/color_by_depth.png");
  attr[0]->setIcon (attrIcon[0]);

  attr[1] = attrMenu->addAction (tr ("Color by number of points"));
  attributeGrp->addAction (attr[1]);
  attrIcon[1] = QIcon (":/icons/number.png");
  attr[1]->setIcon (attrIcon[1]);

  attr[2] = attrMenu->addAction (tr ("Color by total uncertainty"));
  attributeGrp->addAction (attr[2]);
  attrIcon[2] = QIcon (":/icons/uncert.png");
  attr[2]->setIcon (attrIcon[2]);

  attr[3] = attrMenu->addAction (tr ("Color by horizontal uncertainty"));
  attributeGrp->addAction (attr[3]);
  attrIcon[3] = QIcon (":/icons/horiz.png");
  attr[3]->setIcon (attrIcon[3]);

  attr[4] = attrMenu->addAction (tr ("Color by vertical uncertainty"));
  attributeGrp->addAction (attr[4]);
  attrIcon[4] = QIcon (":/icons/vert.png");
  attr[4]->setIcon (attrIcon[4]);

  bAttr = new QToolButton (this);
  bAttr->setIcon (QIcon (":/icons/color_by_depth.png"));
  bAttr->setToolTip (tr ("Select color by attribute"));
  bAttr->setWhatsThis (attrText);
  bAttr->setPopupMode (QToolButton::InstantPopup);
  bAttr->setMenu (attrMenu);
  toolBar[1]->addWidget (bAttr);


  bFlagContour = new QToolButton (this);
  bFlagContour->setIcon (QIcon (":/icons/flag_contour.png"));
  bFlagContour->setToolTip (tr ("Flag cells with hand-drawn contour data"));
  bFlagContour->setWhatsThis (flagContourText);
  bFlagContour->setCheckable (true);
  toolBar[1]->addWidget (bFlagContour);


  toolBar[1]->addSeparator ();
  toolBar[1]->addSeparator ();


  bContour = new QToolButton (this);
  misc.button[TOGGLE_CONTOUR_KEY] = bContour;
  bContour->setIcon (misc.buttonIcon[TOGGLE_CONTOUR_KEY]);
  bContour->setWhatsThis (contourText);
  bContour->setCheckable (true);
  bContour->setEnabled (false);
  toolBar[1]->addWidget (bContour);

  bInterpolated = new QToolButton (this);
  misc.button[TOGGLE_INTERPOLATED_KEY] = bInterpolated;
  bInterpolated->setIcon (misc.buttonIcon[TOGGLE_INTERPOLATED_KEY]);
  bInterpolated->setWhatsThis (interpolatedText);
  bInterpolated->setCheckable (true);
  bInterpolated->setEnabled (false);
  toolBar[1]->addWidget (bInterpolated);


  QMenu *coastMenu = new QMenu (this);

  QActionGroup *coastGrp = new QActionGroup (this);
  connect (coastGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotCoastMenu (QAction *)));

  noCoast = coastMenu->addAction (tr ("Turn off coastline display"));
  autoCoast = coastMenu->addAction (tr ("Auto coastline resolution"));
  ngaCoast = coastMenu->addAction (tr ("NGA 1:50,000 coastline"));
  wvsfCoast = coastMenu->addAction (tr ("WVS 1:250,000 coastline"));
  coastGrp->addAction (noCoast);
  coastGrp->addAction (autoCoast);
  coastGrp->addAction (ngaCoast);
  coastGrp->addAction (wvsfCoast);

  bCoast = new QToolButton (this);
  bCoast->setToolTip (tr ("Select coastline display options"));
  bCoast->setWhatsThis (coastText);
  bCoast->setPopupMode (QToolButton::InstantPopup);
  bCoast->setMenu (coastMenu);
  toolBar[1]->addWidget (bCoast);


  toolBar[1]->addSeparator ();
  toolBar[1]->addSeparator ();


  bGeotiff = new QToolButton (this);
  bGeotiff->setIcon (QIcon (":/icons/geo.png"));
  bGeotiff->setToolTip (tr ("Toggle display of the associated GeoTIFF file"));
  bGeotiff->setWhatsThis (geoText);
  connect (bGeotiff, SIGNAL (clicked ()), this, SLOT (slotGeotiff ()));
  toolBar[1]->addWidget (bGeotiff);



  toolBar[1]->addSeparator ();
  toolBar[1]->addSeparator ();


  bDisplayMinMax = new QToolButton (this);
  bDisplayMinMax->setIcon (QIcon (":/icons/display_min_max.png"));
  bDisplayMinMax->setToolTip (tr ("Flag minimum and maximum bins"));
  bDisplayMinMax->setWhatsThis (displayMinMaxText);
  bDisplayMinMax->setCheckable (true);
  toolBar[1]->addWidget (bDisplayMinMax);



  //  Create the Utilities toolbar
 
  toolBar[2] = new QToolBar (tr ("Utilities tool bar"));
  toolBar[2]->setToolTip (tr ("Utilities tool bar"));
  addToolBar (toolBar[2]);
  toolBar[2]->setObjectName (tr ("gridEdit Utilities tool bar"));

  bHigh = new QToolButton (this);
  bHigh->setIcon (QIcon (":/icons/high.png"));
  bHigh->setToolTip (tr ("Select the highest point within a rectangle"));
  bHigh->setWhatsThis (highText);
  editModeGrp->addButton (bHigh, SELECT_HIGH_POINT);
  bHigh->setCheckable (true);
  bHigh->setEnabled (false);
  toolBar[2]->addWidget (bHigh);

  bLow = new QToolButton (this);
  bLow->setIcon (QIcon (":/icons/low.png"));
  bLow->setToolTip (tr ("Select the lowest point within a rectangle"));
  bLow->setWhatsThis (lowText);
  editModeGrp->addButton (bLow, SELECT_LOW_POINT);
  bLow->setCheckable (true);
  bLow->setEnabled (false);
  toolBar[2]->addWidget (bLow);


  bPrefs = new QToolButton (this);
  bPrefs->setIcon (QIcon (":/icons/prefs.png"));
  bPrefs->setToolTip (tr ("Change application preferences"));
  bPrefs->setWhatsThis (prefsText);
  connect (bPrefs, SIGNAL (clicked ()), this, SLOT (slotPrefs ()));
  toolBar[2]->addWidget (bPrefs);


  toolBar[2]->addSeparator ();
  toolBar[2]->addSeparator ();


  bLink = new QToolButton (this);
  bLink->setIcon (QIcon (":/icons/unlink.png"));
  bLink->setToolTip (tr ("Connect to other ABE applications"));
  bLink->setWhatsThis (linkText);
  connect (bLink, SIGNAL (clicked ()), this, SLOT (slotLink ()));
  toolBar[2]->addWidget (bLink);


  toolBar[2]->addSeparator ();
  toolBar[2]->addSeparator ();


  bHelp = QWhatsThis::createAction (this);
  bHelp->setIcon (QIcon (":/icons/contextHelp.png"));
  toolBar[2]->addAction (bHelp);


  //  Set the icon sizes so that the tool bars will set up properly.

  prev_icon_size = options.main_button_icon_size;

  QSize mainButtonIconSize (options.main_button_icon_size, options.main_button_icon_size);

  for (int32_t i = 0 ; i < NUM_TOOLBARS ; i++) toolBar[i]->setIconSize (mainButtonIconSize);


  //  Get the user's defaults if available

  if (!envin (&options, &misc, this))
    {
      //  Set the geometry from defaults since we didn't get any from the saved settings.

      this->resize (misc.width, misc.height);
      this->move (misc.window_x, misc.window_y);
    }


  // Set the application font

  QApplication::setFont (options.font);


  //  Set the checked version of the coverage colors.

  int32_t h, s, v, a;
  options.cov_real_color.getHsv (&h, &s, &v, &a);
  options.cov_checked_r_color.setHsv (h, COV_CHECKED_SATURATION, v, a);
  options.cov_digitized_color.getHsv (&h, &s, &v, &a);
  options.cov_checked_d_color.setHsv (h, COV_CHECKED_SATURATION, v, a);
  options.cov_interpolated_color.getHsv (&h, &s, &v, &a);
  options.cov_checked_i_color.setHsv (h, COV_CHECKED_SATURATION, v, a);


  //  Compute the filter weights.

  double cut = 1.0 / options.filter_cutoff;
  martin (cut, options.filter_slope, options.filter_length, options.filter_weights, options.filter_type);


  //  Get the command line arguments.  We might be specifying the HSV min and max color values on the command line
  //  and we want those to override what's in the QSettings (from envin).

  extern char *optarg;
  extern int optind;
  int32_t option_index = 0;

  while (NVTrue) 
    {
      static struct option long_options[] = {{"min_hsv_color", required_argument, 0, 0},
                                             {"max_hsv_color", required_argument, 0, 0},
                                             {"min_hsv_value", required_argument, 0, 0},
                                             {"max_hsv_value", required_argument, 0, 0},
                                             {0, no_argument, 0, 0}};

      char c = (char) getopt_long (*argc, argv, "", long_options, &option_index);
      if (c == -1) break;

      switch (c) 
        {
        case 0:

          switch (option_index)
            {
            case 0:
              sscanf (optarg, "%hd", &options.min_hsv_color[0]);
              break;

            case 1:
              sscanf (optarg, "%hd", &options.max_hsv_color[0]);
              break;

            case 2:
              sscanf (optarg, "%f", &options.min_hsv_value[0]);
              options.min_hsv_locked[0] = NVTrue;
              break;

            case 3:
              sscanf (optarg, "%f", &options.max_hsv_value[0]);
              options.max_hsv_locked[0] = NVTrue;
              break;
            }
          break;
        }
    }


  //  Check the min and max colors and flip them if needed.

  if (options.max_hsv_color[0] > 315) options.max_hsv_color[0] = 315;
  if (options.min_hsv_color[0] > 315) options.min_hsv_color[0] = 0;

  if (options.max_hsv_color[0] < options.min_hsv_color[0])
    {
      uint16_t tmpu16 = options.max_hsv_color[0];
      options.max_hsv_color[0] = options.min_hsv_color[0];
      options.min_hsv_color[0] = tmpu16;
    }


  //  Allocate the surface colors.

  float hue_inc = (float) (options.max_hsv_color[0] - options.min_hsv_color[0]) / (float) (NUMHUES + 1);
  for (int32_t m = 0 ; m < 2 ; m++)
    {
      int32_t sat = 255;
      if (m) sat = 25;

      for (int32_t i = 0 ; i < NUMHUES ; i++)
        {
          for (int32_t j = 0 ; j < NUMSHADES ; j++)
            {
              options.color_array[m][i][j].setHsv ((int32_t) (((NUMHUES + 1) - (i - options.min_hsv_color[0])) * hue_inc), sat, j, 255);
            }
        }
    }


  //  Set the sun shading value based on the defaults

  options.sunopts.sun = sun_unv (options.sunopts.azimuth, options.sunopts.elevation);


  //  Set the map and cov map values from the defaults

  mapdef.projection = CYLINDRICAL_EQUIDISTANT;
  mapdef.draw_width = misc.width;
  mapdef.draw_height = misc.height;
  mapdef.overlap_percent = options.overlap_percent;
  mapdef.grid_inc_x = 0.0;
  mapdef.grid_inc_y = 0.0;

  mapdef.coasts = options.coast;
  mapdef.landmask = NVFalse;

  mapdef.border = 5;
  mapdef.coast_color = options.coast_color;
  mapdef.grid_color = options.coast_color;
  mapdef.background_color = options.background_color;


  mapdef.initial_bounds.min_x = -180.0;
  mapdef.initial_bounds.min_y = -90.0;
  mapdef.initial_bounds.max_x = 180.0;
  mapdef.initial_bounds.max_y = 90.0;


  QFrame *frame = new QFrame (this, 0);

  setCentralWidget (frame);


  //  Make the map.

  map = new nvMap (this, &mapdef);


  map->setWhatsThis (mapText);


  misc.map_widget = map;


  //  Connect to the signals from the map class.

  connect (map, SIGNAL (mousePressSignal (QMouseEvent *, double, double)), this, SLOT (slotMousePress (QMouseEvent *, double, double)));
  connect (map, SIGNAL (mouseDoubleClickSignal (QMouseEvent *, double, double)), this, SLOT (slotMouseDoubleClick (QMouseEvent *, 
                                                                                                                           double, double)));
  connect (map, SIGNAL (preliminaryMousePressSignal (QMouseEvent *)), this, SLOT (slotPreliminaryMousePress (QMouseEvent *)));
  connect (map, SIGNAL (mouseReleaseSignal (QMouseEvent *, double, double)), this, SLOT (slotMouseRelease (QMouseEvent *, double, double)));
  connect (map, SIGNAL (mouseMoveSignal (QMouseEvent *, double, double)), this, SLOT (slotMouseMove (QMouseEvent *, double, double)));
  connect (map, SIGNAL (closeSignal (QCloseEvent *)), this, SLOT (slotClose (QCloseEvent *)));
  connect (map, SIGNAL (resizeSignal (QResizeEvent *)), this, SLOT (slotResize (QResizeEvent *)));
  connect (map, SIGNAL (preRedrawSignal (NVMAP_DEF)), this, SLOT (slotPreRedraw (NVMAP_DEF)));
  connect (map, SIGNAL (postRedrawSignal (NVMAP_DEF)), this, SLOT (slotPostRedraw (NVMAP_DEF)));


  //  Layouts, what fun!

  QVBoxLayout *vBox = new QVBoxLayout (frame);


  QGroupBox *leftBox = new QGroupBox ();
  QVBoxLayout *leftBoxLayout = new QVBoxLayout ();
  leftBox->setLayout (leftBoxLayout);


  QGroupBox *covBox = new QGroupBox (tr ("Coverage"));
  QVBoxLayout *covBoxLayout = new QVBoxLayout ();
  covBox->setLayout (covBoxLayout);


  //  Make the coverage map.

  covdef.projection = CYLINDRICAL_EQUIDISTANT;
  covdef.draw_width = misc.cov_width;
  covdef.draw_height = misc.cov_height;

  covdef.coasts = NVMAP_NO_COAST;
  covdef.landmask = NVFalse;

  covdef.coast_color = options.coast_color;

  covdef.grid_inc_x = 0.0;
  covdef.grid_inc_y = 0.0;
  covdef.border = 5;
  covdef.background_color = QColor (255, 255, 255);


  covdef.initial_bounds.min_x = -180.0;
  covdef.initial_bounds.min_y = 180.0;
  covdef.initial_bounds.max_x = 90.0;
  covdef.initial_bounds.max_y = -90.0;


  cov = new nvMap (this, &covdef);
  cov->setMinimumSize (misc.cov_width, misc.cov_height);
  cov->setMaximumSize (misc.cov_width, misc.cov_height);


  cov->setWhatsThis (covText);


  //  Connect to the signals from the map class.
    
  connect (cov, SIGNAL (mousePressSignal (QMouseEvent *, double, double)), this, SLOT (slotCovMousePress (QMouseEvent *, double, double)));
  connect (cov, SIGNAL (mouseReleaseSignal (QMouseEvent *, double, double)), this, SLOT (slotCovMouseRelease (QMouseEvent *, double, double)));
  connect (cov, SIGNAL (mouseMoveSignal (QMouseEvent *, double, double)), this, SLOT (slotCovMouseMove (QMouseEvent *, double, double)));
  connect (cov, SIGNAL (closeSignal (QCloseEvent *)), this, SLOT (slotClose (QCloseEvent *)));
  connect (cov, SIGNAL (preRedrawSignal (NVMAP_DEF)), this, SLOT (slotCovPreRedraw (NVMAP_DEF)));
  connect (cov, SIGNAL (postRedrawSignal (NVMAP_DEF)), this, SLOT (slotCovPostRedraw (NVMAP_DEF)));


  //  Right click popup menu

  covPopupMenu = new QMenu (cov);

  covPopup0 = covPopupMenu->addAction (tr ("Display area"));
  connect (covPopup0, SIGNAL (triggered ()), this, SLOT (slotCovPopupMenu0 ()));
  covPopup1 = covPopupMenu->addAction (tr ("Discard rectangle"));
  connect (covPopup1, SIGNAL (triggered ()), this, SLOT (slotCovPopupMenu1 ()));
  covPopup2 = covPopupMenu->addAction (tr ("Drag rectangle"));
  connect (covPopup2, SIGNAL (triggered ()), this, SLOT (slotCovPopupMenu2 ()));
  covPopupMenu->addSeparator ();
  covPopup2 = covPopupMenu->addAction (tr ("Help"));
  connect (covPopup2, SIGNAL (triggered ()), this, SLOT (slotCovPopupHelp ()));


  cov->setCursor (Qt::WaitCursor);
  misc.cov_function = COV_START_DRAW_RECTANGLE;
  cov_area_defined = NVFalse;

  covBoxLayout->addWidget (cov);

  leftBoxLayout->addWidget (covBox);


  
  QGroupBox *chrtr2Box = new QGroupBox ();
  QVBoxLayout *chrtr2BoxLayout = new QVBoxLayout ();
  chrtr2Box->setLayout (chrtr2BoxLayout);


  chrtr2Label = new QLabel (this);
  chrtr2Label->setMaximumWidth (misc.cov_width);
  chrtr2Label->setFrameStyle (QFrame::StyledPanel | QFrame::Sunken);
  chrtr2Label->setToolTip (tr ("CHRTR2 layer name"));
  chrtr2Label->setWhatsThis (chrtr2LabelText);
  chrtr2BoxLayout->addWidget (chrtr2Label);
  leftBoxLayout->addWidget (chrtr2Box);


  //  Set up the status and CHRTR2 layer tabbed notebook

  QGroupBox *noteBox = new QGroupBox ();
  QVBoxLayout *noteBoxLayout = new QVBoxLayout ();
  noteBox->setLayout (noteBoxLayout);


  QGroupBox *statBox = new QGroupBox ();
  QGridLayout *statBoxLayout = new QGridLayout;
  statBox->setLayout (statBoxLayout);
  statBox->setMaximumWidth (misc.cov_width);


  for (int32_t i = 0 ; i < NUM_SCALE_LEVELS ; i++)
    {
      misc.scale[i] = new scaleBox (this);
      misc.scale[i]->setMinimumWidth (80);
      misc.scale[i]->setMaximumWidth (80);
      statBoxLayout->addWidget (misc.scale[i], i, 0, Qt::AlignLeft);
    }
  misc.scale[0]->setToolTip (tr ("Click here to lock or unlock the minimum value and/or change the minimum color"));
  misc.scale[0]->setWhatsThis (tr ("Click here to lock or unlock the minimum value and/or change the minimum color"));
  connect (misc.scale[0], SIGNAL (mousePressSignal (QMouseEvent *)), this, SLOT (slotMinScalePressed (QMouseEvent *)));
  misc.scale[NUM_SCALE_LEVELS - 1]->setToolTip (tr ("Click here to lock or unlock the maximum value and/or change the maximum color"));
  misc.scale[NUM_SCALE_LEVELS - 1]->setWhatsThis (tr ("Click here to lock or unlock the maximum value and/or change the maximum color"));
  connect (misc.scale[NUM_SCALE_LEVELS - 1], SIGNAL (mousePressSignal (QMouseEvent *)), this, SLOT (slotMaxScalePressed (QMouseEvent *)));


  QLabel *latName = new QLabel (" Lat: ", this);
  latName->setToolTip (tr ("Latitude of cursor"));
  latName->setMinimumSize (latName->sizeHint ());
  statBoxLayout->addWidget (latName, 0, 1, Qt::AlignLeft);

  latLabel = new QLabel (" N 99 99 99.99 ", this);
  latLabel->setToolTip (tr ("Latitude of cursor"));
  latLabel->setMinimumSize (latLabel->sizeHint ());
  statBoxLayout->addWidget (latLabel, 0, 2, Qt::AlignLeft);

  QLabel *lonName = new QLabel (" Lon: ", this);
  lonName->setToolTip (tr ("Longitude of cursor"));
  lonName->setMinimumSize (lonName->sizeHint ());
  statBoxLayout->addWidget (lonName, 1, 1, Qt::AlignLeft);

  lonLabel = new QLabel (" W 999 99 99.99 ", this);
  lonLabel->setToolTip (tr ("Longitude of cursor"));
  lonLabel->setMinimumSize (lonLabel->sizeHint ());
  statBoxLayout->addWidget (lonLabel, 1, 2, Qt::AlignLeft);

  QLabel *numName = new QLabel (" #: ", this);
  numName->setToolTip (tr ("Number of points in bin"));
  numName->setMinimumSize (numName->sizeHint ());
  statBoxLayout->addWidget (numName, 2, 1, Qt::AlignLeft);

  numLabel = new QLabel (" 00000 ", this);
  numLabel->setToolTip (tr ("Number of points in bin"));
  numLabel->setMinimumSize (numLabel->sizeHint ());
  statBoxLayout->addWidget (numLabel, 2, 2, Qt::AlignLeft);

  QLabel *zName = new QLabel (" Z: ", this);
  zName->setToolTip (tr ("Z value for bin"));
  zName->setMinimumSize (zName->sizeHint ());
  statBoxLayout->addWidget (zName, 3, 1, Qt::AlignLeft);

  zLabel = new QLabel (" 0000.00 ", this);
  zLabel->setToolTip (tr ("Z value for bin"));
  zLabel->setMinimumSize (zLabel->sizeHint ());
  zLabel->setAutoFillBackground (true);
  zPalette = zLabel->palette ();
  misc.widgetBackgroundColor = zPalette.color (QPalette::Normal, QPalette::Window);
  statBoxLayout->addWidget (zLabel, 3, 2, Qt::AlignLeft);

  QLabel *hvName = new QLabel (" H / V: ", this);
  hvName->setToolTip (tr ("Horizontal/Vertical uncertainty of current point"));
  hvName->setMinimumSize (hvName->sizeHint ());
  hvName->setMaximumSize (hvName->sizeHint ());
  statBoxLayout->addWidget (hvName, 4, 1, Qt::AlignLeft);

  hvLabel = new QLabel (" 000.00 ", this);
  hvLabel->setToolTip (tr ("Horizontal/Vertical uncertainty of current point"));
  hvLabel->setMinimumSize (hvLabel->sizeHint ());
  hvLabel->setAutoFillBackground (true);
  statBoxLayout->addWidget (hvLabel, 4, 2, Qt::AlignLeft);


  QLabel *uncName = new QLabel (" Uncertainty: ", this);
  uncName->setToolTip (tr ("Total uncertainty of current point"));
  uncName->setMinimumSize (uncName->sizeHint ());
  uncName->setMaximumSize (uncName->sizeHint ());
  statBoxLayout->addWidget (uncName, 5, 1, Qt::AlignLeft);

  uncLabel = new QLabel (" 000.00 ", this);
  uncLabel->setToolTip (tr ("Total uncertainty of current point"));
  uncLabel->setMinimumSize (uncLabel->sizeHint ());
  uncLabel->setAutoFillBackground (true);
  statBoxLayout->addWidget (uncLabel, 5, 2, Qt::AlignLeft);


  QLabel *tName = new QLabel (" Type: ", this);
  tName->setToolTip (tr ("Real (R), Drawn/Digitized (D), Interpolated (I), or Null (N)"));
  tName->setMinimumSize (tName->sizeHint ());
  statBoxLayout->addWidget (tName, 6, 1, Qt::AlignLeft);

  tLabel = new QLabel (" (R) ", this);
  tLabel->setToolTip (tr ("Real (R), Drawn/Digitized (D), Interpolated (I), or Null (N)"));
  tLabel->setMinimumSize (tLabel->sizeHint ());
  tLabel->setAutoFillBackground (true);
  statBoxLayout->addWidget (tLabel, 6, 2, Qt::AlignLeft);

  QLabel *coordXName = new QLabel (" CoordX: ", this);
  coordXName->setToolTip (tr ("X grid coordinate of current point"));
  coordXName->setMinimumSize (coordXName->sizeHint ());
  coordXName->setMaximumSize (coordXName->sizeHint ());
  statBoxLayout->addWidget (coordXName, 7, 1, Qt::AlignLeft);

  coordXLabel = new QLabel (" 00000 ", this);
  coordXLabel->setToolTip (tr ("X grid coordinate of current point"));
  coordXLabel->setMinimumSize (coordXLabel->sizeHint ());
  coordXLabel->setAutoFillBackground (true);
  statBoxLayout->addWidget (coordXLabel, 7, 2, Qt::AlignLeft);

  QLabel *coordYName = new QLabel (" CoordY: ", this);
  coordYName->setToolTip (tr ("Y grid coordinate of current point"));
  coordYName->setMinimumSize (coordYName->sizeHint ());
  coordYName->setMaximumSize (coordYName->sizeHint ());
  statBoxLayout->addWidget (coordYName, 8, 1, Qt::AlignLeft);

  coordYLabel = new QLabel (" 00000 ", this);
  coordYLabel->setToolTip (tr ("Y grid coordinate of current point"));
  coordYLabel->setMinimumSize (coordYLabel->sizeHint ());
  coordYLabel->setAutoFillBackground (true);
  statBoxLayout->addWidget (coordYLabel, 8, 2, Qt::AlignLeft);

  xSizeName = new QLabel (" X grid: ", this);
  xSizeName->setToolTip (tr ("X grid size in meters"));
  xSizeName->setMinimumSize (xSizeName->sizeHint ());
  statBoxLayout->addWidget (xSizeName, 9, 1, Qt::AlignLeft);

  xSizeLabel = new QLabel (" 0000.00 ", this);
  xSizeLabel->setToolTip (tr ("X grid size in meters"));
  xSizeLabel->setMinimumSize (xSizeLabel->sizeHint ());
  xSizeLabel->setAutoFillBackground (true);
  statBoxLayout->addWidget (xSizeLabel, 9, 2, Qt::AlignLeft);

  ySizeName = new QLabel (" Y grid: ", this);
  ySizeName->setToolTip (tr ("Y grid size in meters"));
  ySizeName->setMinimumSize (ySizeName->sizeHint ());
  statBoxLayout->addWidget (ySizeName, 10, 1, Qt::AlignLeft);

  ySizeLabel = new QLabel (" 0000.00 ", this);
  ySizeLabel->setToolTip (tr ("Y grid size in meters"));
  ySizeLabel->setMinimumSize (ySizeLabel->sizeHint ());
  ySizeLabel->setAutoFillBackground (true);
  statBoxLayout->addWidget (ySizeLabel, 10, 2, Qt::AlignLeft);


  statBoxLayout->setColumnStretch (1, 1);
  statBoxLayout->setRowStretch (NUM_SCALE_LEVELS + 1, 1);


  notebook = new QTabWidget ();
  notebook->setTabPosition (QTabWidget::North);


  notebook->addTab (statBox, tr ("Status"));


  QGroupBox *layerBox = new QGroupBox ();
  QGridLayout *layerBoxLayout = new QGridLayout;
  layerBox->setLayout (layerBoxLayout);
  layerBox->setMaximumWidth (misc.cov_width);
  layerBoxLayout->setColumnStretch (2, 1);


  QButtonGroup *layerDisplayGrp = new QButtonGroup (this);
  layerDisplayGrp->setExclusive (false);
  connect (layerDisplayGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotDisplayLayerClicked (int)));

  QButtonGroup *layerLockGrp = new QButtonGroup (this);
  layerLockGrp->setExclusive (false);
  connect (layerLockGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotLockLayerClicked (int)));


  for (int32_t i = 0 ; i < MAX_CHRTR2_FILES ; i++)
    {
      layerDisplay[i] = new QPushButton (this);
      layerDisplay[i]->setIcon (QIcon (":/icons/display_layer.png"));
      layerDisplay[i]->setCheckable (true);
      layerDisplay[i]->setToolTip (tr ("Click to hide layer"));
      layerDisplay[i]->setWhatsThis (layerDisplayText);
      layerDisplayGrp->addButton (layerDisplay[i], i);
      layerBoxLayout->addWidget (layerDisplay[i], i, 0, Qt::AlignLeft);

      layerLock[i] = new QPushButton (this);
      layerLock[i]->setIcon (QIcon (":/icons/unlock.png"));
      layerLock[i]->setCheckable (true);
      layerLock[i]->setToolTip (tr ("Click to lock layer"));
      layerLock[i]->setWhatsThis (layerLockText);
      layerLockGrp->addButton (layerLock[i], i);
      layerBoxLayout->addWidget (layerLock[i], i, 1, Qt::AlignLeft);

      layerName[i] = new QLabel (this);
      layerBoxLayout->addWidget (layerName[i], i, 2, Qt::AlignLeft);

      layerDisplay[i]->hide ();
      layerLock[i]->hide ();
      layerName[i]->hide ();
    }
 
  layerBoxLayout->setRowStretch (MAX_CHRTR2_FILES - 1, 1);


  notebook->addTab (layerBox, tr ("Layers"));


  leftBoxLayout->addWidget (notebook, 1);


  QHBoxLayout *hBox = new QHBoxLayout ();
  hBox->addWidget (leftBox);
  hBox->addWidget (map, 1);
  vBox->addLayout (hBox);


  QGroupBox *progBox = new QGroupBox ();
  QHBoxLayout *progBoxLayout = new QHBoxLayout;
  progBox->setLayout (progBoxLayout);


  misc.statusProgLabel = new QLabel (this);
  misc.statusProgLabel->setFrameStyle (QFrame::StyledPanel | QFrame::Sunken);
  misc.statusProgLabel->setToolTip (tr ("Active mode"));
  misc.statusProgLabel->setWhatsThis (miscLabelText);
  misc.statusProgLabel->setAutoFillBackground (true);
  misc.statusProgPalette = misc.statusProgLabel->palette ();
  progBoxLayout->addWidget (misc.statusProgLabel);


  //  Progress status bar

  misc.progStatus = new QStatusBar ();
  misc.progStatus->setSizeGripEnabled (false);
  misc.progStatus->setToolTip (tr ("Progress bar and label"));
  misc.progStatus->setWhatsThis (progStatusText);
  progBoxLayout->addWidget (misc.progStatus);


  misc.statusProg = new QProgressBar (this);
  misc.statusProg->setRange (0, 100);
  misc.statusProg->setValue (0);
  misc.statusProg->setTextVisible (false);
  misc.progStatus->addWidget (misc.statusProg, 10);


  vBox->addWidget (progBox);


  //  Set View toolbar button states based on saved options

  QString depthString;
  depthString.sprintf (tr ("Highlight data inside depth range (%0.1f to %0.1f)").toLatin1 (), options.highlight_depth[0], options.highlight_depth[1]);
  highlightInside->setText (depthString);
  depthString.sprintf (tr ("Highlight data outside depth range (%0.1f to %0.1f)").toLatin1 (), options.highlight_depth[0], options.highlight_depth[1]);
  highlightOutside->setText (depthString);

  bHighlight->setIcon (highlightIcon[options.highlight]);

  bFlagContour->setChecked (options.flag_contour);
  connect (bFlagContour, SIGNAL (clicked ()), this, SLOT (slotFlagContour ()));

  bContour->setChecked (options.contour);
  connect (bContour, SIGNAL (clicked ()), this, SLOT (slotContour ()));

  bInterpolated->setChecked (options.display_interpolated);
  connect (bInterpolated, SIGNAL (clicked ()), this, SLOT (slotInterpolated ()));

  bDisplayMinMax->setChecked (options.display_minmax);
  connect (bDisplayMinMax, SIGNAL (clicked ()), this, SLOT (slotDisplayMinMax ()));

  tip.sprintf (tr (" Set rectangle fill mode (%.2f) ").toLatin1 (), options.draw_contour_level);
  bFillRect->setToolTip (tip);

  tip.sprintf (tr (" Set polygon fill mode (%.2f) ").toLatin1 (), options.draw_contour_level);
  bFillPoly->setToolTip (tip);

  tip.sprintf (tr ("Draw and insert a contour (%.2f)").toLatin1 (), options.draw_contour_level);
  bDrawContour->setToolTip (tip);


  //  Set all of the button hotkey tooltips and shortcuts

  for (int32_t i = 0 ; i < HOTKEYS ; i++) slotPrefHotKeyChanged (i);


  //  Set the edit function from the defaults

  misc.function = options.edit_mode;
  misc.save_function = misc.last_edit_function = misc.function;


  //  Right click popup menu

  popupMenu = new QMenu (map);

  popup0 = popupMenu->addAction (tr ("Close polygon and filter area"));
  connect (popup0, SIGNAL (triggered ()), this, SLOT (slotPopupMenu0 ()));
  popup1 = popupMenu->addAction (tr ("Set filter standard deviation (2.4)"));
  connect (popup1, SIGNAL (triggered ()), this, SLOT (slotPopupMenu1 ()));
  popup2 = popupMenu->addAction (tr ("Set deep filter only (Yes)"));
  connect (popup2, SIGNAL (triggered ()), this, SLOT (slotPopupMenu2 ()));
  popup3 = popupMenu->addAction (tr ("Discard polygon"));
  connect (popup3, SIGNAL (triggered ()), this, SLOT (slotPopupMenu3 ()));
  popup4 = popupMenu->addAction (tr ("Clear all filter masks"));
  connect (popup4, SIGNAL (triggered ()), this, SLOT (slotPopupMenu4 ()));
  popupMenu->addSeparator ();
  popupHelp = popupMenu->addAction (tr ("Help"));
  connect (popupHelp, SIGNAL (triggered ()), this, SLOT (slotPopupHelp ()));


  //  The following menus are in the menu bar.

  //  Setup the file menu.

  QAction *fileOpenAction = new QAction (tr ("Open"), this);
  fileOpenAction->setStatusTip (tr ("Open CHRTR2 file"));
  fileOpenAction->setWhatsThis (openText);
  connect (fileOpenAction, SIGNAL (triggered ()), this, SLOT (slotOpen ()));


  recentMenu = new QMenu (tr ("Open Recent..."));

  recentGrp = new QActionGroup (this);
  connect (recentGrp, SIGNAL (triggered (QAction *)), this, SLOT (slotOpenRecent (QAction *)));

  for (int32_t i = 0 ; i < MAX_RECENT ; i++)
    {
      recentFileAction[i] = recentMenu->addAction (options.recentFile[i]);
      recentGrp->addAction (recentFileAction[i]);
      if (i < options.recent_file_count)
        {
          recentFileAction[i]->setVisible (true);
        }
      else
        {
          recentFileAction[i]->setVisible (false);
        }
    }


  fileCloseAction = new QAction (tr ("Close CHRTR2 file"), this);
  fileCloseAction->setStatusTip (tr ("Close top level/layer CHRTR2 file"));
  fileCloseAction->setWhatsThis (closeCHRTR2Text);
  fileCloseAction->setEnabled (false);
  connect (fileCloseAction, SIGNAL (triggered ()), this, SLOT (slotCloseCHRTR2 ()));

  geotiffOpenAction = new QAction (tr ("Open GeoTIFF file"), this);
  geotiffOpenAction->setStatusTip (tr ("Open a GeoTIFF file to be displayed with the CHRTR2 data"));
  geotiffOpenAction->setWhatsThis (geotiffText);
  geotiffOpenAction->setEnabled (false);
  connect (geotiffOpenAction, SIGNAL (triggered ()), this, SLOT (slotOpenGeotiff ()));


  QAction *fileQuitAction = new QAction (tr ("&Quit"), this);
  fileQuitAction->setShortcut (tr ("Ctrl+Q"));
  fileQuitAction->setStatusTip (tr ("Exit from application"));
  connect (fileQuitAction, SIGNAL (triggered ()), this, SLOT (slotQuit ()));

  QMenu *fileMenu = menuBar ()->addMenu (tr ("File"));
  fileMenu->addAction (fileOpenAction);
  fileMenu->addMenu (recentMenu);
  fileMenu->addAction (fileCloseAction);
  fileMenu->addSeparator ();
  fileMenu->addAction (geotiffOpenAction);
  fileMenu->addSeparator ();
  fileMenu->addAction (fileQuitAction);


  //  Setup the edit menu.

  QActionGroup *editGrp = new QActionGroup (this);
  connect (editGrp, SIGNAL (triggered (QAction*)), this, SLOT (slotCheckMenu (QAction *)));


  fileChecked = new QAction (tr ("Set File Checked"), this);
  fileChecked->setStatusTip (tr ("Set file checked"));
  fileChecked->setWhatsThis (fileCheckedText);
  editGrp->addAction (fileChecked);


  fileUnchecked = new QAction (tr ("Set File Unchecked"), this);
  fileUnchecked->setStatusTip (tr ("Set file unchecked"));
  fileUnchecked->setWhatsThis (fileUncheckedText);
  editGrp->addAction (fileUnchecked);


  QAction *prefsAct = new QAction (tr ("Preferences"), this);
  prefsAct->setStatusTip (tr ("Change program preferences"));
  prefsAct->setWhatsThis (prefsText);
  connect (prefsAct, SIGNAL (triggered ()), this, SLOT (slotPrefs ()));


  editMenu = menuBar ()->addMenu (tr ("Edit"));
  editMenu->addAction (fileChecked);
  editMenu->addAction (fileUnchecked);
  editMenu->addSeparator ();
  editMenu->addAction (prefsAct);


  //  Setup the Utilities menu.

  QAction *overlay = new QAction (tr ("Select Overlays"), this);
  overlay->setStatusTip (tr ("Select overlays"));
  overlay->setWhatsThis (selectOverlaysText);
  connect (overlay, SIGNAL (triggered ()), this, SLOT (slotOverlays ()));


  start3D = new QAction (tr ("Start &3D viewer"), this);
  start3D->setShortcut (tr ("3"));
  start3D->setStatusTip (tr ("Start the 3D viewer"));
  start3D->setWhatsThis (start3DText);
  connect (start3D, SIGNAL (triggered ()), this, SLOT (slotStart3DViewer ()));


  exportImage = new QAction (tr ("Export Image"), this);
  exportImage->setStatusTip (tr ("Export an image file of the displayed data"));
  exportImage->setWhatsThis (exportImageText);
  connect (exportImage, SIGNAL (triggered ()), this, SLOT (slotExportImage ()));


  QAction *displayCHRTR2Header = new QAction (tr ("Display CHRTR2 Header"), this);
  displayCHRTR2Header->setStatusTip (tr ("Display the CHRTR2 bin file header"));
  displayCHRTR2Header->setWhatsThis (displayCHRTR2HeaderText);
  connect (displayCHRTR2Header, SIGNAL (triggered ()), this, SLOT (slotDisplayHeader ()));


  QAction *outputDataPoints = new QAction (tr ("Output Data Points File"), this);
  outputDataPoints->setStatusTip (tr ("Output a data points file"));
  outputDataPoints->setWhatsThis (outputDataPointsText);
  connect (outputDataPoints, SIGNAL (triggered ()), this, SLOT (slotOutputDataPoints ()));


  QAction *zoomToArea = new QAction (tr ("Zoom To Area File"), this);
  zoomToArea->setStatusTip (tr ("Zoom in view to selected area file"));
  zoomToArea->setWhatsThis (zoomToAreaText);
  connect (zoomToArea, SIGNAL (triggered ()), this, SLOT (slotZoomToArea ()));


  QAction *defineRectArea = new QAction (tr ("Define Rectangular Area File"), this);
  defineRectArea->setStatusTip (tr ("Define a rectangular area and output to a file"));
  defineRectArea->setWhatsThis (defineRectAreaText);
  connect (defineRectArea, SIGNAL (triggered ()), this, SLOT (slotDefineRectArea ()));


  QAction *definePolyArea = new QAction (tr ("Define Polygonal Area File"), this);
  definePolyArea->setStatusTip (tr ("Define a polygonal area and output to a file"));
  definePolyArea->setWhatsThis (definePolyAreaText);
  connect (definePolyArea, SIGNAL (triggered ()), this, SLOT (slotDefinePolyArea ()));


  utilitiesMenu = menuBar ()->addMenu (tr ("Utilities"));
  utilitiesMenu->addAction (start3D);
  utilitiesMenu->addSeparator ();
  utilitiesMenu->addAction (overlay);
  utilitiesMenu->addSeparator ();
  utilitiesMenu->addAction (displayCHRTR2Header);
  utilitiesMenu->addSeparator ();
  utilitiesMenu->addAction (exportImage);
  utilitiesMenu->addAction (outputDataPoints);
  utilitiesMenu->addSeparator ();
  utilitiesMenu->addAction (zoomToArea);
  utilitiesMenu->addSeparator ();
  utilitiesMenu->addAction (defineRectArea);
  utilitiesMenu->addAction (definePolyArea);


  //  Setup the Layers menu.

  QActionGroup *layerGrp = new QActionGroup (this);
  connect (layerGrp, SIGNAL (triggered (QAction*)), this, SLOT (slotLayerGrpTriggered (QAction *)));

  for (int32_t i = 0 ; i < MAX_CHRTR2_FILES ; i++)
    {
      QString layerString;
      layerString.sprintf (tr ("Layer %d").toLatin1 (), i + 1);
      layer[i] = new QAction (layerString, this);
      layerString.sprintf (tr ("Move layer %d to the top level").toLatin1 (), i);
      layer[i]->setStatusTip (layerString);
      layer[i]->setWhatsThis (layerText);
      layerGrp->addAction (layer[i]);
      layer[i]->setVisible (false);
    }


  QAction *layerPrefs = new QAction (tr ("Manage Layers"), this);
  layerPrefs->setStatusTip (tr ("Modify the preferences for each layer"));
  layerPrefs->setWhatsThis (layerPrefsText);
  connect (layerPrefs, SIGNAL (triggered ()), this, SLOT (slotLayers ()));


  layerMenu = menuBar ()->addMenu (tr ("Layers"));
  for (int32_t i = 0 ; i < MAX_CHRTR2_FILES ; i++) layerMenu->addAction (layer[i]);
  layerMenu->addSeparator ();
  layerMenu->addAction (layerPrefs);


  //  Setup the help menu.

  QAction *toolHelp = new QAction (tr ("Tool bars"), this);
  toolHelp->setStatusTip (tr ("Help on tool bars"));
  connect (toolHelp, SIGNAL (triggered ()), this, SLOT (slotToolbarHelp ()));

  QAction *whatsThisAct = QWhatsThis::createAction (this);
  whatsThisAct->setIcon (QIcon (":/icons/contextHelp.png"));

  QAction *aboutAct = new QAction (tr ("About"), this);
  aboutAct->setStatusTip (tr ("Information about gridEdit"));
  connect (aboutAct, SIGNAL (triggered ()), this, SLOT (about ()));

  QAction *acknowledgments = new QAction (tr ("Acknowledgments"), this);
  acknowledgments->setStatusTip (tr ("Information about supporting libraries"));
  connect (acknowledgments, SIGNAL (triggered ()), this, SLOT (slotAcknowledgments ()));

  QAction *aboutQtAct = new QAction (tr ("About Qt"), this);
  aboutQtAct->setStatusTip (tr ("Information about Qt"));
  connect (aboutQtAct, SIGNAL (triggered ()), this, SLOT (aboutQt ()));

  QMenu *helpMenu = menuBar ()->addMenu (tr ("Help"));
  helpMenu->addAction (toolHelp);
  helpMenu->addSeparator ();
  helpMenu->addAction (whatsThisAct);
  helpMenu->addSeparator ();
  helpMenu->addAction (aboutAct);
  helpMenu->addSeparator ();
  helpMenu->addAction (acknowledgments);
  helpMenu->addAction (aboutQtAct);


  //  Get the CHRTR2 shared memory area.  If it exists, use it.  The key is the process ID plus "_chrtr2".  

  QString skey;
  skey.sprintf ("%d_chrtr2", misc.process_id);

  misc.chrtr2Share = new QSharedMemory (skey);

  if (!misc.chrtr2Share->create (sizeof (CHRTR2_SHARE), QSharedMemory::ReadWrite)) misc.chrtr2Share->attach (QSharedMemory::ReadWrite);

  misc.chrtr2_share = (CHRTR2_SHARE *) misc.chrtr2Share->data ();


  //  Clear the shared memory area and set the ppid key.

  memset (misc.chrtr2_share, 0, sizeof (CHRTR2_SHARE));
  misc.chrtr2_share->ppid = misc.process_id;

  misc.chrtr2_share->chrtr2_count = 0;


  //  Set a couple of things that gridPointEdit3D will need to know.

  misc.chrtr2_share->settings_changed = NVFalse;
  misc.chrtr2_share->zoom_requested = NVFalse;
  misc.chrtr2_share->position_form = options.position_form;
  misc.chrtr2_share->smoothing_factor = options.smoothing_factor;
  misc.chrtr2_share->z_factor = options.z_factor;
  misc.chrtr2_share->z_offset = options.z_offset;


  //  Move stuff from options to share.

  misc.chrtr2_share->cint = options.cint;
  misc.chrtr2_share->num_levels = options.num_levels;
  for (int32_t i = 0 ; i < options.num_levels ; i++) misc.chrtr2_share->contour_levels[i] = options.contour_levels[i];


  //  Set the toolbar buttons and the cursor.

  setMainButtons (NVFalse);
  setFunctionCursor (misc.function);


  //  New feature message.  Increment the number when you change the message and append the old message (in startupMessage.cpp) to new_features.txt

  new_feature = 2;
  startup = NVTrue;

  if (options.new_feature < new_feature) options.startup_message = NVTrue;


  //  Show the widget so that command line file open will work.

  show ();


  //  If there was a filename on the command line...

  if (*argc > 1)
    {
      commandFile = QFileInfo (QString (argv[optind])).absoluteFilePath ();
      command_file_flag = NVTrue;

      slotOpen ();
    }


  //  Set the tracking timer function.  This is used to track the cursor when it is in other CHRTR2 or ABE program windows.

  trackCursor = new QTimer (this);
  connect (trackCursor, SIGNAL (timeout ()), this, SLOT (slotTrackCursor ()));
  trackCursor->start (50);
}



gridEdit::~gridEdit ()
{
}



//  This is where we link to or unlink from other (non-parent or child) ABE programs.

void 
gridEdit::slotLink ()
{
  //  If we're linked, unlink.

  if (misc.linked)
    {
      //  Unregister this application.

      unregisterABE (misc.abeRegister, misc.abe_register, abe_register_group, misc.process_id);


      abe_register_group = -1;


      bLink->setToolTip (tr ("Connect to other ABE applications"));
      bLink->setWhatsThis (linkText);
      bLink->setIcon (QIcon (":/icons/unlink.png"));


      //  Unset the link flag.

      misc.linked = NVFalse;


      //  Remove the group id from the title

      QString title = "gridEdit : " + QString (misc.chrtr2_share->chrtr2_file[0]);
      this->setWindowTitle (title);


      setMainButtons (NVTrue);
    }
  else
    {
      //  Get the ABE_REGISTER shared memory area.

      registerABE *registerABEDialog = new registerABE (this, "gridEdit", misc.process_id, misc.chrtr2_share->chrtr2_file[0],
                                                        &misc.abeRegister, &misc.abe_register);


      //  Link to a group (or create a new group).

      connect (registerABEDialog, SIGNAL (keySetSignal (int32_t)), this, SLOT (slotRegisterABEKeySet (int32_t)));
    }
}



// Callback from the link/unlink slot.

void 
gridEdit::slotRegisterABEKeySet (int32_t key)
{
  if (key >= 0)
    {
      abe_register_group = key;


      misc.linked = NVTrue;


      QString title;
      title.sprintf ("gridEdit (ABE group %02d) : ", abe_register_group);
      title += QString (misc.chrtr2_share->chrtr2_file[0]);
      this->setWindowTitle (title);


      bLink->setToolTip (tr ("Disconnect from other ABE applications"));
      bLink->setWhatsThis (unlinkText);
      bLink->setIcon (QIcon (":/icons/link.png"));
    }
}



//  Enable or disable all of the main buttons.

void 
gridEdit::setMainButtons (uint8_t enabled)
{
  //  When we disable the main buttons we want to make sure that mouse presses don't work.

  if (enabled)
    {
      process_running = NVFalse;
    }
  else
    {
      process_running = NVTrue;
    }


  //  If we changed the icon size in the preferences dialog we need to resize all of the buttons.

  if (options.main_button_icon_size != prev_icon_size)
    {
      QSize mainButtonIconSize (options.main_button_icon_size, options.main_button_icon_size);

      for (int32_t i = 0 ; i < NUM_TOOLBARS ; i++)
        {
          toolBar[i]->setIconSize (mainButtonIconSize);
          toolBar[i]->adjustSize ();
        }

      prev_icon_size = options.main_button_icon_size;
    }


  //  The stop button is only enabled during drawing

  if (cov_area_defined) bStop->setEnabled (!enabled);


  //  Only enable the layers menu if we have more than one layer.

  if (misc.chrtr2_share->chrtr2_count > 1)
    {
      layerMenu->setEnabled (enabled);
    }
  else
    {
      layerMenu->setEnabled (false);
    }


  //  Only enable the Undo button if we have undo data.

  if (misc.undo_count)
    {
      bUndo->setEnabled (enabled);
    }
  else
    {
      bUndo->setEnabled (false);
    }


  //  Only enable the clear drawn contours buttons if we have drawn contours.

  if (misc.hd_contour_count)
    {
      bClearContour->setEnabled (enabled);
      bClear->setEnabled (enabled);
    }
  else
    {
      bClearContour->setEnabled (false);
      bClear->setEnabled (false);
    }

  if (misc.chrtr2_share->chrtr2_count)
    {
      editMenu->setEnabled (enabled);
      utilitiesMenu->setEnabled (enabled);
    }
  else
    {
      editMenu->setEnabled (false);
      utilitiesMenu->setEnabled (false);
    }


  bHighlight->setEnabled (enabled);
  bContour->setEnabled (enabled);
  bFlagContour->setEnabled (enabled);
  bInterpolated->setEnabled (enabled);
  bCoast->setEnabled (enabled);
  fileCloseAction->setEnabled (enabled);
  geotiffOpenAction->setEnabled (enabled);

  switch (options.coast)
    {
    case NVMAP_NO_COAST:
      bCoast->setIcon (QIcon (":/icons/no_coast.png"));
      break;

    case NVMAP_AUTO_COAST:
      bCoast->setIcon (QIcon (":/icons/auto_coast.png"));
      break;

    case NVMAP_NGA_50K_COAST:
      bCoast->setIcon (QIcon (":/icons/nga_coast.png"));
      break;

    case NVMAP_WVS_FULL_COAST:
      bCoast->setIcon (QIcon (":/icons/wvs_coast.png"));
      break;
    }


  //  Only enable the GeoTIFF display button if we have opened a GeoTIFF file

  if (enabled && misc.GeoTIFF_open)
    {
      bGeotiff->setEnabled (enabled);
    }
  else
    {
      bGeotiff->setEnabled (false);
    }


  //  Only enable the zoom out button if we've zoomed in.

  mapdef = map->getMapdef ();
  if (enabled && mapdef.zoom_level > 0)
    {
      bZoomOut->setEnabled (enabled);
    }
  else
    {
      bZoomOut->setEnabled (false);
    }

  bZoomIn->setEnabled (enabled);
  bPrefs->setEnabled (enabled);
  bEditRect3D->setEnabled (enabled);
  bEditPoly3D->setEnabled (enabled);
  bDeleteRect->setEnabled (enabled);
  bDeletePoly->setEnabled (enabled);
  bDeleteContourPoly->setEnabled (enabled);
  bFillRect->setEnabled (enabled);
  bFillPoly->setEnabled (enabled);
  bFillPolyPFM->setEnabled (enabled);
  bDisplayMinMax->setEnabled (enabled);
  bRedraw->setEnabled (enabled);
  bRedrawCoverage->setEnabled (enabled);
  bLink->setEnabled (enabled);
  bSetChecked->setEnabled (enabled);


  bFilterRect->setEnabled (enabled);
  bFilterPoly->setEnabled (enabled);
  bFilterRectMask->setEnabled (enabled);
  bFilterPolyMask->setEnabled (enabled);


  //  Only enable the clear filter mask button if we have active filter masks.

  if (!misc.poly_filter_mask_count)
    {
      bClearMasks->setEnabled (false);
    }
  else
    {
      bClearMasks->setEnabled (enabled);
    }


  bDrawContour->setEnabled (enabled);
  bGrabContour->setEnabled (enabled);
  bRemisp->setEnabled (enabled);


  bHigh->setEnabled (enabled);
  bLow->setEnabled (enabled);


  bAttr->setEnabled (enabled);


  //  Set the button states based on the active function.

  switch (misc.function)
    {
    case RECT_EDIT_AREA_3D:
      bEditRect3D->setChecked (true);
      break;

    case POLY_EDIT_AREA_3D:
      bEditPoly3D->setChecked (true);
      break;

    case RECT_DELETE_AREA:
      bDeleteRect->setChecked (true);
      break;

    case POLY_DELETE_AREA:
      bDeletePoly->setChecked (true);
      break;

    case POLY_DELETE_CONTOUR:
      bDeleteContourPoly->setChecked (true);
      break;

    case RECT_FILL_AREA:
      bFillRect->setChecked (true);
      break;

    case POLY_FILL_AREA:
      bFillPoly->setChecked (true);
      break;

    case POLY_FILL_PFM:
      bFillPolyPFM->setChecked (true);
      break;

    case RECT_FILTER_AREA:
      bFilterRect->setChecked (true);
      break;

    case POLY_FILTER_AREA:
      bFilterPoly->setChecked (true);
      break;

    case RECT_FILTER_MASK:
      bFilterRectMask->setChecked (true);
      break;

    case POLY_FILTER_MASK:
      bFilterPolyMask->setChecked (true);
      break;

    case DRAW_CONTOUR:
      bDrawContour->setChecked (true);
      break;

    case GRAB_CONTOUR:
      bGrabContour->setChecked (true);
      break;

    case CLEAR_CONTOUR:
      bClearContour->setChecked (true);
      break;

    case REMISP_AREA:
      bRemisp->setChecked (true);
      break;

    case SELECT_HIGH_POINT:
      bHigh->setChecked (true);
      break;

    case SELECT_LOW_POINT:
      bLow->setChecked (true);
      break;
    }
}



//  Clear all movable objects.

void 
gridEdit::discardMovableObjects ()
{
  map->closeMovingPath (&mv_arrow);
  map->closeMovingPath (&mv_marker);
  map->closeMovingPath (&mv_tracker);
  map->discardRubberbandRectangle (&rb_rectangle);
  map->discardRubberbandPolygon (&rb_polygon);
}



//  Left mouse press.  Called from map mouse press callback.

void 
gridEdit::leftMouse (int32_t mouse_x __attribute__ ((unused)), int32_t mouse_y __attribute__ ((unused)), double lon, double lat)
{
  void filterPolyArea (OPTIONS *options, MISC *misc, double *mx, double *my, int32_t count);
  uint8_t setHighLow (double *mx, double *my, MISC *misc, OPTIONS *options, nvMap *map);
  void writeContour (MISC *misc, OPTIONS *options, int32_t count, double *cur_x, double *cur_y);


  int32_t             count, *px, *py, ndx;
  double              *mx, *my, *cur_x, *cur_y;
  float               value;
  NV_F64_XYMBR        bounds;
  QFileDialog         *fd;
  QString             file, string;
  FILE                *fp;
  char                fname[512], ltstring[25], lnstring[25], hem;
  double              deg, min, sec;
  uint8_t             hit;
  CHRTR2_RECORD       chrtr2_record;


  //  If the popup menu is up discard this mouse press

  if (popup_active)
    {
      popup_active = NVFalse;
      return;
    }


  //  Actions based on the active function.  For the most part, if a rubberband rectangle or polygon is active then
  //  this is the second mouse press and we want to perform the operation.  If one isn't present then it's the
  //  first mouse click and we want to start (anchor) a rectangle, line, or polygon.

  switch (misc.function)
    {
    case ZOOM_IN_AREA:

      if (map->rubberbandRectangleIsActive (rb_rectangle))
        {
          map->closeRubberbandRectangle (rb_rectangle, lon, lat, &px, &py, &mx, &my);

          bounds.min_x = 999999.0;
          bounds.min_y = 999999.0;
          bounds.max_x = -999999.0;
          bounds.max_y = -999999.0;
          for (int32_t i = 0 ; i < 4 ; i++)
            {
              if (misc.dateline && mx[i] < 0.0) mx[i] += 360.0;

              if (mx[i] < bounds.min_x) bounds.min_x = mx[i];
              if (my[i] < bounds.min_y) bounds.min_y = my[i];
              if (mx[i] > bounds.max_x) bounds.max_x = mx[i];
              if (my[i] > bounds.max_y) bounds.max_y = my[i];
            }

          misc.clear = NVTrue;

          misc.GeoTIFF_init = NVTrue;

          zoomIn (bounds, NVTrue);


          misc.cov_function = COV_START_DRAW_RECTANGLE;

          redrawCovBounds ();

          map->setToolTip ("");      
          misc.function = misc.save_function;

          setFunctionCursor (misc.function);
        }
      else
        {
          map->anchorRubberbandRectangle (&rb_rectangle, lon, lat, options.contour_color, LINE_WIDTH, Qt::SolidLine); 
       }
      break;

    case RECT_EDIT_AREA_3D:
      if (map->rubberbandRectangleIsActive (rb_rectangle))
        {
          map->closeRubberbandRectangle (rb_rectangle, lon, lat, &px, &py, &mx, &my);
          editCommand (mx, my, 4);
        }
      else
        {
          map->anchorRubberbandRectangle (&rb_rectangle, lon, lat, options.contour_color, LINE_WIDTH, Qt::SolidLine);
        }
      break;

    case RECT_DELETE_AREA:
    case RECT_FILL_AREA:
      if (map->rubberbandRectangleIsActive (rb_rectangle))
        {
          map->closeRubberbandRectangle (rb_rectangle, lon, lat, &px, &py, &mx, &my);

          value = options.draw_contour_level;
          if (misc.function == RECT_DELETE_AREA) value = CHRTR2_NULL_Z_VALUE;
          fillArea (&misc, value, mx, my, 4);

          redrawMap (NVTrue, NVTrue);
          cov->redrawMap (NVTrue);
          redrawCovBounds ();
        }
      else
        {
          if (misc.function == RECT_FILL_AREA)
            {
              map->anchorRubberbandRectangle (&rb_rectangle, lon, lat, options.contour_highlight_color, LINE_WIDTH, Qt::SolidLine);
            }
          else
            {
              map->anchorRubberbandRectangle (&rb_rectangle, lon, lat, options.contour_color, LINE_WIDTH, Qt::SolidLine);
            }
        }
      break;

    case SELECT_HIGH_POINT:
    case SELECT_LOW_POINT:
      if (map->rubberbandRectangleIsActive (rb_rectangle))
        {
          map->closeRubberbandRectangle (rb_rectangle, lon, lat, &px, &py, &mx, &my);

          if (setHighLow (mx, my, &misc, &options, map))
            {
              cov->redrawMap (NVTrue);
              redrawCovBounds ();
            }

          map->discardRubberbandRectangle (&rb_rectangle);
        }
      else
        {
          map->anchorRubberbandRectangle (&rb_rectangle, lon, lat, options.contour_color, LINE_WIDTH, Qt::SolidLine);
        }
      break;


    case RECT_FILTER_AREA:
      if (map->rubberbandRectangleIsActive (rb_rectangle))
        {
          map->closeRubberbandRectangle (rb_rectangle, lon, lat, &px, &py, &mx, &my);


          //  Using a 4 point polygon to avoid writing new filter code.

          filterPolyArea (&options, &misc, mx, my, 4);
          redrawMap (NVTrue, NVTrue);
          cov->redrawMap (NVTrue);
          redrawCovBounds ();
        }
      else
        {
          map->anchorRubberbandRectangle (&rb_rectangle, lon, lat, options.contour_color, LINE_WIDTH, Qt::SolidLine);
        }
      break;

    case RECT_FILTER_MASK:
      if (map->rubberbandRectangleIsActive (rb_rectangle))
        {
          map->closeRubberbandRectangle (rb_rectangle, lon, lat, &px, &py, &mx, &my);


          //  Increment the filter mask count and reallocate the memory.

          try
            {
              misc.poly_filter_mask.resize (misc.poly_filter_mask_count + 1);
            }
          catch (std::bad_alloc&)
            {
              perror ("Allocating poly_filter_mask memory");
              exit (-1);
            }


          //  Using a 4 point polygon to avoid writing new masking code.

          misc.poly_filter_mask[misc.poly_filter_mask_count].count = 4;

          for (int32_t i = 0 ; i < 4 ; i++)
            {
              misc.poly_filter_mask[misc.poly_filter_mask_count].x[i] = mx[i];
              misc.poly_filter_mask[misc.poly_filter_mask_count].y[i] = my[i];
            }


          map->discardRubberbandRectangle (&rb_rectangle);


          ndx = misc.poly_filter_mask_count;
          if (options.poly_filter_mask_color.alpha () < 255)
            {
              map->fillPolygon (misc.poly_filter_mask[ndx].count, misc.poly_filter_mask[ndx].x, misc.poly_filter_mask[ndx].y, 
                                options.poly_filter_mask_color, NVTrue);
            }
          else
            {
              //  We don't have to worry about clipping this because moving the area discards the mask areas.

              map->drawPolygon (misc.poly_filter_mask[ndx].count, misc.poly_filter_mask[ndx].x, misc.poly_filter_mask[ndx].y,
                                options.poly_filter_mask_color, 2, NVTrue, Qt::SolidLine, NVTrue);
            }

          misc.poly_filter_mask[ndx].displayed = NVTrue;

          misc.poly_filter_mask_count++;
          bClearMasks->setEnabled (true);
        }
      else
        {
          map->anchorRubberbandRectangle (&rb_rectangle, lon, lat, options.contour_color, LINE_WIDTH, Qt::SolidLine);
        }
      break;

    case REMISP_AREA:
      if (map->rubberbandRectangleIsActive (rb_rectangle))
        {
          map->closeRubberbandRectangle (rb_rectangle, lon, lat, &px, &py, &mx, &my);
          count = 4;

          bounds.min_x = 999999.0;
          bounds.min_y = 999999.0;
          bounds.max_x = -999999.0;
          bounds.max_y = -999999.0;
          for (int32_t i = 0 ; i < count ; i++)
            {
              if (misc.dateline && mx[i] < 0.0) mx[i] += 360.0;

              bounds.min_x = qMin (bounds.min_x, mx[i]);
              bounds.min_y = qMin (bounds.min_y, my[i]);
              bounds.max_x = qMax (bounds.max_x, mx[i]);
              bounds.max_y = qMax (bounds.max_y, my[i]);
            }

          qApp->setOverrideCursor (Qt::WaitCursor);
          qApp->processEvents ();

          remisp (&misc, &options, &bounds, NVTrue);

          qApp->restoreOverrideCursor ();

          map->discardRubberbandRectangle (&rb_rectangle);


          misc.function = misc.save_function;
          setFunctionCursor (misc.function);


          redrawMap (NVTrue, NVTrue);
          cov->redrawMap (NVTrue);
          redrawCovBounds ();
       }
      else
        {
          map->anchorRubberbandRectangle (&rb_rectangle, lon, lat, options.contour_color, LINE_WIDTH, Qt::SolidLine);
        }
      break;


    case DRAW_CONTOUR:

      //  Second left click

      if (map->rubberbandPolygonIsActive (rb_polygon))
        {
          int32_t count;
          map->getRubberbandPolygon (rb_polygon, &count, &px, &py, &mx, &my);

          cur_x = (double *) malloc ((count) * sizeof (double));
          cur_y = (double *) malloc ((count) * sizeof (double));
          double *new_x = (double *) malloc ((count) * sizeof (double));
          double *new_y = (double *) malloc ((count) * sizeof (double));

          if (new_y == NULL)
            {
              fprintf (stderr , tr ("Error allocating memory - %s %s %d\n").toLatin1 (), __FILE__,  __FUNCTION__, __LINE__);
              exit (-1);
            }


          for (int32_t i = 0 ; i < count ; i++)
            {
	      if (misc.dateline && mx[i] < 0.0) mx[i] += 360.0;
              cur_x[i] = mx[i];
              cur_y[i] = my[i];
            }
          map->discardRubberbandPolygon (&rb_polygon);


          //  We want to plot the line after we have gotten rid of its moveable image.
          //  We also check to find out which CHRTR2 layers the line passes through.

          int32_t new_count = 0;
          double prev_x = -181.0, prev_y = -91.0;
          for (int32_t chrtr2 = 0 ; chrtr2 < misc.chrtr2_share->chrtr2_count ; chrtr2++) misc.contour_in_chrtr2[chrtr2] = NVFalse;

          for (int32_t i = 0 ; i < count ; i++)
            {
              NV_F64_COORD2 nxy = {cur_x[i], cur_y[i]};


              //  Check against any of the CHRTR2 layers.  Don't save points outside of the CHRTR2 areas.

              uint8_t hit = NVFalse;
              for (int32_t chrtr2 = 0 ; chrtr2 < misc.chrtr2_share->chrtr2_count ; chrtr2++)
                {
                  if (inside_mbr_coord2 (&misc.chrtr2_share->chrtr2_header[chrtr2].mbr, nxy))
                    {
                      if (prev_x > -181.0) map->drawLine (prev_x, prev_y, cur_x[i], cur_y[i], options.contour_highlight_color,
                                                          FAT_LINE_WIDTH, NVTrue, Qt::SolidLine);

                      prev_x = cur_x[i];
                      prev_y = cur_y[i];
                      new_x[new_count] = cur_x[i];
                      new_y[new_count] = cur_y[i];
                      new_count++;
                      misc.contour_in_chrtr2[chrtr2] = hit = NVTrue;
                      break;
                    }
                }

              if (!hit) prev_x = -181.0;
            }


          //  Force the map to update.

          map->flush ();


          free (cur_x);
          free (cur_y);


          //  If the contour passed through any of the CHRTR2 layers we want to insert the contour values into the CHRTR2 file (in the case of DRAW_CONTOUR).

          if (new_count)
            {
              qApp->setOverrideCursor (Qt::WaitCursor);
              qApp->processEvents ();


              //  Write the points to the CHRTR2 file.

              writeContour (&misc, &options, new_count, new_x, new_y);


              qApp->restoreOverrideCursor ();

              setMainButtons (NVTrue);
            }
          else
            {
              QMessageBox::warning (this, tr ("gridEdit Draw contour"), tr ("No input points defined in cells without valid data"));
            }


          free (new_x);
          free (new_y);
        }


      //  First left click

      else
        {
          QString msc;
          msc.sprintf (tr (" Draw contour - %.2f ").toLatin1 (), options.draw_contour_level);
          misc.statusProgLabel->setText (msc);
          misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
          misc.statusProgLabel->setPalette (misc.statusProgPalette);

          map->anchorRubberbandPolygon (&rb_polygon, lon, lat, options.contour_highlight_color, FAT_LINE_WIDTH, NVFalse, Qt::SolidLine);
        }

      prev_poly_lon = -181.0;
      break;


    case CLEAR_CONTOUR:

      if (nearest_drawn_contour >= 0)
        {
          misc.hd_contour[nearest_drawn_contour].count = 0;
          misc.hd_contour[nearest_drawn_contour].xyz.clear ();


          int32_t new_contour_count = 0;
          for (int32_t i = 0 ; i < misc.hd_contour_count ; i++)
            {
              //  The contour still has data.

              if (misc.hd_contour[i].count)
                {
                  misc.hd_contour[new_contour_count].count = misc.hd_contour[i].count;
                  misc.hd_contour[new_contour_count].xyz = misc.hd_contour[i].xyz;
                  new_contour_count++;
                }
            }

          misc.hd_contour_count = new_contour_count;

          if (!misc.hd_contour_count)
            {
              misc.hd_contour.clear ();
            }

          redrawMap (NVTrue, NVFalse);
        }
      break;


    case DEFINE_RECT_AREA:
      if (map->rubberbandRectangleIsActive (rb_rectangle))
        {
          map->closeRubberbandRectangle (rb_rectangle, lon, lat, &px, &py, &mx, &my);
          count = 4;

          bounds.min_x = 999999.0;
          bounds.min_y = 999999.0;
          bounds.max_x = -999999.0;
          bounds.max_y = -999999.0;
          for (int32_t i = 0 ; i < count ; i++)
            {
              if (misc.dateline && mx[i] < 0.0) mx[i] += 360.0;

              bounds.min_x = qMin (bounds.min_x, mx[i]);
              bounds.min_y = qMin (bounds.min_y, my[i]);
              bounds.max_x = qMax (bounds.max_x, mx[i]);
              bounds.max_y = qMax (bounds.max_y, my[i]);
            }


          if (!QDir (options.output_area_dir).exists ()) options.output_area_dir = options.input_chrtr2_dir;


          fd = new QFileDialog (this, tr ("gridEdit Output Area File"));
          fd->setViewMode (QFileDialog::List);


          //  Always add the current working directory and the last used directory to the sidebar URLs in case we're running from the command line.
          //  This function is in the nvutility library.

          setSidebarUrls (fd, options.output_area_dir);


          QStringList filters;
          filters << tr ("Generic area file (*.are)")
                  << tr ("Army Corps area file (*.afs)");

          fd->setNameFilters (filters);
          fd->setFileMode (QFileDialog::AnyFile);
          fd->selectNameFilter (tr ("Generic area file (*.are)"));

          hit = NVFalse;

          QStringList files;
          QString file;
          if (fd->exec () == QDialog::Accepted)
            {
              options.output_area_dir = fd->directory ().absolutePath ();

              files = fd->selectedFiles ();

              file = files.at (0);


              if (!file.isEmpty())
                {

                  //  Add extension to filename if not there.
            
                  if (fd->selectedNameFilter ().contains ("*.are"))
                    {
                      if (!file.endsWith (".are")) file.append (".are");
                    }
                  else if (fd->selectedNameFilter ().contains ("*.afs"))
                    {
                      if (!file.endsWith (".afs")) file.append (".afs");
                    }

 
                  strcpy (fname, file.toLatin1 ());


                  //  Write the file.

                  if ((fp = fopen (fname, "w")) != NULL)
                    {
                      if (file.endsWith (".are"))
                        {
                          for (int32_t i = 0 ; i < count ; i++)
                            {
                              strcpy (ltstring, fixpos (my[i], &deg, &min, &sec, &hem, POS_LAT, options.position_form));
                              strcpy (lnstring, fixpos (mx[i], &deg, &min, &sec, &hem, POS_LON, options.position_form));

                              fprintf (fp, "%s, %s\n", ltstring, lnstring);
                            }
                        }
                      else
                        {
                          for (int32_t i = 0 ; i < count ; i++)
                            {
                              //  Make sure we haven't created any duplicate points

                              if (i && mx[i] == mx[i - 1] && my[i] == my[i -1]) continue;

                              fprintf (fp, "%.9f, %.9f\n", mx[i], my[i]);
                            }
                        }

                      fclose (fp);


                      //  Add the new file to the first available overlay slot.

                      for (int32_t i = 0 ; i < NUM_OVERLAYS ; i++)
                        {
                          if (!misc.overlays[i].file_type)
                            {
                              if (file.endsWith (".are"))
                                {
                                  misc.overlays[i].file_type = GENERIC_AREA;
                                }
                              else
                                {
                                  misc.overlays[i].file_type = ACE_AREA;
                                }
                              strcpy (misc.overlays[i].filename, file.toLatin1 ());
                              misc.overlays[i].display = NVTrue;
                              misc.overlays[i].color = options.contour_color;

                              hit = NVTrue;

                              break;
                            }
                        }
                    }
                }
            }
          options.output_area_dir = fd->directory ().path ();

          map->discardRubberbandRectangle (&rb_rectangle);

          map->setToolTip ("");      
          misc.function = misc.save_function;

          setFunctionCursor (misc.function);

          if (hit) redrawMap (NVTrue, NVFalse);
        }
      else
        {
          map->anchorRubberbandRectangle (&rb_rectangle, lon, lat, options.contour_color, LINE_WIDTH, Qt::SolidLine);
        }
      break;


    case POLY_EDIT_AREA_3D:
    case POLY_DELETE_AREA:
    case POLY_DELETE_CONTOUR:
    case POLY_FILL_AREA:
    case POLY_FILL_PFM:
    case POLY_FILTER_AREA:
    case POLY_FILTER_MASK:
    case DEFINE_POLY_AREA:
    case GRAB_CONTOUR:
      if (map->rubberbandPolygonIsActive (rb_polygon))
        {
          map->vertexRubberbandPolygon (rb_polygon, lon, lat);
        }
      else
        {
          if (misc.function == POLY_FILL_AREA || misc.function == POLY_FILL_PFM)
            {
              map->anchorRubberbandPolygon (&rb_polygon, lon, lat, options.contour_highlight_color, LINE_WIDTH, NVTrue, Qt::SolidLine);
            }
          else
            {
              map->anchorRubberbandPolygon (&rb_polygon, lon, lat, options.contour_color, LINE_WIDTH, NVTrue, Qt::SolidLine);
            }
        }
      break;


    case OUTPUT_POINTS:
      try
        {
          misc.output_points.resize (misc.output_point_count + 1);
        }
      catch (std::bad_alloc&)
        {
          perror ("Allocating output_points");
          exit (-1);
        }

      misc.output_points[misc.output_point_count].x = lon;
      misc.output_points[misc.output_point_count].y = lat;

      chrtr2_read_record_lat_lon (misc.chrtr2_handle[0], lat, lon, &chrtr2_record);

      if (chrtr2_record.status)
        {
          misc.output_points[misc.output_point_count].z = chrtr2_record.z;

          map->fillCircle (lon, lat, 10, 0.0, 360.0, options.contour_color, NVTrue);

          string.sprintf (tr (" Point #%d").toLatin1 (), misc.output_point_count);

          map->drawText (string, lon, lat, options.contour_color, NVTrue);

          misc.output_point_count++;
        }
      break;
    }
}



//  Middle mouse button press.  Called from the map mouse press callback.  This is usually a discard operation.

void 
gridEdit::midMouse (int32_t mouse_x __attribute__ ((unused)), int32_t mouse_y __attribute__ ((unused)),
		    double lon, double lat)
{
  void highlight_contour (MISC *misc, OPTIONS *options, double lat, double lon, nvMap *map);


  //  Actions based on the active function

  switch (misc.function)
    {
    case RECT_EDIT_AREA_3D:
    case RECT_DELETE_AREA:
    case SELECT_HIGH_POINT:
    case SELECT_LOW_POINT:
    case ZOOM_IN_AREA:
    case REMISP_AREA:
    case RECT_FILTER_AREA:
    case RECT_FILTER_MASK:
    case DEFINE_RECT_AREA:
      map->discardRubberbandRectangle (&rb_rectangle);
      break;

    case POLY_EDIT_AREA_3D:
    case POLY_DELETE_AREA:
    case POLY_DELETE_CONTOUR:
    case GRAB_CONTOUR:
      map->discardRubberbandPolygon (&rb_polygon);
      prev_poly_lon = -181.0;
      break;

    case POLY_FILTER_AREA:
    case POLY_FILTER_MASK:
    case DEFINE_POLY_AREA:
      map->discardRubberbandPolygon (&rb_polygon);
      prev_poly_lon = -181.0;
      break;

    case OUTPUT_POINTS:
      misc.output_points.clear ();
      misc.output_point_count = 0;
      redrawMap (NVTrue, NVFalse);
      break;


      //  For this we want to load the nearest contour value as the drawn contour value.

    case DRAW_CONTOUR:
      if (map->rubberbandPolygonIsActive (rb_polygon))
        {
          map->discardRubberbandPolygon (&rb_polygon);
        }
      else
        {
          //  Grab the nearest cell and highlight the contours.

          highlight_contour (&misc, &options, lat, lon, map);


          QString msc;

          msc.sprintf (tr (" Draw contour - %.2f ").toLatin1 (), options.draw_contour_level);
          misc.statusProgLabel->setText (msc);
          misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
          misc.statusProgLabel->setPalette (misc.statusProgPalette);


          //  Set the tool tip.

          msc.sprintf (tr ("Draw and insert a contour (%.2f)").toLatin1 (), options.draw_contour_level);
          bDrawContour->setToolTip (msc);
        }
      prev_poly_lon = -181.0;
      break;

    case CLEAR_CONTOUR:
      misc.function = misc.save_function;
      setFunctionCursor (misc.function);
      map->closeMovingPath (&mv_marker);
      break;

    case RECT_FILL_AREA:
      if (map->rubberbandRectangleIsActive (rb_rectangle))
        {
          map->discardRubberbandRectangle (&rb_rectangle);
        }
      else
        {
          QString msc;


          //  Grab the last valid nearest point value (set in slotMouseMove).

          options.draw_contour_level = nearest_value;


          //  Set the tool tips.

          msc.sprintf (tr ("Draw and insert a contour (%.2f)").toLatin1 (), options.draw_contour_level);
          bDrawContour->setToolTip (msc);
          msc.sprintf (tr (" Set rectangle fill mode (%.2f) ").toLatin1 (), options.draw_contour_level);
          bFillRect->setToolTip (msc);
          msc.sprintf (tr (" Set polygon fill mode (%.2f) ").toLatin1 (), options.draw_contour_level);
          bFillPoly->setToolTip (msc);
        }
      break;

    case POLY_FILL_AREA:
      if (map->rubberbandPolygonIsActive (rb_polygon))
        {
          map->discardRubberbandPolygon (&rb_polygon);
        }
      else
        {
          QString msc;


          //  Grab the last valid nearest point value (set in slotMouseMove).

          options.draw_contour_level = nearest_value;


          //  Set the tool tips.

          msc.sprintf (tr ("Draw and insert a contour (%.2f)").toLatin1 (), options.draw_contour_level);
          bDrawContour->setToolTip (msc);
          msc.sprintf (tr (" Set rectangle fill mode (%.2f) ").toLatin1 (), options.draw_contour_level);
          bFillRect->setToolTip (msc);
          msc.sprintf (tr (" Set polygon fill mode (%.2f) ").toLatin1 (), options.draw_contour_level);
          bFillPoly->setToolTip (msc);
        }
      break;

    case POLY_FILL_PFM:
      if (map->rubberbandPolygonIsActive (rb_polygon)) map->discardRubberbandPolygon (&rb_polygon);
      break;
    }
}



//  Mouse double click.  Called from the map double click callback.  This is usually a completion operation.

void 
gridEdit::slotMouseDoubleClick (QMouseEvent *e, double lon, double lat)
{
  int32_t         count, *px, *py, ndx;
  double          *mx, *my;
  QFileDialog     *fd;
  QString         file, string;
  FILE            *fp;
  char            fname[512], ltstring[25], lnstring[25], hem;
  double          deg, min, sec;
  float           value;
  QStringList     filters;
  QStringList     files;
  static QDir     dir = QDir (".");
  uint8_t         hit = NVFalse;
  CHRTR2_RECORD   chrtr2_record;


  void filterPolyArea (OPTIONS *, MISC *, double *, double *, int32_t);


  //  Flip the double_click flag.  The right-click menu sets this to NVTrue so it will flip to NVFalse.
  //  Left-click sets it to NVFalse so it will flip to NVTrue;

  double_click = !double_click;


  hit = NVFalse;


  //  Actions based on the active function

  QList<QUrl> urls;
  QSet<QUrl> set;
  switch (misc.function)
    {
    case DEFINE_POLY_AREA:
      map->closeRubberbandPolygon (rb_polygon, lon, lat, &count, &px, &py, &mx, &my);

      if (double_click) count--;


      if (!QDir (options.output_area_dir).exists ()) options.output_area_dir = options.input_chrtr2_dir;


      fd = new QFileDialog (this, tr ("gridEdit Output Area File"));
      fd->setViewMode (QFileDialog::List);


      //  Always add the current working directory and the last used directory to the sidebar URLs in case we're running from the command line.
      //  This function is in the nvutility library.

      setSidebarUrls (fd, options.output_area_dir);


      filters << tr ("Generic area file (*.are)")
              << tr ("Army Corps area file (*.afs)");

      fd->setNameFilters (filters);
      fd->setFileMode (QFileDialog::AnyFile);
      fd->selectNameFilter (tr ("Generic area file (*.are)"));


      if (fd->exec () == QDialog::Accepted)
        {
          files = fd->selectedFiles ();

          file = files.at (0);


          if (!file.isEmpty())
            {

              //  Add extension to filename if not there.
            
              if (fd->selectedNameFilter ().contains ("*.are"))
                {
                  if (!file.endsWith (".are")) file.append (".are");
                }
              else if (fd->selectedNameFilter ().contains ("*.afs"))
                {
                  if (!file.endsWith (".afs")) file.append (".afs");
                }

 
              strcpy (fname, file.toLatin1 ());


              if ((fp = fopen (fname, "w")) != NULL)
                {
                  if (file.endsWith (".are"))
                    {
                      for (int32_t i = 0 ; i < count ; i++)
                        {
                          strcpy (ltstring, fixpos (my[i], &deg, &min, &sec, &hem, POS_LAT, options.position_form));
                          strcpy (lnstring, fixpos (mx[i], &deg, &min, &sec, &hem, POS_LON, options.position_form));

                          fprintf (fp, "%s, %s\n", ltstring, lnstring);
                        }
                    }
                  else
                    {
                      for (int32_t i = 0 ; i < count ; i++)
                        {
                          //  Make sure we haven't created any duplicate points

                          if (i && mx[i] == mx[i - 1] && my[i] == my[i -1]) continue;

                          fprintf (fp, "%.9f, %.9f\n", mx[i], my[i]);
                        }
                    }

                  fclose (fp);


                  //  Add the new file to the first available overlay slot.

                  for (int32_t i = 0 ; i < NUM_OVERLAYS ; i++)
                    {
                      if (!misc.overlays[i].file_type)
                        {
                          if (file.endsWith (".are"))
                            {
                              misc.overlays[i].file_type = GENERIC_AREA;
                            }
                          else
                            {
                              misc.overlays[i].file_type = ACE_AREA;
                            }
                          strcpy (misc.overlays[i].filename, file.toLatin1 ());
                          misc.overlays[i].display = NVTrue;
                          misc.overlays[i].color = options.contour_color;

                          hit = NVTrue;

                          break;
                        }
                    }
                }
            }
          options.output_area_dir = fd->directory ().absolutePath ();
        }

      map->discardRubberbandPolygon (&rb_polygon);

      map->setToolTip ("");      
      misc.function = misc.save_function;

      setFunctionCursor (misc.function);

      if (hit) redrawMap (NVTrue, NVFalse);
      break;


    case POLY_FILTER_AREA:
      map->closeRubberbandPolygon (rb_polygon, lon, lat, &count, &px, &py, &mx, &my);

      filterPolyArea (&options, &misc, mx, my, count);
      redrawMap (NVTrue, NVTrue);
      cov->redrawMap (NVTrue);
      redrawCovBounds ();

      break;


    case POLY_FILTER_MASK:
      map->closeRubberbandPolygon (rb_polygon, lon, lat, &count, &px, &py, &mx, &my);

      try
        {
          misc.poly_filter_mask.resize (misc.poly_filter_mask_count + 1);
        }
      catch (std::bad_alloc&)
        {
          perror ("Allocating poly_filter_mask memory");
          exit (-1);
        }

      misc.poly_filter_mask[misc.poly_filter_mask_count].count = count;

      for (int32_t i = 0 ; i < count ; i++)
        {
          misc.poly_filter_mask[misc.poly_filter_mask_count].x[i] = mx[i];
          misc.poly_filter_mask[misc.poly_filter_mask_count].y[i] = my[i];
        }


      map->discardRubberbandPolygon (&rb_polygon);


      ndx = misc.poly_filter_mask_count;
      if (options.poly_filter_mask_color.alpha () < 255)
        {
          map->fillPolygon (misc.poly_filter_mask[ndx].count, misc.poly_filter_mask[ndx].x, misc.poly_filter_mask[ndx].y, 
                            options.poly_filter_mask_color, NVTrue);
        }
      else
        {
          //  We don't have to worry about clipping this because moving the area discards the mask areas.

          map->drawPolygon (misc.poly_filter_mask[ndx].count, misc.poly_filter_mask[ndx].x, misc.poly_filter_mask[ndx].y,
                            options.poly_filter_mask_color, 2, NVTrue, Qt::SolidLine, NVTrue);
        }
      misc.poly_filter_mask[ndx].displayed = NVTrue;

      misc.poly_filter_mask_count++;
      bClearMasks->setEnabled (true);
      break;


    case POLY_EDIT_AREA_3D:
      map->closeRubberbandPolygon (rb_polygon, lon, lat, &count, &px, &py, &mx, &my);
      editCommand (mx, my, count);
      break;


    case POLY_DELETE_AREA:
    case POLY_DELETE_CONTOUR:
    case POLY_FILL_AREA:
      map->closeRubberbandPolygon (rb_polygon, lon, lat, &count, &px, &py, &mx, &my);

      value = options.draw_contour_level;
      if (misc.function == POLY_DELETE_AREA) value = CHRTR2_NULL_Z_VALUE;
      if (misc.function == POLY_DELETE_CONTOUR) value = -99999.0;
      fillArea (&misc, value, mx, my, count);

      redrawMap (NVTrue, NVTrue);
      cov->redrawMap (NVTrue);
      redrawCovBounds ();
      break;


    case POLY_FILL_PFM:
      map->closeRubberbandPolygon (rb_polygon, lon, lat, &count, &px, &py, &mx, &my);

      if (double_click) count--;


      if (!QDir (options.pfm_dir).exists ()) options.pfm_dir = options.input_chrtr2_dir;


      fd = new QFileDialog (this, tr ("gridEdit Input PFM Data File"));
      fd->setViewMode (QFileDialog::List);


      //  Always add the current working directory and the last used directory to the sidebar URLs in case we're running from the command line.
      //  This function is in the nvutility library.

      setSidebarUrls (fd, options.pfm_dir);


      filters << tr ("PFM (*.pfm)");

      fd->setNameFilters (filters);
      fd->setFileMode (QFileDialog::AnyFile);
      fd->selectNameFilter (tr ("PFM (*.pfm)"));


      if (fd->exec () == QDialog::Accepted)
        {
          files = fd->selectedFiles ();

          file = files.at (0);


          if (!file.isEmpty())
            {
              strcpy (fname, file.toLatin1 ());

              fillPFM (&misc, &options, fname, mx, my, count);
            }

          options.pfm_dir = fd->directory ().absolutePath ();
        }

      map->discardRubberbandPolygon (&rb_polygon);

      map->setToolTip ("");      
      misc.function = misc.save_function;

      setFunctionCursor (misc.function);

      redrawMap (NVTrue, NVTrue);
      cov->redrawMap (NVTrue);
      redrawCovBounds ();
      break;


    case GRAB_CONTOUR:
      map->closeRubberbandPolygon (rb_polygon, lon, lat, &count, &px, &py, &mx, &my);

      for (int32_t i = 0 ; i < count ; i++)
        {
          misc.polygon_x[i] = mx[i];
          misc.polygon_y[i] = my[i];
        }
      misc.poly_count = count;

      misc.function = misc.save_function;

      setFunctionCursor (misc.function);

      redrawMap (NVTrue, NVTrue);

      misc.poly_count = 0;
      break;


      //  Output manually selected points to a file.

    case OUTPUT_POINTS:
      try
        {
          misc.output_points.resize (misc.output_point_count + 1);
        }
      catch (std::bad_alloc&)
        {
          perror ("Allocating output_points");
          exit (-1);
        }

      misc.output_points[misc.output_point_count].x = lon;
      misc.output_points[misc.output_point_count].y = lat;

      chrtr2_read_record_lat_lon (misc.chrtr2_handle[0], lat, lon, &chrtr2_record);

      if (chrtr2_record.status)
        {
          misc.output_points[misc.output_point_count].z = chrtr2_record.z;

          map->fillCircle (lon, lat, 10, 0.0, 360.0, options.contour_color, NVTrue);

          string.sprintf (tr (" Point #%d").toLatin1 (), misc.output_point_count);

          map->drawText (string, lon, lat, options.contour_color, NVTrue);

          misc.output_point_count++;
        }


      if (!QDir (options.output_points_dir).exists ()) options.output_points_dir = options.input_chrtr2_dir;


      fd = new QFileDialog (this, tr ("gridEdit Output Points File"));
      fd->setViewMode (QFileDialog::List);


      //  Always add the current working directory and the last used directory to the sidebar URLs in case we're running from the command line.
      //  This function is in the nvutility library.

      setSidebarUrls (fd, options.output_points_dir);


      filters << tr ("Points (*.pts)");

      fd->setNameFilters (filters);
      fd->setFileMode (QFileDialog::AnyFile);
      fd->selectNameFilter (tr ("Points (*.pts)"));

      if (fd->exec () == QDialog::Accepted)
        {
          files = fd->selectedFiles ();

          file = files.at (0);


          if (!file.isEmpty())
            {
              //  Add .pts to filename if not there.

              if (!file.endsWith (".pts")) file.append (".pts");
 
              strcpy (fname, file.toLatin1 ());


              if ((fp = fopen (fname, "w")) != NULL)
                {
                  for (int32_t i = 0 ; i < misc.output_point_count ; i++)
                    fprintf (fp, tr ("%.9f, %.9f, Point #%d %f\n").toLatin1 (), misc.output_points[i].y, 
                             misc.output_points[i].x, i, misc.output_points[i].z);

                  fclose (fp);


                  //  Add the new file to the first available overlay slot.

                  for (int32_t i = 0 ; i < NUM_OVERLAYS ; i++)
                    {
                      if (!misc.overlays[i].file_type)
                        {
                          misc.overlays[i].file_type = GENERIC_YXZ;
                          strcpy (misc.overlays[i].filename, file.toLatin1 ());
                          misc.overlays[i].display = NVTrue;
                          misc.overlays[i].color = options.contour_color;

                          hit = NVTrue;

                          break;
                        }
                    }
                }
            }
          options.output_points_dir = fd->directory ().absolutePath ();
        }
      if (e != NULL) midMouse (e->x (), e->y (), lon, lat);

      map->setToolTip ("");      
      misc.function = misc.save_function;

      setFunctionCursor (misc.function);

      if (hit) redrawMap (NVTrue, NVFalse);

      break;
    }
}



//  Right mouse press.  Called from the map mouse press callback.  This pops up the right mouse click menu.

void 
gridEdit::rightMouse (int32_t mouse_x, int32_t mouse_y, double lon, double lat)
{
  QString tmp;

  menu_cursor_lon = lon;
  menu_cursor_lat = lat;
  menu_cursor_x = mouse_x;
  menu_cursor_y = mouse_y;


  QPoint pos (mouse_x, mouse_y);


  //  Popups need global positioning

  QPoint global_pos = map->mapToGlobal (pos);


  popup4->setVisible (false);


  //  Define the menu entrys based on the active function.

  switch (misc.function)
    {
    case ZOOM_IN_AREA:
      popup0->setText (tr ("Close rectangle and zoom in"));
      popup1->setText (tr ("Discard rectangle"));
      popup2->setVisible (false);
      popup3->setVisible (false);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case DEFINE_RECT_AREA:
      popup0->setText (tr ("Close rectangle and save to file"));
      popup1->setText (tr ("Discard rectangle"));
      popup2->setVisible (false);
      popup3->setVisible (false);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case RECT_EDIT_AREA_3D:
      popup0->setText (tr ("Close rectangle and edit area"));
      popup1->setText (tr ("Discard rectangle"));
      popup2->setVisible (false);
      popup3->setVisible (false);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case RECT_DELETE_AREA:
      popup0->setText (tr ("Close rectangle and delete data"));
      popup1->setText (tr ("Discard rectangle"));
      popup2->setVisible (false);
      popup3->setVisible (false);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case RECT_FILL_AREA:
      popup0->setText (tr ("Close rectangle and fill area with data"));
      popup1->setText (tr ("Discard rectangle"));
      tmp.sprintf (tr ("Set depth for fill (%f)").toLatin1 (), options.draw_contour_level);
      popup2->setText (tmp);
      popup3->setVisible (false);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case SELECT_HIGH_POINT:
      popup0->setText (tr ("Close rectangle and select highest point"));
      popup1->setText (tr ("Discard rectangle"));
      popup2->setVisible (false);
      popup3->setVisible (false);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case SELECT_LOW_POINT:
      popup0->setText (tr ("Close rectangle and select lowest point"));
      popup1->setText (tr ("Discard rectangle"));
      popup2->setVisible (false);
      popup3->setVisible (false);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case REMISP_AREA:
      popup0->setText (tr ("Close rectangle and regrid area"));
      tmp.sprintf (tr ("Set MISP weight factor (%d)").toLatin1 (), options.misp_weight);
      popup1->setText (tmp);
      if (options.misp_force_original)
        {
          tmp = tr ("Set MISP force original input (Yes)");
        }
      else
        {
          tmp = tr ("Set MISP force original input (No)");
        }
      popup2->setText (tmp);
      popup2->setVisible (true);
      popup3->setText (tr ("Discard rectangle"));
      popup3->setVisible (true);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case DRAW_CONTOUR:
      popup0->setText (tr ("End contour"));
      tmp.sprintf (tr ("Set depth for contour (%f)").toLatin1 (), options.draw_contour_level);
      popup1->setText (tmp);
      popup2->setText (tr ("Grab nearest contour depth for contour"));
      popup3->setText (tr ("Discard contour"));

      if (map->rubberbandPolygonIsActive (rb_polygon))
        {
          popup0->setVisible (true);
          popup1->setVisible (false);
          popup2->setVisible (false);
          popup3->setVisible (true);
        }
      else
        {
          popup0->setVisible (false);
          popup1->setVisible (true);
          popup2->setVisible (true);
          popup3->setVisible (false);
        }
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case GRAB_CONTOUR:
      popup0->setText (tr ("Close polygon and capture contours"));
      popup1->setText (tr ("Discard polygon"));
      popup2->setVisible (false);
      popup3->setVisible (false);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case CLEAR_CONTOUR:
      popup0->setText (tr ("Remove contour"));
      popup1->setText (tr ("Cancel"));
      popup2->setVisible (false);
      popup3->setVisible (false);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case DEFINE_POLY_AREA:
      popup0->setText (tr ("Close polygon and save to file"));
      popup1->setText (tr ("Discard polygon"));
      popup2->setVisible (false);
      popup3->setVisible (false);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case POLY_EDIT_AREA_3D:
      popup0->setText (tr ("Close polygon and edit area"));
      popup1->setText (tr ("Discard polygon"));
      popup2->setVisible (false);
      popup3->setVisible (false);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case POLY_DELETE_AREA:
      popup0->setText (tr ("Close polygon and delete data"));
      popup1->setText (tr ("Discard polygon"));
      popup2->setVisible (false);
      popup3->setVisible (false);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case POLY_DELETE_CONTOUR:
      popup0->setText (tr ("Close polygon and delete contours"));
      popup1->setText (tr ("Discard polygon"));
      popup2->setVisible (false);
      popup3->setVisible (false);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case POLY_FILL_AREA:
      popup0->setText (tr ("Close polygon and fill area with data"));
      popup1->setText (tr ("Discard polygon"));
      tmp.sprintf (tr ("Set depth for fill (%f)").toLatin1 (), options.draw_contour_level);
      popup2->setText (tmp);
      popup3->setVisible (false);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case POLY_FILL_PFM:
      popup0->setText (tr ("Close polygon and fill area with data from PFM"));
      popup1->setText (tr ("Discard polygon"));
      popup2->setVisible (false);
      popup3->setVisible (false);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case RECT_FILTER_AREA:
      popup0->setText (tr ("Close rectangle and filter area"));
      tmp.sprintf (tr ("Set filter cutoff (%d)").toLatin1 (), options.filter_cutoff);
      popup1->setText (tmp);
      tmp.sprintf (tr ("Set filter slope (%.2f)").toLatin1 (), options.filter_slope);
      popup2->setText (tmp);
      tmp.sprintf (tr ("Set filter length (%d)").toLatin1 (), options.filter_length);
      popup3->setText (tmp);
      popup4->setText (tr ("Discard rectangle"));
      popup2->setVisible (true);
      popup3->setVisible (true);
      popup4->setVisible (true);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case POLY_FILTER_AREA:
      popup0->setText (tr ("Close polygon and filter area"));
      tmp.sprintf (tr ("Set filter cutoff (%d)").toLatin1 (), options.filter_cutoff);
      popup1->setText (tmp);
      tmp.sprintf (tr ("Set filter slope (%.2f)").toLatin1 (), options.filter_slope);
      popup2->setText (tmp);
      tmp.sprintf (tr ("Set filter length (%d)").toLatin1 (), options.filter_length);
      popup3->setText (tmp);
      popup3->setText (tr ("Discard polygon"));
      popup2->setVisible (true);
      popup3->setVisible (true);
      popup4->setVisible (true);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case RECT_FILTER_MASK:
      popup0->setText (tr ("Close rectangle and save mask"));
      popup1->setText (tr ("Discard rectangle"));
      popup2->setText (tr ("Clear all masks"));
      popup2->setVisible (true);
      popup3->setVisible (false);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case POLY_FILTER_MASK:
      popup0->setText (tr ("Close polygon and save mask"));
      popup1->setText (tr ("Discard polygon"));
      popup2->setText (tr ("Clear all masks"));
      popup2->setVisible (true);
      popup3->setVisible (false);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;

    case OUTPUT_POINTS:
      popup0->setText (tr ("Save points to file"));
      popup1->setText (tr ("Discard points"));
      popup2->setVisible (false);
      popup3->setVisible (false);
      popup_active = NVTrue;
      popupMenu->popup (global_pos);
      break;
    }
}



//  Right click popup menu first entry has been selected.  Perform operations based on the active function.

void 
gridEdit::slotPopupMenu0 ()
{
  popup_active = NVFalse;

  switch (misc.function)
    {
    case ZOOM_IN_AREA:
    case DEFINE_RECT_AREA:
    case RECT_EDIT_AREA_3D:
    case RECT_DELETE_AREA:
    case RECT_FILL_AREA:
    case SELECT_HIGH_POINT:
    case SELECT_LOW_POINT:
    case RECT_FILTER_AREA:
    case RECT_FILTER_MASK:
    case DRAW_CONTOUR:
    case CLEAR_CONTOUR:
    case REMISP_AREA:
      leftMouse (menu_cursor_x, menu_cursor_y, menu_cursor_lon, menu_cursor_lat);
      break;

    case DEFINE_POLY_AREA:
    case POLY_EDIT_AREA_3D:
    case POLY_DELETE_AREA:
    case POLY_DELETE_CONTOUR:
    case POLY_FILL_AREA:
    case POLY_FILL_PFM:
    case POLY_FILTER_AREA:
    case POLY_FILTER_MASK:
    case OUTPUT_POINTS:
    case GRAB_CONTOUR:

      //  Fake out slotMouseDoubleClick so that it won't decrement the point count.

      double_click = NVTrue;

      slotMouseDoubleClick (NULL, menu_cursor_lon, menu_cursor_lat);
      break;
    }

}



//  Right click popup menu second entry has been selected.  Perform operations based on the active function.

void 
gridEdit::slotPopupMenu1 ()
{
  popup_active = NVFalse;

  bool ok;
  int32_t ires;
  QString text;

  switch (misc.function)
    {
    case DRAW_CONTOUR:
      double ret;
      ret = QInputDialog::getDouble (this, tr ("gridEdit"), tr ("Enter contour level:"), options.draw_contour_level, -20000.0, 20000.0, 1, &ok);

      if (ok)
        {
          if (ret > misc.chrtr2_share->chrtr2_header[0].max_z || ret < misc.chrtr2_share->chrtr2_header[0].min_z)
            {
              QMessageBox::warning (this, tr ("gridEdit value out of range"), 
                                    tr ("Contour value entered is out of range for this CHRTR2 file."));
            }
          else
            {
              options.draw_contour_level = (float) ret;
            }


          QString msc;
          msc.sprintf (tr ("Draw and insert a contour (%.2f)").toLatin1 (), options.draw_contour_level);
          bDrawContour->setToolTip (msc);
          msc.sprintf (tr (" Set rectangle fill mode (%.2f) ").toLatin1 (), options.draw_contour_level);
          bFillRect->setToolTip (msc);
          msc.sprintf (tr (" Set polygon fill mode (%.2f) ").toLatin1 (), options.draw_contour_level);
          bFillPoly->setToolTip (msc);
        }
      break;

    case POLY_FILTER_AREA:
    case RECT_FILTER_AREA:
      ires = QInputDialog::getInt (this, tr ("gridEdit"), tr ("Enter filter cutoff frequency (1 - 30):"), options.filter_cutoff, 1, 30, 1, &ok);
      if (ok) options.filter_cutoff = ires;
      break;

    case POLY_FILTER_MASK:
    case RECT_FILTER_MASK:
    case GRAB_CONTOUR:
      midMouse (menu_cursor_x, menu_cursor_y, menu_cursor_lon, menu_cursor_lat);
      break;

    case REMISP_AREA:
      ires = QInputDialog::getInt (this, tr ("gridEdit"), tr ("Enter MISP weight:"), options.misp_weight, 1, 3, 1, &ok);

      if (ok) options.misp_weight = ires;
      break;

    case OUTPUT_POINTS:
    case CLEAR_CONTOUR:
      midMouse (menu_cursor_x, menu_cursor_y, menu_cursor_lon, menu_cursor_lat);
      break;
    }
}



//  Right click popup menu third entry has been selected.  Perform operations based on the active function.

void 
gridEdit::slotPopupMenu2 ()
{
  QStringList lst, items;
  QString res, item;
  int32_t current;
  double slope;
  bool ok;

  popup_active = NVFalse;

  switch (misc.function)
    {
    case RECT_FILL_AREA:
    case POLY_FILL_AREA:
      double ret;
      ret = QInputDialog::getDouble (this, tr ("gridEdit"), tr ("Enter fill value:"), options.draw_contour_level, -20000.0, 20000.0, 1, &ok);

      if (ok)
        {
          if (ret > misc.chrtr2_share->chrtr2_header[0].max_z || ret < misc.chrtr2_share->chrtr2_header[0].min_z)
            {
              QMessageBox::warning (this, tr ("gridEdit value out of range"), 
                                    tr ("Fill value entered is out of range for this CHRTR2 file."));
            }
          else
            {
              options.draw_contour_level = (float) ret;
            }


          QString msc;
          msc.sprintf (tr ("Draw and insert a contour (%.2f)").toLatin1 (), options.draw_contour_level);
          bDrawContour->setToolTip (msc);
          msc.sprintf (tr (" Set rectangle fill mode (%.2f) ").toLatin1 (), options.draw_contour_level);
          bFillRect->setToolTip (msc);
          msc.sprintf (tr (" Set polygon fill mode (%.2f) ").toLatin1 (), options.draw_contour_level);
          bFillPoly->setToolTip (msc);
        }
      break;

    case POLY_FILTER_AREA:
    case RECT_FILTER_AREA:
      slope = QInputDialog::getDouble (this, tr ("gridEdit"), tr ("Enter filter slope (0.0 - 1.0):"), options.filter_slope, 0.0, 1.0, 2, &ok);
      if (ok) options.filter_slope = slope;
      break;

    case REMISP_AREA:
      items << tr ("Yes") << tr ("No");

      current = 1;
      if (options.misp_force_original) current = 0;

      item = QInputDialog::getItem (this, tr ("gridEdit"),
                                    tr ("MISP force original input value:"), items, current, false, &ok);
      if (ok && !item.isEmpty ())
        {
          if (item == tr ("Yes"))
            {
              options.misp_force_original = NVTrue;
            }
          else
            {
              options.misp_force_original = NVFalse;
            }
        }
      break;

    case RECT_FILTER_MASK:
    case POLY_FILTER_MASK:
      clearFilterMasks ();

      redrawMap (NVTrue, NVFalse);
      break;

    case DRAW_CONTOUR:
      midMouse (menu_cursor_x, menu_cursor_y, menu_cursor_lon, menu_cursor_lat);
      break;
    }
}



//  Right click popup menu fourth entry has been selected.  Perform operations based on the active function.

void 
gridEdit::slotPopupMenu3 ()
{
  bool ok;
  int32_t res;


  popup_active = NVFalse;

  switch (misc.function)
    {
    case POLY_FILTER_AREA:
    case RECT_FILTER_AREA:
      res = QInputDialog::getInt (this, tr ("gridEdit"), tr ("Enter filter length (5 - 90):"), options.filter_length, 5, 90, 1, &ok);
      if (ok) options.filter_length = res;
      break;

    case DRAW_CONTOUR:
    case REMISP_AREA:
      midMouse (menu_cursor_x, menu_cursor_y, menu_cursor_lon, menu_cursor_lat);
      break;
    }
}



//  Right click popup menu fifth entry has been selected.  Perform operations based on the active function.

void 
gridEdit::slotPopupMenu4 ()
{
  popup_active = NVFalse;

  if (misc.function == POLY_FILTER_AREA || misc.function == RECT_FILTER_AREA)
    midMouse (menu_cursor_x, menu_cursor_y, menu_cursor_lon, menu_cursor_lat);
}



//  Right click popup menu help entry has been selected.  Display help based on the active function.

void 
gridEdit::slotPopupHelp ()
{
  switch (misc.function)
    {
    case ZOOM_IN_AREA:
      QWhatsThis::showText (QCursor::pos ( ), zoomInText, map);
      break;

    case DEFINE_RECT_AREA:
      QWhatsThis::showText (QCursor::pos ( ), defineRectAreaText, map);
      break;

    case RECT_EDIT_AREA_3D:
      QWhatsThis::showText (QCursor::pos ( ), editRect3DText, map);
      break;

    case RECT_DELETE_AREA:
      QWhatsThis::showText (QCursor::pos ( ), deleteRectText, map);
      break;

    case RECT_FILL_AREA:
      QWhatsThis::showText (QCursor::pos ( ), fillRectText, map);
      break;

    case SELECT_HIGH_POINT:
      QWhatsThis::showText (QCursor::pos ( ), highText, map);
      break;

    case SELECT_LOW_POINT:
      QWhatsThis::showText (QCursor::pos ( ), lowText, map);
      break;

    case REMISP_AREA:
      QWhatsThis::showText (QCursor::pos ( ), remispText, map);
      break;

    case DEFINE_POLY_AREA:
      QWhatsThis::showText (QCursor::pos ( ), definePolyAreaText, map);
      break;

    case POLY_EDIT_AREA_3D:
      QWhatsThis::showText (QCursor::pos ( ), editPoly3DText, map);
      break;

    case POLY_DELETE_AREA:
      QWhatsThis::showText (QCursor::pos ( ), deletePolyText, map);
      break;

    case POLY_DELETE_CONTOUR:
      QWhatsThis::showText (QCursor::pos ( ), deleteContourPolyText, map);
      break;

    case POLY_FILL_AREA:
      QWhatsThis::showText (QCursor::pos ( ), fillPolyText, map);
      break;

    case POLY_FILL_PFM:
      QWhatsThis::showText (QCursor::pos ( ), fillPolyPFMText, map);
      break;

    case RECT_FILTER_AREA:
      QWhatsThis::showText (QCursor::pos ( ), filterRectText, map);
      break;

    case POLY_FILTER_AREA:
      QWhatsThis::showText (QCursor::pos ( ), filterPolyText, map);
      break;

    case RECT_FILTER_MASK:
      QWhatsThis::showText (QCursor::pos ( ), filterRectMaskText, map);
      break;

    case POLY_FILTER_MASK:
      QWhatsThis::showText (QCursor::pos ( ), filterPolyMaskText, map);
      break;

    case OUTPUT_POINTS:
      QWhatsThis::showText (QCursor::pos ( ), outputDataPointsText, map);
      break;

    case DRAW_CONTOUR:
      QWhatsThis::showText (QCursor::pos ( ), drawContourText, map);
      break;

    case GRAB_CONTOUR:
      QWhatsThis::showText (QCursor::pos ( ), grabContourText, map);
      break;

    case CLEAR_CONTOUR:
      QWhatsThis::showText (QCursor::pos ( ), clearContourText, map);
      break;
    }


  //  Discard the operation.

  midMouse (menu_cursor_x, menu_cursor_y, menu_cursor_lon, menu_cursor_lat);
}



//  If we errored out of the gridPointEdit3D process...

void 
gridEdit::slotEditError (QProcess::ProcessError error)
{
  //  If we manually stopped it we don't want a message.

  if (gridPointEdit_stopped)
    {
      gridPointEdit_stopped = NVFalse;
      return;
    }


  switch (error)
    {
    case QProcess::FailedToStart:
      QMessageBox::critical (this, tr ("gridEdit edit process"), tr ("Unable to start the edit process!"));
      break;

    case QProcess::Crashed:
      QMessageBox::critical (this, tr ("gridEdit edit process"), tr ("The edit process crashed!"));
      break;

    case QProcess::Timedout:
      QMessageBox::critical (this, tr ("gridEdit edit process"), tr ("The edit process timed out!"));
      break;

    case QProcess::WriteError:
      QMessageBox::critical (this, tr ("gridEdit edit process"), tr ("There was a write error to the edit process!"));
      break;

    case QProcess::ReadError:
      QMessageBox::critical (this, tr ("gridEdit edit process"), tr ("There was a read error from the edit process!"));
      break;

    case QProcess::UnknownError:
      QMessageBox::critical (this, tr ("gridEdit edit process"), tr ("The edit process died with an unknown error!"));
      break;
    }
}



//  This is the return from the edit QProcess (when finished normally)

void 
gridEdit::slotEditDone (int exitCode __attribute__ ((unused)), QProcess::ExitStatus exitStatus __attribute__ ((unused)))
{
  //  If grid3D is up we need to tell it to reload it's memory now that we are finished with the 
  //  3D editor.  The other end of this (i.e. setting GRIDPOINTEDIT3D_OPENED) happens in gridPointEdit3D.

  if (threeD_edit)
    {
      //  I put brackets inside the lock/unlock for clarity.

      misc.chrtr2Share->lock ();
      {
        misc.chrtr2_share->key = GRIDPOINTEDIT3D_CLOSED;
      }
      misc.chrtr2Share->unlock ();

      threeD_edit = NVFalse;
    }


  //  If we canceled drawing, bail out.

  if (misc.drawing_canceled)
    {
      misc.drawing_canceled = NVFalse;
      return;
    }


  grid_edit_active = NVFalse;


  //  If we changed the CHRTR2 file in the edit we need to remisp the edited area.  If we didn't change it we want to
  //  discard the undo file that we created prior to running the edit operation.

  NV_F64_XYMBR mbr = misc.chrtr2_share->edit_area;

  if (!gridPointEditMod)
    {
      remove_last_undo (&misc);
    }
  else
    {
      remisp (&misc, &options, &mbr, NVFalse);

      if (gridPointEditMask)
        {
          int32_t count = misc.chrtr2_share->polygon_count;


          //  If we passed 0 to the edit command we were editing a rectangle.

          if (!count) count = 4;


          try
            {
              misc.poly_filter_mask.resize (misc.poly_filter_mask_count + 1);
            }
          catch (std::bad_alloc&)
            {
              perror ("Allocating poly_filter_mask memory");
              exit (-1);
            }

          misc.poly_filter_mask[misc.poly_filter_mask_count].count = count;

          for (int32_t i = 0 ; i < count ; i++)
            {
              misc.poly_filter_mask[misc.poly_filter_mask_count].x[i] = misc.chrtr2_share->polygon_x[i];
              misc.poly_filter_mask[misc.poly_filter_mask_count].y[i] = misc.chrtr2_share->polygon_y[i];
            }

          misc.poly_filter_mask[misc.poly_filter_mask_count].displayed = NVTrue;

          misc.poly_filter_mask_count++;
          bClearMasks->setEnabled (true);
        }
    }


  //  Make sure we reset the important things in case we did a partial redraw.

  setFunctionCursor (misc.function);


  //  If the user changed settings in gridPointEdit...

  if (misc.chrtr2_share->settings_changed)
    {
      misc.chrtr2_share->settings_changed = NVFalse;
      options.smoothing_factor = misc.chrtr2_share->smoothing_factor;
      options.z_factor = misc.chrtr2_share->z_factor;
      options.z_offset = misc.chrtr2_share->z_offset;
      options.position_form = misc.chrtr2_share->position_form;


      if (options.z_factor != 1.0 || options.z_offset != 0.0)
        {
          zPalette.setColor (QPalette::Normal, QPalette::Window, Qt::yellow);

          zLabel->setToolTip (tr ("Scaled/offset Z value for bin"));
        }
      else if (options.z_factor == 1.0 && options.z_offset == 0.0)
        {
          zPalette.setColor (QPalette::Normal, QPalette::Window, misc.widgetBackgroundColor);

          zLabel->setToolTip (tr ("Z value for bin"));
        }
      zLabel->setPalette (zPalette);


      gridPointEditMod = NVTrue;
    }


  if (gridPointEditMod)
    {
      redrawMap (NVTrue, NVFalse);
      cov->redrawMap (NVTrue);
      redrawCovBounds ();
      gridPointEditMod = NVFalse;
    }


  setMainButtons (NVTrue);


  //  No matter what, we need to set the displayed bounds back to the gridEdit displayed bounds
  //  when we exit gridPointEdit.

  misc.chrtr2_share->displayed_area = misc.chrtr2_share->viewer_displayed_area = misc.total_displayed_area;


  misc.drawing = NVFalse;
}



//  This is the stderr read return from the edit QProcess.  Hopefully you won't see anything here.

void 
gridEdit::slotEditReadyReadStandardError ()
{
  QByteArray response = editProc->readAllStandardError ();
  char *res = response.data ();


  fprintf (stderr, "%s %s %d %s\n", __FILE__,  __FUNCTION__, __LINE__, res);
  fflush (stderr);
}



//  This is the stdout read return from the edit QProcess
//  We need to look at the value returned on exit to decide if we need to redraw the map.
//  gridPointEdit3D will print out a 0 for no changes or a 1 for CHRTR2 file changes.

void 
gridEdit::slotEditReadyReadStandardOutput ()
{
  QByteArray response = editProc->readAllStandardOutput ();
  char *res = response.data ();


  //  Only those messages that begin with "Edit return status:" are valid.  The rest may be error messages.

  if (!strncmp (res, "Edit return status:", 19))
    {
      sscanf (res, "Edit return status:%d,%d", &gridPointEditMod, &gridPointEditMask);
    }
  else
    {
      fprintf (stdout, "%s %s %d %s\n", __FILE__,  __FUNCTION__, __LINE__, res);
      fflush (stdout);
    }
}



//  Kick off the edit QProcess

void 
gridEdit::editCommand (double *mx, double *my, int32_t count)
{
  //  Only do the following if we don't already have an edit window opened

  if (!grid_edit_active)
    {
      gridPointEditMod = 0;
      gridPointEditMask = 0;
      grid_edit_active = NVTrue;


      NV_F64_XYMBR       mbr;


      //  Figure out the MBR of the area to be filled

      mbr.min_x = 999999.0;
      mbr.min_y = 999999.0;
      mbr.max_x = -999999.0;
      mbr.max_y = -999999.0;

      for (int32_t i = 0 ; i < count ; i++)
        {
          if (misc.dateline && mx[i] < 0.0) mx[i] += 360.0;

          misc.chrtr2_share->polygon_x[i] = mx[i];
          misc.chrtr2_share->polygon_y[i] = my[i];

          if (mx[i] < mbr.min_x) mbr.min_x = mx[i];
          if (my[i] < mbr.min_y) mbr.min_y = my[i];
          if (mx[i] > mbr.max_x) mbr.max_x = mx[i];
          if (my[i] > mbr.max_y) mbr.max_y = my[i];
        }
      misc.chrtr2_share->polygon_count = count;


      //  Adjust to displayed area bounds

      if (mbr.min_y < misc.total_displayed_area.min_y) mbr.min_y = misc.total_displayed_area.min_y;
      if (mbr.max_y > misc.total_displayed_area.max_y) mbr.max_y = misc.total_displayed_area.max_y;
      if (mbr.min_x < misc.total_displayed_area.min_x) mbr.min_x = misc.total_displayed_area.min_x;
      if (mbr.max_x > misc.total_displayed_area.max_x) mbr.max_x = misc.total_displayed_area.max_x;


      misc.chrtr2_share->edit_area.min_x = mbr.min_x;
      misc.chrtr2_share->edit_area.max_x = mbr.max_x;
      misc.chrtr2_share->edit_area.min_y = mbr.min_y;
      misc.chrtr2_share->edit_area.max_y = mbr.max_y;


      //  Stop!  Undo time...

      if (!create_undo (&misc))
        {
          QMessageBox::critical (this, tr ("gridEdit"), tr ("Unable to create and/or open undo file!"));
          return;
        }


      for (int32_t chrtr2 = 0 ; chrtr2 < misc.chrtr2_share->chrtr2_count ; chrtr2++)
        {
          //  Only if we want to display it and it's not locked.

          if (!misc.chrtr2_share->display_chrtr2[chrtr2] || misc.chrtr2_share->lock_chrtr2[chrtr2])
            {
              start_undo_block (&misc, chrtr2, 0, 0, 0, 0);
            }
          else
            {
              double half_x = misc.chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees * 0.5;
              double half_y = misc.chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees * 0.5;


              //  Adjust bounds to nearest grid point for the CHRTR2 file.

              double min_y = misc.chrtr2_share->chrtr2_header[chrtr2].mbr.slat +
                (NINT ((mbr.min_y - misc.chrtr2_share->chrtr2_header[chrtr2].mbr.slat) / misc.chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees)) *
                misc.chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees;
              double max_y = misc.chrtr2_share->chrtr2_header[chrtr2].mbr.slat + 
                (NINT ((mbr.max_y - misc.chrtr2_share->chrtr2_header[chrtr2].mbr.slat) / misc.chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees)) * 
                misc.chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees;
              double min_x = misc.chrtr2_share->chrtr2_header[chrtr2].mbr.wlon + 
                (NINT ((mbr.min_x - misc.chrtr2_share->chrtr2_header[chrtr2].mbr.wlon) / misc.chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees)) *
                misc.chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees;
              double max_x = misc.chrtr2_share->chrtr2_header[chrtr2].mbr.wlon + 
                (NINT ((mbr.max_x - misc.chrtr2_share->chrtr2_header[chrtr2].mbr.wlon) / misc.chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees)) *
                misc.chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees;


              min_y = qMax (min_y, misc.chrtr2_share->chrtr2_header[chrtr2].mbr.slat);
              max_y = qMin (max_y, misc.chrtr2_share->chrtr2_header[chrtr2].mbr.nlat);
              min_x = qMax (min_x, misc.chrtr2_share->chrtr2_header[chrtr2].mbr.wlon);
              max_x = qMin (max_x, misc.chrtr2_share->chrtr2_header[chrtr2].mbr.elon);


              int32_t width = (NINT ((max_x - min_x) / misc.chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees));
              int32_t height = (NINT ((max_y - min_y) / misc.chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees));
              int32_t row = NINT ((min_y - misc.chrtr2_share->chrtr2_header[chrtr2].mbr.slat) /
                                   misc.chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees);
              int32_t column = NINT ((min_x - misc.chrtr2_share->chrtr2_header[chrtr2].mbr.wlon) /
                                      misc.chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees);

              if (column + width >= misc.chrtr2_share->chrtr2_header[chrtr2].width)
                {
                  width = misc.chrtr2_share->chrtr2_header[chrtr2].width - column - 1;
                  max_x = misc.chrtr2_share->chrtr2_header[chrtr2].mbr.elon;
                }

              if (row + height >= misc.chrtr2_share->chrtr2_header[chrtr2].height)
                {
                  height = misc.chrtr2_share->chrtr2_header[chrtr2].height - row - 1;
                  max_y = misc.chrtr2_share->chrtr2_header[chrtr2].mbr.nlat;
                }


              //  If width or height is less than 0 we're outside the MBR so we want to go to the next CHRTR2 file.

              if (width <= 0 || height <= 0)
                {
                  start_undo_block (&misc, chrtr2, 0, 0, 0, 0);
                }
              else
                {
                  start_undo_block (&misc, chrtr2, row, row + height - 1, column, column + width - 1);


                  //  Loop for the height of the displayed area.

                  NV_I32_COORD2 coord;
                  for (int32_t j = 0 ; j < height ; j++)
                    {
                      coord.y = row + j;


                      //  Compute the latitude of the center of the bin.

                      double lat = misc.chrtr2_share->chrtr2_header[chrtr2].mbr.slat + coord.y *
                        misc.chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees + half_y;


                      //  Loop for the width of the displayed area.
    
                      for (int32_t i = 0 ; i < width ; i++)
                        {
                          coord.x = column + i;


                          //  Compute the longitude of the center of the bin.

                          double lon = misc.chrtr2_share->chrtr2_header[chrtr2].mbr.wlon + coord.x *
                            misc.chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees + half_x;


                          CHRTR2_RECORD chrtr2_record;
                          chrtr2_read_record_lat_lon (misc.chrtr2_handle[chrtr2], lat, lon, &chrtr2_record);


                          if (!append_undo (&misc, chrtr2_record))
                            {
                              QMessageBox::critical (this, tr ("gridEdit"), tr ("Unable to write undo records!"));
                              remove_last_undo (&misc);
                              return;
                            }
                        }
                    }
                }
            }
        }


      map->setCursor (Qt::WaitCursor);


      //  For rectangles we pass a count of zero to tell gridPointEdit that it's a rectangle.

      if (misc.function == RECT_EDIT_AREA_3D)
        {
          misc.chrtr2_share->polygon_count = 0;
        }
      else
        {
          misc.chrtr2_share->polygon_count = count;
        }


      editProc = new QProcess (this);


      QStringList arguments;
      QString arg;


      //  If we are doing a Z value conversion...

      misc.chrtr2_share->z_factor = options.z_factor;


      //  If we are doing a Z value offset...

      misc.chrtr2_share->z_offset = options.z_offset;


      //  If the min or max scale box is locked we need to pass those values to the editors.

      if (options.min_hsv_locked[0])
        {
          arg.sprintf ("--min_hsv_value=%f", options.min_hsv_value[0]);
          arguments += arg;
        }

      if (options.max_hsv_locked[0])
        {
          arg.sprintf ("--max_hsv_value=%f", options.max_hsv_value[0]);
          arguments += arg;
        }


      //  Always add the min and max HSV values.

      arg.sprintf ("--min_hsv_color=%d", options.min_hsv_color[0]);
      arguments += arg;
      arg.sprintf ("--max_hsv_color=%d", options.max_hsv_color[0]);
      arguments += arg;


      //  Always add the shared memory ID (the process ID).

      arg.sprintf ("--shared_memory_key=%d", misc.chrtr2_share->ppid);
      arguments += arg;


      connect (editProc, SIGNAL (finished (int, QProcess::ExitStatus)), this, SLOT (slotEditDone (int, QProcess::ExitStatus)));
      connect (editProc, SIGNAL (readyReadStandardError ()), this, SLOT (slotEditReadyReadStandardError ()));
      connect (editProc, SIGNAL (readyReadStandardOutput ()), this, SLOT (slotEditReadyReadStandardOutput ()));
      connect (editProc, SIGNAL (error (QProcess::ProcessError)), this, SLOT (slotEditError (QProcess::ProcessError)));

      setMainButtons (NVFalse);
      qApp->processEvents ();


      threeD_edit = NVTrue;

      editProc->start (QString (options.edit_name_3D), arguments);
    }
}



//  Timer - timeout signal.  Very much like an X workproc.  This tracks the cursor in associated (linked or child)
//  programs not in this window.  This is active whenever the mouse leaves this window.

void
gridEdit::slotTrackCursor ()
{
  char               ltstring[25], lnstring[25];
  QString            string, x_string, y_string;
  static double      prev_child_x = -999.0, prev_child_y = -999.0, prev_linked_x = -999.0, prev_linked_y = -999.0;
  static int32_t     prev_linked_command_num = 0;


  if (misc.drawing || !cov_area_defined) return;


  //  If we received a bunch of resize signals (see slotResize) and we're not currently drawing
  //  the map, we want to make sure we haven't received a resize for about two seconds.  If that's the
  //  case we will redraw.

  if (need_redraw)
    {
      redraw_count++;
      if (redraw_count > 40)
        {
          redrawMap (NVTrue, NVFalse);
          need_redraw = NVFalse;
          redraw_count = 0;
        }
    }


  //  Boy, is this weird!  We might be getting position information from one of two sources.  Either from a child process
  //  or from a linked application.  If it's a child and we're linked we need to inform other linked processes about the
  //  cursor location.  If it's a linked app we need to inform the child processes about the location.  In order to figure
  //  out which one it's from we have to check for change in either the abe_register memory or the chrtr2_share memory.


  uint8_t hit = NVFalse;
  double lat = 0.0;
  double lon = 0.0;


  //  This is the case where one of our children has focus.

  if (misc.chrtr2_share->active_window_id != misc.process_id && (prev_child_x != misc.chrtr2_share->cursor_position.x ||
                                                                 prev_child_y != misc.chrtr2_share->cursor_position.y))
    {
      lat = misc.chrtr2_share->cursor_position.y;
      lon = misc.chrtr2_share->cursor_position.x;

      prev_child_x = lon;
      prev_child_y = lat;

      if (lat >= misc.total_displayed_area.min_y && lat <= misc.total_displayed_area.max_y && 
          lon >= misc.total_displayed_area.min_x && lon <= misc.total_displayed_area.max_x) hit = NVTrue;


      //  Tell other apps what our child is doing.

      if (misc.linked)
        {
          misc.abe_register->data[abe_register_group].active_window_id = misc.chrtr2_share->active_window_id;
          misc.abe_register->data[abe_register_group].cursor_position.x = lon;
          misc.abe_register->data[abe_register_group].cursor_position.y = lat;
        }
    }


  //  This is the case where some other app has focus.

  else if (misc.linked && misc.abe_register->data[abe_register_group].active_window_id != misc.process_id &&
           (prev_linked_x != misc.abe_register->data[abe_register_group].cursor_position.x ||
            prev_linked_y != misc.abe_register->data[abe_register_group].cursor_position.y))
    {

      //  Check for a command, make sure it's a new command (we haven't already done it), and make sure
      //  it's not one that we issued.

      if (misc.abe_register->command[abe_register_group].number != prev_linked_command_num && 
          misc.abe_register->command[abe_register_group].window_id != misc.process_id)
        {
          prev_linked_command_num = misc.abe_register->command[abe_register_group].number;

          switch (misc.abe_register->command[abe_register_group].id)
            {
            case ZOOM_TO_MBR_COMMAND:
              misc.clear = NVTrue;

              misc.GeoTIFF_init = NVTrue;

              zoomIn (misc.abe_register->command[abe_register_group].mbr, NVTrue);


              misc.cov_function = COV_START_DRAW_RECTANGLE;

              redrawCovBounds ();

              map->setToolTip ("");      
              misc.function = misc.save_function;

              setFunctionCursor (misc.function);

              break;
            }
        }


      //  Get the geographic location of the cursor from the other application.

      lat = misc.abe_register->data[abe_register_group].cursor_position.y;
      lon = misc.abe_register->data[abe_register_group].cursor_position.x;

      prev_linked_x = lon;
      prev_linked_y = lat;

      if (lat >= misc.total_displayed_area.min_y && lat <= misc.total_displayed_area.max_y && 
          lon >= misc.total_displayed_area.min_x && lon <= misc.total_displayed_area.max_x) hit = NVTrue;
    }


  //  If the geographic position of the cursor in the other application is in our displayed area set the marker and the
  //  status bar information.

  if (hit)
    {
      double deg, min, sec;
      char       hem;


      strcpy (ltstring, fixpos (lat, &deg, &min, &sec, &hem, POS_LAT, options.position_form));
      strcpy (lnstring, fixpos (lon, &deg, &min, &sec, &hem, POS_LON, options.position_form));

      latLabel->setText (ltstring);
      lonLabel->setText (lnstring);

      NV_I32_COORD2 coord;
      chrtr2_get_coord (misc.chrtr2_handle[0], lat, lon, &coord);

      coordXLabel->setNum (coord.x);
      coordYLabel->setNum (coord.y);

      QBrush b1;
      map->setMovingPath (&mv_tracker, marker, lon, lat, 2, options.contour_color, b1, NVFalse, Qt::SolidLine);
    }
}



//  Mouse press signal from the map class.

void 
gridEdit::slotMousePress (QMouseEvent *e, double lon, double lat)
{
  if (!misc.drawing && !grid_edit_active && !process_running)
    {
      //  Check for dateline.

      if (misc.dateline && lon < 0.0) lon += 360.0;


      if (e->button () == Qt::LeftButton)
        {
          //  Bring the nearest CHRTR2 layer to the top level if Ctrl-left-click.

          if (e->modifiers () == Qt::ControlModifier && misc.nearest_chrtr2 > 0)
            {
              slotLayerGrpTriggered (layer[misc.nearest_chrtr2]);
            }
          else
            {
              leftMouse (e->x (), e->y (), lon, lat);
            }
        }
      else if (e->button () == Qt::MidButton)
        {
          midMouse (e->x (), e->y (), lon, lat);
        }
      else if (e->button () == Qt::RightButton)
        {
          rightMouse (e->x (), e->y (), lon, lat);
        }
    }
}



//  Mouse press signal prior to signals being enabled from the map class.  This is just used to pop up the help verbiage
//  if you haven't opened a CHRTR2 file yet.

void 
gridEdit::slotPreliminaryMousePress (QMouseEvent *e)
{
  QPoint pos = QPoint (e->x (), e->y ());

  QWhatsThis::showText (pos, mapText, map);
}



//  Mouse release signal from the map class.

void 
gridEdit::slotMouseRelease (QMouseEvent *e, double lon __attribute__ ((unused)), double lat __attribute__ ((unused)))
{
  if (!misc.drawing && !grid_edit_active)
    {
      if (e->button () == Qt::LeftButton) popup_active = NVFalse;
      if (e->button () == Qt::MidButton) popup_active = NVFalse;
      if (e->button () == Qt::RightButton) {};
    }
}



//  Mouse move signal from the map class.

void
gridEdit::slotMouseMove (QMouseEvent *e, double lon, double lat)
{
  char                 ltstring[25], lnstring[25], hem;
  QString              string, x_string, y_string, z_string, hv_string, unc_string, num_string;
  double               deg, min, sec;
  NV_F64_COORD2        xy;
  NV_I32_COORD2        ixy, prev_xy = {-1, -1};
  CHRTR2_RECORD        chrtr2_record;
  static float         prev_z_factor = 1.0;
  static float         prev_z_offset = 0.0;



  //  Don't worry about the mouse if we're drawing

  if (misc.drawing) return;


  //  Check for dateline.

  if (misc.dateline && lon < 0.0) lon += 360.0;


  //  Get rid of the tracking cursor from slotTrackCursor ()since, if we got a move signal, we're obviously in
  //  this window.  We only want to kill it if it already exists, otherwise we will be creating a new one (which
  //  we don't want to do).

  if (mv_tracker >= 0) map->closeMovingPath (&mv_tracker);


  //  Let other CHRTR2 programs know which window we're in.

  misc.chrtr2_share->active_window_id = misc.process_id;


  //  Track the cursor position for other CHRTR2 programs

  misc.chrtr2_share->cursor_position.y = lat;
  misc.chrtr2_share->cursor_position.x = lon;


  //  Track the cursor position for linked apps

  if (misc.linked)
    {
      misc.abe_register->data[abe_register_group].active_window_id = misc.process_id;
      misc.abe_register->data[abe_register_group].cursor_position.y = lat;
      misc.abe_register->data[abe_register_group].cursor_position.x = lon;
    }


  //  Save the cursor position in screen space (pixels).

  ixy.x = e->x ();
  ixy.y = e->y ();


  //  If the cursor position has changed we want to set the status display and markers (also other goodies).

  if (lat >= misc.total_displayed_area.min_y && lat <= misc.total_displayed_area.max_y && 
      lon >= misc.total_displayed_area.min_x && lon <= misc.total_displayed_area.max_x)
    {
      strcpy (ltstring, fixpos (lat, &deg, &min, &sec, &hem, POS_LAT, options.position_form));
      strcpy (lnstring, fixpos (lon, &deg, &min, &sec, &hem, POS_LON, options.position_form));


      //  Try to find the highest layer with a valid value to display in the status bar.

      xy.y = lat;
      xy.x = lon;
      int32_t hit = -1;

      for (int32_t chrtr2 = 0 ; chrtr2 < misc.chrtr2_share->chrtr2_count ; chrtr2++)
        {
          if (misc.chrtr2_share->display_chrtr2[chrtr2] && inside_mbr_coord2 (&misc.chrtr2_share->chrtr2_header[chrtr2].mbr, xy))
            {
              chrtr2_read_record_lat_lon (misc.chrtr2_handle[chrtr2], lat, lon, &chrtr2_record);


              //  Check the status.

              if (chrtr2_record.status)
                {
                  hit = chrtr2;
                  break;
                }
            }
        }


      //  We only change the status bar CHRTR2 values if we were actually over a CHRTR2 data area

      if (hit >= 0)
        {
          misc.nearest_chrtr2 = hit;


          QString xtmp, ytmp;


          //  If the either bin size is less than .1 minute let's report bin size in meters.

          if (misc.ss_cell_size_x[hit] < 185.2 || misc.ss_cell_size_y[hit] < 185.2)
            {
              xtmp = QString ("%1").arg (misc.ss_cell_size_x[hit], 0, 'f', 2);
              xSizeName->setToolTip (tr ("X bin size in meters"));

              ytmp = QString ("%1").arg (misc.ss_cell_size_y[hit], 0, 'f', 2);
              ySizeName->setToolTip (tr ("Y bin size in meters"));
            }
          else
            {
              xtmp = QString ("%1").arg (misc.chrtr2_share->chrtr2_header[hit].lon_grid_size_degrees * 60.0, 0, 'f', 2);
              xSizeName->setToolTip (tr ("X bin size in minutes"));

              ytmp = QString ("%1").arg (misc.chrtr2_share->chrtr2_header[hit].lat_grid_size_degrees * 60.0, 0, 'f', 2);
              ySizeName->setToolTip (tr ("Y bin size in minutes"));
            }


          xSizeLabel->setText (xtmp);
          ySizeLabel->setText (ytmp);


          chrtr2Label->setText (QFileInfo (QString (misc.chrtr2_share->chrtr2_file[hit])).fileName ().remove (".ch2"));


          nearest_value = chrtr2_record.z * options.z_factor + options.z_offset;


          //  We may need the nearest Z value for 3D apps.

          misc.chrtr2_share->cursor_position.z = chrtr2_record.z;


          z_string.sprintf ("%.2f", nearest_value);


          hv_string.sprintf ("%.2f / %.2f", chrtr2_record.horizontal_uncertainty, chrtr2_record.vertical_uncertainty);
          hvLabel->setText (hv_string);

          unc_string.sprintf ("%.2f", chrtr2_record.uncertainty);
          uncLabel->setText (unc_string);

          num_string.sprintf ("%d", chrtr2_record.number_of_points);
          numLabel->setText (num_string);


          //  If we're scaling or offsetting the values, color the status background yellow.

          if ((options.z_factor != 1.0 && prev_z_factor == 1.0) || (options.z_offset != 0.0 && prev_z_offset == 0.0))
            {
              zPalette.setColor (QPalette::Normal, QPalette::Window, Qt::yellow);

              zLabel->setToolTip (tr ("Scaled/offset Z value for grid"));
            }
          else if ((options.z_factor == 1.0 && prev_z_factor != 1.0) || (options.z_offset == 0.0 && prev_z_offset != 0.0))
            {
              zPalette.setColor (QPalette::Normal, QPalette::Window, misc.widgetBackgroundColor);

              zLabel->setToolTip (tr ("Z value for grid"));
            }
          zLabel->setPalette (zPalette);
          prev_z_factor = options.z_factor;
          prev_z_offset = options.z_offset;

          zLabel->setText (z_string);

          if (chrtr2_record.status & CHRTR2_REAL)
            {
              tLabel->setText ("(R)");
            } 
          else if (chrtr2_record.status & CHRTR2_DIGITIZED_CONTOUR)
            {
              tLabel->setText ("(D)");
            }
          else if (chrtr2_record.status & CHRTR2_INTERPOLATED)
            {
              tLabel->setText ("(I)");
            }
        }
      else
        {
          zLabel->setText ("");
          hvLabel->setText ("");
          uncLabel->setText ("");
          numLabel->setText ("");
          tLabel->setText ("(N)");

          misc.chrtr2_share->cursor_position.z = CHRTR2_NULL_Z_VALUE;
        }

      latLabel->setText (ltstring);
      lonLabel->setText (lnstring);

      NV_I32_COORD2 coord;
      chrtr2_get_coord (misc.chrtr2_handle[0], lat, lon, &coord);

      coordXLabel->setNum (coord.x);
      coordYLabel->setNum (coord.y);
    }


  double anc_lat, anc_lon, az, dist;
  QBrush b1;
  QColor c1;


  //  Actions based on the active function.  Here we're usually draggin a rubberband rectangle, line, or polygon line based
  //  on the active function.

  switch (misc.function)
    {
    case ZOOM_IN_AREA:
    case RECT_EDIT_AREA_3D:
    case RECT_DELETE_AREA:
    case RECT_FILL_AREA:
    case SELECT_HIGH_POINT:
    case SELECT_LOW_POINT:
    case RECT_FILTER_AREA:
    case RECT_FILTER_MASK:
    case REMISP_AREA:
    case DEFINE_RECT_AREA:
      if (map->rubberbandRectangleIsActive (rb_rectangle))
        {
          map->dragRubberbandRectangle (rb_rectangle, lon, lat);

          map->getRubberbandRectangleAnchor (rb_rectangle, &anc_lon, &anc_lat);

          invgp (NV_A0, NV_B0, anc_lat, anc_lon, lat, lon, &dist, &az);


          string.sprintf (tr (" Distance from anchor in meters: %.2f ").toLatin1 (), dist);
          misc.statusProgLabel->setText (string);
        }
      break;

    case POLY_EDIT_AREA_3D:
    case POLY_DELETE_AREA:
    case POLY_DELETE_CONTOUR:
    case POLY_FILL_AREA:
    case POLY_FILL_PFM:
    case POLY_FILTER_AREA:
    case POLY_FILTER_MASK:
    case DEFINE_POLY_AREA:
    case GRAB_CONTOUR:
      if (map->rubberbandPolygonIsActive (rb_polygon))
        {
          map->dragRubberbandPolygon (rb_polygon, lon, lat);

          map->getRubberbandPolygonCurrentAnchor (rb_polygon, &anc_lon, &anc_lat);

          invgp (NV_A0, NV_B0, anc_lat, anc_lon, lat, lon, &dist, &az);

 
          string.sprintf (tr (" Distance from anchor in meters: %.2f ").toLatin1 (), dist);
          misc.statusProgLabel->setText (string);
        }
      break;

    case OUTPUT_POINTS:
      string.sprintf (tr (" Point #: %d ").toLatin1 (), misc.output_point_count);
      misc.statusProgLabel->setText (string);
      break;

    case DRAW_CONTOUR:

      //  Only process if the pointer position has changed pixels.

      if ((ixy.x != prev_xy.x || ixy.y != prev_xy.y) && map->rubberbandPolygonIsActive (rb_polygon))
        {
          xy.y = lat;
          xy.x = lon;


          if (prev_poly_lon > -180.5) map->vertexRubberbandPolygon (rb_polygon, prev_poly_lon, prev_poly_lat);
          map->dragRubberbandPolygon (rb_polygon, lon, lat);
          prev_poly_lat = lat;
          prev_poly_lon = lon;

          QString msc;
          msc.sprintf (tr (" Draw contour - %.2f ").toLatin1 (), options.draw_contour_level);
          misc.statusProgLabel->setText (msc);
          misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
          misc.statusProgLabel->setPalette (misc.statusProgPalette);
        }
      break;


    case CLEAR_CONTOUR:
      double min_dist = 999999999.0;
      int32_t np = -1;
      nearest_drawn_contour = -1;
      for (int32_t i = 0 ; i < misc.hd_contour_count ; i++)
        {
          for (int32_t j = 0 ; j < misc.hd_contour[i].count ; j++)
            {
              double x_diff = misc.hd_contour[i].xyz[j].x - lon;
              double y_diff = misc.hd_contour[i].xyz[j].y - lat;
              double dist = sqrt (x_diff * x_diff + y_diff * y_diff);
              if (dist < min_dist)
                {
                  min_dist = dist;
                  nearest_drawn_contour = i;
                  np = j;
                }
            }
        }

      if (nearest_drawn_contour >= 0)
        {
          QBrush b1;
          map->setMovingPath (&mv_marker, marker, misc.hd_contour[nearest_drawn_contour].xyz[np].x, misc.hd_contour[nearest_drawn_contour].xyz[np].y, 2,
                              options.contour_color, b1, NVFalse, Qt::SolidLine);
        }
      break;
    }


  //  Set the previous cursor.
             
  prev_xy = ixy;
}



//  Resize signal from the map class.

void
gridEdit::slotResize (QResizeEvent *e __attribute__ ((unused)))
{
  //  The first time we get a resize (or if we have closed all of the open CHRTR2 files) we want to display the 
  //  fabulously elegant gridEdit splash screen ;-)

  if (logo_first)
    {
      QPixmap logo = QPixmap (":/icons/gridEdit.jpg");

      int32_t w = map->width () - 3 * mapdef.border;
      int32_t h = map->height () - 3 * mapdef.border;

      QPixmap logo_scaled = logo.scaled (w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

      map->drawPixmap (mapdef.border, mapdef.border, &logo_scaled, 0, 0, w, h, NVTrue);

      map->setCursor (Qt::WhatsThisCursor);


      //  Display the new features (startup) message if needed.

      if (startup && options.startup_message)
        {
          startupMessage *sm = new startupMessage (this, &options, new_feature);


          //  Put the dialog in the middle of the screen.

          sm->move (x () + width () / 2 - sm->width () / 2, y () + height () / 2 - sm->height () / 2);

          startup = NVFalse;
        }
    }
  else
    {
      if (misc.drawing) return;


      //  If the windowing system is doing "Display content in resizing windows" we'll get about a million
      //  resize callbacks all stacked up.  What we want to do is use the trackCursor function to wait for
      //  2 seconds of inactivity (i.e. no resizes) and then redraw the map.  We use the redraw_count to 
      //  time it.  I thought this was easier than starting and killing a one-shit timer every time we
      //  came through this section of code.

      need_redraw = NVTrue;
      redraw_count = 0;
      misc.GeoTIFF_init = NVTrue;

      map->setCursor (Qt::WaitCursor);
      qApp->processEvents ();
    }
}



//  Prior to drawing coastline signal from the map class.  This is where we do our heavy lifting (actually in paint_surface).

void 
gridEdit::slotPreRedraw (NVMAP_DEF l_mapdef)
{
  void paint_surface (MISC *misc, OPTIONS *options, nvMap *map, NVMAP_DEF *mapdef, uint8_t cov_area_defined);


  //  Set the drawing flag and turn the map mouse signals off until we are through painting.

  misc.drawing = NVTrue;
  map->disableMouseSignals ();


  setMainButtons (NVFalse);

  map->setCursor (Qt::WaitCursor);
  cov->setCursor (Qt::WaitCursor);

  qApp->processEvents ();

  mapdef = l_mapdef;


  paint_surface (&misc, &options, map, &mapdef, cov_area_defined);
}



//  Move the display left, right, up, or down.  This is called from the keyPressEvent or when a move
//  is requested in gridPointEdit3D.

void 
gridEdit::moveMap (int32_t direction)
{
  discardMovableObjects ();


  clearFilterMasks ();


  map->moveMap (direction);


  //  We want to redraw the grid3D view if it's up.

  if (threeDProc)
    {
      if (threeDProc->state () == QProcess::Running) misc.chrtr2_share->key = GRID3D_FORCE_RELOAD;
    }
}



void
gridEdit::zoomIn (NV_F64_XYMBR bounds, uint8_t grid3D)
{
  discardMovableObjects ();

  clearFilterMasks ();

  map->zoomIn (bounds);

  bZoomOut->setEnabled (true);


  //  We want to redraw the grid3D view if it's up.

  if (grid3D && threeDProc && threeDProc->state () == QProcess::Running) misc.chrtr2_share->key = GRID3D_FORCE_RELOAD;
}



void
gridEdit::zoomOut ()
{
  discardMovableObjects ();


  clearFilterMasks ();


  map->zoomOut ();


  //  We want to redraw the grid3D view if it's up.

  if (threeDProc && threeDProc->state () == QProcess::Running) misc.chrtr2_share->key = GRID3D_FORCE_RELOAD;
}



void
gridEdit::redrawMap (uint8_t clear, uint8_t grid3D)
{
  misc.clear = NVTrue;

  discardMovableObjects ();


  //  Clear the filter mask displayed flags.

  for (int32_t i = 0 ; i < misc.poly_filter_mask_count ; i++) misc.poly_filter_mask[i].displayed = NVFalse;


  map->redrawMap (clear);


  if (grid3D && threeDProc && threeDProc->state () == QProcess::Running) misc.chrtr2_share->key = GRID3D_FORCE_RELOAD;
}



void
gridEdit::slotRedraw ()
{
  redrawMap (NVTrue, NVFalse);
}



void
gridEdit::slotRedrawCoverage ()
{
  misc.cov_clear = NVTrue;

  cov->redrawMap (NVTrue);
  redrawCovBounds ();
}



//  Using the keys to move around.  We also may be starting the GeoTIFF viewer (mosaicView) or chrtr23D.

void
gridEdit::keyPressEvent (QKeyEvent *e)
{
  //  Discard key presses if we're drawing or if one of the editors is running.

  if (!misc.drawing && !grid_edit_active)
    {
      QString qkey = e->text ();


      if (qkey == options.threeD_hotkey)
        {
          //  3D surface viewer.

          slotStart3DViewer ();
        }


      //  Move the viewing area up, down, left, right, up/right, down/right, up/left, and down/left based on the arrow (or keypad) key pressed.

      else
        {
          misc.clear = NVTrue;
          switch (e->key ())
            {
            case Qt::Key_Left:
              moveMap (NVMAP_LEFT);
              break;

            case Qt::Key_Up:
              moveMap (NVMAP_UP);
              break;

            case Qt::Key_Right:
              moveMap (NVMAP_RIGHT);
              break;

            case Qt::Key_Down:
              moveMap (NVMAP_DOWN);
              break;

            case Qt::Key_PageUp:
              moveMap (NVMAP_UP_RIGHT);
              break;

            case Qt::Key_PageDown:
              moveMap (NVMAP_DOWN_RIGHT);
              break;

            case Qt::Key_Home:
              moveMap (NVMAP_UP_LEFT);
              break;

            case Qt::Key_End:
              moveMap (NVMAP_DOWN_LEFT);
              break;

            default:
              return;
            }

          redrawCovBounds ();
        }
    }
}



//  Somebody pressed the little X in the window decoration.  DOH!

void 
gridEdit::closeEvent (QCloseEvent *event __attribute__ ((unused)))
{
  slotQuit ();
}



void 
gridEdit::slotClose (QCloseEvent *event __attribute__ ((unused)))
{
  slotQuit ();
}



//  A bunch of slots.

void 
gridEdit::slotQuit ()
{
  void envout (OPTIONS *options, MISC *misc, QMainWindow *mainWindow);


  //  Check to see if we have any unused hand drawn contours

  if (misc.hd_contour_count)
    {
      int32_t ret = QMessageBox::Yes;
      ret = QMessageBox::information (this, tr ("gridEdit"), tr ("You have hand drawn contours that have not been regridded.\n") +
                                      tr ("These will be discarded if you exit.\n") +
                                      tr ("Do you wish to exit anyway?"), QMessageBox::Yes,  QMessageBox::No,  QMessageBox::NoButton);

      if (ret != QMessageBox::Yes) return;
    }


  options.edit_mode = misc.last_edit_function;


  //  Save the user's defaults

  envout (&options, &misc, this);


  //  Close any open undo files

  delete_undo (&misc);


  //  Get rid of the shared memory.

  misc.chrtr2Share->detach ();


  //  If we're linked to another program(s), unregister the link.

  if (misc.linked) unregisterABE (misc.abeRegister, misc.abe_register, abe_register_group, misc.process_id);


  exit (0);
}



//  Post grid drawing signal from map class.  Mostly just cleanup work.

void 
gridEdit::slotPostRedraw (NVMAP_DEF mapdef __attribute__ ((unused)))
{
  //  Turn the map mouse signals back on.

  map->enableMouseSignals ();


  logo_first = NVFalse;


  if (cov_area_defined)
    {
      //  Redraw the coverage map if needed.

      if (misc.prev_total_displayed_area.min_x != 999.0 &&
          (misc.prev_total_displayed_area.min_x != misc.total_displayed_area.min_x || 
           misc.prev_total_displayed_area.min_y != misc.total_displayed_area.min_y ||
           misc.prev_total_displayed_area.max_x != misc.total_displayed_area.max_x || 
           misc.prev_total_displayed_area.max_y != misc.total_displayed_area.max_y))
        {
          misc.cov_clear = NVTrue;
          cov->redrawMap (NVFalse);
          redrawCovBounds ();
        }
    }

  misc.prev_total_displayed_area = misc.total_displayed_area;
  misc.drawing_canceled = NVFalse;

  setFunctionCursor (misc.function);

  setMainButtons (NVTrue);

  cov->setCursor (editRectCursor);

  misc.drawing = NVFalse;
}



//  Open CHRTR2 file

void 
gridEdit::slotOpen ()
{
  QStringList files, filters;
  QString file, dir;
  int32_t chrtr2_handle = -1;
  char chrtr2_file[512];
  CHRTR2_HEADER chrtr2_header;


  uint8_t check_recent (QWidget *, OPTIONS *options, MISC *misc, QString file, QActionGroup *recentGrp, QMenu *recentMenu, QAction **recentFileAction);


  uint8_t accepted = NVFalse;

  if (recent_file_flag)
    {
      files << options.recentFile[recent_file_flag - 1];
      dir = QFileInfo (options.recentFile[recent_file_flag - 1]).dir ().absolutePath ();

      recent_file_flag = 0;

      accepted = NVTrue;
    }
  else if (command_file_flag)
    {
      files << commandFile;
      dir = QFileInfo (commandFile).dir ().absolutePath ();

      command_file_flag = NVFalse;

      accepted = NVTrue;
    }
  else
    {
      if (!QDir (options.input_chrtr2_dir).exists ()) options.input_chrtr2_dir = ".";


      QFileDialog *fd = new QFileDialog (this, tr ("gridEdit Open CHRTR2 File"));
      fd->setViewMode (QFileDialog::List);


      //  Always add the current working directory and the last used directory to the sidebar URLs in case we're running from the command line.
      //  This function is in the nvutility library.

      setSidebarUrls (fd, options.input_chrtr2_dir);


      filters << tr ("CHRTR2 (*.ch2)");

      fd->setNameFilters (filters);
      fd->setFileMode (QFileDialog::ExistingFiles);
      fd->selectNameFilter (tr ("CHRTR2 (*.ch2)"));


      if (fd->exec () == QDialog::Accepted) accepted = NVTrue;

      files = fd->selectedFiles ();
      dir = fd->directory ().absolutePath ();
    }


  if (accepted)
    {
      if ((misc.chrtr2_share->chrtr2_count + files.size ()) >= MAX_CHRTR2_FILES)
        {
          QMessageBox::warning (this, tr ("gridEdit Open CHRTR2 File"), tr ("Too many open CHRTR2 files.\n") +
                                tr ("Please close some before trying to open new ones."));
          return;
        }


      for (int32_t i = 0 ; i < files.size () ; i++)
        {
          file = files.at (i);


          if (!file.isEmpty())
            {
              //  Check the file to see if we already have it opened.  If we do then we'll just move it to the top level.

              uint8_t opened = NVFalse;
              for (int32_t chrtr2 = 0 ; chrtr2 < misc.chrtr2_share->chrtr2_count ; chrtr2++)
                {
                  if (!strcmp (misc.chrtr2_share->chrtr2_file[chrtr2], chrtr2_file))
                    {
                      //  No point in moving the top level layer to the top level.

                      if (chrtr2) slotLayerGrpTriggered (layer[chrtr2]);

                      opened = NVTrue;
                    }
                }


              //  Open the file and make sure it is a valid CHRTR2 file.

              strcpy (chrtr2_file, file.toLatin1 ());

              chrtr2_handle = chrtr2_open_file (chrtr2_file, &chrtr2_header, CHRTR2_UPDATE);

              if (chrtr2_handle < 0)
                {
                  QMessageBox::warning (this, tr ("gridEdit Open CHRTR2 File"), tr ("The file ") + 
                                        QDir::toNativeSeparators (file) + 
                                        tr (" is not a CHRTR2 file or there was an error reading the file.") +
                                        tr ("  The error message returned was:\n\n") + QString (chrtr2_strerror ()));

                  return;
                }

              if (chrtr2_header.z_units != CHRTR2_METERS)
                {
                  QMessageBox::warning (this, gridEdit::tr ("Open CHRTR2 File"), gridEdit::tr ("The file ") + 
                                        QDir::toNativeSeparators (QString (chrtr2_file)) + 
                                        gridEdit::tr (" does not use CHRTR2_METERS units.  This is not allowed."));
                  chrtr2_close_file (chrtr2_handle);
                  return;
                }


              //  Open up a coverage map in memory.  This is maintained by the chrtr2 API and freed on chrtr2_close_file.

              qApp->setOverrideCursor (Qt::WaitCursor);
              qApp->processEvents ();

              chrtr2_open_cov_map (chrtr2_handle);

              qApp->restoreOverrideCursor ();


              if (!opened)
                {
                  //  Check to see if this is one of the recently opened files.  If not, add it to the list.

                  if (!check_recent (this, &options, &misc, file, recentGrp, recentMenu, recentFileAction)) return;


                  for (int32_t chrtr2 = misc.chrtr2_share->chrtr2_count ; chrtr2 > 0 ; chrtr2--)
                    {
                      strcpy (misc.chrtr2_share->chrtr2_file[chrtr2], misc.chrtr2_share->chrtr2_file[chrtr2 - 1]);
                      misc.chrtr2_share->chrtr2_header[chrtr2] = misc.chrtr2_share->chrtr2_header[chrtr2 - 1];
                      misc.ss_cell_size_x[chrtr2] = misc.ss_cell_size_x[chrtr2 - 1];
                      misc.ss_cell_size_y[chrtr2] = misc.ss_cell_size_y[chrtr2 - 1];
                      misc.chrtr2_handle[chrtr2] = misc.chrtr2_handle[chrtr2 - 1];
                      misc.chrtr2_share->display_chrtr2[chrtr2] = misc.chrtr2_share->display_chrtr2[chrtr2 - 1];
                      misc.chrtr2_share->lock_chrtr2[chrtr2] = misc.chrtr2_share->lock_chrtr2[chrtr2 - 1];
                      misc.chrtr2_alpha[chrtr2] = misc.chrtr2_alpha[chrtr2 - 1];


                      QString layerString;
                      layerString.sprintf ("%d - ", chrtr2 + 1);
                      layerString += QFileInfo (QString (misc.chrtr2_share->chrtr2_file[chrtr2])).fileName ();
                      layer[chrtr2]->setText (layerString);
                      layer[chrtr2]->setVisible (true);
                    }

                  misc.chrtr2_handle[0] = chrtr2_handle;
                  misc.chrtr2_share->chrtr2_header[0] = chrtr2_header;
                  strcpy (misc.chrtr2_share->chrtr2_file[0], file.toLatin1 ());

                  misc.GeoTIFF_open = NVFalse;
                  misc.display_GeoTIFF = NVFalse;
                  bGeotiff->setEnabled (false);
                  misc.GeoTIFF_init = NVTrue;


                  QString tmp = tr ("Highlight ") + QString (misc.chrtr2_share->chrtr2_header[0].user_flag_name[0]) + tr (" data");
                  highlightUser01->setText (tmp);
                  tmp = tr ("Highlight ") + QString (misc.chrtr2_share->chrtr2_header[0].user_flag_name[1]) + tr (" data");
                  highlightUser02->setText (tmp);
                  tmp = tr ("Highlight ") + QString (misc.chrtr2_share->chrtr2_header[0].user_flag_name[2]) + tr (" data");
                  highlightUser03->setText (tmp);
                  tmp = tr ("Highlight ") + QString (misc.chrtr2_share->chrtr2_header[0].user_flag_name[3]) + tr (" data");
                  highlightUser04->setText (tmp);

                  highlightUser01->setVisible (!QString (misc.chrtr2_share->chrtr2_header[0].user_flag_name[0]).isEmpty ());
                  highlightUser02->setVisible (!QString (misc.chrtr2_share->chrtr2_header[0].user_flag_name[1]).isEmpty ());
                  highlightUser03->setVisible (!QString (misc.chrtr2_share->chrtr2_header[0].user_flag_name[2]).isEmpty ());
                  highlightUser04->setVisible (!QString (misc.chrtr2_share->chrtr2_header[0].user_flag_name[3]).isEmpty ());


                  //  Compute cell sizes (in meters) for sunshading.

                  double dist, az;
                  double center_map_x = misc.chrtr2_share->chrtr2_header[0].mbr.wlon + (misc.chrtr2_share->chrtr2_header[0].mbr.elon -
                                                                                            misc.chrtr2_share->chrtr2_header[0].mbr.wlon) / 2.0;
                  double center_map_y = misc.chrtr2_share->chrtr2_header[0].mbr.slat + (misc.chrtr2_share->chrtr2_header[0].mbr.nlat -
                                                                                            misc.chrtr2_share->chrtr2_header[0].mbr.slat) / 2.0;

                  invgp (NV_A0, NV_B0, center_map_y, center_map_x, center_map_y, center_map_x + misc.chrtr2_share->chrtr2_header[0].lon_grid_size_degrees,
                         &dist, &az);
                  misc.ss_cell_size_x[0] = dist;

                  invgp (NV_A0, NV_B0, center_map_y, center_map_x, center_map_y + misc.chrtr2_share->chrtr2_header[0].lat_grid_size_degrees, center_map_x,
                         &dist, &az);
                  misc.ss_cell_size_y[0] = dist;

                  QString layerString = "1 - " + QFileInfo (QString (misc.chrtr2_share->chrtr2_file[0])).fileName ();
                  layer[0]->setText (layerString);
                  layer[0]->setVisible (true);
                }


              misc.chrtr2_share->display_chrtr2[0] = NVTrue;
              misc.chrtr2_alpha[0] = 255;


              misc.chrtr2_share->chrtr2_count++;


              //  Set the locks if neccessary (the buttons will get set in initializeMaps).

              misc.chrtr2_share->lock_chrtr2[0] = NVFalse;

              for (int32_t chrtr2 = 1 ; chrtr2 < misc.chrtr2_share->chrtr2_count ; chrtr2++)
                {
                  if (options.non_top_layers_locked) misc.chrtr2_share->lock_chrtr2[chrtr2] = NVTrue;
                }


              //  We have to discard all undo files if we open a new file after editing.

              if (misc.undo_count) delete_undo (&misc);


              //  If we are editing a new CHRTR2, switch the working directories to be the CHRTR2 directory.  Otherwise, we
              //  leave them where they were.

              if (options.input_chrtr2_dir != dir)
                {
                  options.overlay_dir = dir;
                  options.output_area_dir = dir;
                  options.output_points_dir = dir;
                  options.geotiff_dir = dir;
                  options.area_dir = dir;
                }
              options.input_chrtr2_dir = dir;
            }
        }


      //  Initialize the coverage and main maps

      initializeMaps ();
    }
}



void 
gridEdit::slotOpenRecent (QAction *action)
{
  for (int32_t i = 0 ; i < MAX_RECENT ; i++)
    {
      if (recentFileAction[i] == action)
        {
          recent_file_flag = i + 1;
          break;
        }
    }

  slotOpen ();
}



//  Close one of the CHRTR2 layers.

void 
gridEdit::slotCloseCHRTR2 ()
{
  if (QMessageBox::information (this, tr ("gridEdit Close CHRTR2 confirmation"), 
                                tr ("Do you really want to close\n") + 
                                QDir::toNativeSeparators (QString (misc.chrtr2_share->chrtr2_file[0])),
                                QMessageBox::Yes,  QMessageBox::No,  QMessageBox::NoButton) == QMessageBox::No) return;


  remove_layer (&misc, 0);

  if (!misc.chrtr2_share->chrtr2_count) cov_area_defined = NVFalse;


  //  We have to discard all undo files if we open a new file after editing.

  if (misc.undo_count) delete_undo (&misc);


  slotManageLayersDataChanged ();
}



void 
gridEdit::initializeMaps ()
{
  //  Set up the CHRTR2 layers notebook page filenames

  int32_t num_displayed = 0;
  notebook->setTabEnabled (1, false);
  for (int32_t chrtr2 = 0 ; chrtr2 < MAX_CHRTR2_FILES ; chrtr2++)
    {
      if (chrtr2 < misc.chrtr2_share->chrtr2_count)
        {
          QString name = QFileInfo (QString (misc.chrtr2_share->chrtr2_file[chrtr2])).fileName ().remove (".ch2");

          layerName[chrtr2]->setText (name);

          if (chrtr2) notebook->setTabEnabled (1, true);

          if (misc.chrtr2_share->display_chrtr2[chrtr2])
            {
              num_displayed++;
              layerDisplay[chrtr2]->setChecked (true);
            }
          else
            {
              layerDisplay[chrtr2]->setChecked (false);
            }

          if (misc.chrtr2_share->lock_chrtr2[chrtr2])
            {
              layerLock[chrtr2]->setChecked (true);
            }
          else
            {
              layerLock[chrtr2]->setChecked (false);
            }
          layerName[chrtr2]->show ();
          layerDisplay[chrtr2]->show ();
          layerLock[chrtr2]->show ();
        }
      else
        {
          layerName[chrtr2]->hide ();
          layerDisplay[chrtr2]->hide ();
          layerLock[chrtr2]->hide ();
        }
    }


  if (misc.linked)
    {
      changeFileRegisterABE (misc.abeRegister, misc.abe_register, abe_register_group, misc.process_id, misc.chrtr2_share->chrtr2_file[0]);
      QString title;
      title.sprintf ("gridEdit (ABE group %02d) : ", abe_register_group);
      title += QString (misc.chrtr2_share->chrtr2_file[0]);
      this->setWindowTitle (title);
    }


  //  Compute the MBR of all of the CHRTR2 areas.

  compute_total_mbr (&misc);


  misc.cov_clear = NVTrue;


  NV_F64_XYMBR mbr = mapdef.bounds[mapdef.zoom_level];


  //  Disable the map signals, reset the bounds and then enable the map signals.

  map->disableSignals ();

  clearFilterMasks ();

  map->resetBounds (misc.total_mbr);

  map->enableSignals ();


  //  Don't want to draw coasts or grids when opening the file, but we need to open it to get a size for the coverage map.

  map->setCoasts (NVMAP_NO_COAST);
  map->setGridSpacing (0.0, 0.0);

  if (cov_area_defined)
    {
      zoomIn (mbr, NVTrue);
    }
  else
    {
      redrawMap (NVTrue, NVFalse);
    }

  map->setCoasts (options.coast);


  //  Get the size and other goodies

  mapdef = map->getMapdef ();


  //  Redefine the coverage map bounds.

  covdef.initial_bounds.min_x = misc.total_mbr.min_x;
  covdef.initial_bounds.min_y = misc.total_mbr.min_y;
  covdef.initial_bounds.max_x = misc.total_mbr.max_x;
  covdef.initial_bounds.max_y = misc.total_mbr.max_y;


  cov->resetBounds (covdef.initial_bounds);

  cov->enableSignals ();

  covdef = cov->getMapdef ();

  slotRedrawCoverage ();
}



//  Open the GeoTIFF (image) file.

void 
gridEdit::slotOpenGeotiff ()
{
  QStringList files, filters;
  QString file;

  QFileDialog *fd = new QFileDialog (this, tr ("gridEdit Open GeoTIFF"));
  fd->setViewMode (QFileDialog::List);

  if (!QDir (options.geotiff_dir).exists ()) options.geotiff_dir = options.input_chrtr2_dir;


  //  Always add the current working directory and the last used directory to the sidebar URLs in case we're running from the command line.
  //  This function is in the nvutility library.

  setSidebarUrls (fd, options.geotiff_dir);


  filters << tr ("GeoTIFF (*.tif)");

  fd->setNameFilters (filters);
  fd->setFileMode (QFileDialog::ExistingFile);
  fd->selectNameFilter (tr ("GeoTIFF (*.tif)"));

  if (fd->exec () == QDialog::Accepted)
    {
      files = fd->selectedFiles ();

      file = files.at (0);


      if (!file.isEmpty())
        {
          //  Check the file to make sure it is in the area

          GDALDataset        *poDataset;
          double             adfGeoTransform[6];
          double             GeoTIFF_wlon, GeoTIFF_nlat, GeoTIFF_lon_step, 
                             GeoTIFF_lat_step, GeoTIFF_elon, GeoTIFF_slat;
          int32_t            width, height;


          GDALAllRegister ();


          char path[512];
          strcpy (path, file.toLatin1 ());

          poDataset = (GDALDataset *) GDALOpen (path, GA_ReadOnly);
          if (poDataset != NULL)
            {
              if (poDataset->GetProjectionRef ()  != NULL)
                {
                  QString projRef = QString (poDataset->GetProjectionRef ());

                  if (projRef.contains ("GEOGCS"))
                    {
                      if (poDataset->GetGeoTransform (adfGeoTransform) == CE_None)
                        {
                          GeoTIFF_lon_step = adfGeoTransform[1];
                          GeoTIFF_lat_step = -adfGeoTransform[5];


                          width = poDataset->GetRasterXSize ();
                          height = poDataset->GetRasterYSize ();


                          GeoTIFF_wlon = adfGeoTransform[0];
                          GeoTIFF_nlat = adfGeoTransform[3];


                          GeoTIFF_slat = GeoTIFF_nlat - height * GeoTIFF_lat_step;
                          GeoTIFF_elon = GeoTIFF_wlon + width * GeoTIFF_lon_step;


                          if (GeoTIFF_nlat < misc.total_displayed_area.min_y || 
                              GeoTIFF_slat > misc.total_displayed_area.max_y ||
                              GeoTIFF_elon < misc.total_displayed_area.min_x || 
                              GeoTIFF_wlon > misc.total_displayed_area.max_x)
                            {
                              QMessageBox::warning (this, tr ("gridEdit Open GeoTIFF"), 
                                                    tr ("No part of this GeoTIFF file falls within the displayed area."));
                              return;
                            }
                        }
                      else
                        {
                          QMessageBox::warning (this, tr ("gridEdit Open GeoTIFF"), 
                                                tr ("This program only handles unprojected GeoTIFF files."));
                          return;
                        }
                    }
                  else
                    {
                      QMessageBox::warning (this, tr ("gridEdit Open GeoTIFF"), 
                                            tr ("This program only handles geographic GeoTIFF files."));
                      return;
                    }
                }
              else
                {
                  QMessageBox::warning (this, tr ("gridEdit Open GeoTIFF"), 
                                        tr ("This file has no datum/projection information."));
                  return;
                }
            }
          else
            {
              QMessageBox::warning (this, tr ("gridEdit Open GeoTIFF"), 
                                    tr ("Unable to open file."));
              return;
            }

          delete poDataset;

          misc.GeoTIFF_open = NVTrue;
          bGeotiff->setEnabled (true);

          strcpy (misc.GeoTIFF_name, file.toLatin1 ());
          misc.GeoTIFF_init = NVTrue;

          bGeotiff->setEnabled (true);
          bGeotiff->setChecked (true);
          misc.display_GeoTIFF = NVTrue;
          redrawMap (NVTrue, NVFalse);

          options.geotiff_dir = fd->directory ().absolutePath ();
        }
    }
}



//  Kick off the preferences dialog.

void 
gridEdit::slotPrefs ()
{
  if (prefs_dialog) prefs_dialog->close ();
  prefs_dialog = new Prefs (this, &options, &misc);


  //  Put the dialog in the middle of the screen.

  prefs_dialog->move (x () + width () / 2 - prefs_dialog->width () / 2, y () + height () / 2 - prefs_dialog->height () / 2);

  connect (prefs_dialog, SIGNAL (dataChangedSignal (uint8_t)), this, SLOT (slotPrefDataChanged (uint8_t)));
  connect (prefs_dialog, SIGNAL (hotKeyChangedSignal (int32_t)), this, SLOT (slotPrefHotKeyChanged (int32_t)));
}



//  Changed some of the preferences in the preferences dialog.

void 
gridEdit::slotPrefDataChanged (uint8_t lockChanged)
{
  misc.GeoTIFF_init = NVTrue;


  int32_t h, s, v, a;
  options.cov_real_color.getHsv (&h, &s, &v, &a);
  options.cov_checked_r_color.setHsv (h, COV_CHECKED_SATURATION, v, a);
  options.cov_digitized_color.getHsv (&h, &s, &v, &a);
  options.cov_checked_d_color.setHsv (h, COV_CHECKED_SATURATION, v, a);
  options.cov_interpolated_color.getHsv (&h, &s, &v, &a);
  options.cov_checked_i_color.setHsv (h, COV_CHECKED_SATURATION, v, a);


  bContour->setChecked (options.contour);
  bFlagContour->setChecked (options.flag_contour);
  bInterpolated->setChecked (options.display_interpolated);
  bDisplayMinMax->setChecked (options.display_minmax);


  QString depthString;
  depthString.sprintf (tr ("Highlight data inside depth range (%0.1f to %0.1f)").toLatin1 (), options.highlight_depth[0], options.highlight_depth[1]);
  highlightInside->setText (depthString);
  depthString.sprintf (tr ("Highlight data outside depth range (%0.1f to %0.1f)").toLatin1 (), options.highlight_depth[0], options.highlight_depth[1]);
  highlightOutside->setText (depthString);


  map->setCoastColor (options.coast_color);
  map->setBackgroundColor (options.background_color);
  map->setOverlapPercent (options.overlap_percent);


  //  Check for the layer locks changing.

  if (lockChanged)
    {
      misc.chrtr2_share->lock_chrtr2[0] = NVFalse;

      for (int32_t chrtr2 = 1 ; chrtr2 < misc.chrtr2_share->chrtr2_count ; chrtr2++)
        {
          if (options.non_top_layers_locked)
            {
              misc.chrtr2_share->lock_chrtr2[chrtr2] = NVTrue;
              layerLock[chrtr2]->setIcon (QIcon (":/icons/lock.png"));
              layerLock[chrtr2]->setToolTip (tr ("Click to unlock layer"));
            }
          else
            {
              misc.chrtr2_share->lock_chrtr2[chrtr2] = NVFalse;
              layerLock[chrtr2]->setIcon (QIcon (":/icons/unlock.png"));
              layerLock[chrtr2]->setToolTip (tr ("Click to lock layer"));
            }
        }
    }


  slotRedrawCoverage ();
  if (!misc.drawing) redrawMap (NVTrue, NVFalse);


  //  Set a couple of things that gridPointEdit3D will need to know.

  misc.chrtr2_share->position_form = options.position_form;
  misc.chrtr2_share->smoothing_factor = options.smoothing_factor;
  misc.chrtr2_share->z_factor = options.z_factor;
  misc.chrtr2_share->z_offset = options.z_offset;


  for (int32_t i = 0 ; i < HOTKEYS ; i++) slotPrefHotKeyChanged (i);
}



//  Changed some of the hot keys in the preferences dialog.

void 
gridEdit::slotPrefHotKeyChanged (int32_t i)
{
  QString tip = misc.buttonText[i] + "  [" + options.buttonAccel[i] + "]";
  misc.button[i]->setToolTip (tip);
  misc.button[i]->setShortcut (QKeySequence (options.buttonAccel[i]));
}



void
gridEdit::slotHighlightMenu (QAction *action)
{
  setMainButtons (NVFalse);


  if (action == highlightNone) options.highlight = H_NONE;
  if (action == highlightAll) options.highlight = H_ALL;
  if (action == highlightChecked) options.highlight = H_CHECKED;
  if (action == highlightUser01) options.highlight = H_01;
  if (action == highlightUser02) options.highlight = H_02;
  if (action == highlightUser03) options.highlight = H_03;
  if (action == highlightUser04) options.highlight = H_04;
  if (action == highlightLandMask) options.highlight = H_LAND_MASK;
  if (action == highlightInt) options.highlight = H_INT;

  if (action == highlightInside || action == highlightOutside)
    {
      double res;
      bool ok;

      res = QInputDialog::getDouble (this, tr ("gridEdit"), tr ("Enter the minimum depth for the highlight range:"), options.highlight_depth[0],
                                     -9500.0, 11500.0, 1, &ok);
      if (ok) options.highlight_depth[0] = (float) res;


      res = QInputDialog::getDouble (this, tr ("gridEdit"), tr ("Enter the maximum depth for the highlight range:"), options.highlight_depth[1],
                                     -9500.0, 11500.0, 1, &ok);
      if (ok) options.highlight_depth[1] = (float) res;

      if (options.highlight_depth[1] < options.highlight_depth[0])
        {
          float tmp_depth = options.highlight_depth[1];
          options.highlight_depth[1] = options.highlight_depth[0];
          options.highlight_depth[0] = tmp_depth;
        }

      QString depthString;
      depthString.sprintf (tr ("Highlight data inside depth range (%0.1f to %0.1f)").toLatin1 (), options.highlight_depth[0], options.highlight_depth[1]);
      highlightInside->setText (depthString);
      depthString.sprintf (tr ("Highlight data outside depth range (%0.1f to %0.1f)").toLatin1 (), options.highlight_depth[0], options.highlight_depth[1]);
      highlightOutside->setText (depthString);

      if (action == highlightInside)
        {
          options.highlight = H_INSIDE;
        }
      else
        {
          options.highlight = H_OUTSIDE;
        }
    }

  bHighlight->setIcon (highlightIcon[options.highlight]);


  redrawMap (NVTrue, NVFalse);
}



//  The following functions are display toggle functions (from toolbar buttons).

void
gridEdit::slotDisplayMinMax ()
{
  if (bDisplayMinMax->isChecked ())
    {
      options.display_minmax = NVTrue;
    }
  else
    {
      options.display_minmax = NVFalse;
    }

  redrawMap (NVTrue, NVFalse);
}



void 
gridEdit::slotLockValueDone (uint8_t accepted)
{
  if (accepted)
    {
      //  We have to reset the colors here because we might have changed the min or max color.

      int32_t k = misc.color_by_attribute;

      float hue_inc = (float) (options.max_hsv_color[k] - options.min_hsv_color[k]) / (float) (NUMHUES + 1);
      for (int32_t m = 0 ; m < 2 ; m++)
        {
          int32_t sat = 255;
          if (m) sat = 25;

          for (int32_t i = 0 ; i < NUMHUES ; i++)
            {
              for (int32_t j = 0 ; j < NUMSHADES ; j++)
                {
                  options.color_array[m][i][j].setHsv ((int32_t) (((NUMHUES + 1) - (i - options.min_hsv_color[k])) * hue_inc), sat, j, 255);
                }
            }
        }


      redrawMap (NVTrue, NVTrue);
      slotRedrawCoverage ();
    }
}



void 
gridEdit::slotMinScalePressed (QMouseEvent *e __attribute__ ((unused)))
{
  if (misc.drawing) return;

  lockValue *lck = new lockValue (map, &options, NVTrue);
  connect (lck, SIGNAL (lockValueDone (uint8_t)), this, SLOT (slotLockValueDone (uint8_t)));

  lck->move (x () + width () / 2, y () + height () / 2);
}



void 
gridEdit::slotMaxScalePressed (QMouseEvent *e __attribute__ ((unused)))
{
  if (misc.drawing) return;

  lockValue *lck = new lockValue (map, &options, NVFalse);
  connect (lck, SIGNAL (lockValueDone (uint8_t)), this, SLOT (slotLockValueDone (uint8_t)));

  lck->move (x () + width () / 2, y () + height () / 2);
}



void 
gridEdit::slotDisplayLayerClicked (int id)
{
  if (!misc.drawing)
    {
      if (layerDisplay[id]->isChecked ())
        {
          misc.chrtr2_share->display_chrtr2[id] = NVTrue;
          layerDisplay[id]->setIcon (QIcon (":/icons/display_layer.png"));
          layerDisplay[id]->setToolTip (tr ("Click to hide layer"));
        }
      else
        {
          misc.chrtr2_share->display_chrtr2[id] = NVFalse;
          layerDisplay[id]->setIcon (QIcon (":/icons/undisplay_layer.png"));
          layerDisplay[id]->setToolTip (tr ("Click to show layer"));
        }

      redrawMap (NVTrue, NVTrue);
      slotRedrawCoverage ();
    }
}



void 
gridEdit::slotLockLayerClicked (int id)
{
  if (!misc.drawing)
    {
      if (layerLock[id]->isChecked ())
        {
          misc.chrtr2_share->lock_chrtr2[id] = NVTrue;
          layerLock[id]->setIcon (QIcon (":/icons/lock.png"));
          layerLock[id]->setToolTip (tr ("Click to unlock layer"));
        }
      else
        {
          misc.chrtr2_share->lock_chrtr2[id] = NVFalse;
          layerLock[id]->setIcon (QIcon (":/icons/unlock.png"));
          layerLock[id]->setToolTip (tr ("Click to lock layer"));
        }
    }
}



//  More toolbar button display toggles.

void
gridEdit::slotContour ()
{
  if (bContour->isChecked ())
    {
      options.contour = NVTrue;
    }
  else
    {
      options.contour = NVFalse;
    }

  redrawMap (NVTrue, NVFalse);
}



void
gridEdit::slotFlagContour ()
{
  if (bFlagContour->isChecked ())
    {
      options.flag_contour = NVTrue;
      overlayFlag (map, &options, &misc);
    }
  else
    {
      options.flag_contour = NVFalse;
      redrawMap (NVTrue, NVFalse);
    }
}



void
gridEdit::slotInterpolated ()
{
  if (bInterpolated->isChecked ())
    {
      options.display_interpolated = NVTrue;
    }
  else
    {
      options.display_interpolated = NVFalse;
    }

  redrawMap (NVTrue, NVFalse);
}



void
gridEdit::slotGeotiff ()
{
  if (!misc.display_GeoTIFF)
    {
      misc.display_GeoTIFF = 1;
      bGeotiff->setIcon (QIcon (":/icons/geo.png"));
    }
  else if (misc.display_GeoTIFF > 0)
    {
      misc.display_GeoTIFF = -1;
      bGeotiff->setIcon (QIcon (":/icons/geo_under.png"));
    }
  else
    {
      misc.display_GeoTIFF = 0;
      bGeotiff->setIcon (QIcon (":/icons/geo_off.png"));
    }
  qApp->processEvents ();

  misc.clear = NVTrue;
  redrawMap (NVTrue, NVFalse);
}



//  Stop drawing

void
gridEdit::slotStop ()
{
  misc.clear = NVTrue;
  misc.drawing_canceled = NVTrue;


  if (grid_edit_active) 
    {
      gridPointEdit_stopped = NVTrue;

      editProc->kill ();
      setMainButtons (NVTrue);
      grid_edit_active = NVFalse;


      //  Make sure we reset the important things in case we did a partial redraw.

      setFunctionCursor (misc.function);
      map->discardRubberbandRectangle (&rb_rectangle);
      map->discardRubberbandPolygon (&rb_polygon);
    }

  setMainButtons (NVTrue);
  misc.drawing = NVFalse;
}



void 
gridEdit::slotAttrMenu (QAction *action)
{
  for (int32_t k = 0 ; k < NUM_HSV ; k++)
    {
      if (action == attr[k])
        {
          misc.color_by_attribute = k;


          //  We have to reset the colors here in case the "color by" attribute has a different color range than the last 
          //  option.

          float hue_inc = (float) (options.max_hsv_color[k] - options.min_hsv_color[k]) / (float) (NUMHUES + 1);
          for (int32_t m = 0 ; m < 2 ; m++)
            {
              int32_t sat = 255;
              if (m) sat = 25;

              for (int32_t i = 0 ; i < NUMHUES ; i++)
                {
                  for (int32_t j = 0 ; j < NUMSHADES ; j++)
                    {
                      options.color_array[m][i][j].setHsv ((int32_t) (((NUMHUES + 1) - (i - options.min_hsv_color[k])) * hue_inc), sat, j, 255);
                    }
                }
            }


          //  If we're coloring by attribute, invert the tooltips and help.

          if (k)
            {
              misc.scale[0]->setToolTip (tr ("Click here to lock or unlock the maximum value and/or change the maximum color"));
              misc.scale[0]->setWhatsThis (tr ("Click here to lock or unlock the maximum value and/or change the maximum color"));
              misc.scale[NUM_SCALE_LEVELS - 1]->setToolTip (tr ("Click here to lock or unlock the minimum value and/or change the minimum color"));
              misc.scale[NUM_SCALE_LEVELS - 1]->setWhatsThis (tr ("Click here to lock or unlock the minimum value and/or change the minimum color"));
            }
          else
            {
              misc.scale[0]->setToolTip (tr ("Click here to lock or unlock the minimum value and/or change the minimum color"));
              misc.scale[0]->setWhatsThis (tr ("Click here to lock or unlock the minimum value and/or change the minimum color"));
              misc.scale[NUM_SCALE_LEVELS - 1]->setToolTip (tr ("Click here to lock or unlock the maximum value and/or change the maximum color"));
              misc.scale[NUM_SCALE_LEVELS - 1]->setWhatsThis (tr ("Click here to lock or unlock the maximum value and/or change the maximum color"));
            }
          break;
        }
    }

  bAttr->setIcon (attrIcon[misc.color_by_attribute]);

  redrawMap (NVTrue, NVFalse);
}



void
gridEdit::slotZoomOut ()
{
  mapdef = map->getMapdef ();
  if (mapdef.zoom_level)
    {
      misc.clear = NVTrue;
      misc.GeoTIFF_init = NVTrue;

      zoomOut ();

      redrawCovBounds ();

      mapdef = map->getMapdef ();
      if (mapdef.zoom_level == 0) bZoomOut->setEnabled (false);

      misc.cov_function = COV_START_DRAW_RECTANGLE;
    }
}



void
gridEdit::slotZoomIn ()
{
  map->setToolTip (tr ("Left click to start rectangle, middle click to abort, right click for menu"));
  misc.function = ZOOM_IN_AREA;
  setFunctionCursor (misc.function);
}



void
gridEdit::slotClearFilterMasks ()
{
  clearFilterMasks ();
  redrawMap (NVTrue, NVTrue);
}



//  Clear all hand drawn contours.

void
gridEdit::slotClearContours ()
{
  if (misc.hd_contour_count)
    {
      for (int32_t i = 0 ; i < misc.hd_contour_count ; i++)
        {
          misc.hd_contour[i].count = 0;
          misc.hd_contour[i].xyz.clear ();
        }

      misc.hd_contour_count = 0;

      misc.hd_contour.clear ();


      redrawMap (NVTrue, NVTrue);
      cov->redrawMap (NVTrue);
      redrawCovBounds ();
    }
}



//  Undo the last edit operation.

void
gridEdit::slotUndo ()
{
  undo (&misc);

  redrawMap (NVTrue, NVTrue);
  cov->redrawMap (NVTrue);
  redrawCovBounds ();
}



//  This determines the active function.  The id is set by the editMode button group.

void
gridEdit::slotEditMode (int id)
{
  QString msc;


  switch (id)
    {
    case RECT_EDIT_AREA_3D:
    case POLY_EDIT_AREA_3D:
    case RECT_DELETE_AREA:
    case RECT_FILL_AREA:
    case POLY_DELETE_AREA:
    case POLY_DELETE_CONTOUR:
    case POLY_FILL_AREA:
    case POLY_FILL_PFM:
    case RECT_FILTER_AREA:
    case RECT_FILTER_MASK:
    case SELECT_HIGH_POINT:
    case SELECT_LOW_POINT:
    case POLY_FILTER_AREA:
    case POLY_FILTER_MASK:
    case REMISP_AREA:
    case DRAW_CONTOUR:
    case GRAB_CONTOUR:
    case CLEAR_CONTOUR:
      misc.function = misc.last_edit_function = id;
      misc.save_function = misc.function;
      break;
    }
  setFunctionCursor (misc.function);
}



//  Setting the displayed area or the entire file to checked or unchecked.  The displayed area
//  set comes from the toolbar button.  The entire file set comes from the "Edit" pulldown menu.

void 
gridEdit::slotCheckMenu (QAction *action)
{
  void setAreaChecked (MISC *misc, int32_t type, uint8_t area);


  setMainButtons (NVFalse);


  int32_t type = 0;


  //  Determine what action to take and whether we need to perform it for an area or the entire file.

  uint8_t area = NVTrue;
  if (action == setChecked || action == fileChecked) type = 0;
  if (action == setUnchecked || action == fileUnchecked) type = 1;

  if (action == fileChecked || action == fileUnchecked) area = NVFalse;


  setMainButtons (NVFalse);

  qApp->setOverrideCursor (Qt::WaitCursor);
  qApp->processEvents ();


  setAreaChecked (&misc, type, area);


  qApp->restoreOverrideCursor ();
  qApp->processEvents ();


  redrawMap (NVTrue, NVFalse);

  cov->redrawMap (NVFalse);
  redrawCovBounds ();
}



void 
gridEdit::slotCoastMenu (QAction *action)
{
  if (action == noCoast)
    {
      options.coast = NVMAP_NO_COAST;
    }
  else if (action == ngaCoast)
    {
      options.coast = NVMAP_NGA_50K_COAST;
    }
  else if (action == wvsfCoast)
    {
      options.coast = NVMAP_WVS_FULL_COAST;
    }
  else
    {
      options.coast = NVMAP_AUTO_COAST;
    }

  map->setCoasts (options.coast);
  redrawMap (NVTrue, NVFalse);
}



//  Move the selected layer to the top of the CHRTR2 layers.
  
void 
gridEdit::slotLayerGrpTriggered (QAction *action)
{
  int32_t hit = -1;


  //  Find out which layer we selected

  for (int32_t chrtr2 = 0 ; chrtr2 < misc.chrtr2_share->chrtr2_count ; chrtr2++)
    {
      if (action == layer[chrtr2])
        {
          hit = chrtr2;
          break;
        }
    }


  //  If we selected a layer other than 0, move the layer to the top.

  if (hit > 0)
    {
      move_layer_to_top (&misc, hit);

      slotManageLayersDataChanged ();
    }
}



//  Kick off the layer management dialog.

void 
gridEdit::slotLayers ()
{
  manageLayers_dialog = new manageLayers (this, &options, &misc);


  //  Put the dialog in the middle of the screen.

  manageLayers_dialog->move (x () + width () / 2 - manageLayers_dialog->width () / 2, y () + height () / 2 - manageLayers_dialog->height () / 2);

  connect (manageLayers_dialog, SIGNAL (dataChangedSignal ()), this, SLOT (slotManageLayersDataChanged ()));
}



//  We've changed something about the layers (multiple CHRTR2s) in the manage layer dialog so we have to do something.

void 
gridEdit::slotManageLayersDataChanged ()
{
  if (!misc.drawing)
    {
      //  Set up the layers stuff

      int32_t num_displayed = 0;
      notebook->setTabEnabled (1, false);

      for (int32_t chrtr2 = 0 ; chrtr2 < MAX_CHRTR2_FILES ; chrtr2++)
        {
          if (chrtr2 < misc.chrtr2_share->chrtr2_count)
            {
              QString layerString;
              layerString.sprintf ("%d - ", chrtr2 + 1);
              layerString += QFileInfo (QString (misc.chrtr2_share->chrtr2_file[chrtr2])).fileName ();
              layer[chrtr2]->setText (layerString);
              layer[chrtr2]->setVisible (true);


              //  This is for the CHRTR2 layers notebook page.

              QString name = QFileInfo (QString (misc.chrtr2_share->chrtr2_file[chrtr2])).fileName ().remove (".ch2");

              layerName[chrtr2]->setText (name);

              if (chrtr2) notebook->setTabEnabled (1, true);

              if (misc.chrtr2_share->display_chrtr2[chrtr2])
                {
                  num_displayed++;
                  layerDisplay[chrtr2]->setDown (true);
                }
              else
                {
                  layerDisplay[chrtr2]->setDown (false);
                }


              //  Check to see if we want to lock all non-zero layers

              if (options.non_top_layers_locked)
                {
                  if (chrtr2)
                    {
                      misc.chrtr2_share->lock_chrtr2[chrtr2] = NVTrue;
                    }
                  else
                    {
                      misc.chrtr2_share->lock_chrtr2[chrtr2] = NVFalse;
                    }
                }

              if (misc.chrtr2_share->lock_chrtr2[chrtr2])
                {
                  layerLock[chrtr2]->setDown (true);
                }
              else
                {
                  layerLock[chrtr2]->setDown (false);
                }
              layerName[chrtr2]->show ();
              layerDisplay[chrtr2]->show ();
              layerLock[chrtr2]->show ();
            }
          else
            {
              layer[chrtr2]->setVisible (false);

              layerName[chrtr2]->hide ();
              layerDisplay[chrtr2]->hide ();
              layerLock[chrtr2]->hide ();
            }
        }
    }


  if (!misc.chrtr2_share->chrtr2_count)
    {
      discardMovableObjects ();

      cov_area_defined = NVFalse;

      misc.clear = NVTrue;
      misc.cov_clear = NVTrue;

      clearFilterMasks ();

      map->setZoomLevel (0);

      slotRedrawCoverage ();

      logo_first = NVTrue;
      slotResize (NULL);
    }
  else
    {
      misc.cov_clear = NVTrue;
      misc.clear = NVTrue;


      //  Compute the MBR of all of the CHRTR2 areas.

      compute_total_mbr (&misc);


      map->disableSignals ();

      clearFilterMasks ();

      map->resetBounds (misc.total_mbr);

      map->enableSignals ();


      covdef.initial_bounds.min_x = misc.total_mbr.min_x;
      covdef.initial_bounds.min_y = misc.total_mbr.min_y;
      covdef.initial_bounds.max_x = misc.total_mbr.max_x;
      covdef.initial_bounds.max_y = misc.total_mbr.max_y;

      cov->resetBounds (covdef.initial_bounds);

      slotRedrawCoverage ();


      if (cov_area_defined) zoomIn (misc.total_displayed_area, NVTrue);
    }
}



void
gridEdit::slotToolbarHelp ()
{
  QWhatsThis::showText (QCursor::pos ( ), toolbarText, map);
}


void
gridEdit::about ()
{
  QMessageBox msgBox;
  msgBox.setText (QString ("<center>") + VERSION + QString ("</center>"));
  msgBox.setInformativeText (gridEditAboutText);
  msgBox.setStandardButtons (QMessageBox::Ok);
  msgBox.setDefaultButton (QMessageBox::Ok);
  msgBox.setIconPixmap (QPixmap (":/icons/gridEdit.jpg").scaled (128, 128, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
  msgBox.exec ();
}



void
gridEdit::slotAcknowledgments ()
{
  QMessageBox::about (this, VERSION, acknowledgmentsText);
}



void
gridEdit::aboutQt ()
{
  QMessageBox::aboutQt (this, VERSION);
}



// ************************************************************************************************
//
//    Coverage map functions.
//
//    These work the same way as the main map functions (after all, they use the same type of
//    map widget).  The coverage map just has fewer possible actions.
//
// ************************************************************************************************

void 
gridEdit::redrawCovBounds ()
{
  if (cov_area_defined)
    {
      mapdef = map->getMapdef ();

      if (cov->rubberbandRectangleIsActive (cov_rb_rectangle)) cov->discardRubberbandRectangle (&cov_rb_rectangle);

      cov->drawRectangle (mapdef.bounds[mapdef.zoom_level].min_x,
                          mapdef.bounds[mapdef.zoom_level].min_y, mapdef.bounds[mapdef.zoom_level].max_x,
                          mapdef.bounds[mapdef.zoom_level].max_y, Qt::red,
                          LINE_WIDTH, Qt::SolidLine, NVTrue);
    }
}



void 
gridEdit::covLeftMouse (int32_t mouse_x __attribute__ ((unused)), int32_t mouse_y __attribute__ ((unused)), double lon, double lat)
{
  //  If the popup menu is up discard this mouse press

  if (cov_popup_active)
    {
      cov_popup_active = NVFalse;
      return;
    }


  int32_t *px, *py;
  double *mx, *my;
  NV_F64_XYMBR bounds;


  switch (misc.cov_function)
    {
    case COV_START_DRAW_RECTANGLE:
      cov->discardRubberbandRectangle (&cov_rb_rectangle);

      misc.cov_function = COV_DRAW_RECTANGLE;
      cov->anchorRubberbandRectangle (&cov_rb_rectangle, lon, lat, Qt::red, LINE_WIDTH, Qt::SolidLine);
      break;

    case COV_DRAW_RECTANGLE:

      //  If we have a rubberband rectangle active (moving) we close it and do the zoom in the main map.

      if (cov->rubberbandRectangleIsActive (cov_rb_rectangle))
        {
          cov->closeRubberbandRectangle (cov_rb_rectangle, lon, lat, &px, &py, &mx, &my);


          bounds.min_x = 999999.0;
          bounds.min_y = 999999.0;
          bounds.max_x = -999999.0;
          bounds.max_y = -999999.0;

          for (int32_t i = 0 ; i < 4 ; i++)
            {
              if (misc.dateline && mx[i] < 0.0) mx[i] += 360.0;

              if (mx[i] < bounds.min_x) bounds.min_x = mx[i];
              if (my[i] < bounds.min_y) bounds.min_y = my[i];
              if (mx[i] > bounds.max_x) bounds.max_x = mx[i];
              if (my[i] > bounds.max_y) bounds.max_y = my[i];
            }


          //  The first time in we want to enable all of the main buttons after an area
          //  has been defined.

          if (!cov_area_defined) setMainButtons (NVTrue);

          cov_area_defined = NVTrue;

          misc.cov_function = COV_START_DRAW_RECTANGLE;

          zoomIn (bounds, NVTrue);
        }
      break;


    case COV_START_DRAG_RECTANGLE:

      cov_drag_bounds = mapdef.bounds[mapdef.zoom_level];

      cov->setMovingRectangle (&cov_mv_rectangle, cov_drag_bounds.min_x, cov_drag_bounds.min_y, cov_drag_bounds.max_x, cov_drag_bounds.max_y,
                               Qt::red, LINE_WIDTH, NVFalse, Qt::SolidLine);

      cov_start_drag_lat = lat;
      cov_start_drag_lon = lon;

      cov->setCursor (Qt::ClosedHandCursor);
      misc.cov_function = COV_DRAG_RECTANGLE;
      break;


    case COV_DRAG_RECTANGLE:

      cov->getMovingRectangle (cov_mv_rectangle, &px, &py, &mx, &my);

      cov_drag_bounds.min_x = 999999.0;
      cov_drag_bounds.min_y = 999999.0;
      cov_drag_bounds.max_x = -999999.0;
      cov_drag_bounds.max_y = -999999.0;

      for (int32_t i = 0 ; i < 4 ; i++)
        {
          if (misc.dateline && mx[i] < 0.0) mx[i] += 360.0;

          if (mx[i] < cov_drag_bounds.min_x) cov_drag_bounds.min_x = mx[i];
          if (my[i] < cov_drag_bounds.min_y) cov_drag_bounds.min_y = my[i];
          if (mx[i] > cov_drag_bounds.max_x) cov_drag_bounds.max_x = mx[i];
          if (my[i] > cov_drag_bounds.max_y) cov_drag_bounds.max_y = my[i];
        }

      cov->closeMovingRectangle (&cov_mv_rectangle);

      cov->setCursor (editRectCursor);
      misc.cov_function = COV_START_DRAW_RECTANGLE;
      zoomIn (cov_drag_bounds, NVTrue);
      break;
    }
}



void 
gridEdit::covMidMouse (int32_t mouse_x __attribute__ ((unused)), int32_t mouse_y __attribute__ ((unused)),
                       double lon __attribute__ ((unused)), double lat __attribute__ ((unused)))
{
  misc.cov_function = COV_START_DRAW_RECTANGLE;
  cov->setCursor (editRectCursor);

  cov->discardRubberbandRectangle (&cov_rb_rectangle);
  cov->closeMovingRectangle (&cov_mv_rectangle);

  redrawCovBounds ();
}



void 
gridEdit::covRightMouse (int32_t mouse_x, int32_t mouse_y, double lon, double lat)
{
  cov_menu_cursor_lon = lon;
  cov_menu_cursor_lat = lat;
  cov_menu_cursor_x = mouse_x;
  cov_menu_cursor_y = mouse_y;


  cov_popup_active = NVTrue;

  covPopupMenu->popup (QCursor::pos ( ));
}



void 
gridEdit::slotCovPopupMenu0 ()
{
  cov_popup_active = NVFalse;

  covLeftMouse (cov_menu_cursor_x, cov_menu_cursor_y, cov_menu_cursor_lon, cov_menu_cursor_lat);
}



void 
gridEdit::slotCovPopupMenu1 ()
{
  cov_popup_active = NVFalse;

  covMidMouse (cov_menu_cursor_x, cov_menu_cursor_y, cov_menu_cursor_lon, cov_menu_cursor_lat);
}



void 
gridEdit::slotCovPopupMenu2 ()
{
  cov_popup_active = NVFalse;

  if (cov_area_defined)
    {
      cov->setCursor (Qt::OpenHandCursor);

      misc.cov_function = COV_START_DRAG_RECTANGLE;
    }
}



void 
gridEdit::slotCovPopupHelp ()
{
  QWhatsThis::showText (QCursor::pos ( ), covMenuText, cov);
  covMidMouse (cov_menu_cursor_x, cov_menu_cursor_y, cov_menu_cursor_lon, cov_menu_cursor_lat);
}



void 
gridEdit::slotCovMousePress (QMouseEvent *e, double lon, double lat)
{
  if (!misc.drawing && !grid_edit_active)
    {
      if (e->button () == Qt::LeftButton)
        {
          //  Check for the control key modifier.  If it's set, we want to drag the rectangle.

          if (e->modifiers () == Qt::ControlModifier) misc.cov_function = COV_START_DRAG_RECTANGLE;

          covLeftMouse (e->x (), e->y (), lon, lat);
        }
      if (e->button () == Qt::MidButton) covMidMouse (e->x (), e->y (), lon, lat);
      if (e->button () == Qt::RightButton) covRightMouse (e->x (), e->y (), lon, lat);
    }
}



//  This isn't used yet but maybe later

void 
gridEdit::slotCovMouseRelease (QMouseEvent *e, double lon __attribute__ ((unused)), double lat __attribute__ ((unused)))
{
  if (e->button () == Qt::LeftButton) {};
  if (e->button () == Qt::MidButton) {};
  if (e->button () == Qt::RightButton) {};
}



void
gridEdit::slotCovMouseMove (QMouseEvent *e __attribute__ ((unused)), double lon, double lat)
{
  char              ltstring[25], lnstring[25], hem;
  double            deg, min, sec;
  NV_F64_COORD2     xy;
  QString           y_string, x_string;
  CHRTR2_RECORD     chrtr2_record;
  

  //  Let linked programs know which window we're in.  We're lying about the active window ID (process ID) by subtracting 1
  //  from this program's PID.  This makes trackCursor track it locally.

  if (misc.linked)
    {
      misc.abe_register->data[abe_register_group].active_window_id = misc.process_id - 1;
      misc.abe_register->data[abe_register_group].cursor_position.y = lat;
      misc.abe_register->data[abe_register_group].cursor_position.x = lon;
    }


  //  Try to find the highest layer with a valid value to display in the status bar.

  xy.y = lat;
  xy.x = lon;
  int32_t hit = -1;

  for (int32_t chrtr2 = 0 ; chrtr2 < misc.chrtr2_share->chrtr2_count ; chrtr2++)
    {
      if (misc.chrtr2_share->display_chrtr2[chrtr2] && inside_mbr_coord2 (&misc.chrtr2_share->chrtr2_header[chrtr2].mbr, xy))
        {
          chrtr2_read_record_lat_lon (misc.chrtr2_handle[chrtr2], lat, lon, &chrtr2_record);

          hit = chrtr2;
          break;
        }
    }


  //  We only change the status CHRTR2 values if we were actually over a CHRTR2 data area

  if (hit >= 0) chrtr2Label->setText (QFileInfo (QString (misc.chrtr2_share->chrtr2_file[hit])).fileName ().remove (".ch2"));


  strcpy (ltstring, fixpos (lat, &deg, &min, &sec, &hem, POS_LAT, options.position_form));
  strcpy (lnstring, fixpos (lon, &deg, &min, &sec, &hem, POS_LON, options.position_form));

  latLabel->setText (ltstring);
  lonLabel->setText (lnstring);


  NV_I32_COORD2 coord;
  chrtr2_get_coord (misc.chrtr2_handle[0], lat, lon, &coord);

  coordXLabel->setNum (coord.x);
  coordYLabel->setNum (coord.y);


  double diff_lat, diff_lon;

  switch (misc.cov_function)
    {
    case COV_DRAW_RECTANGLE:
      cov->dragRubberbandRectangle (cov_rb_rectangle, lon, lat);
      break;

    case COV_DRAG_RECTANGLE:
      diff_lat = lat - cov_start_drag_lat;
      diff_lon = lon - cov_start_drag_lon;

      for (int32_t i = 0 ; i < 4 ; i++)
        {
          cov_drag_bounds.min_x = mapdef.bounds[mapdef.zoom_level].min_x + diff_lon;
          cov_drag_bounds.max_x = mapdef.bounds[mapdef.zoom_level].max_x + diff_lon;
          cov_drag_bounds.min_y = mapdef.bounds[mapdef.zoom_level].min_y + diff_lat;
          cov_drag_bounds.max_y = mapdef.bounds[mapdef.zoom_level].max_y + diff_lat;
        }

      if (cov_drag_bounds.min_x < mapdef.bounds[0].min_x)
        {
          cov_drag_bounds.min_x = mapdef.bounds[0].min_x;
          cov_drag_bounds.max_x = mapdef.bounds[0].min_x + (mapdef.bounds[mapdef.zoom_level].max_x - mapdef.bounds[mapdef.zoom_level].min_x);
        }

      if (cov_drag_bounds.max_x > mapdef.bounds[0].max_x)
        {
          cov_drag_bounds.max_x = mapdef.bounds[0].max_x;
          cov_drag_bounds.min_x = mapdef.bounds[0].max_x - (mapdef.bounds[mapdef.zoom_level].max_x - mapdef.bounds[mapdef.zoom_level].min_x);
        }

      if (cov_drag_bounds.min_y < mapdef.bounds[0].min_y)
        {
          cov_drag_bounds.min_y = mapdef.bounds[0].min_y;
          cov_drag_bounds.max_y = mapdef.bounds[0].min_y + (mapdef.bounds[mapdef.zoom_level].max_y - mapdef.bounds[mapdef.zoom_level].min_y);
        }

      if (cov_drag_bounds.max_y > mapdef.bounds[0].max_y)
        {
          cov_drag_bounds.max_y = mapdef.bounds[0].max_y;
          cov_drag_bounds.min_y = mapdef.bounds[0].max_y - (mapdef.bounds[mapdef.zoom_level].max_y - mapdef.bounds[mapdef.zoom_level].min_y);
        }

      cov->setMovingRectangle (&cov_mv_rectangle, cov_drag_bounds.min_x, cov_drag_bounds.min_y, cov_drag_bounds.max_x, cov_drag_bounds.max_y,
                               Qt::red, LINE_WIDTH, NVFalse, Qt::SolidLine);

      break;
    }
}



//  We don't do anything pre-coastline draw in the coverage map

void 
gridEdit::slotCovPreRedraw (NVMAP_DEF map_covdef __attribute__ ((unused)))
{
}


void 
gridEdit::slotCovPostRedraw (NVMAP_DEF covdef)
{
  double z;
  int32_t pix_z, cov_area_x, cov_area_y, cov_area_width, cov_area_height, cov_area_bin_x, cov_area_bin_y;


  qApp->setOverrideCursor (Qt::WaitCursor);
  qApp->processEvents ();


  //  Get the actual map area corner pixels.

  cov->map_to_screen (1, &covdef.bounds[0].min_x, &covdef.bounds[0].max_y, &z, &cov_area_x, &cov_area_y, &pix_z);
  cov->map_to_screen (1, &covdef.bounds[0].max_x, &covdef.bounds[0].min_y, &z, &cov_area_width, &cov_area_height, &pix_z);

  cov_area_width = cov_area_width - cov_area_x;
  cov_area_height = cov_area_height - cov_area_x;


  uint8_t *coverage = NULL;
  QColor c1, prev_c1;
  int32_t cov_start_row, cov_start_col, cov_start_height, cov_start_width;


  //  Remove the box outline prior to drawing.

  cov->discardRubberbandRectangle (&cov_rb_rectangle);


  if (misc.cov_clear)
    {
      cov->fillRectangle (misc.total_mbr.min_x, misc.total_mbr.min_y, misc.total_mbr.max_x, misc.total_mbr.max_y, options.cov_background_color, NVTrue);
    }
  else
    {
      cov->fillRectangle (misc.total_displayed_area.min_x, misc.total_displayed_area.min_y, misc.total_displayed_area.max_x, 
                          misc.total_displayed_area.max_y, options.cov_background_color, NVTrue);
    }


  //  Loop through the open (and displayed) CHRTR2s and paint the coverage for each.

  for (int32_t chrtr2 = misc.chrtr2_share->chrtr2_count - 1 ; chrtr2 >= 0 ; chrtr2--)
    {
      //  Only draw coverage if we're displaying the layer.

      if (misc.chrtr2_share->display_chrtr2[chrtr2])
        {
          if (misc.cov_clear)
            {
              cov_start_row = 0;
              cov_start_col = 0;
              cov_start_height = misc.chrtr2_share->chrtr2_header[chrtr2].height;
              cov_start_width = misc.chrtr2_share->chrtr2_header[chrtr2].width;
            }
          else
            {
              int32_t displayed_area_width = (NINT ((misc.total_displayed_area.max_x - misc.total_displayed_area.min_x) / 
                                                     misc.chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees));
              int32_t displayed_area_height = (NINT ((misc.total_displayed_area.max_y - misc.total_displayed_area.min_y) / 
                                                      misc.chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees));
              int32_t displayed_area_row = NINT ((misc.total_displayed_area.min_y - misc.chrtr2_share->chrtr2_header[chrtr2].mbr.slat) / 
                                                  misc.chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees);
              int32_t displayed_area_column = NINT ((misc.total_displayed_area.min_x - misc.chrtr2_share->chrtr2_header[chrtr2].mbr.wlon) / 
                                                     misc.chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees);

              if (displayed_area_column + displayed_area_width > misc.chrtr2_share->chrtr2_header[chrtr2].width - 1)
                displayed_area_width = misc.chrtr2_share->chrtr2_header[chrtr2].width - displayed_area_column - 1;

              if (displayed_area_row + displayed_area_height > misc.chrtr2_share->chrtr2_header[chrtr2].height - 1)
                displayed_area_height = misc.chrtr2_share->chrtr2_header[chrtr2].height - displayed_area_row - 1;


              cov_start_row = displayed_area_row;
              cov_start_col = displayed_area_column;
              cov_start_height = displayed_area_height;
              cov_start_width = displayed_area_width;
            }


          //  Determine how many bins cover a single pixel???

          cov_area_bin_x = misc.chrtr2_share->chrtr2_header[chrtr2].width / cov_area_width;
          if (misc.chrtr2_share->chrtr2_header[chrtr2].width % cov_area_width) cov_area_bin_x++;

          cov_area_bin_y = misc.chrtr2_share->chrtr2_header[chrtr2].height / cov_area_height;
          if (misc.chrtr2_share->chrtr2_header[chrtr2].height % cov_area_height) cov_area_bin_y++;


          int32_t checked_sum, cov_col, cov_width;
          uint16_t status;
          NV_I32_COORD2 coord;
          uint8_t has_real, has_interpolated, has_digitized;

          int32_t cov_row = cov_start_row / cov_area_bin_y;

          int32_t cov_height = cov_start_height / cov_area_bin_y;

          int32_t size = (cov_area_bin_y + 1) * ((cov_start_width) + cov_area_bin_x + 1);


          //  If size is less than or equal to 0 we have asked for an area outside of the CHRTR2's MBR so we don't want to do anything.

          if (size > 0)
            {
              if ((coverage = (uint8_t *) calloc (size, sizeof (uint8_t))) == NULL)
                {
                  fprintf (stderr, "Unable to allocate coverage map memory.\n");
                  fprintf (stderr, "Requesting %d bytes.\n", size);
                  exit (-1);
                }


              for (int32_t i = cov_row ; i <= cov_row + cov_height ; i++)
                {
                  //  Drawing one row at a time.

                  double start_lat = misc.chrtr2_share->chrtr2_header[chrtr2].mbr.slat + (i * cov_area_bin_y) *
                    misc.chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees;
                  double lat = misc.chrtr2_share->chrtr2_header[chrtr2].mbr.slat + ((i + 1) * cov_area_bin_y) *
                    misc.chrtr2_share->chrtr2_header[chrtr2].lat_grid_size_degrees;


                  memset (coverage, 0, size);

                  for (int32_t k = 0 ; k <= cov_area_bin_y ; k++)
                    {
                      coord.y = (i * cov_area_bin_y) + k;

                      for (int32_t m = 0 ; m <= cov_start_width ; m++)
                        {
                          coord.x = m + cov_start_col;


                          if (!chrtr2_read_cov_map (misc.chrtr2_handle[chrtr2], coord, &status))
                            {
                              coverage[k * cov_start_width + m] = status;
                            }
                        }
                    }


                  cov_col = cov_start_col / cov_area_bin_x;
                  cov_width = cov_start_width / cov_area_bin_x + cov_col;


                  //  We're only going to paint once the color has changed.  That is, we're making a long skinny rectangle
                  //  until the color needs to change and then we're painting the rectangle.

                  prev_c1.setRgb (255, 255, 255);
                  prev_c1.setAlpha (0);


                  double start_lon = misc.chrtr2_share->chrtr2_header[chrtr2].mbr.wlon + ((cov_col - 1) * cov_area_bin_x) * 
                    misc.chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees;
                  double lon = 0.0;

                  for (int32_t j = cov_col ; j < cov_width ; j++)
                    {
                      lon = misc.chrtr2_share->chrtr2_header[chrtr2].mbr.wlon + (j * cov_area_bin_x) *
                        misc.chrtr2_share->chrtr2_header[chrtr2].lon_grid_size_degrees;

                      has_real = has_interpolated = has_digitized = NVFalse;
                      checked_sum = 0;

                      for (int32_t k = 0 ; k < cov_area_bin_y ; k++)
                        {
                          for (int32_t m = 0 ; m < cov_area_bin_x ; m++)
                            {
                              if (coverage[k * cov_start_width + (j - cov_col) * cov_area_bin_x + m] & CHRTR2_CHECKED) checked_sum++;

                              if (coverage[k * cov_start_width + (j - cov_col) * cov_area_bin_x + m] & CHRTR2_REAL) 
                                {
                                  has_real = NVTrue;
                                }
                              else if (coverage[k * cov_start_width + (j - cov_col) * cov_area_bin_x + m] & CHRTR2_DIGITIZED_CONTOUR)
                                {
                                  has_digitized = NVTrue;
                                }
                              else if (coverage[k * cov_start_width + (j - cov_col) * cov_area_bin_x + m] & CHRTR2_INTERPOLATED)
                                {
                                  has_interpolated = NVTrue;
                                }
                            }
                        }

                      if (has_real)
                        {
                          c1 = options.cov_real_color;

                          if (checked_sum == cov_area_bin_x * cov_area_bin_y) c1 = options.cov_checked_r_color;

                          c1.setAlpha (255);
                        }
                      else if (has_digitized)
                        {
                          c1 = options.cov_digitized_color;

                          if (checked_sum == cov_area_bin_x * cov_area_bin_y) c1 = options.cov_checked_d_color;

                          c1.setAlpha (255);
                        }
                      else if (has_interpolated)
                        {
                          c1 = options.cov_interpolated_color;

                          if (checked_sum == cov_area_bin_x * cov_area_bin_y) c1 = options.cov_checked_i_color;

                          c1.setAlpha (255);
                        }
                      else
                        {
                          c1.setRgb (255, 255, 255);


                          //  If it's gonna be white we want it to be clear so it doesn't write over any 
                          //  underlaying CHRTR2 coverage layers.

                          c1.setAlpha (0);
                        }


                      if (c1 != prev_c1 && j != cov_col)
                        {
                          cov->fillRectangle (start_lon, start_lat, lon, lat, prev_c1, NVTrue);

                          start_lon = lon;

                          prev_c1 = c1;
                        }
                    }

                  cov->fillRectangle (start_lon, start_lat, lon, lat, prev_c1, NVTrue);
                }

              free (coverage);
            }
        }
    }


  //  Figure out which is the highest active (displayed) layer.

  int32_t hit = 0;
  for (int32_t chrtr2 = misc.chrtr2_share->chrtr2_count - 1 ; chrtr2 >= 0 ; chrtr2--)
    {
      if (misc.chrtr2_share->display_chrtr2[chrtr2]) hit = chrtr2;
    }


  //  Draw a semi-transparent outline around each CHRTR2.

  for (int32_t chrtr2 = misc.chrtr2_share->chrtr2_count - 1 ; chrtr2 >= 0 ; chrtr2--)
    {
      //  Only draw coverage if we're displaying the layer.

      if (misc.chrtr2_share->display_chrtr2[chrtr2])
        {
          if (chrtr2 == hit)
            {
              c1 = Qt::green;
            }
          else
            {
              c1 = Qt::red;
            }

          c1.setAlpha (128);

          cov->drawRectangle (misc.chrtr2_share->chrtr2_header[chrtr2].mbr.wlon, misc.chrtr2_share->chrtr2_header[chrtr2].mbr.slat,
                              misc.chrtr2_share->chrtr2_header[chrtr2].mbr.elon, misc.chrtr2_share->chrtr2_header[chrtr2].mbr.nlat,
                              c1, LINE_WIDTH, Qt::SolidLine, NVTrue);
        }
    }


  //  Restore the box outline after drawing.

  redrawCovBounds ();


  cov->setCursor (editRectCursor);



  //  Draw an outline around the entire area.

  cov->drawRectangle (covdef.initial_bounds.min_x, covdef.initial_bounds.min_y, covdef.initial_bounds.max_x,
                      covdef.initial_bounds.max_y, Qt::red, LINE_WIDTH, Qt::SolidLine, NVTrue);

  qApp->restoreOverrideCursor ();
}



//  Kick off the overlays dialog.

void 
gridEdit::slotOverlays ()
{
  manageOverlays_dialog = new manageOverlays (this, &options, &misc);


  //  Put the dialog in the middle of the screen.

  manageOverlays_dialog->move (x () + width () / 2 - manageOverlays_dialog->width () / 2, y () + height () / 2 - manageOverlays_dialog->height () / 2);

  connect (manageOverlays_dialog, SIGNAL (dataChangedSignal ()), this, SLOT (slotManageOverlaysDataChanged ()));
}



void 
gridEdit::slotManageOverlaysDataChanged ()
{
  if (!misc.drawing) redrawMap (NVTrue, NVFalse);
}



//  Display the 0 layer CHRTR2 header.

void 
gridEdit::slotDisplayHeader ()
{
  displayHeader_dialog = new displayHeader (this, &misc);


  //  Put the dialog in the middle of the screen.

  displayHeader_dialog->move (x () + width () / 2 - displayHeader_dialog->width () / 2, y () + height () / 2 - displayHeader_dialog->height () / 2);
}



void 
gridEdit::slotOutputDataPoints ()
{
  map->setToolTip (tr ("Left click to define points, double click to define last point, middle click to abort"));
  misc.save_function = misc.function;
  misc.function = OUTPUT_POINTS;
  setFunctionCursor (misc.function);
}



//  Zooms to a specific area based on an input area file.

void 
gridEdit::slotZoomToArea ()
{
  static QString filter = tr ("AREA (*.ARE *.are *.afs)");


  if (!QDir (options.area_dir).exists ()) options.area_dir = options.input_chrtr2_dir;


  QFileDialog fd (this, tr ("gridEdit Zoom to area file")); 
  fd.setViewMode (QFileDialog::List);


  //  Always add the current working directory and the last used directory to the sidebar URLs in case we're running from the command line.
  //  This function is in the nvutility library.

  setSidebarUrls (&fd, options.area_dir);


  QStringList filters;
  filters << tr ("AREA (*.ARE *.are *.afs *.shp)");

  fd.setNameFilters (filters);
  fd.setFileMode (QFileDialog::ExistingFile);
  fd.selectNameFilter (filter);

  QStringList files;
  QString newfile;
  if (fd.exec () == QDialog::Accepted) 
    {
      files = fd.selectedFiles ();
      newfile = files.at (0);


      if (!newfile.isEmpty())
        {
          int32_t polygon_count;
          double polygon_x[200], polygon_y[200];
          char path[512];
          NV_F64_XYMBR mbr;

          strcpy (path, newfile.toLatin1 ());

          if (get_area_mbr (path, &polygon_count, polygon_x, polygon_y, &mbr))
            {
              cov_area_defined = NVTrue;

              zoomIn (mbr, NVTrue);


              misc.cov_function = COV_START_DRAW_RECTANGLE;

              redrawCovBounds ();
            }

          options.area_dir = fd.directory ().absolutePath ();
        }
    }
}



void 
gridEdit::slotDefineRectArea ()
{
  map->setToolTip (tr ("Left click to start rectangle, middle click to abort, right click for menu"));
  misc.save_function = misc.function;
  misc.function = DEFINE_RECT_AREA;
  setFunctionCursor (misc.function);
}



void 
gridEdit::slotDefinePolyArea ()
{
  map->setToolTip (tr ("Left click to define polygon vertices, double click to define last vertex, middle click to abort"));
  misc.save_function = misc.function;
  misc.function = DEFINE_POLY_AREA;
  setFunctionCursor (misc.function);
}



//  Define the cursor based on the active function.  Also adds a background color and function message to the status bar.

void 
gridEdit::setFunctionCursor (int32_t function)
{
  QString msc;


  discardMovableObjects ();

  switch (function)
    {
    case RECT_EDIT_AREA_3D:
      misc.statusProgLabel->setText (tr (" Rectangular edit mode "));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (editRectCursor);
      break;

    case RECT_DELETE_AREA:
      misc.statusProgLabel->setText (tr (" Delete rectangle mode "));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (deleteRectCursor);
      break;

    case RECT_FILL_AREA:
      misc.statusProgLabel->setText (tr (" Fill rectangle mode "));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (fillRectCursor);
      break;

    case SELECT_HIGH_POINT:
      misc.statusProgLabel->setText (tr (" Select high point mode "));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (highCursor);
      break;

    case SELECT_LOW_POINT:
      misc.statusProgLabel->setText (tr (" Select low point mode "));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (lowCursor);
      break;

    case POLY_EDIT_AREA_3D:
      misc.statusProgLabel->setText (tr (" Polygon edit mode "));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (editPolyCursor);
      break;

    case POLY_DELETE_AREA:
      misc.statusProgLabel->setText (tr (" Delete polygon mode "));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (deletePolyCursor);
      break;

    case POLY_DELETE_CONTOUR:
      misc.statusProgLabel->setText (tr (" Delete polygon contours mode "));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (deleteContourPolyCursor);
      break;

    case POLY_FILL_AREA:
      misc.statusProgLabel->setText (tr (" Fill polygon mode "));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (fillPolyCursor);
      break;

    case POLY_FILL_PFM:
      misc.statusProgLabel->setText (tr (" Fill polygon from PFM mode "));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (fillPolyPFMCursor);
      break;

    case RECT_FILTER_AREA:
      misc.statusProgLabel->setText (tr (" Rectangle filter mode "));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (filterRectCursor);
      break;

    case POLY_FILTER_AREA:
      misc.statusProgLabel->setText (tr (" Polygon filter mode "));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (filterPolyCursor);
      break;

    case RECT_FILTER_MASK:
      misc.statusProgLabel->setText (tr (" Rectangle filter mask mode "));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (filterMaskRectCursor);
      break;

    case POLY_FILTER_MASK:
      misc.statusProgLabel->setText (tr (" Polygon filter mask mode "));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (filterMaskPolyCursor);
      break;

    case ZOOM_IN_AREA:
      misc.statusProgLabel->setText (tr (" Zoom in rectangle mode "));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::cyan);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (zoomCursor);
      break;

    case OUTPUT_POINTS:
      misc.statusProgLabel->setText (tr (" Output data points mode "));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::green);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (Qt::CrossCursor);
      break;

    case DEFINE_RECT_AREA:
      misc.statusProgLabel->setText (tr (" Define rectangular area file mode "));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::green);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (Qt::SizeFDiagCursor);
      break;

    case DEFINE_POLY_AREA:
      misc.statusProgLabel->setText (tr (" Define polygonal area file mode "));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::green);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (Qt::ArrowCursor);
      break;

    case DRAW_CONTOUR:
      msc.sprintf (tr (" Draw contour - %.2f ").toLatin1 (), options.draw_contour_level);
      misc.statusProgLabel->setText (msc);
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (drawContourCursor);
      break;

    case GRAB_CONTOUR:
      misc.statusProgLabel->setText (tr (" Capture contour(s) mode "));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (grabContourCursor);
      break;

    case CLEAR_CONTOUR:
      misc.statusProgLabel->setText (tr (" Remove hand drawn contour mode "));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::red);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (clearContourCursor);
      break;

    case REMISP_AREA:
      misc.statusProgLabel->setText (tr (" Rectangular regrid mode "));
      misc.statusProgPalette.setColor (QPalette::Normal, QPalette::Window, Qt::yellow);
      misc.statusProgLabel->setPalette (misc.statusProgPalette);
      map->setCursor (remispCursor);
      break;
    }
}



//  This is the stderr read return from the 3D viewer QProcess.  Hopefully you won't see anything here.

void 
gridEdit::slot3DReadyReadStandardError ()
{
  QByteArray response = threeDProc->readAllStandardError ();
  char *res = response.data ();

  fprintf (stderr, "%s %s %d %s\n", __FILE__,  __FUNCTION__, __LINE__, res);
  fflush (stderr);
}



//  This is the error return from the 3D viewer process.

void 
gridEdit::slot3DError (QProcess::ProcessError error)
{
  switch (error)
    {
    case QProcess::FailedToStart:
      QMessageBox::critical (this, tr ("gridEdit 3D viewer"), tr ("Unable to start the 3D viewer process!"));
      break;

    case QProcess::Crashed:
      QMessageBox::critical (this, tr ("gridEdit 3D viewer"), tr ("The 3D viewer process crashed!"));
      break;

    case QProcess::Timedout:
      QMessageBox::critical (this, tr ("gridEdit 3D viewer"), tr ("The 3D viewer process timed out!"));
      break;

    case QProcess::WriteError:
      QMessageBox::critical (this, tr ("gridEdit 3D viewer"), tr ("There was a write error to the 3D viewer process!"));
      break;

    case QProcess::ReadError:
      QMessageBox::critical (this, tr ("gridEdit 3D viewer"), tr ("There was a read error from the 3D viewer process!"));
      break;

    case QProcess::UnknownError:
      QMessageBox::critical (this, tr ("gridEdit 3D viewer"), tr ("The 3D viewer process died with an unknown error!"));
      break;
    }

  qApp->restoreOverrideCursor ();
}



//  This is the return from the 3D viewer QProcess (when finished normally)

void 
gridEdit::slot3DDone (int exitCode __attribute__ ((unused)), QProcess::ExitStatus exitStatus __attribute__ ((unused)))
{
  if (threeDProc->exitCode ())
    {
      QMessageBox::critical (this, tr ("gridEdit 3D Viewer"), tr ("An error occurred while running the 3D viewer"));
    }
}



void 
gridEdit::slotStart3DViewer ()
{
  if (threeDProc)
    {
      if (threeDProc->state () == QProcess::Running)
        {
          disconnect (threeDProc, SIGNAL (finished (int, QProcess::ExitStatus)), this, SLOT (slot3DDone (int, QProcess::ExitStatus)));
          disconnect (threeDProc, SIGNAL (error (QProcess::ProcessError)), this, SLOT (slot3DError (QProcess::ProcessError)));
          disconnect (threeDProc, SIGNAL (readyReadStandardError ()), this, SLOT (slot3DReadyReadStandardError ()));

          threeDProc->kill ();
        }
      delete threeDProc;
    }


  program = options.threeD_prog.section (' ', 0, 0);

  threeDProc = new QProcess (this);

  QStringList arguments;
  arguments.clear ();

  arguments += QString ("--shared_memory_key=%1").arg (misc.chrtr2_share->ppid);


  QString command;

  command = options.threeD_prog;


  connect (threeDProc, SIGNAL (finished (int, QProcess::ExitStatus)), this, SLOT (slot3DDone (int, QProcess::ExitStatus)));
  connect (threeDProc, SIGNAL (error (QProcess::ProcessError)), this, SLOT (slot3DError (QProcess::ProcessError)));
  connect (threeDProc, SIGNAL (readyReadStandardError ()), this, SLOT (slot3DReadyReadStandardError ()));


  threeDProc->start (command, arguments);
}



//  Export the displayed view to a graphics file (jpg, GeoTIFF, etc.).

void 
gridEdit::slotExportImage ()
{
  void export_image (QWidget *parent, MISC *misc, OPTIONS *options, nvMap *map);


  QStringList lst;
  bool ok;

  int32_t w = misc.displayed_area_width[0];
  int32_t h = misc.displayed_area_height[0];

  QString chrtr2_str = QString ("%1x%2").arg (w).arg (h) + tr (" (CHRTR2 resolution)");
  QString screen_str = QString ("%1x%2").arg (mapdef.draw_width).arg (mapdef.draw_height) + tr (" (Screen resolution)");

  lst << chrtr2_str << screen_str << tr ("1280x1024") << tr ("2560x2048") << tr ("5120x4096") << tr ("10240x8192");


  QString res = QInputDialog::getItem (this, tr ("gridEdit"), tr ("Image resolution"), lst, 0, false, &ok);

  if (ok && !res.isEmpty ())
    {
      w = res.section ('x', 0, 0).toInt ();


      //  Make sure we don't have a problem with the parentheses on the first two options.

      if (res.contains ('('))
        {
          h = res.section ('x', 1, 1).section (' ', 0, 0).toInt ();
        }
      else
        {
          h = res.section ('x', 1, 1).toInt ();
        }

      map->dummyBuffer (w, h, NVTrue);

      qApp->setOverrideCursor (Qt::WaitCursor);
      qApp->processEvents ();

      redrawMap (NVTrue, NVTrue);

      export_image (this, &misc, &options, map);


      qApp->restoreOverrideCursor ();


      map->dummyBuffer (0, 0, NVFalse);
    }
}



void 
gridEdit::clearFilterMasks ()
{
  //  Clear any filter masked areas.
  
  if (misc.poly_filter_mask_count)
    {
      misc.poly_filter_mask.clear ();
      misc.poly_filter_mask_count = 0;
    }
}
