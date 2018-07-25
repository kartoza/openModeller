#include <openModellerPluginWindow.h>

#include <TeQtViewItem.h>
#include <TeQtThemeItem.h>

#include <qmessagebox.h>
#include <qfile.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qprogressbar.h>
#include <qapplication.h>
#include <qfiledialog.h> 
#include <qsettings.h> 
#include <qstring.h>
#include <qpixmap.h> 
#include <qlistbox.h> 

#include <parametersWindow.h>
#include <progressForm.h>
#include <om.hh>

#include <fstream>


/**
* Count selected Projection layers.
*/
void 
openModellerPluginWindow::projLayersListBox_selectionChanged()
{
	int limit =  projLayersListBox->numRows();
	int i, count;

	count = 0;
	for(i=0; i<limit; ++i)
	{
		if( projLayersListBox->isSelected(i) )
			++count;
	}
	projectionLayersCountTextLabel->setText( QString("%1").arg( count ) );
}

/**
* Count selected layers.
*/
void 
openModellerPluginWindow::buildLayersListBox_selectionChanged()
{
	int limit =  buildLayersListBox->numRows();
	int i, count;

	count = 0;
	for(i=0; i<limit; ++i)
	{
		if( buildLayersListBox->isSelected(i) )
			++count;
	}
	buildingLayersCountTextLabel->setText( QString("%1").arg( count ) );
}


/**
* Default destructor.
*/       
openModellerPluginWindow::~openModellerPluginWindow()
{
	//fclose( flog );
	/*if(qOpenModeller_)
	{
		delete qOpenModeller_;
		qOpenModeller_ = 0;
	}*/
}

/**
* lastModel.
*/
void
openModellerPluginWindow::lastModelPushButton_clicked()
{
	QSettings settings;
	QString aux;
	ifstream ini("last.ini");

	if( !ini )
	{
		QMessageBox::information ( 0,
		  "Open Modeller Plugin", "The \"last.ini\" was not find." );
		return;
	}

	// Database
	string databaseTest = settings.readEntry( "/openmodeller/url_" ).latin1();
	if( databaseTest.compare( url_ ) )
	{
		QMessageBox::information ( 0,
		  "Open Modeller Plugin", "The database is not the same as last model." );
		return;
	}/**/

	// Algorithm name
	aux = settings.readEntry("/openmodeller/algorithmComboBox");
	if( !aux.isEmpty() )
		algorithmComboBox->setCurrentText( aux );
	aux = "";
	// Algorithim parameters
	qOpenModeller_->readParametes( algorithmComboBox->currentText() );

	// WKT
	aux = settings.readEntry( "/openmodeller/coordinateComboBox", coordinateComboBox->currentText() );
	if( !aux.isEmpty() )
        coordinateComboBox->setCurrentText( aux );
	aux = "";

	// Output Name
	aux = settings.readEntry( "/openmodeller/outputLayerLineEdit", outputLayerLineEdit->text() );
	if( !aux.isEmpty() )
		outputLayerLineEdit->setText( aux );
	aux = "";

	// Locality Data
	aux = settings.readEntry( "/openmodeller/localityLayerComboBox" );
	if( !aux.isEmpty() )
	{
		localityLayerComboBox->setCurrentText( aux );
		localityLayerComboBox_activated( aux );   
	}
	aux = "";
	aux = settings.readEntry( "/openmodeller/tablesComboBox" );
	if( !aux.isEmpty() )
	{
		tablesComboBox->setCurrentText( aux );
		tablesComboBox_activated( aux );
	}
	aux = "";
	aux = settings.readEntry( "/openmodeller/columnsComboBox" );
	if( !aux.isEmpty() )
	{
		columnsComboBox->setCurrentText( aux );
		columnsComboBox_activated( aux );

	}
	aux = "";
	
	//* Species
	int rows = speciesListBox->numRows();
	int i, j;
	char* linha;
	linha = new char[255];

	int num;
	ini.getline( linha, 255 );
	aux.append( linha );
	num  = aux.toInt();
	aux = "";

	speciesListBox->selectAll( false );

	for(i=0; i<num; i++)
	{
		ini.getline( linha, 255 );
		aux.append( linha );

		for(j=0; j<rows; j++)
		{			
			if( aux.compare( speciesListBox->text( j ) ) == 0 )
				speciesListBox->setSelected( j, true );
		}
		aux = "";
	}

	//Enviroment Data
	// Model Building
	rows = buildLayersListBox->numRows();
	
	ini.getline( linha, 255 );
	aux.append( linha );
	num  = aux.toInt();
	aux = "";

	buildLayersListBox->selectAll( false );

	for(i=0; i<num; i++)
	{
		ini.getline( linha, 255 );
		aux.append( linha );

		for(j=0; j<rows; j++)
		{			
			if( aux.compare( buildLayersListBox->text( j ) ) == 0 )
				buildLayersListBox->setSelected( j, true );
		}
		aux = "";
	}

	// Model Pojection
	rows = projLayersListBox->numRows();
	
	ini.getline( linha, 255 );
	aux.append( linha );
	num  = aux.toInt();
	aux = "";

	projLayersListBox->selectAll( false );

	for(i=0; i<num; i++)
	{
		ini.getline( linha, 255 );
		aux.append( linha );

		for(j=0; j<rows; j++)
		{			
			if( aux.compare( projLayersListBox->text( j ) ) == 0 )
				projLayersListBox->setSelected( j, true );
		}
		aux = "";
	}

	// Mask and Format
	aux = settings.readEntry( "/openmodeller/inputMaskComboBox" );
	if( !aux.isEmpty() )
        inputMaskComboBox->setCurrentText( aux );
	aux = "";
	aux = settings.readEntry( "/openmodeller/outputMaskComboBox" );
	if( !aux.isEmpty() )
		outputMaskComboBox->setCurrentText( aux );
	aux = "";
	aux = settings.readEntry( "/openmodeller/outputFormatComboBox" );
	if( !aux.isEmpty() )
		outputFormatComboBox->setCurrentText( aux );
	aux = "";

	delete linha;
	/**/

	// Fecha o arquivo.
	ini.close();
}


