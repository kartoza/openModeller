#ifndef OPENMODELLERPLUGINWINDOW_H
#define OPENMODELLERPLUGINWINDOW_H

#include <openModellerForm.h>

#include <PluginParameters.h>
#include <QOpenModeller.h>

#include <om.hh>
#include <MapFormat.hh>

#include <qcombobox.h>
#include <qstringlist.h>
#include <qmessagebox.h>

#include <TeDatabaseFactory.h>
#include <TeSharedPtr.h>

class openModellerPluginWindow : public openModellerForm
{
Q_OBJECT

public:
    /**
	* Default constructor.
	*/      
	openModellerPluginWindow( QWidget* parent );

	/**
	* Default destructor.
	*/       
	~openModellerPluginWindow();
	
	/**
    * The current plugin parameters.
    */        
    PluginParameters plugin_params_;
    
	/**
	* Load data from TerraView and start a new model.
	* Return false if there isn't data or data selected.
	*/
	bool loadData();

	//X	QOpenModeller* qOpenModeller_;
	TeSharedPtr< QOpenModeller > qOpenModeller_;

protected:
	/**
	* Check inputs. 
	*/
	bool check();
	/**
	* Clear all combos and lists boxes.
	*/
	void clearAll();

	/**
	* Change the model algotithm.
	*/
	void algorithmComboBox_activated( const QString &theModelAlgorithm );
	/**
	* Open parametersWindows.
	*/
	void setParametersPushButton_clicked();
	
	/**
	* Update the locatity tableComboBox.
	*/
	void localityLayerComboBox_activated( const QString &localityLayer );
	/**
	* Update locatity columnComboBox.
	*/
	void tablesComboBox_activated( const QString &table );
	/**
	* Update locatity speciesListBox.
	*/
	void columnsComboBox_activated( const QString &column );
	/**
	* outputMaskComboBox and outputFormatComboBox changes when inputMaskComboBox change.
	*/
	void inputMaskComboBox_activated( const QString &layer );
	/**
	* Change output dir.
	*/
	void directoryToolButton_clicked();
	/**
	* sameLayers.
	*/
	void sameLayersPushButton_clicked();
	/**
	* lastModel.
	*/
	void lastModelPushButton_clicked();
	/**
	* Count selected layers.
	*/
	void buildLayersListBox_selectionChanged();
	/**
	* Count selected Projection layers.
	*/
	void projLayersListBox_selectionChanged();

	/**
	* Make the model and projection.
	*/
	void finishPushButton_clicked();

	void clearLocalityPushButton_clicked();
    void clearBuildLayersPushButton_clicked();
    void clearProjectionLayersPushButton_clicked();

	/**
	* Members.
	*/
	TeSharedPtr< TeDatabase > db_;

	// Url begin.
	string url_;

	//OM LOG
	std::string flog;
};

#endif
