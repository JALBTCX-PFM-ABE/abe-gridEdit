
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



#include "prefs.hpp"
#include "prefsHelp.hpp"


//  This is the preferences dialog.  There are about a bazillion (that's a technical term) different
//  options here.  Hopefully the code is easy to follow.

Prefs::Prefs (QWidget * parent, OPTIONS *op, MISC *mi):
  QDialog (parent, (Qt::WindowFlags) Qt::WA_DeleteOnClose)
{
  options = op;
  misc = mi;

  mod_options = *op;
  mod_misc = *mi;


  setModal (true);


  contoursD = hotKeyD = geotiffD = NULL;


  dataChanged = NVFalse;
  lockChanged = NVFalse;


  setWindowTitle (tr ("gridEdit Preferences"));

  QVBoxLayout *vbox = new QVBoxLayout (this);
  vbox->setMargin (5);
  vbox->setSpacing (5);


  QRadioButton *hdms = new QRadioButton (tr ("Hemisphere Degrees Minutes Seconds.decimal"));
  QRadioButton *hdm_ = new QRadioButton (tr ("Hemisphere Degrees Minutes.decimal"));
  QRadioButton *hd__ = new QRadioButton (tr ("Hemisphere Degrees.decimal"));
  QRadioButton *sdms = new QRadioButton (tr ("+/-Degrees Minutes Seconds.decimal"));
  QRadioButton *sdm_ = new QRadioButton (tr ("+/-Degrees Minutes.decimal"));
  QRadioButton *sd__ = new QRadioButton (tr ("+/-Degrees.decimal"));

  QGroupBox *formBox = new QGroupBox (this);
  QVBoxLayout *formBoxLayout = new QVBoxLayout;
  formBox->setLayout (formBoxLayout);

  bGrp = new QButtonGroup (this);
  bGrp->setExclusive (true);
  connect (bGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotPositionClicked (int)));

  bGrp->addButton (hdms, 0);
  bGrp->addButton (hdm_, 1);
  bGrp->addButton (hd__, 2);
  bGrp->addButton (sdms, 3);
  bGrp->addButton (sdm_, 4);
  bGrp->addButton (sd__, 5);

  formBoxLayout->addWidget (hdms);
  formBoxLayout->addWidget (hdm_);
  formBoxLayout->addWidget (hd__);
  formBoxLayout->addWidget (sdms);
  formBoxLayout->addWidget (sdm_);
  formBoxLayout->addWidget (sd__);

  formBoxLayout->addStretch (1);


  prefTab = new QTabWidget ();
  prefTab->setTabPosition (QTabWidget::North);

  prefTab->addTab (formBox, tr ("Position Format"));
  prefTab->setTabToolTip (0, tr ("Set the position display format"));
  prefTab->setTabWhatsThis (0, bGrpText);


  QGroupBox *cbox = new QGroupBox (this);
  QHBoxLayout *cboxLayout = new QHBoxLayout;
  cbox->setLayout (cboxLayout);
  QVBoxLayout *cboxLeftLayout = new QVBoxLayout;
  QVBoxLayout *cboxRightLayout = new QVBoxLayout;
  cboxLayout->addLayout (cboxLeftLayout);
  cboxLayout->addLayout (cboxRightLayout);


  bCoastColor = new QPushButton (tr ("Coast"), this);
  bCoastColor->setToolTip (tr ("Change coastline color"));
  bCoastColor->setWhatsThis (coastColorText);
  bCoastPalette = bCoastColor->palette ();
  connect (bCoastColor, SIGNAL (clicked ()), this, SLOT (slotCoastColor ()));
  cboxRightLayout->addWidget (bCoastColor);


  bBackgroundColor = new QPushButton (tr ("Background"), this);
  bBackgroundColor->setToolTip (tr ("Change display background color"));
  bBackgroundColor->setWhatsThis (backgroundColorText);
  bBackgroundPalette = bBackgroundColor->palette ();
  connect (bBackgroundColor, SIGNAL (clicked ()), this, SLOT (slotBackgroundColor ()));
  cboxLeftLayout->addWidget (bBackgroundColor);


  bContourColor = new QPushButton (tr ("Contour"), this);
  bContourColor->setToolTip (tr ("Change contour color"));
  bContourColor->setWhatsThis (contourColorText);
  bContourPalette = bContourColor->palette ();
  connect (bContourColor, SIGNAL (clicked ()), this, SLOT (slotContourColor ()));
  cboxRightLayout->addWidget (bContourColor);


  bContourHighlightColor = new QPushButton (tr ("Highlight"), this);
  bContourHighlightColor->setToolTip (tr ("Change contour highlight color"));
  bContourHighlightColor->setWhatsThis (contourHighlightColorText);
  bContourHighlightPalette = bContourHighlightColor->palette ();
  connect (bContourHighlightColor, SIGNAL (clicked ()), this, SLOT (slotContourHighlightColor ()));
  cboxRightLayout->addWidget (bContourHighlightColor);


  bMaskColor = new QPushButton (tr ("Filter Mask"), this);
  bMaskColor->setToolTip (tr ("Change filter mask color"));
  bMaskColor->setWhatsThis (maskColorText);
  bMaskPalette = bMaskColor->palette ();
  connect (bMaskColor, SIGNAL (clicked ()), this, SLOT (slotMaskColor ()));
  cboxRightLayout->addWidget (bMaskColor);


  bCovRealColor = new QPushButton (tr ("Real coverage"), this);
  bCovRealColor->setToolTip (tr ("Change real coverage color"));
  bCovRealColor->setWhatsThis (covRealColorText);
  bCovRealPalette = bCovRealColor->palette ();
  connect (bCovRealColor, SIGNAL (clicked ()), this, SLOT (slotCovRealColor ()));
  cboxLeftLayout->addWidget (bCovRealColor);


  bCovDigitizedColor = new QPushButton (tr ("Digitized coverage"), this);
  bCovDigitizedColor->setToolTip (tr ("Change digitized/drawn coverage color"));
  bCovDigitizedColor->setWhatsThis (covDigitizedColorText);
  bCovDigitizedPalette = bCovDigitizedColor->palette ();
  connect (bCovDigitizedColor, SIGNAL (clicked ()), this, SLOT (slotCovDigitizedColor ()));
  cboxLeftLayout->addWidget (bCovDigitizedColor);


  bCovInterpolatedColor = new QPushButton (tr ("Interpolated coverage"), this);
  bCovInterpolatedColor->setToolTip (tr ("Change interpolated coverage color"));
  bCovInterpolatedColor->setWhatsThis (covInterpolatedColorText);
  bCovInterpolatedPalette = bCovInterpolatedColor->palette ();
  connect (bCovInterpolatedColor, SIGNAL (clicked ()), this, SLOT (slotCovInterpolatedColor ()));
  cboxLeftLayout->addWidget (bCovInterpolatedColor);


  bCovBackgroundColor = new QPushButton (tr ("Coverage background"), this);
  bCovBackgroundColor->setToolTip (tr ("Change coverage background color"));
  bCovBackgroundColor->setWhatsThis (covBackgroundColorText);
  bCovBackgroundPalette = bCovBackgroundColor->palette ();
  connect (bCovBackgroundColor, SIGNAL (clicked ()), this, SLOT (slotCovBackgroundColor ()));
  cboxLeftLayout->addWidget (bCovBackgroundColor);


  cboxLeftLayout->addStretch (1);
  cboxRightLayout->addStretch (1);


  prefTab->addTab (cbox, tr ("Colors"));
  prefTab->setTabToolTip (1, tr ("Set the display colors"));
  prefTab->setTabWhatsThis (1, colorText);


  QGroupBox *conBox = new QGroupBox (this);
  QHBoxLayout *conBoxLayout = new QHBoxLayout;
  conBox->setLayout (conBoxLayout);
  QVBoxLayout *conBoxLeftLayout = new QVBoxLayout;
  QVBoxLayout *conBoxRightLayout = new QVBoxLayout;
  conBoxLayout->addLayout (conBoxLeftLayout);
  conBoxLayout->addLayout (conBoxRightLayout);


  QGroupBox *intBox = new QGroupBox (tr ("Interval"), this);
  QHBoxLayout *intBoxLayout = new QHBoxLayout;
  intBox->setLayout (intBoxLayout);

  contourInt = new QLineEdit (intBox);
  contourInt->setToolTip (tr ("Change the contour interval"));
  contourInt->setWhatsThis (contourIntText);
  intBoxLayout->addWidget (contourInt);

  bContourLevels = new QPushButton (tr ("Set Contour Levels"), intBox);
  bContourLevels->setToolTip (tr ("Set non-uniform individual contour intervals"));
  bContourLevels->setWhatsThis (contourLevelsText);
  connect (bContourLevels, SIGNAL (clicked ()), this, SLOT (slotContourLevelsClicked ()));
  intBoxLayout->addWidget (bContourLevels);

  conBoxLeftLayout->addWidget (intBox);


  QGroupBox *smBox = new QGroupBox (tr ("Smoothing"), this);
  QHBoxLayout *smBoxLayout = new QHBoxLayout;
  smBox->setLayout (smBoxLayout);
  contourSm = new QSpinBox (smBox);
  contourSm->setRange (0, 10);
  contourSm->setSingleStep (1);
  contourSm->setToolTip (tr ("Change the contour smoothing factor (0-10)"));
  contourSm->setWhatsThis (contourSmText);
  smBoxLayout->addWidget (contourSm);
  conBoxLeftLayout->addWidget (smBox);


  QGroupBox *zBox = new QGroupBox (tr ("Scale"), this);
  QHBoxLayout *zBoxLayout = new QHBoxLayout;
  zBox->setLayout (zBoxLayout);
  ZFactor = new QComboBox (zBox);
  ZFactor->setToolTip (tr ("Change the contour/Z scale factor"));
  ZFactor->setWhatsThis (ZFactorText);
  ZFactor->setEditable (true);
  ZFactor->addItem ("1.00000 " + tr ("(no scaling)"));
  ZFactor->addItem ("3.28084 " + tr ("(meters to feet)"));
  ZFactor->addItem ("0.34080 " + tr ("(feet to meters)"));
  ZFactor->addItem ("0.54681 " + tr ("(meters to fathoms)"));
  ZFactor->addItem ("1.82880 " + tr ("(fathoms to meters)"));
  ZFactor->addItem ("0.53333 " + tr ("(meters to fathoms (at 4800 ft/sec)"));
  zBoxLayout->addWidget (ZFactor);
  conBoxLeftLayout->addWidget (zBox);


  QGroupBox *widBox = new QGroupBox (tr ("Width"), this);
  QHBoxLayout *widBoxLayout = new QHBoxLayout;
  widBox->setLayout (widBoxLayout);
  Width = new QComboBox (widBox);
  Width->setToolTip (tr ("Change the contour line width/thickness (pixels)"));
  Width->setWhatsThis (WidthText);
  Width->setEditable (true);
  Width->addItem ("1");
  Width->addItem ("2");
  Width->addItem ("3");
  widBoxLayout->addWidget (Width);
  conBoxRightLayout->addWidget (widBox);


  QGroupBox *iBox = new QGroupBox (tr ("Index"), this);
  QHBoxLayout *iBoxLayout = new QHBoxLayout;
  iBox->setLayout (iBoxLayout);
  Index = new QComboBox (iBox);
  Index->setToolTip (tr ("Change the index contour (Nth contour emphasized)"));
  Index->setWhatsThis (IndexText);
  Index->setEditable (true);
  Index->addItem ("0");
  Index->addItem ("2");
  Index->addItem ("4");
  Index->addItem ("5");
  Index->addItem ("10");
  iBoxLayout->addWidget (Index);
  conBoxRightLayout->addWidget (iBox);


  QGroupBox *offBox = new QGroupBox (tr ("Offset"), this);
  QHBoxLayout *offBoxLayout = new QHBoxLayout;
  offBox->setLayout (offBoxLayout);
  offset = new QDoubleSpinBox (offBox);
  offset->setDecimals (2);
  offset->setRange (-1000.0, 1000.0);
  offset->setSingleStep (10.0);
  offset->setToolTip (tr ("Change the contour/Z offset value"));
  offset->setWhatsThis (offsetText);
  offBoxLayout->addWidget (offset);
  conBoxRightLayout->addWidget (offBox);


  conBoxLeftLayout->addStretch (1);
  conBoxRightLayout->addStretch (1);


  prefTab->addTab (conBox, tr ("Contour/Depth"));
  prefTab->setTabToolTip (2, tr ("Set the contour and depth options"));
  prefTab->setTabWhatsThis (2, conText);


  QGroupBox *filterBox = new QGroupBox (this);
  QVBoxLayout *filterBoxLayout = new QVBoxLayout;
  filterBox->setLayout (filterBoxLayout);


  QGroupBox *cutBox = new QGroupBox (tr ("Cutoff frequency"), this);
  QHBoxLayout *cutBoxLayout = new QHBoxLayout;
  cutBox->setLayout (cutBoxLayout);
  filterCutoff = new QSpinBox (cutBox);
  filterCutoff->setRange (1, 30);
  filterCutoff->setSingleStep (1);
  filterCutoff->setToolTip (tr ("Change the filter cutoff frequency"));
  cutBoxLayout->addWidget (filterCutoff);
  filterBoxLayout->addWidget (cutBox);

  QGroupBox *sBox = new QGroupBox (tr ("Slope"), this);
  QHBoxLayout *sBoxLayout = new QHBoxLayout;
  sBox->setLayout (sBoxLayout);
  filterSlope = new QDoubleSpinBox (sBox);
  filterSlope->setDecimals (2);
  filterSlope->setRange (0.0, 1.0);
  filterSlope->setSingleStep (0.01);
  filterSlope->setToolTip (tr ("Change the filter slope"));
  sBoxLayout->addWidget (filterSlope);
  filterBoxLayout->addWidget (sBox);

  QGroupBox *lenBox = new QGroupBox (tr ("Length"), this);
  QHBoxLayout *lenBoxLayout = new QHBoxLayout;
  lenBox->setLayout (lenBoxLayout);
  filterLength = new QSpinBox (lenBox);
  filterLength->setRange (5, 90);
  filterLength->setSingleStep (5);
  filterLength->setToolTip (tr ("Change the filter length"));
  lenBoxLayout->addWidget (filterLength);
  filterBoxLayout->addWidget (lenBox);

  filterBoxLayout->addStretch (1);


  prefTab->addTab (filterBox, tr ("Filter"));
  prefTab->setTabToolTip (3, tr ("Set the statistical filter options"));
  prefTab->setTabWhatsThis (3, filterText);


  QGroupBox *miscBox = new QGroupBox (this);
  QHBoxLayout *miscBoxLayout = new QHBoxLayout;
  miscBox->setLayout (miscBoxLayout);
  QVBoxLayout *miscBoxLeftLayout = new QVBoxLayout;
  QVBoxLayout *miscBoxRightLayout = new QVBoxLayout;
  miscBoxLayout->addLayout (miscBoxLeftLayout);
  miscBoxLayout->addLayout (miscBoxRightLayout);

  QGroupBox *wBox = new QGroupBox (tr ("MISP weight factor"), this);
  QHBoxLayout *wBoxLayout = new QHBoxLayout;
  wBox->setLayout (wBoxLayout);
  WFactor = new QComboBox (wBox);
  WFactor->setToolTip (tr ("Change the MISP weighting factor"));
  WFactor->setWhatsThis (WFactorText);
  WFactor->setEditable (true);
  WFactor->addItem (tr ("1"));
  WFactor->addItem (tr ("2"));
  WFactor->addItem (tr ("3"));
  wBoxLayout->addWidget (WFactor);
  miscBoxLeftLayout->addWidget (wBox);


  QGroupBox *forceBox = new QGroupBox (tr ("MISP force original input value"), this);
  QHBoxLayout *forceBoxLayout = new QHBoxLayout;
  forceBox->setLayout (forceBoxLayout);
  force = new QCheckBox (forceBox);
  force->setToolTip (tr ("Force MISP to use the original input value when available"));
  force->setWhatsThis (forceText);
  forceBoxLayout->addWidget (force);
  miscBoxLeftLayout->addWidget (forceBox);


  QGroupBox *zeroBox = new QGroupBox (tr ("Restart colors at zero"), this);
  QHBoxLayout *zeroBoxLayout = new QHBoxLayout;
  zeroBox->setLayout (zeroBoxLayout);
  zero = new QCheckBox (zeroBox);
  zero->setToolTip (tr ("Restart the color range at zero depth"));
  zero->setWhatsThis (zeroText);
  zeroBoxLayout->addWidget (zero);
  miscBoxLeftLayout->addWidget (zeroBox);


  bGeotiff = new QPushButton (tr ("GeoTIFF transparency"), this);
  bGeotiff->setToolTip (tr ("Change the amount of transparency for GeoTIFF overlay"));
  bGeotiff->setWhatsThis (prefsGeotiffText);
  connect (bGeotiff, SIGNAL (clicked ()), this, SLOT (slotGeotiff ()));
  miscBoxLeftLayout->addWidget (bGeotiff);


  QGroupBox *oBox = new QGroupBox (tr ("Window overlap percentage"), this);
  QHBoxLayout *oBoxLayout = new QHBoxLayout;
  oBox->setLayout (oBoxLayout);
  overlap = new QSpinBox (oBox);
  overlap->setRange (0, 95);
  overlap->setSingleStep (1);
  overlap->setToolTip (tr ("Change the move window overlap percentage"));
  overlap->setWhatsThis (overlapText);
  oBoxLayout->addWidget (overlap);
  miscBoxRightLayout->addWidget (oBox);


  bHotKeys = new QPushButton (tr ("Button Hot Keys"), this);
  bHotKeys->setToolTip (tr ("Change hot key sequences for buttons"));
  bHotKeys->setWhatsThis (hotKeysText);
  connect (bHotKeys, SIGNAL (clicked ()), this, SLOT (slotHotKeys ()));
  miscBoxRightLayout->addWidget (bHotKeys);


  QGroupBox *iconBox = new QGroupBox (tr ("Button Icon Size"), this);
  QHBoxLayout *iconBoxLayout = new QHBoxLayout;
  iconBox->setLayout (iconBoxLayout);

  iconSize = new QComboBox (iconBox);
  iconSize->setToolTip (tr ("Set the size (in pixels) of the main button icons"));
  iconSize->setWhatsThis (iconSizeText);
  iconSize->setEditable (false);
  iconSize->addItem ("16");
  iconSize->addItem ("20");
  iconSize->addItem ("24");
  iconSize->addItem ("28");
  iconSize->addItem ("32");
  iconBoxLayout->addWidget (iconSize);

  miscBoxRightLayout->addWidget (iconBox);


  QGroupBox *lockBox = new QGroupBox (tr ("Lock all layers except top"), this);
  QHBoxLayout *lockBoxLayout = new QHBoxLayout;
  lockBox->setLayout (lockBoxLayout);
  lock = new QCheckBox (lockBox);
  lock->setToolTip (tr ("By default write lock all layers except for the top layer"));
  lock->setWhatsThis (nonZeroLockText);
  lockBoxLayout->addWidget (lock);
  miscBoxRightLayout->addWidget (lockBox);


  miscBoxLeftLayout->addStretch (1);
  miscBoxRightLayout->addStretch (1);


  prefTab->addTab (miscBox, tr ("Miscellaneous"));
  prefTab->setTabToolTip (4, tr ("Set miscellaneous options"));
  prefTab->setTabWhatsThis (4, miscText);


  QGroupBox *sunBox = new QGroupBox (this);
  QVBoxLayout *sunBoxLayout = new QVBoxLayout;
  sunBox->setLayout (sunBoxLayout);


  QGroupBox *angBox = new QGroupBox (tr ("Angle"), this);
  QHBoxLayout *angBoxLayout = new QHBoxLayout;
  angBox->setLayout (angBoxLayout);
  sunAz = new QDoubleSpinBox (angBox);
  sunAz->setDecimals (1);
  sunAz->setRange (0.0, 359.9);
  sunAz->setSingleStep (1.0);
  sunAz->setValue (mod_options.sunopts.azimuth);
  sunAz->setWrapping (true);
  sunAz->setToolTip (tr ("Change the sun angle (0.0-360.0)"));
  sunAz->setWhatsThis (sunAzText);
  angBoxLayout->addWidget (sunAz);
  sunBoxLayout->addWidget (angBox);


  QGroupBox *eleBox = new QGroupBox (tr ("Elevation"), this);
  QHBoxLayout *eleBoxLayout = new QHBoxLayout;
  eleBox->setLayout (eleBoxLayout);
  sunEl = new QDoubleSpinBox (eleBox);
  sunEl->setDecimals (1);
  sunEl->setRange (0.0, 90.0);
  sunEl->setSingleStep (1.0);
  sunEl->setValue (mod_options.sunopts.elevation);
  sunEl->setWrapping (true);
  sunEl->setToolTip (tr ("Change the sun elevation (0.0-90.0)"));
  sunEl->setWhatsThis (sunElText);
  eleBoxLayout->addWidget (sunEl);
  sunBoxLayout->addWidget (eleBox);


  QGroupBox *exaBox = new QGroupBox (tr ("Exaggeration"), this);
  QHBoxLayout *exaBoxLayout = new QHBoxLayout;
  exaBox->setLayout (exaBoxLayout);
  sunEx = new QDoubleSpinBox (exaBox);
  sunEx->setDecimals (1);
  sunEx->setRange (0.0, 359.9);
  sunEx->setSingleStep (1.0);
  sunEx->setValue (mod_options.sunopts.exag);
  sunEx->setWrapping (true);
  sunEx->setToolTip (tr ("Change the sun Z exaggeration (1-10)"));
  sunEx->setWhatsThis (sunExText);
  exaBoxLayout->addWidget (sunEx);
  sunBoxLayout->addWidget (exaBox);

  sunBoxLayout->addStretch (1);


  prefTab->addTab (sunBox, tr ("Sun Shading"));
  prefTab->setTabToolTip (5, tr ("Set sun shading options"));
  prefTab->setTabWhatsThis (5, sunText);


  vbox->addWidget (prefTab);


  setFields ();


  QHBoxLayout *actions = new QHBoxLayout (0);
  vbox->addLayout (actions);

  QPushButton *bHelp = new QPushButton (this);
  bHelp->setIcon (QIcon (":/icons/contextHelp.png"));
  bHelp->setToolTip (tr ("Enter What's This mode for help"));
  connect (bHelp, SIGNAL (clicked ()), this, SLOT (slotHelp ()));
  actions->addWidget (bHelp);

  actions->addStretch (10);

  bRestoreDefaults = new QPushButton (tr ("Restore Defaults"), this);
  bRestoreDefaults->setToolTip (tr ("Restore all preferences to the default state"));
  bRestoreDefaults->setWhatsThis (restoreDefaultsText);
  connect (bRestoreDefaults, SIGNAL (clicked ()), this, SLOT (slotRestoreDefaults ()));
  actions->addWidget (bRestoreDefaults);

  QPushButton *applyButton = new QPushButton (tr ("OK"), this);
  applyButton->setToolTip (tr ("Accept changes and close dialog"));
  applyButton->setWhatsThis (applyPrefsText);
  connect (applyButton, SIGNAL (clicked ()), this, SLOT (slotApplyPrefs ()));
  actions->addWidget (applyButton);

  QPushButton *closeButton = new QPushButton (tr ("Cancel"), this);
  closeButton->setToolTip (tr ("Discard changes and close dialog"));
  closeButton->setWhatsThis (closePrefsText);
  connect (closeButton, SIGNAL (clicked ()), this, SLOT (slotClosePrefs ()));
  actions->addWidget (closeButton);


  show ();
}