/**
* sameLayers.
*/
void
openModellerPluginWindow::sameLayersPushButton_clicked()
{
	int i, end;

	end = buildLayersListBox->numRows();
	
	for(i=0; i<end;++i)
	{
		if( buildLayersListBox->isSelected(i) )
			projLayersListBox->setSelected ( i, true );
		else
			projLayersListBox->setSelected ( i, false );
	}
}

/**
* Make the model and projection.
*/
void
openModellerPluginWindow::finishPushButton_clicked()
{
	int i, total;

	if( !check() )
		return;

	// Occurrences. /////////
	total = speciesListBox->numRows();
	qOpenModeller_->speciesNames_.clear();
	for(i=0; i<total; i++)
	{
		if( speciesListBox->isSelected(i) )
			qOpenModeller_->speciesNames_.push_back( speciesListBox->text(i).latin1() );
	}
	Log::instance()->debug( "finishPushButton_clicked: Ocurrences.\n" );
	// set species URL.
	qOpenModeller_->speciesUrl_ = url_ + 
			localityLayerComboBox->currentText().latin1() + ">" +
			tablesComboBox->currentText().latin1() + ">" +
			columnsComboBox->currentText().latin1();
	Log::instance()->debug( "finishPushButton_clicked: set species URL.\n" );
	// set WTK projection.
	qOpenModeller_->projection_ = coordinateComboBox->currentText();
	Log::instance()->debug( "finishPushButton_clicked: set WTK projection.\n" );

	// Environmental Maps /////////
	qOpenModeller_->inputMask_ = url_ + inputMaskComboBox->currentText().latin1();
	qOpenModeller_->continuousMaps_.clear();
	total = buildLayersListBox->numRows();
	for(i=0; i<total; i++)
	{
		if( buildLayersListBox->isSelected(i) )
			qOpenModeller_->continuousMaps_.push_back( url_ + buildLayersListBox->text(i).latin1() );
	}
	Log::instance()->debug( "finishPushButton_clicked: Environmental Maps.\n" );

	// Projection Maps ///////
	qOpenModeller_->projectionLayer_ = url_ + outputLayerLineEdit->text().latin1();
	qOpenModeller_->outputMask_ = url_ + outputMaskComboBox->currentText().latin1();
	qOpenModeller_->outputFormat_ = url_ + outputFormatComboBox->currentText().latin1();
	total = projLayersListBox->numRows();
	qOpenModeller_->projectionContinuousMaps_.clear();
	for(i=0; i<total; i++)
	{
		if( projLayersListBox->isSelected(i) )
			qOpenModeller_->projectionContinuousMaps_.push_back( url_ + projLayersListBox->text(i).latin1() );
	}
	Log::instance()->debug( "finishPushButton_clicked: Projection Maps.\n" );

	// Run the model and projection.
	qOpenModeller_->run();

	// Log in file again.
	Log::instance()->set(Log::Debug, flog, "");

	Log::instance()->debug( "openModellerPluginWindow::finishPushButton_clicked: end run.\n" );

	// Uptade TerraView.
	plugin_params_.updateTVInterface( plugin_params_ );
	accept();
}

