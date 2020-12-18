
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



#include "manageLayers.hpp"
#include "manageLayersHelp.hpp"


//  This is the CHRTR2 layer management dialog.

manageLayers::manageLayers (QWidget * parent, OPTIONS *op, MISC *mi):
  QDialog (parent, (Qt::WindowFlags) Qt::WA_DeleteOnClose)
{
  options = op;
  misc = mi;
  modified = NVFalse;


  setWindowTitle ("gridEdit Layer Preferences");


  resize (800, 100);


  setSizeGripEnabled (true);


  QVBoxLayout *vbox = new QVBoxLayout (this);
  vbox->setMargin (5);
  vbox->setSpacing (5);


  upGrp = new QButtonGroup (this);
  connect (upGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotUp (int)));

  dnGrp = new QButtonGroup (this);
  connect (dnGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotDown (int)));

  tpGrp = new QButtonGroup (this);
  connect (tpGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotTop (int)));

  bmGrp = new QButtonGroup (this);
  connect (bmGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotBottom (int)));

  closeGrp = new QButtonGroup (this);
  connect (closeGrp, SIGNAL (buttonClicked (int)), this, SLOT (slotCloseCHRTR2 (int)));


  QHBoxLayout *layersLayout[MAX_CHRTR2_FILES];

  for (int32_t i = 0 ; i < MAX_CHRTR2_FILES ; i++)
    {
      layers[i] = new QGroupBox (tr ("Layer  "), this);
      layers[i]->setWhatsThis (layersGridText);
      layersLayout[i] = new QHBoxLayout;
      layers[i]->setLayout (layersLayout[i]);

      file[i] = new QLineEdit (layers[i]);
      file[i]->setReadOnly (true);
      file[i]->setToolTip (tr ("Filename (read-only)"));
      layersLayout[i]->addWidget (file[i]);

      up[i] = new QPushButton (layers[i]);
      up[i]->setIcon (QIcon (":/icons/go-up.png"));
      up[i]->setToolTip (tr ("Move up"));
      upGrp->addButton (up[i], i);
      layersLayout[i]->addWidget (up[i]);

      dn[i] = new QPushButton (layers[i]);
      dn[i]->setIcon (QIcon (":/icons/go-down.png"));
      dn[i]->setToolTip (tr ("Move down"));
      dnGrp->addButton (dn[i], i);
      layersLayout[i]->addWidget (dn[i]);

      tp[i] = new QPushButton (layers[i]);
      tp[i]->setIcon (QIcon (":/icons/go-top.png"));
      tp[i]->setToolTip (tr ("Move to top"));
      tpGrp->addButton (tp[i], i);
      layersLayout[i]->addWidget (tp[i]);

      bm[i] = new QPushButton (layers[i]);
      bm[i]->setIcon (QIcon (":/icons/go-bottom.png"));
      bm[i]->setToolTip (tr ("Move to bottom"));
      bmGrp->addButton (bm[i], i);
      layersLayout[i]->addWidget (bm[i]);

      closeCHRTR2[i] = new QPushButton (tr ("Close CHRTR2"), layers[i]);
      closeCHRTR2[i]->setToolTip (tr ("Close this file and remove it from the layers list"));
      layersLayout[i]->addWidget (closeCHRTR2[i]);
      closeGrp->addButton (closeCHRTR2[i], i);

      vbox->addWidget (layers[i], 0, 0);
    }


  QHBoxLayout *actions = new QHBoxLayout (0);
  vbox->addLayout (actions);

  QPushButton *bHelp = new QPushButton (this);
  bHelp->setIcon (QIcon (":/icons/contextHelp.png"));
  bHelp->setToolTip (tr ("Enter What's This mode for help"));
  connect (bHelp, SIGNAL (clicked ()), this, SLOT (slotHelp ()));
  actions->addWidget (bHelp);

  actions->addStretch (10);

  QPushButton *applyButton = new QPushButton (tr ("Apply"), this);
  applyButton->setToolTip (tr ("Apply layer changes"));
  applyButton->setWhatsThis (applyMLText);
  connect (applyButton, SIGNAL (clicked ()), this, SLOT (slotApply ()));
  actions->addWidget (applyButton);

  QPushButton *closeButton = new QPushButton (tr ("Close"), this);
  closeButton->setToolTip (tr ("Close the layers dialog"));
  closeButton->setWhatsThis (closeMLText);
  connect (closeButton, SIGNAL (clicked ()), this, SLOT (slotClose ()));
  actions->addWidget (closeButton);


  setFields ();


  show ();
}



manageLayers::~manageLayers ()
{
}



void
manageLayers::slotHelp ()
{
  QWhatsThis::enterWhatsThisMode ();
}



void
manageLayers::slotUp (int id)
{
  swap_layers (misc, id, id - 1);

  modified = NVTrue;

  setFields ();
}



void
manageLayers::slotDown (int id)
{
  swap_layers (misc, id, id + 1);

  modified = NVTrue;

  setFields ();
}



void
manageLayers::slotTop (int id)
{
  move_layer_to_top (misc, id);

  modified = NVTrue;

  setFields ();
}



void
manageLayers::slotBottom (int id)
{
  move_layer_to_bottom (misc, id);

  modified = NVTrue;

  setFields ();
}



void
manageLayers::slotCloseCHRTR2 (int id)
{
  if (QMessageBox::information (this, tr ("gridEdit Close CHRTR2 confirmation"), 
				tr ("Do you really want to close this CHRTR2 file?"),
				QMessageBox::Yes,  QMessageBox::No,  QMessageBox::NoButton) == QMessageBox::No) return;


  modified = NVTrue;


  remove_layer (misc, id);


  if (!misc->chrtr2_share->chrtr2_count) slotClose ();


  setFields ();
}



void 
manageLayers::setFields ()
{
  for (int32_t i = 0 ; i < MAX_CHRTR2_FILES ; i++)
    {
      if (i < misc->chrtr2_share->chrtr2_count)
	{
	  file[i]->setText (QFileInfo (QString (misc->chrtr2_share->chrtr2_file[i])).fileName ());

	  layers[i]->show ();


	  up[i]->setEnabled (true);
	  tp[i]->setEnabled (true);
	  dn[i]->setEnabled (true);
	  bm[i]->setEnabled (true);

	  if (!i)
	    {
	      up[i]->setEnabled (false);
	      tp[i]->setEnabled (false);
	    }

	  if (i == misc->chrtr2_share->chrtr2_count - 1)
	    {
	      dn[i]->setEnabled (false);
	      bm[i]->setEnabled (false);
	    }
	}
      else
	{
	  layers[i]->hide ();
	}
    }
}



void
manageLayers::slotApply ()
{
  if (modified) emit dataChangedSignal ();

  modified = NVFalse;
}



void
manageLayers::slotClose ()
{
  close ();
}