Prefs::~Prefs ()
{
}



//  The following functions (until further notice) are just servicing pushbuttons.  They're intuitively obvious to the
//  most casual observer.

void
Prefs::slotCoastColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.coast_color, this, tr ("gridEdit Coastline Color"), QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.coast_color = clr;

  setFields ();
}



void 
Prefs::slotBackgroundColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.background_color, this, tr ("gridEdit Background Color"));

  if (clr.isValid ()) mod_options.background_color = clr;

  setFields ();
}



void
Prefs::slotContourColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.contour_color, this, tr ("gridEdit Contour Color"), QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.contour_color = clr;

  setFields ();
}



void
Prefs::slotContourHighlightColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.contour_highlight_color, this, tr ("gridEdit Highlighted Contour Color"),
                                QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.contour_highlight_color = clr;

  setFields ();
}



void
Prefs::slotMaskColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.poly_filter_mask_color, this, tr ("gridEdit Filter Mask Color"), QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.poly_filter_mask_color = clr;

  setFields ();
}



void
Prefs::slotCovRealColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.cov_real_color, this, tr ("gridEdit Real Coverage Color"), QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.cov_real_color = clr;

  setFields ();
}



void
Prefs::slotCovDigitizedColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.cov_digitized_color, this, tr ("gridEdit Digitized/Drawn Coverage Color"), QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.cov_digitized_color = clr;

  setFields ();
}