/**
* Load data from TerraView and start a new model.
* Return false if there isn't data or data selected.
*/
bool 
openModellerPluginWindow::loadData()
{
	// Check if TerraView is connected to a database.
	if( plugin_params_.current_database_ptr_ == 0 )
		return false;

	// Get last output dir.
	QSettings settings;
	directoryLineEdit->setText( settings.readEntry( "/openmodeller/outputDirectoryName_" )  );
	qOpenModeller_->outputDirectoryName_ = directoryLineEdit->text();

	// Clear all plugin combos and lists boxes.
	clearAll();

	// Check layers
	// Creating a local database connection - Windows DLLs workaround 
	TeDatabaseFactoryParams dbf_params;
	
	dbf_params.database_ = plugin_params_.current_database_ptr_->databaseName();
	dbf_params.dbms_name_ = plugin_params_.current_database_ptr_->dbmsName();
	
	dbf_params.host_ = plugin_params_.current_database_ptr_->host();
	// change empty host to localhost for MySQL
	if( dbf_params.dbms_name_ == "MySQL" && dbf_params.host_.empty() )
		dbf_params.host_ = "localhost";
	
	dbf_params.password_ = plugin_params_.current_database_ptr_->password();
	dbf_params.port_ = plugin_params_.current_database_ptr_->portNumber();
	dbf_params.user_ = plugin_params_.current_database_ptr_->user();
		
	db_.reset(TeDatabaseFactory::make( dbf_params ));
	
	if( ! db_->isConnected() )
	{
		QMessageBox::critical( this, "TerraView", 
		tr( "Unable to connect to database" ) );    
		return false;     
	} 

	bool flag_load = false;

	/* If exists a View selected.
	if( plugin_params_.teqtviewslistview_ptr_ )
	{
		// Load View selected.
		TeQtViewItem* current_view_item = 
			plugin_params_.teqtviewslistview_ptr_->currentViewItem();

		// If exists items selected (themes).
		if( current_view_item )
		{
			flag_load = true;

			std::vector< QListViewItem* > view_items =
				current_view_item->getChildren();

			std::vector< QListViewItem* >::iterator it = view_items.begin();
			std::vector< QListViewItem* >::iterator it_end = view_items.end();

			// Check items selected (themes).
			for(; it != it_end; ++it )
			{
				std::string theme_name( (*it)->text( 0 ).ascii() );
				TeTheme* selected_theme = 
					plugin_params_.current_view_ptr_->get( theme_name );
				TeQtThemeItem* selected_theme_item = 
					plugin_params_.teqtviewslistview_ptr_->getThemeItem( selected_theme);
		            
				// If theme is active.
				if( selected_theme_item->isOn() )
				{
					TeLayer* selected_layer = selected_theme->layer();

					// If layer is a TeRaster.
					if( selected_layer->hasGeometry( TeRASTER ))
					{
						buildLayersListBox->insertItem( selected_layer->name().c_str() );
						projLayersListBox->insertItem( selected_layer->name().c_str() );
						inputMaskComboBox->insertItem( selected_layer->name().c_str() );
						outputMaskComboBox->insertItem( selected_layer->name().c_str() );
						outputFormatComboBox->insertItem( selected_layer->name().c_str() );
					}

					// If layer is a Table Points.
					if( selected_layer->hasGeometry( TePOINTS ))
					{
						localityLayerComboBox->insertItem( selected_layer->name().c_str() );
					}
				}
			}
		}
	}/**/
	
	if( !flag_load )
	{
		// Check all layers in database.
		if(db_->layerMap().size() == 0)
			db_->loadLayerSet();
		
		TeLayerMap& layerMap = db_->layerMap();
		TeLayerMap::iterator it = layerMap.begin();
		TeLayerMap::iterator it_end = layerMap.end();
		for(; it != it_end; ++it )
		{
			TeLayer* selected_layer = (*it).second;

			// If layer is a TeRaster.
			if( selected_layer->hasGeometry( TeRASTER ))
			{
				buildLayersListBox->insertItem( selected_layer->name().c_str() );
				projLayersListBox->insertItem( selected_layer->name().c_str() );
				inputMaskComboBox->insertItem( selected_layer->name().c_str() );
				outputMaskComboBox->insertItem( selected_layer->name().c_str() );
				outputFormatComboBox->insertItem( selected_layer->name().c_str() );
				//Alexandre
				Log::instance()->debug( "Enviroment Layers = %s\n", selected_layer->name().c_str() );
			}

			// If layer is a Table Points.
			if( selected_layer->hasGeometry( TePOINTS ))
			{
				localityLayerComboBox->insertItem( selected_layer->name().c_str() );
				//Alexandre
				Log::instance()->debug( "localityLayerComboBox = %s\n", selected_layer->name().c_str() );
			}
		}
	}

	// Set the url begin.
	// terralib>user>password\@DatabaseName>host>DatabaseFile>port>layerName>tableName>columnName
	char port[6];
	itoa(dbf_params.port_, port, 10);
	url_ = "terralib>" +
		dbf_params.user_ + ">" +
		dbf_params.password_ + "@" +
		dbf_params.dbms_name_ + ">" +
		dbf_params.host_ + ">" +
		dbf_params.database_  + ">" +
		port + ">";

	//Alexandre
	// Set first table if its exists.
	if( localityLayerComboBox->currentText().length() != 0 )
		localityLayerComboBox_activated( localityLayerComboBox->currentText() );

	// Select all layers in listBoxs.
	buildLayersListBox->clearSelection();
	buildLayersListBox->invertSelection();
	projLayersListBox->clearSelection();
	projLayersListBox->invertSelection();

	return true;
}

