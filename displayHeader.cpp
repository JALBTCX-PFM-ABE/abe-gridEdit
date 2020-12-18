
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



#include "displayHeader.hpp"
#include "displayHeaderHelp.hpp"
#include "ngets.h"

displayHeader::displayHeader (QWidget * parent, MISC *misc):
  QDialog (parent, (Qt::WindowFlags) Qt::WA_DeleteOnClose)
{
  setWindowTitle (tr ("gridEdit Display CHRTR2 Header"));
  resize (500, 600);


  setSizeGripEnabled (true);


  QVBoxLayout *vbox = new QVBoxLayout (this);
  vbox->setMargin (5);
  vbox->setSpacing (5);


  displayBox = new QListWidget (this);
  displayBox->setWhatsThis (displayTableText);


  FILE *fp;
  char temp[1024];
  if ((fp = fopen (misc->chrtr2_share->chrtr2_file[0], "r")) == NULL)
    {
      QMessageBox::warning (this, tr ("gridEdit CHRTR2 Header"), tr ("Unable to read the CHRTR2 header."));
      slotClose ();
    }


  //  The ever popular "Drew loop" (you had to be in the Gravity Division at NAVOCEANO in the late 70's to get that reference).

  while (NVTrue)
    {
      ngets (temp, sizeof (temp), fp);

      if (!strncmp (temp, "[END OF HEADER]", 15)) break;

      displayBox->addItem (QString (temp));
    }

  fclose (fp);


  vbox->addWidget (displayBox, 0, 0);


  QPushButton *closeButton = new QPushButton (tr ("Close"), this);
  closeButton->setToolTip (tr ("Close the Display CHRTR2 Header dialog"));
  closeButton->setWhatsThis (closeDHText);
  connect (closeButton, SIGNAL (clicked ()), this, SLOT (slotClose ()));


  vbox->addWidget (closeButton, 1, 0);


  show ();
}



displayHeader::~displayHeader ()
{
}



void
displayHeader::slotClose ()
{
  close ();
}