void
Prefs::slotCovInterpolatedColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.cov_interpolated_color, this, tr ("gridEdit Interpolated Coverage Color"), QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.cov_interpolated_color = clr;

  setFields ();
}



void
Prefs::slotCovBackgroundColor ()
{
  QColor clr;

  clr = QColorDialog::getColor (mod_options.cov_background_color, this, tr ("gridEdit Coverage Background Color"), QColorDialog::ShowAlphaChannel);

  if (clr.isValid ()) mod_options.cov_background_color = clr;

  setFields ();
}



void
Prefs::slotHelp ()
{
  QWhatsThis::enterWhatsThisMode ();
}



//  Restore all of the application defaults (this calls set_defaults and gets the original defaults).

void
Prefs::slotRestoreDefaults ()
{
  void set_defaults (MISC *misc, OPTIONS *options, uint8_t restore);


  set_defaults (misc, options, NVTrue);

  mod_options = *options;
  mod_misc = *misc;

  mod_options.sunopts.sun = sun_unv (mod_options.sunopts.azimuth, mod_options.sunopts.elevation);

  setFields ();


  mod_misc.GeoTIFF_init = NVTrue;


  *options = mod_options;
  *misc = mod_misc;

  emit dataChangedSignal (NVFalse);

  close ();
}