/**
* Change the model algotithm.
*/
void openModellerPluginWindow::algorithmComboBox_activated( const QString &theModelAlgorithm )
{
	qOpenModeller_->changeAlgorithm( theModelAlgorithm );
}

/**
* Open parametersWindows.
*/
void 
openModellerPluginWindow::setParametersPushButton_clicked()
{
	qOpenModeller_->setAlgorithmParameters();
}

/**
* Update the tableComboBox.
*/
void openModellerPluginWindow::localityLayerComboBox_activated( const QString &localityLayer )
{
	int i;

	tablesComboBox->clear();
		
	if( db_->layerExist( localityLayer.latin1() ) )
	{
		TeLayer* layer = new TeLayer( localityLayer.latin1(), db_.nakedPointer() );

		TeAttrTableVector attr;
		layer->getAttrTables( attr );

		int tables_number = attr.size();
		for(i=0; i<tables_number; ++i)
		{
			tablesComboBox->insertItem( attr[i].name().c_str() );
			//Alexandre
			Log::instance()->debug( "tablesComboBox[%i] = %s\n", i , attr[i].name().c_str() );

		}		
	}

	// Update columnComboBox.
	// Set first column if its exists.
	if( tablesComboBox->currentText().length() != 0 )
		tablesComboBox_activated( tablesComboBox->currentText().latin1() );
}

