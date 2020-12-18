
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



#include "startupMessage.hpp"

startupMessage::startupMessage (QWidget *parent, OPTIONS *op, int32_t nf):
  QDialog (parent, (Qt::WindowFlags) Qt::WA_DeleteOnClose | Qt::WindowStaysOnTopHint)
{
  options = op;
  new_feature = nf;

  setWindowTitle (tr ("gridEdit New Features"));

  QVBoxLayout *vbox = new QVBoxLayout (this);
  vbox->setMargin (5);
  vbox->setSpacing (5);

  QTextEdit *msgBox = new QTextEdit (0);
  msgBox->setAcceptRichText (true);
  msgBox->setMinimumWidth (800);
  msgBox->setMinimumHeight (800);

  msgBox->setText (tr ("<br><center><h3>Five new features have been added to the CHRTR2 Editor.</h3></center><br>"
                       "First, you can now highlight and/or un-highlight points inside a polygon in the 3D and 2D editors.  You can also "
                       "highlight single points using the right click popup menu when in <b>DELETE_POINT</b> mode.  What is not obvious "
                       "(unless you read the context sensitive help) is that you can delete all of the highlighted points by pressing the "
                       "<b>Enter</b> key.  Note that the highlighted points are now zoom insensitive and also that they do not disappear "
                       "when rotating or zooming.<br><br>"
                       "Second, you can now lock minimum and maximum values and colors in the surface viewer when coloring the surface by "
                       "any of the attributes (not just depth).  The tool to change the scale color value has been moved from the preferences dialog "
                       "to the value lock dialog that pops up whenever you click on the min or max scale box.<br><br>"
                       "Third, after years of badgering, I have added an actual scale behind the data in the 3D editor.  Of course, now that "
                       "it's there no one likes it ;-)  C'est la vie.<br><br>"
                       "Fourth, you can now view contours in the 2D tracker map in the 3D editor.  Sort of handy I guess ;-)<br><br>"
                       "Last, but not least, you can now move the coverage box in the coverage map by pressing <b>Ctrl</b>, left clicking "
                       "in the center of the coverage box, moving the box, and then left clicking again to redraw the main map.  You can also "
                       "do this by selecting <b>Drag rectangle</b> from the right click menu in the coverage map, clicking and dragging the "
                       "rectangle, and the left clicking to place the box and redraw the main map.<br><br>"
                       "<b>IMPORTANT NOTES:<br>"
                       "<ul>"
                       "<li>Scale colors and locked scale values are persistent.  That is, if you exit and restart the viewer the "
                       "colors in the color scale will remain as you have set them and locked scale values will remain locked.</li>"
                       "<li>The color scale and locks for color by depth will be passed to the 2D and 3D editors.</li>"
                       "<li>Changes to the color scale and/or locks can only be done in the viewer.  There are no controls for "
                       "these in the editors.</li>"
                       "<li>If you lock a scale and there are no data points outside that value the lock will be temporarily "
                       "disabled.  For example, if you set the minimum lock to 10.0 meters and the data you are viewing is all deeper than "
                       "12.0 meters then the color scale will be recomputed until you go to an area with data less than 10.0 meters "
                       "at which point the lock icon will reappear and values outside the range will be color locked.</li>"
                       "</ul><br></b>"));

  vbox->addWidget (msgBox);

  QHBoxLayout *actions = new QHBoxLayout (0);
  vbox->addLayout (actions);

  QCheckBox *dontShow = new QCheckBox (tr ("Don't show this again"), 0);
  dontShow->setToolTip (tr ("Check this box to make sure that the new feature message does not display at next start."));
  connect (dontShow, SIGNAL (stateChanged (int)), this, SLOT (slotDontShowStartupMessage (int)));
  actions->addWidget (dontShow);

  actions->addStretch (10);

  QPushButton *closeButton = new QPushButton (tr ("Close"), this);
  closeButton->setToolTip (tr ("Close the new feature dialog"));
  connect (closeButton, SIGNAL (clicked ()), this, SLOT (slotCloseStartupMessage ()));
  actions->addWidget (closeButton);

  show ();
}



startupMessage::~startupMessage ()
{
}



void 
startupMessage::slotDontShowStartupMessage (int state)
{
  if (state)
    {
      options->new_feature = new_feature;
      options->startup_message = NVFalse;
    }
  else
    {
      options->startup_message = NVTrue;
    }
}



void 
startupMessage::slotCloseStartupMessage ()
{
  close ();
}
