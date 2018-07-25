#include <parametersWindow.h>

#include <qmessagebox.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qtooltip.h> 

parametersWindow::~parametersWindow()
{
	delete mOpenModeller;
}
 
/**
* Write paramteres in register.
*/
void parametersWindow::writeParametes()
{
	QSettings settings;

	ParametersMap::Iterator myIterator;
	for ( myIterator = mMap.begin(); myIterator != mMap.end(); ++myIterator ) 
	{
		QString myWidgetName = myIterator.data()->name();
		QString myValueString = "";
		if (myWidgetName.left(2)=="le")
		{
			QLineEdit* myLineEdit = (QLineEdit*) myIterator.data();
			settings.writeEntry("/openmodeller/" +algorithmID_+ "/"+myIterator.key(),
                myLineEdit->text());
		}
		else if (myWidgetName.left(4)=="spin")
		{
			QSpinBox* mySpinBox = (QSpinBox*) myIterator.data();
			settings.writeEntry("/openmodeller/" +algorithmID_+ "/"+myIterator.key(),
                mySpinBox->value());
		}
	}
}


/**
* Read paramteres from register.
*/
void parametersWindow::readParametes( const QString algID )
{
	QSettings settings;

	//Clear any previous parameters from list
	extraParametersQStringList->clear();

	//Get the algorithm parameters and store in QStringList
	ParametersMap::Iterator myIterator;
	for ( myIterator = mMap.begin(); myIterator != mMap.end(); ++myIterator ) 
	{
		QString myWidgetName = myIterator.data()->name();
		QString myValueString = "";
		if (myWidgetName.left(2)=="le")
		{
			QLineEdit* myLineEdit = (QLineEdit*) myIterator.data();
			myValueString = myLineEdit->text();
			QString aux = settings.readEntry("/openmodeller/" +algorithmID_+ "/"+myIterator.key());
			myLineEdit->setText( aux );
		}
		else if (myWidgetName.left(4)=="spin")
		{
			QSpinBox* mySpinBox = (QSpinBox*) myIterator.data();
			myValueString = QString::number(mySpinBox->value());
			int aux = settings.readNumEntry("/openmodeller/" +algorithmID_+ "/"+myIterator.key(), -32000 );

			if( aux!=-32000 )
				mySpinBox->setValue( aux );
		}
		extraParametersQStringList->append(myIterator.key() + " " + myValueString);
	}
	accept();
}

/**
* Qt functions.
*/
void parametersWindow::okPushButton_clicked()
{
	QSettings settings;

	//Clear any previous parameters from list
	extraParametersQStringList->clear();

	//Get the algorithm parameters and store in QStringList
	ParametersMap::Iterator myIterator;
	for ( myIterator = mMap.begin(); myIterator != mMap.end(); ++myIterator ) 
	{
		QString myWidgetName = myIterator.data()->name();
		QString myValueString = "";
		if (myWidgetName.left(2)=="le")
		{
			QLineEdit* myLineEdit = (QLineEdit*) myIterator.data();
			myValueString = myLineEdit->text();
			settings.writeEntry("/openmodeller/" +algorithmID_+ "/"+myIterator.key(),
                myLineEdit->text());
		}
		else if (myWidgetName.left(4)=="spin")
		{
			QSpinBox* mySpinBox = (QSpinBox*) myIterator.data();
			myValueString = QString::number(mySpinBox->value());
			settings.writeEntry("/openmodeller/" +algorithmID_+ "/"+myIterator.key(),
                mySpinBox->value());
		}
		extraParametersQStringList->append(myIterator.key() + " " + myValueString);
	}
	accept();
}

// (from omgui).
void 
parametersWindow::defaulsParametersPushButton_clicked()
{
	QSettings settings;

    ParametersMap::Iterator myIterator;
	for ( myIterator = mMap.begin(); myIterator != mMap.end(); ++myIterator ) 
	{
		QString myWidgetName = myIterator.data()->name();
		if (myWidgetName.left(2)=="le")
		{
			QLineEdit* myLineEdit = (QLineEdit*) myIterator.data();
			myLineEdit->setText(mDefaultParametersMap[myWidgetName]);

		}
		else if (myWidgetName.left(4)=="spin")
		{
			QSpinBox* mySpinBox = (QSpinBox*) myIterator.data();
			mySpinBox->setValue(atoi(mDefaultParametersMap[myWidgetName]));
		}
	}
}