/**
* Update columnComboBox.
*/
void openModellerPluginWindow::tablesComboBox_activated( const QString & )
{
	int i;
	columnsComboBox->clear();
	
	string layerName = localityLayerComboBox->currentText().latin1();
	if( db_->layerExist( layerName ) )
	{
		TeLayer* layer = new TeLayer( layerName, db_.nakedPointer() );

		TeAttrTableVector attr;
		layer->getAttrTables( attr );

		int i_table = tablesComboBox->currentItem();
		TeAttributeList columns = attr[i_table].attributeList();

		int columns_number = columns.size();
		for(i=0; i<columns_number; ++i)
		{
			if(columns[i].rep_.type_ == TeSTRING)
				if( columns[i].rep_.name_.find("object_id") == -1 )
				{
					columnsComboBox->insertItem( columns[i].rep_.name_.c_str() );
					//Alexandre
					Log::instance()->debug( "columnsComboBox = %s\n", columns[i].rep_.name_.c_str() );
				}
		}
	}

	// Update locatity specisListBox.
	// Set first column if its exists.
	if( columnsComboBox->currentText().length() != 0 )
		columnsComboBox_activated( columnsComboBox->currentText().latin1() );
}

/**
* Update locatity speciesListBox.
*/
void openModellerPluginWindow::columnsComboBox_activated( const QString &column )
{
	speciesListBox->clear();
	
	/*if( column.length() == 0 )
	{
        //QMessageBox::information ( 0,
		//  "Open Modeller Plugin", "This table dont have a valid column.\nPlease select another layer. "+localityLayerComboBox->currentItem() );
		localityLayerComboBox->removeItem( localityLayerComboBox->currentItem() );
		localityLayerComboBox->setCurrentItem( localityLayerComboBox->currentItem() + 1 );		
		// Set first table if its exists.
		if( localityLayerComboBox->currentText().length() != 0 )
			localityLayerComboBox_activated( localityLayerComboBox->currentText() );
		return;
	}*/

	string table = tablesComboBox->currentText().latin1();
	string sql = "SELECT DISTINCT " + table + "." + column.latin1() + " FROM " + table + ";";
	
	TeDatabasePortal* portal = db_->getPortal();
	
	if (portal && portal->query(sql))
	{
		while (portal->fetchRow())
		{
			string sp = portal->getData(0);
			speciesListBox->insertItem( sp.c_str() );
			//Alexandre
			Log::instance()->debug( "speciesListBox = %s\n", sp.c_str() );
		}
	}

	delete portal;

    // select all species.
	speciesListBox->clearSelection();
	speciesListBox->invertSelection();

}

/**
* outputMaskComboBox and outputFormatComboBox changes when inputMaskComboBox change.
*/
void openModellerPluginWindow::inputMaskComboBox_activated( const QString &layer )
{
	outputMaskComboBox->setCurrentText( layer );
	outputFormatComboBox->setCurrentText( layer );
}

/**
* Change output dir.
*/
void openModellerPluginWindow::directoryToolButton_clicked()
{
	QSettings settings;

	qOpenModeller_->outputDirectoryName_ = QFileDialog::getExistingDirectory(
        settings.readEntry( "/openmodeller/outputDirectoryName_" ), //initial dir
        this,
        "get existing directory",
        "Choose a directory",
        TRUE );

	directoryLineEdit->setText( qOpenModeller_->outputDirectoryName_ );
	settings.writeEntry( "/openmodeller/outputDirectoryName_" ,
		qOpenModeller_->outputDirectoryName_ );
}

/**
* Clear all combos and lists boxes.
*/
void openModellerPluginWindow::clearAll()
{
	buildLayersListBox->clear();
	projLayersListBox->clear();
	inputMaskComboBox->clear();
	outputMaskComboBox->clear();
	outputFormatComboBox->clear();
	localityLayerComboBox->clear();
}

/**
* Default constructor.
*/      
openModellerPluginWindow::openModellerPluginWindow( QWidget* parent ) :
	qOpenModeller_( new QOpenModeller( parent ) )//,
    //flog( NULL )
{
/*#ifdef BUILD_TERRALIB
  USE_TERRALIB_IO   Alexandre
#endif*/

	Log::instance()->setLevel(Log::Debug);
	Log::instance()->setPrefix("");
	flog = "libopenmodellerLOG.txt";
	Log::instance()->set(Log::Debug, flog, "");

	// Get algorithm list.
	qOpenModeller_->getAlgorithmList( algorithmComboBox );

	// Get projections list.
	qOpenModeller_->getProjList( coordinateComboBox );
}

