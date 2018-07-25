#ifndef QOPENMODELLER_H
#define QOPENMODELLER_H

#include <TeSharedPtr.h>

#include <om.hh>
#include <MapFormat.hh>

#include <qlogcallback.h>

using namespace std;

#include <qcombobox.h>
#include <qstringlist.h>

#include <parametersWindow.h>

class QOpenModeller : public QObject
{
	Q_OBJECT
public:
	/**
	* Default constructor.
	*/
	QOpenModeller( QWidget* parent = 0 );

	~QOpenModeller();

	/**
	* Run complet openModeller (Make modell and projection).
	*/
	void run();

	/**
	* Get avaliable algorithms.
	*/
	void getAlgorithmList( QComboBox* algorithmComboBox );
	/**
	* Change the model algotithm.
	*/
	void changeAlgorithm( const QString &algorithmComboText );
	/**
	* Open set the algorthm parameters window.
	*/
	void setAlgorithmParameters();
	/**
	* Get Projections list.
	*/
	void getProjList( QComboBox* projectionComboBox );

	/**
	* Read paramteres from register.
	*/
	void readParametes( const QString algID );
	/**
	* Write paramteres in register.
	*/
	void writeParametes(  );

    //! Log
	QLogCallback* logCallback_;
	QString outputDirectoryName_;
	
	//! Species vector
	vector<string> speciesNames_;
	string speciesUrl_;

	//! WTK Projection
	QString projection_;

	//! Input Maps
	vector<string> categoricalMaps_;
	vector<string> continuousMaps_;
	string inputMask_;

	//! Projection Maps
	vector<string> projectionContinuousMaps_;
	vector<string> projectionCategoricalMaps_;
	//! Output layer.
	string projectionLayer_;
	string outputMask_;
	string outputFormat_;

	//! Algorithm name.
	QString algorithmID_;
	QString lastAlgorithmID_;
	//! Algorithm parameters.
	QStringList algorithmParameters_;

private:
	//! My openModeller.
	OpenModeller* openModeller_;
	OpenModeller* openModeller2_;

	/**
	* Extract the parameter value.
	*/
	string extractParameter( string const name, vector<string> vet );
	/**
	* openModeller progress communication.
	*/
	static void progressBarCallback( float progress, void *extra_param );
	/**
	* Get occurrences.
	*/
	void getOccurrences( const string& oc_name );
	/**
	* Set projction maps.
	*/
	void setProjection();
	/**
	* Set setAlgorithm and parameters.
	*/
	void setAlgorithm();
	/**
	* Make Configure Report.
	*/
	void makeCofigureReport();
	/**
	* Make Configure File.
	*/
	void makeConfigFile( const string& specieName );

	//! Log
	QString logFileName_;
	QString configureFileName_;

	//! XML
	string xmlFileName_;
    
	//! Parent window.
	QWidget* parent_;

	//! Projections Map.
	typedef QMap<QString,QString> ProjectionWKTMap; //wkt = well known text (see gdal/ogr)
	ProjectionWKTMap projectionsMap_;
	
	//! Output map format.
	MapFormat mapFormat_;

	parametersWindow* parametersWindow_instance;

};

#endif