void
Prefs::slotPositionClicked (int id)
{
  mod_options.position_form = id;
}



void
Prefs::slotCloseContours ()
{
  //  Get the values.

  QString string;
  int32_t status, j;
  float tmp;

  j = 0;
  for (int32_t i = 0 ; i < MAX_CONTOUR_LEVELS ; i++)
    {
      string = contoursTable->item (i, 0)->text ();
      status = sscanf (string.toLatin1 (), "%f", &tmp);
      if (status == 1)
        {
          mod_options.contour_levels[j] = tmp;
          j++;
        }
    }

  mod_options.num_levels = j + 1;


  setFields ();

  contoursD->close ();
}



//  This function is used to set user defined contour levels.

void
Prefs::slotContourLevelsClicked ()
{
  if (contoursD) slotCloseContours ();


  QString string;

  mod_options.cint = 0.0;
  string.sprintf ("%.2f", mod_options.cint);
  contourInt->setText (string);

  
  contoursD = new QDialog (this, (Qt::WindowFlags) Qt::WA_DeleteOnClose);
  contoursD->setWindowTitle (tr ("gridEdit Contour Levels"));

  QVBoxLayout *vbox = new QVBoxLayout (contoursD);
  vbox->setMargin (5);
  vbox->setSpacing (5);


  contoursTable = new QTableWidget (MAX_CONTOUR_LEVELS, 1, this);
  contoursTable->setWhatsThis (contoursTableText);
  contoursTable->setAlternatingRowColors (true);
  QTableWidgetItem *chItem = new QTableWidgetItem (tr ("Contour level"));
  contoursTable->setHorizontalHeaderItem (0, chItem);

  QTableWidgetItem *cntItem[MAX_CONTOUR_LEVELS];

  for (int32_t i = 0 ; i < MAX_CONTOUR_LEVELS ; i++)
    {
      if (i < mod_options.num_levels)
        {
          string.sprintf ("%f", mod_options.contour_levels[i]);
        }
      else
        {
          string.sprintf (" ");
        }
      cntItem[i] = new QTableWidgetItem (string); 
      contoursTable->setItem (i, 0, cntItem[i]);
    }
  contoursTable->resizeColumnsToContents ();


  vbox->addWidget (contoursTable, 1);

  int32_t w = qMin (800, contoursTable->columnWidth (0) + 60);
  contoursD->resize (w, 600);


  QHBoxLayout *actions = new QHBoxLayout (0);
  vbox->addLayout (actions);

  QPushButton *bHelp = new QPushButton (contoursD);
  bHelp->setIcon (QIcon (":/icons/contextHelp.png"));
  bHelp->setToolTip (tr ("Enter What's This mode for help"));
  connect (bHelp, SIGNAL (clicked ()), this, SLOT (slotHelp ()));
  actions->addWidget (bHelp);

  actions->addStretch (10);

  QPushButton *closeButton = new QPushButton (tr ("Close"), contoursD);
  closeButton->setToolTip (tr ("Close the contour levels dialog"));
  closeButton->setWhatsThis (closeContoursText);
  connect (closeButton, SIGNAL (clicked ()), this, SLOT (slotCloseContours ()));
  actions->addWidget (closeButton);


  //  Put the dialog in the middle of the screen.

  contoursD->move (x () + width () / 2 - contoursD->width () / 2, y () + height () / 2 - contoursD->height () / 2);

  contoursD->show ();
}



