#include <WApplication>
#include <WContainerWidget>
#include <WText>
#include <WPushButton>
#undef SLOT
//qt includes

#include <QString>
#include <QStringList>

//omg includes
#include <omgui.h>

//
// This is the entry-point for the program.
//
// This function is executed when a new user surfs to the Wt application,
// and after the library has negotiated browser support. When this function
// returns, the application is terminated.

int wmain(int argc, char **argv)
{
  // Instantiate the Wt application.

  WApplication appl(argc, argv);

  // Widgets can be added to a parent by calling addWidget()...
  // WApplication::root() is a WContainerWidget that is associated with
  // the entire browser window.

  appl.root()->addWidget(new WText("<h1>Hello, World!</h1>"));

  // .. or by specifying a parent at construction

  WPushButton *Button = new WPushButton("Quit", appl.root());

  //Print a list of available algorithms
  QStringList myList = Omgui::getModellerPluginNameList();
  QStringListIterator myIterator(myList);
  qDebug("Looking for plugins");
  while (myIterator.hasNext())
  {
    QString myString = myIterator.next();
    qDebug("Found plugin : " + myString.toLocal8Bit());
    myString += "<br />";
    //appl.root()->addWidget(new WText(myString.toStdString()));
  }
  // React to user events using a signal/slot mechanism.

  Button->clicked.connect(&appl, &WApplication::quit);

  // Start the application event loop.
  //   This function returns when the user has closed the application window
  //   (which is detected by a time-out of the keep-alive messages), or when
  //   the application is exited using the Quit button.

  return appl.exec();
}