/**
* Check inputs. 
*/
bool
openModellerPluginWindow::check()
{
	// Output layer
	if( outputLayerLineEdit->text().isEmpty() )
	{
		QMessageBox::information ( this, "Output", tr( "Set output layer name." ) );
		return false;
	}

	// Build Layers
	if( buildLayersListBox->count() == 0 )
	{
		QMessageBox::information ( this, "Model Building", tr( "Select at least one build layer." ) );
		return false;
	}

	// Projection Layers
	if( projLayersListBox->count() == 0 )
	{
		QMessageBox::information ( this, "Model Building", tr( "Select at least one projection layer." ) );
		return false;
	}
	
	// Locality Layer
	if( localityLayerComboBox->currentText().isEmpty() )
	{
		QMessageBox::information ( this, "Locality Data", tr( "Select a locality layer." ) );
		return false;
	}

	// Locality table
	if( tablesComboBox->currentText().isEmpty() )
	{
		QMessageBox::information( this, "Locality Data", tr( "Select a table." ) );
		return false;
	}

	// Locality column
	if( columnsComboBox->currentText().isEmpty() )
	{
		QMessageBox::information( this, "Locality Data", tr( "Select a column." ) );
		return false;
	}

	// Locality Specie
	if( speciesListBox->count() == 0 )
	{
		QMessageBox::information( this, "Locality Data", tr( "Select a specie." ) );
		return false;
	}

	/*********************************************************************************/
	ofstream ini("last.ini");
	/* Save last parameters */
	QSettings settings;
	settings.writeEntry( "/openmodeller/url_" ,	url_.c_str() );

	// Algorithim
	settings.writeEntry( "/openmodeller/algorithmComboBox" , algorithmComboBox->currentText() );
	// Algorithim Parameters
	qOpenModeller_->writeParametes();

	// WKT
	settings.writeEntry( "/openmodeller/coordinateComboBox", coordinateComboBox->currentText() );

	// Output Name
	settings.writeEntry( "/openmodeller/outputLayerLineEdit", outputLayerLineEdit->text() );

	// Locality Data
	settings.writeEntry( "/openmodeller/localityLayerComboBox", localityLayerComboBox->currentText() );
	settings.writeEntry( "/openmodeller/tablesComboBox", tablesComboBox->currentText() );
	settings.writeEntry( "/openmodeller/columnsComboBox", columnsComboBox->currentText() );

	int rows = speciesListBox->numRows();
	int i,j;

	j=0;
	for(i=0; i<rows; i++)
	{
		if( speciesListBox->isSelected( i ) )
			j++;
	}

	ini << j << endl;

	for(i=0; i<rows; i++)
	{
		if( speciesListBox->isSelected( i ) )
			ini << speciesListBox->text( i ) << endl;
	}

	//Enviroment Data

	// Model Building
	rows = buildLayersListBox->numRows();

	j=0;
	for(i=0; i<rows; i++)
	{
		if( buildLayersListBox->isSelected( i ) )
			j++;
	}

	ini << j << endl;

	for(i=0; i<rows; i++)
	{
		if( buildLayersListBox->isSelected( i ) )
			ini << buildLayersListBox->text( i ) << endl;
	}


	// Model Pojection
	rows = projLayersListBox->numRows();

	j=0;
	for(i=0; i<rows; i++)
	{
		if( projLayersListBox->isSelected( i ) )
			j++;
	}

	ini << j << endl;

	for(i=0; i<rows; i++)
	{
		if( projLayersListBox->isSelected( i ) )
			ini << projLayersListBox->text( i ) << endl;
	}

	// Mask and Format
	settings.writeEntry( "/openmodeller/inputMaskComboBox", inputMaskComboBox->currentText() );
	settings.writeEntry( "/openmodeller/outputMaskComboBox", outputMaskComboBox->currentText() );
	settings.writeEntry( "/openmodeller/outputFormatComboBox", outputFormatComboBox->currentText() );
	

	return true;
}


// Clear Buttons
void 
openModellerPluginWindow::clearLocalityPushButton_clicked()
{
	speciesListBox->selectAll( false );
}
void 
openModellerPluginWindow::clearBuildLayersPushButton_clicked()
{
	buildLayersListBox->selectAll( false );	
}
void 
openModellerPluginWindow::clearProjectionLayersPushButton_clicked()
{
	projLayersListBox->selectAll( false );
}