//  This function sets the GeoTIFF transparency level (it uses a dummy GeoTIFF).

void 
Prefs::slotGeotiff ()
{
  if (geotiffD) geotiffD->close ();


  geotiffD = new QDialog (this, (Qt::WindowFlags) Qt::WA_DeleteOnClose);
  geotiffD->setWindowTitle (tr ("gridEdit GeoTIFF transparency"));


  QImage tmp_layer1 = QImage (":/icons/average.jpg");
  layer1 = tmp_layer1.convertToFormat (QImage::Format_ARGB32);

  geotiff_w = layer1.width ();
  geotiff_h = layer1.height ();


  QImage tmp_layer2 = QImage (":/icons/geotiff.jpg");
  layer2 = tmp_layer2.convertToFormat (QImage::Format_ARGB32);


  geotiffD->resize (geotiff_w + 20, geotiff_h + 120);



  QVBoxLayout *vbox = new QVBoxLayout (geotiffD);
  vbox->setMargin (5);
  vbox->setSpacing (5);


  QFrame *frame = new QFrame (geotiffD);
  frame->setFrameStyle (QFrame::Panel | QFrame::Sunken);


  //  Seriously cheating here - I'm just using the map class so I don't have to make a special widget just
  //  to paint the demo part of the transparency dialog.

  NVMAP_DEF mapdef;
  mapdef.projection = 3;
  mapdef.draw_width = geotiff_w;
  mapdef.draw_height = geotiff_h;
  mapdef.grid_inc_x = 0.0;
  mapdef.grid_inc_y = 0.0;
 
  mapdef.border = 0;
  mapdef.background_color = Qt::white;
  mapdef.coasts = NVMAP_NO_COAST;


  geotiff = new nvMap (geotiffD, &mapdef);
  connect (geotiff, SIGNAL (preRedrawSignal (NVMAP_DEF)), this, SLOT (slotPreRedraw (NVMAP_DEF)));

  geotiff->setWhatsThis (transparencyText);


  vbox->addWidget (frame);


  QVBoxLayout *fBox = new QVBoxLayout (frame);


  QHBoxLayout *hBox = new QHBoxLayout ();
  hBox->addWidget (geotiff);
  fBox->addLayout (hBox);



  QGroupBox *sbox = new QGroupBox (tr ("GeoTIFF transparency"), geotiffD);
  QHBoxLayout *sboxLayout = new QHBoxLayout;
  sbox->setLayout (sboxLayout);

  QString tmp;
  tmp.sprintf ("%03d", mod_options.GeoTIFF_alpha);
  geotiff_left = new QLabel (tmp, geotiffD);
  geotiff_left->setWhatsThis (transparencyText);
  sboxLayout->addWidget (geotiff_left);

  geotiffScroll = new QScrollBar (Qt::Horizontal, geotiffD);
  geotiffScroll->setRange (25, 255);
  geotiffScroll->setSliderPosition (mod_options.GeoTIFF_alpha);
  geotiffScroll->setTracking (NVFalse);
  connect (geotiffScroll, SIGNAL (valueChanged (int)), this, SLOT (slotGeotiffScrollValueChanged (int)));
  connect (geotiffScroll, SIGNAL (sliderMoved (int)), this, SLOT (slotGeotiffScrollSliderMoved (int)));
  geotiffScroll->setToolTip (tr ("Move to change the amount of transparency in the GeoTIFF overlay"));
  geotiffScroll->setWhatsThis (transparencyText);
  sboxLayout->addWidget (geotiffScroll, 10);

  QLabel *right = new QLabel (tr ("Opaque"), geotiffD);
  right->setWhatsThis (transparencyText);
  sboxLayout->addWidget (right);


  vbox->addWidget (sbox);


  QHBoxLayout *actions = new QHBoxLayout (0);
  vbox->addLayout (actions);

  QPushButton *bHelp = new QPushButton (geotiffD);
  bHelp->setIcon (QIcon (":/icons/contextHelp"));
  bHelp->setToolTip (tr ("Enter What's This mode for help"));
  connect (bHelp, SIGNAL (clicked ()), this, SLOT (slotHelp ()));
  actions->addWidget (bHelp);

  actions->addStretch (10);

  QPushButton *closeButton = new QPushButton (tr ("Close"), geotiffD);
  closeButton->setToolTip (tr ("Close the GeoTIFF transparancy dialog"));
  connect (closeButton, SIGNAL (clicked ()), this, SLOT (slotCloseGeotiff ()));
  actions->addWidget (closeButton);


  //  Put the dialog in the middle of the screen.

  geotiffD->move (x () + width () / 2 - geotiffD->width () / 2, y () + height () / 2 - geotiffD->height () / 2);

  geotiffD->show ();

  geotiff->enableSignals ();

  geotiff->redrawMap (NVTrue);
}