/**
* Get algorithm parameter list (from omgui).
*/
void 
parametersWindow::getParameterList( QString theAlgorithmNameQString )
{
	algorithmID_ = theAlgorithmNameQString;

	if(!mOpenModeller)
		return;

	// Find out which model algorithm is to be used.
	const AlgMetadata **myAlgorithmsMetadataArray = mOpenModeller->availableAlgorithms();
	const AlgMetadata *myAlgorithmMetadata;

	//find out how many params and clear maps
	int myRowCountInt = 0;
	while ( myAlgorithmMetadata = *myAlgorithmsMetadataArray++ )
	{ 
		myRowCountInt++;
		//delete current parameter map contents
		ParametersMap::Iterator myIterator;

		for ( myIterator = mMap.begin(); myIterator != mMap.end(); ++myIterator ) 
		{
			delete myIterator.data();
			//mMap.remove(myIterator);
		}
		mMap.clear();
		mDefaultParametersMap.clear();
		ParameterLabels::Iterator myLabelIterator;
		for ( myLabelIterator = mLabelsMap.begin(); myLabelIterator != mLabelsMap.end(); ++myLabelIterator ) 
		{
			delete myLabelIterator.data();
			//mLabelsMap.remove(myIterator);
		}
		mLabelsMap.clear();
	}
	
	if (mLayout) 
	{
		delete mLayout;
	}

	//mLayoutWidget->setGeometry();
	//GridLayout within the LayoutWidget
	mLayout = new QGridLayout(mLayoutWidget, myRowCountInt+1,3); 
	mLayout->setColSpacing(1,10);

	//reinitialise the metadataarray 
	myAlgorithmsMetadataArray = mOpenModeller->availableAlgorithms();

	//Buidling maps for widgets
	while ( myAlgorithmMetadata = *myAlgorithmsMetadataArray++ )
	{
		//Set fonts for parameter labels
		QString myFontName = "Arial [Monotype]";
		QFont myLabelFont(myFontName, 11, 75);

		// Scan openModeller algorithm list for the selected algorithm
		QString myAlgorithmNameQString=myAlgorithmMetadata->id.c_str();

		if (myAlgorithmNameQString==theAlgorithmNameQString)
		{
			int myParameterCountInt = myAlgorithmMetadata->nparam;
			AlgParamMetadata* myParameter = myAlgorithmMetadata->param;

			if (myParameterCountInt==0)
			{
				//Algorithms with NO parameters

				//Set label and button for algorithms with no parameters
				lblParameters->setText("No user definable parameters available");
				defaulsParametersPushButton->setEnabled(false);
			}
			else //Algorithms WITH parameters
			{
				//Set label and button for algorithms with parameters
				lblParameters->setText("Algorithm specific parameters");
				defaulsParametersPushButton->setEnabled(true);

				QSettings settings;

				//interate through parameters adding the correct control type
				for ( int i = 0; i < myParameterCountInt; i++, myParameter++ )
				{
					QString myParameterType;
					if(myParameter->type == 0) myParameterType = "Integer";
					if(myParameter->type == 1) myParameterType = "Real";
					if(myParameter->type == 2) myParameterType = "String";

					if (myParameterType=="Integer")
					{
						//Create a spinbox for integer values
						QString myControlName = QString("spin"+QString(myParameter->id.c_str()));
						//Create components
						QSpinBox* mySpinBox = new QSpinBox (mLayoutWidget, myControlName);
						QLabel* myLabel = new QLabel (mLayoutWidget, ("lbl"+QString(myParameter->id.c_str())));

						//set spinbox details and write to map
						if (!myParameter->has_min==0) 
						{
							mySpinBox->setMinValue((int)myParameter->min_val);
						}
						else
						{
							mySpinBox->setMinValue(INT_MIN);
						}        
						if (!myParameter->has_max==0) 
						{
							mySpinBox->setMaxValue((int)myParameter->max_val);
						}
						else
						{
							mySpinBox->setMaxValue(INT_MAX);
						}

						//Set value to previous otherwise to default
						//QString myPreviousValue = settings.readEntry("/openmodeller/"+cboModelAlgorithm->currentText()+"/"+myParameter->id.c_str());
						//if (myPreviousValue)
						//{
						//	mySpinBox->setValue(atoi(myPreviousValue));
						//}
						//else
						//{
							mySpinBox->setValue(atoi(myParameter->typical.c_str()));
						//}

						QToolTip::add(mySpinBox, myParameter->description.c_str());

						//Set label details and write to map
						myLabel->setText(myParameter->name.c_str());	   
						myLabel->setFont(myLabelFont);

						//add label and control to form
						mLayout->addWidget(myLabel, i, 0);
						mLayout->addItem(new QSpacerItem(1,1,QSizePolicy::Expanding,QSizePolicy::Expanding),i,1);
						mLayout->addWidget(mySpinBox, i, 2);
						//mLayout->setRowSpacing(i,30);

						//
						// Add the widget to the map
						//
						mMap[myParameter->id.c_str()] = mySpinBox;
						mDefaultParametersMap[myControlName]=QString(myParameter->typical.c_str());
						mLabelsMap[myParameter->name.c_str()] = myLabel;
					}
					else if (myParameterType.compare("Real") || myParameterType.compare("Double"))
					{
						//Create components
						QString myControlName = QString("le"+QString(myParameter->id.c_str()));
						QLineEdit* myLineEdit = new QLineEdit (mLayoutWidget, myControlName);
						QLabel* myLabel = new QLabel (mLayoutWidget, ("lbl"+QString(myParameter->id.c_str())));	

						//Set value to previous otherwise to default
						//QString myPreviousValue = settings.readEntry("/openmodeller/"+cboModelAlgorithm->currentText()+"/"+myParameter->id.c_str());
						//if (myPreviousValue)
						//{
						//	myLineEdit->setText(myPreviousValue);
						//}
						//else
						//{
							myLineEdit->setText(myParameter->typical.c_str());
						//}

						QToolTip::add(myLineEdit, myParameter->typical.c_str());

						//Set label details and write to map
						myLabel->setText(myParameter->name.c_str());	
						myLabel->setFont(myLabelFont);   

						//add label and control to form
						mLayout->addWidget(myLabel, i,0);
						mLayout->addItem(new QSpacerItem(1,1,QSizePolicy::Expanding,QSizePolicy::Expanding),i,1);
						mLayout->addWidget(myLineEdit,i,2);
						//mLayout->setRowSpacing(i,30);
						myLineEdit->show();

						//
						// Add the widget to the map
						//
						mMap[myParameter->id.c_str()] = myLineEdit;
						mDefaultParametersMap[myControlName]=QString(myParameter->typical.c_str());
						mLabelsMap[myParameter->name.c_str()] = myLabel;
					}

				}
				mScrollView->addChild(mLayoutWidget,0,0);	
				mScrollView->setResizePolicy(QScrollView::AutoOneFit);	
			}     
			//Exit loop because we have found the correct algorithm
			break;      
		}
	}/**/
}

/**
* Default constructor.
*/
parametersWindow::parametersWindow( QStringList* algorithmParameters ) : extraParametersQStringList(algorithmParameters)
{
	mOpenModeller= new OpenModeller();

	// (from omgui).
    mParametersScrollView = new QScrollView(parametersFrame);
	mParametersVBox = new QVBox (mParametersScrollView->viewport());
	mParametersScrollView->addChild(mParametersVBox);
	mParametersFrame = new QFrame(mParametersVBox);
	//Scroll view within the frame
	mScrollView = new QScrollView(parametersFrame);
	//mScrollView->setGeometry();   
	mScrollViewLayout = new QGridLayout(parametersFrame, 0,0);
	mScrollViewLayout->addWidget(mScrollView,0,0);
	//LayoutWidget within the scroll view
	mLayoutWidget=new QWidget();
	//temporarily make a layout
	//mLayout = new QGridLayout(mParametersFrame,1,2);
	mLayout = new QGridLayout(parametersFrame,1,2);
}
