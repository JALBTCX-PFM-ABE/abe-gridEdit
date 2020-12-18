
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



#ifndef PREFS_H
#define PREFS_H

#include "chrtr2.h"
#include "gridEditDef.hpp"


class Prefs:public QDialog
{
  Q_OBJECT 


    public:

  Prefs (QWidget *parent = 0, OPTIONS *op = NULL, MISC *mi = NULL);
  ~Prefs ();


 signals:

  void dataChangedSignal (uint8_t lockChanged);
  void hotKeyChangedSignal (int32_t i);


 protected:

  OPTIONS         *options, mod_options;

  MISC            *misc, mod_misc;

  QTabWidget      *prefTab;

  QButtonGroup    *bGrp, *cGrp;

  QDialog         *contoursD, *geotiffD, *hotKeyD;

  nvMap           *geotiff;

  QImage          layer1, layer2;

  QScrollBar      *geotiffScroll;

  int32_t         geotiff_w, geotiff_h;

  uint8_t         dataChanged, lockChanged;

  QTableWidget    *contoursTable, *hotKeyTable;

  QPushButton     *bTextColor, *bContourColor, *bCoastColor, *bBackgroundColor, *bContourHighlightColor, *bMaskColor, *bRestoreDefaults,
                  *bCovRealColor, *bCovDigitizedColor, *bCovInterpolatedColor, *bCovBackgroundColor;

  QPalette        bContourPalette, bCoastPalette, bBackgroundPalette, bContourHighlightPalette, bMaskPalette,
                  bCovRealPalette, bCovDigitizedPalette, bCovInterpolatedPalette, bCovBackgroundPalette;;

  QPushButton     *bContourLevels, *bGeotiff, *bHotKeys;

  QCheckBox       *force, *zero, *lock;

  QLineEdit       *contourInt;
  
  QDoubleSpinBox  *sunEx, *sunAz, *sunEl, *offset, *filterSlope;

  QComboBox       *ZFactor, *WFactor, *Width, *Index, *iconSize;

  QSpinBox        *contourSm, *overlap, *filterCutoff, *filterLength;

  QPixmap         helpIcon;

  QLabel          *geotiff_left;

  void setFields ();


  protected slots:

  void slotContourColor ();
  void slotCoastColor ();
  void slotBackgroundColor ();
  void slotContourHighlightColor ();
  void slotMaskColor ();
  void slotCovRealColor ();
  void slotCovInterpolatedColor ();
  void slotCovDigitizedColor ();
  void slotCovBackgroundColor ();
  void slotHelp ();
  void slotRestoreDefaults ();
  void slotPositionClicked (int id);
  void slotContourLevelsClicked ();
  void slotGeotiff ();
  void slotPreRedraw (NVMAP_DEF mapdef);
  void slotGeotiffScrollValueChanged (int value);
  void slotGeotiffScrollSliderMoved (int value);
  void slotCloseGeotiff ();
  void slotHotKeys ();
  void slotCloseHotKeys ();
  void slotCloseContours ();
  void slotApplyPrefs ();
  void slotClosePrefs ();

 private:
};

#endif