void 
Prefs::slotPreRedraw (NVMAP_DEF mapdef __attribute__ ((unused)))
{
  QColor pixel;


  for (int32_t i = 0 ; i < geotiff_w ; i++)
    {
      for (int32_t j = 0 ; j < geotiff_h ; j++)
        {
          pixel = QColor (layer2.pixel (i, j));
          pixel.setAlpha (mod_options.GeoTIFF_alpha);
          layer2.setPixel (i, j, pixel.rgba ());
        }
    }

  QPixmap lay1 = QPixmap::fromImage (layer1);
  QPixmap lay2 = QPixmap::fromImage (layer2);

  geotiff->drawPixmap (0, 0, &lay1, 0, 0, geotiff_w, geotiff_h, NVTrue);
  geotiff->drawPixmap (0, 0, &lay2, 0, 0, geotiff_w, geotiff_h, NVTrue);

  geotiff->setCursor (Qt::ArrowCursor);
}



void 
Prefs::slotGeotiffScrollValueChanged (int value)
{
  mod_options.GeoTIFF_alpha = value;

  geotiff->redrawMap (NVTrue);
}



void 
Prefs::slotGeotiffScrollSliderMoved (int value)
{
  QString tmp;
  tmp.sprintf ("%03d", value);
  geotiff_left->setText (tmp);
}



void 
Prefs::slotCloseGeotiff ()
{
  geotiffD->close ();
}



//  This is the hotkey changing dialog.

void 
Prefs::slotHotKeys ()
{
  if (hotKeyD) slotCloseHotKeys ();

  hotKeyD = new QDialog (this, (Qt::WindowFlags) Qt::WA_DeleteOnClose);
  hotKeyD->setWindowTitle (tr ("gridEdit Hot Keys"));

  QVBoxLayout *vbox = new QVBoxLayout (hotKeyD);
  vbox->setMargin (5);
  vbox->setSpacing (5);


  hotKeyTable = new QTableWidget (HOTKEYS, 2, this);
  hotKeyTable->setWhatsThis (hotKeyTableText);
  hotKeyTable->setAlternatingRowColors (true);
  QTableWidgetItem *bItemH = new QTableWidgetItem (tr ("Button"));
  hotKeyTable->setHorizontalHeaderItem (0, bItemH);
  QTableWidgetItem *kItemH = new QTableWidgetItem (tr ("Key"));
  hotKeyTable->setHorizontalHeaderItem (1, kItemH);

  QTableWidgetItem *bItem[HOTKEYS], *kItem[HOTKEYS];


  bItem[1] = new QTableWidgetItem (QIcon (":/icons/redrawcoverage.png"), mod_misc.buttonText[1]);
  bItem[2] = new QTableWidgetItem (QIcon (":/icons/contour.png"), mod_misc.buttonText[2]);
  bItem[3] = new QTableWidgetItem (QIcon (":/icons/display_interpolated.png"), mod_misc.buttonText[3]);


  for (int32_t i = 0 ; i < HOTKEYS ; i++)
    {
      bItem[i] = new QTableWidgetItem (mod_misc.buttonIcon[i], mod_misc.buttonText[i]);
      bItem[i]->setFlags (Qt::ItemIsEnabled);

      hotKeyTable->setItem (i, 0, bItem[i]);
      kItem[i] = new QTableWidgetItem (mod_options.buttonAccel[i]);
      hotKeyTable->setItem (i, 1, kItem[i]);
    }
  hotKeyTable->resizeColumnsToContents ();
  hotKeyTable->resizeRowsToContents ();

  vbox->addWidget (hotKeyTable, 1);

  int32_t w = qMin (800, hotKeyTable->columnWidth (0) + hotKeyTable->columnWidth (1) + 40);
  int32_t h = qMin (600, hotKeyTable->rowHeight (0) * HOTKEYS + 75);
  hotKeyD->resize (w, h);


  QHBoxLayout *actions = new QHBoxLayout (0);
  vbox->addLayout (actions);

  QPushButton *bHelp = new QPushButton (hotKeyD);
  bHelp->setIcon (QIcon (":/icons/contextHelp"));
  bHelp->setToolTip (tr ("Enter What's This mode for help"));
  connect (bHelp, SIGNAL (clicked ()), this, SLOT (slotHelp ()));
  actions->addWidget (bHelp);

  actions->addStretch (10);

  QPushButton *closeButton = new QPushButton (tr ("Close"), hotKeyD);
  closeButton->setToolTip (tr ("Close the hot key dialog"));
  closeButton->setWhatsThis (closeHotKeyText);
  connect (closeButton, SIGNAL (clicked ()), this, SLOT (slotCloseHotKeys ()));
  actions->addWidget (closeButton);


  //  Put the dialog in the middle of the screen.

  hotKeyD->move (x () + width () / 2 - hotKeyD->width () / 2, y () + height () / 2 - hotKeyD->height () / 2);

  hotKeyD->show ();
}



void 
Prefs::slotCloseHotKeys ()
{
  for (int32_t i = 0 ; i < HOTKEYS ; i++) mod_options.buttonAccel[i] = hotKeyTable->item (i, 1)->text ();

  hotKeyD->close ();
}



//  Preliminary field/button setting function.

void 
Prefs::setFields ()
{
  QString string;

  bGrp->button (mod_options.position_form)->setChecked (true);


  int32_t hue, sat, val;

  mod_options.coast_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bCoastPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bCoastPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bCoastPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bCoastPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bCoastPalette.setColor (QPalette::Normal, QPalette::Button, mod_options.coast_color);
  bCoastPalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.coast_color);
  bCoastColor->setPalette (bCoastPalette);


  mod_options.background_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bBackgroundPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bBackgroundPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bBackgroundPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bBackgroundPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bBackgroundPalette.setColor (QPalette::Normal, QPalette::Button, mod_options.background_color);
  bBackgroundPalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.background_color);
  bBackgroundColor->setPalette (bBackgroundPalette);


  mod_options.contour_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bContourPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bContourPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bContourPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bContourPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bContourPalette.setColor (QPalette::Normal, QPalette::Button, mod_options.contour_color);
  bContourPalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.contour_color);
  bContourColor->setPalette (bContourPalette);


  mod_options.contour_highlight_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bContourHighlightPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bContourHighlightPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bContourHighlightPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bContourHighlightPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bContourHighlightPalette.setColor (QPalette::Normal, QPalette::Button, mod_options.contour_highlight_color);
  bContourHighlightPalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.contour_highlight_color);
  bContourHighlightColor->setPalette (bContourHighlightPalette);


  mod_options.poly_filter_mask_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bMaskPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bMaskPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bMaskPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bMaskPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bMaskPalette.setColor (QPalette::Normal, QPalette::Button, mod_options.poly_filter_mask_color);
  bMaskPalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.poly_filter_mask_color);
  bMaskColor->setPalette (bMaskPalette);


  mod_options.cov_real_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bCovRealPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bCovRealPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bCovRealPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bCovRealPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bCovRealPalette.setColor (QPalette::Normal, QPalette::Button, mod_options.cov_real_color);
  bCovRealPalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.cov_real_color);
  bCovRealColor->setPalette (bCovRealPalette);


  mod_options.cov_digitized_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bCovDigitizedPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bCovDigitizedPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bCovDigitizedPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bCovDigitizedPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bCovDigitizedPalette.setColor (QPalette::Normal, QPalette::Button, mod_options.cov_digitized_color);
  bCovDigitizedPalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.cov_digitized_color);
  bCovDigitizedColor->setPalette (bCovDigitizedPalette);


  mod_options.cov_interpolated_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bCovInterpolatedPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bCovInterpolatedPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bCovInterpolatedPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bCovInterpolatedPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bCovInterpolatedPalette.setColor (QPalette::Normal, QPalette::Button, mod_options.cov_interpolated_color);
  bCovInterpolatedPalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.cov_interpolated_color);
  bCovInterpolatedColor->setPalette (bCovInterpolatedPalette);


  mod_options.cov_background_color.getHsv (&hue, &sat, &val);
  if (val < 128)
    {
      bCovBackgroundPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::white);
      bCovBackgroundPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::white);
    }
  else
    {
      bCovBackgroundPalette.setColor (QPalette::Normal, QPalette::ButtonText, Qt::black);
      bCovBackgroundPalette.setColor (QPalette::Inactive, QPalette::ButtonText, Qt::black);
    }
  bCovBackgroundPalette.setColor (QPalette::Normal, QPalette::Button, mod_options.cov_background_color);
  bCovBackgroundPalette.setColor (QPalette::Inactive, QPalette::Button, mod_options.cov_background_color);
  bCovBackgroundColor->setPalette (bCovBackgroundPalette);


  string.sprintf ("%.2f", mod_options.cint);
  contourInt->setText (string);

  contourSm->setValue (mod_options.smoothing_factor);

  string.sprintf ("%.5f", mod_options.z_factor);
  ZFactor->lineEdit ()->setText (string);

  offset->setValue (mod_options.z_offset);

  string.sprintf ("%d", mod_options.contour_width);
  Width->lineEdit ()->setText (string);

  string.sprintf ("%d", mod_options.contour_index);
  Index->lineEdit ()->setText (string);

  filterCutoff->setValue (mod_options.filter_cutoff);
  filterSlope->setValue (mod_options.filter_slope);
  filterLength->setValue (mod_options.filter_length);

  string.sprintf ("%d", mod_options.misp_weight);
  WFactor->lineEdit ()->setText (string);
  force->setChecked (mod_options.misp_force_original);

  zero->setChecked (mod_options.zero_turnover);

  lock->setChecked (mod_options.non_top_layers_locked);

  overlap->setValue (mod_options.overlap_percent);

  sunAz->setValue (mod_options.sunopts.azimuth);
  sunEl->setValue (mod_options.sunopts.elevation);
  sunEx->setValue (mod_options.sunopts.exag);

  switch (mod_options.main_button_icon_size)
    {
    case 16:
      iconSize->setCurrentIndex (0);
      break;

    case 20:
      iconSize->setCurrentIndex (1);
      break;

    case 24:
    default:
      iconSize->setCurrentIndex (2);
      break;

    case 28:
      iconSize->setCurrentIndex (3);
      break;

    case 32:
      iconSize->setCurrentIndex (4);
      break;
    }
}



//  Get the values from the fields/buttons and issue a change signal if needed.

void
Prefs::slotApplyPrefs ()
{
  int32_t tmp_i, status;
  float tmp_f;


  if (mod_options.position_form != options->position_form) dataChanged = NVTrue;


  QString tmp = contourInt->text ();
  status = sscanf (tmp.toLatin1 (), "%f", &tmp_f);
  if (status == 1) mod_options.cint = tmp_f;
  if (mod_options.cint != options->cint) dataChanged = NVTrue;

  tmp = contourSm->text ();
  status = sscanf (tmp.toLatin1 (), "%d", &tmp_i);
  if (status == 1) mod_options.smoothing_factor = tmp_i;
  if (mod_options.smoothing_factor != options->smoothing_factor) dataChanged = NVTrue;

  tmp = ZFactor->currentText ();
  status = sscanf (tmp.toLatin1 (), "%f", &tmp_f);
  if (status == 1) mod_options.z_factor = tmp_f;
  if (mod_options.z_factor != options->z_factor) dataChanged = NVTrue;

  tmp = offset->text ();
  status = sscanf (tmp.toLatin1 (), "%f", &tmp_f);
  if (status == 1) mod_options.z_offset = tmp_f;
  if (mod_options.z_offset != options->z_offset) dataChanged = NVTrue;

  tmp = Width->currentText ();
  status = sscanf (tmp.toLatin1 (), "%d", &tmp_i);
  if (status == 1) mod_options.contour_width = tmp_i;
  if (mod_options.contour_width != options->contour_width) dataChanged = NVTrue;


  tmp = Index->currentText ();
  status = sscanf (tmp.toLatin1 (), "%d", &tmp_i);
  if (status == 1) mod_options.contour_index = tmp_i;
  if (mod_options.contour_index != options->contour_index) dataChanged = NVTrue;

  mod_options.filter_cutoff = filterCutoff->value ();
  if (mod_options.filter_cutoff != options->filter_cutoff) dataChanged = NVTrue;
  mod_options.filter_slope = filterSlope->value ();
  if (mod_options.filter_slope != options->filter_slope) dataChanged = NVTrue;
  mod_options.filter_length = filterLength->value ();
  if (mod_options.filter_length != options->filter_length) dataChanged = NVTrue;


  tmp = WFactor->currentText ();
  status = sscanf (tmp.toLatin1 (), "%d", &tmp_i);
  if (status == 1) mod_options.misp_weight = tmp_i;
  if (mod_options.misp_weight != options->misp_weight) dataChanged = NVTrue;

  mod_options.misp_force_original = force->isChecked ();
  if (mod_options.misp_force_original != options->misp_force_original) dataChanged = NVTrue;

  mod_options.zero_turnover = zero->isChecked ();
  if (mod_options.zero_turnover != options->zero_turnover) dataChanged = NVTrue;

  lockChanged = NVFalse;
  mod_options.non_top_layers_locked = lock->isChecked ();
  if (mod_options.non_top_layers_locked != options->non_top_layers_locked)
    {
      dataChanged = NVTrue;
      lockChanged = NVTrue;
    }

  tmp = overlap->text ();
  status = sscanf (tmp.toLatin1 (), "%d", &tmp_i);
  if (status == 1) mod_options.overlap_percent = tmp_i;
  if (mod_options.overlap_percent != options->overlap_percent) dataChanged = NVTrue;


  mod_options.sunopts.azimuth = sunAz->value ();
  if (mod_options.sunopts.azimuth != options->sunopts.azimuth) dataChanged = NVTrue;
  mod_options.sunopts.elevation = sunEl->value ();
  if (mod_options.sunopts.elevation != options->sunopts.elevation) dataChanged = NVTrue;
  mod_options.sunopts.exag = sunEx->value ();
  if (mod_options.sunopts.exag != options->sunopts.exag) dataChanged = NVTrue;


  mod_options.sunopts.sun = sun_unv (mod_options.sunopts.azimuth, mod_options.sunopts.elevation);


  switch (iconSize->currentIndex ())
    {
    case 0:
      mod_options.main_button_icon_size = 16;
      break;

    case 1:
      mod_options.main_button_icon_size = 20;
      break;

    case 2:
      mod_options.main_button_icon_size = 24;
      break;

    case 3:
      mod_options.main_button_icon_size = 28;
      break;

    case 4:
      mod_options.main_button_icon_size = 32;
      break;
    }
  if (mod_options.main_button_icon_size != options->main_button_icon_size) dataChanged = NVTrue;


  if (mod_options.GeoTIFF_alpha != options->GeoTIFF_alpha)
    {
      dataChanged = NVTrue;
      mod_misc.GeoTIFF_init = NVTrue;
    }


  if (mod_options.coast_color != options->coast_color) dataChanged = NVTrue;
  if (mod_options.background_color != options->background_color) dataChanged = NVTrue;
  if (mod_options.contour_color != options->contour_color) dataChanged = NVTrue;
  if (mod_options.contour_highlight_color != options->contour_highlight_color) dataChanged = NVTrue;
  if (mod_options.poly_filter_mask_color != options->poly_filter_mask_color) dataChanged = NVTrue;
  if (mod_options.cov_real_color != options->cov_real_color) dataChanged = NVTrue;
  if (mod_options.cov_digitized_color != options->cov_digitized_color) dataChanged = NVTrue;
  if (mod_options.cov_interpolated_color != options->cov_interpolated_color) dataChanged = NVTrue;
  if (mod_options.cov_background_color != options->cov_background_color) dataChanged = NVTrue;


  //  Make sure we have no hotkey duplications.

  for (int32_t i = 0 ; i < HOTKEYS ; i++)
    {
      for (int32_t j = i + 1 ; j < HOTKEYS ; j++)
        {
          if (mod_options.buttonAccel[i].toUpper () == mod_options.buttonAccel[j].toUpper ())
            {
              QMessageBox::warning (0, tr ("gridEdit preferences"),
                                    tr ("Hot key for <b>%1</b> button conflicts with hotkey for <b>%2</b> button!<br><br>").arg 
                                    (misc->buttonText[i]).arg (misc->buttonText[j]) +
                                    tr ("Resetting button hotkeys for these two buttons."));

              mod_options.buttonAccel[i] = options->buttonAccel[i];
              mod_options.buttonAccel[j] = options->buttonAccel[j];
              dataChanged = NVFalse;

              hotKeyD->close ();
              hotKeyD = NULL;

              return;
            }
        }
    }


  //  Now check for changes to the hotkeys.

  for (int32_t i = 0 ; i < HOTKEYS ; i++)
    {
      if (mod_options.buttonAccel[i] != options->buttonAccel[i])
        {
          dataChanged = NVTrue;
          emit hotKeyChangedSignal (i);
          break;
        }
    }


  if (mod_options.num_levels != options->num_levels) dataChanged = NVTrue;


  for (int32_t i = 0 ; i < MAX_CONTOUR_LEVELS ; i++)
    {
      if (mod_options.contour_levels[i] != options->contour_levels[i])
        {
          dataChanged = NVTrue;
          break;
        }
    }


  setFields ();


  hide ();


  if (dataChanged)
    {
      *options = mod_options;
      *misc = mod_misc;

      emit dataChangedSignal (lockChanged);
    }

  close ();
}



void
Prefs::slotClosePrefs ()
{
  close ();
}
