// om_JNIdll.cpp : Defines the exported functions for the DLL application.
//
#include <pngwriter.h>
#include "omjni.h"
#include <openmodeller/om.hh>
#include <openmodeller/Configuration.hh>
#include <openmodeller/os_specific.hh>


#include "request_file.hh"
#include "file_parser.hh"

#include <istream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <stdexcept>

extern "C" __declspec(dllexport) int RunAlgorithm(char* request_file);

//JNI function calls org.ioz.openModeller.web.util_ExecuteAlgorithm_RunAlgorithm
//JNIEXPORT int JNICALL Java_org_ioz_openModeller_web_util_ExecuteAlgorithm_RunAlgorithm
JNIEXPORT int JNICALL Java_org_ioz_openModeller_web_util_ExecuteAlgorithm_RunAlgorithm
(JNIEnv *env, jobject obj, jstring s1)
{
	int errorcode=0;
	try{
		Log::instance()->info("Calling PresampleLayers in CPP Code.\n");
		jboolean iscopy;
		char * request_file = (char*) env->GetStringUTFChars(s1, &iscopy);
		Log::instance()->info("calling presample layers now\n");
		errorcode = RunAlgorithm(request_file);
		Log::instance()->info("DONE Calling PresampleLayers in CPP Code.\n");
	}
	catch (...)
	{
		Log::instance()->info( "Unknown error occurred\n" );
	}
	return errorcode;
}

JNIEXPORT int JNICALL Java_org_ioz_openModeller_invoke_RunAlgorithm
(JNIEnv *env, jobject obj, jstring s1)
{
	return Java_org_ioz_openModeller_web_util_ExecuteAlgorithm_RunAlgorithm(env, obj, s1);
}


int showAlgorithms ( AlgMetadata const **availables );
AlgMetadata const *readAlgorithm( AlgMetadata const **availables );
int readParameters( AlgParameter *result, AlgMetadata const *metadata );
char *extractParameter( char *name, int nvet, char **vet );
int dealoutput(std::string _projectionFile,MapFormat _outputFormat);
int getPNG(std::string _projectionFile,MapFormat _outputFormat);
int getPseudoColorPNG(std::string _projectionFile);

void mapCallback( float progress, void *extra_param );
void modelCallback( float progress, void * extra_param );
//GET FILE NAME FROM PATH	
char * getfilename(char * pfilename)
{
	return pfilename;
	char* Period = strrchr(pfilename, '.');
	// It will copy to the first null char.
	char cFileName[100] = "";
	strncpy( cFileName, pfilename, Period - pfilename );
	char* a=cFileName;
	return a;
}

int getPNG(std::string _projectionFile,MapFormat _outputFormat)
 {
	Log::instance()->info("=========getPNG==========\n");
	GDALDriver *poDriver;
	GDALAllRegister();
	char * type="";
	switch( _outputFormat.getFormat() )
	{
		case MapFormat::GreyTiff:
			type="GTiff";
			break;
		default:
			break;
	}
	poDriver = GetGDALDriverManager()->GetDriverByName(type);
	Log::instance()->info("=========get GDAL Device ==========\n");
    if( poDriver == NULL )
        return false;
	Log::instance()->info("request_file:%s\n",_projectionFile.c_str());

	GDALDataset *poSrcDS = (GDALDataset *) GDALOpen( _projectionFile.c_str(), GA_ReadOnly );
	Log::instance()->info("=========open Tiff File ==========\n");
    GDALDataset *poDstDS;
	poDriver = GetGDALDriverManager()->GetDriverByName("PNG");
	char **papszOptions = NULL;
	char * png =getfilename((char*)_projectionFile.c_str());
	strcat(png , ".png");
	Log::instance()->info("=========Prepare to create the png file ==========\n");
    poDstDS = poDriver->CreateCopy( png, poSrcDS, FALSE, NULL, NULL, NULL );
	Log::instance()->info("=========Finished to create the png file ==========\n");
    /* Done, close properly the dataset */
    if( poDstDS != NULL )
        GDALClose( (GDALDatasetH) poDstDS );
    GDALClose( (GDALDatasetH) poSrcDS );
	return true;
 }
int getPseudoColorPNG(std::string _projectionFile)
{
	char * sourcefile =getfilename((char*)_projectionFile.c_str());
	char sourcepng [100];
	strcpy(sourcepng , sourcefile);
	strcat(sourcepng , ".png");

	char colorpng  [100];
	strcpy(colorpng , sourcefile);
	strcat(colorpng , "_color.png");

	char colorsmallpng [100];
	strcpy(colorsmallpng , sourcefile);
	strcat(colorsmallpng , "_small.png");

	pngwriter writer(1,1,0,colorpng); 
	writer.readfromfile(sourcepng);
	Log::instance()->info("=========Read the PNG info==========\n");
	int width = writer.getwidth();
	int height = writer.getheight();

	for (int i=0;i<width;i++)
	{
		for (int j=0;j<height;j++)
		{
			int col=writer.read(i,j);
			if (col!=255)
			{
				writer.plot(i,j,col*256,(256-col)*256,255*256);
			}
			else
			{
				writer.plot(i,j,col*256,col*256,col*256);
			}
		}
	}
	writer.close();
	Log::instance()->info("=========Create pseudo color ==========\n");
	pngwriter writer_small(1,1,0,colorsmallpng); 
	Log::instance()->info("=========Create abbr. PNG ==========\n");
	writer_small.readfromfile(colorpng);
	writer_small.scale_k(300.0/width);
	writer_small.close();
	Log::instance()->info("=========Create abbr. PNG done ==========\n");
	return true;
}
int 
dealoutput(std::string _projectionFile,MapFormat _outputFormat)
{
	if (getPNG(_projectionFile, _outputFormat))
	{
		return getPseudoColorPNG(_projectionFile);
	}
	else
	{
		return false;
	}
}
int RunAlgorithm(char *request_file)
{
	Log::instance()->info("request_file:%s\n",request_file);
	int returnvalue=0;
	try {
		setupExternalResources();
		AlgorithmFactory::searchDefaultDirs();
		OpenModeller om;
		Log::instance()->info( "openModeller version %s\n", om.getVersion().c_str() );
		// Configure the OpenModeller object from data read from the
		// request file.
		RequestFile request;
		int resp = request.configure( &om, request_file );
		if ( resp < 0 ) {
			Log::instance()->error( "Could not read request file %s\n", request_file );
			returnvalue = 1;
		}
		// If something was not set...
		if ( resp ) {
			if ( ! request.occurrencesSet() ) {
				returnvalue = 2;
				exit(2);
			}
			if ( ! request.algorithmSet() ) {
				returnvalue = 3;
				exit(3);
			}
		}
		// Run model
		om.setModelCallback( modelCallback );
		request.makeModel( &om );
		if ( request.requestedProjection() ) {
			// Run projection
			om.setMapCallback( mapCallback );
			try {
				request.makeProjection( &om );
			}
			catch ( ... ) {}
		}
		else {
			Log::instance()->warn( "Skipping projection\n" );
		}
		// Instantiate objects for model statistics
		const ConfusionMatrix * const matrix = om.getConfusionMatrix();
		RocCurve * const roc_curve = om.getRocCurve();
		// Confusion Matrix
		Log::instance()->info( "\nModel statistics for training data\n" );
		Log::instance()->info( "Accuracy:          %7.2f%%\n", matrix->getAccuracy() * 100 );
		int omissions = matrix->getValue(0.0, 1.0);
		int total     = omissions + matrix->getValue(1.0, 1.0);
		Log::instance()->info( "Omission error:    %7.2f%% (%d/%d)\n", matrix->getOmissionError() * 100, omissions, total );
		double commissionError = matrix->getCommissionError();
		if ( commissionError >= 0.0 ) {
			int commissions = matrix->getValue(1.0, 0.0);
			total           = commissions + matrix->getValue(0.0, 0.0);
			Log::instance()->info( "Commission error:  %7.2f%% (%d/%d)\n", commissionError * 100, commissions, total );
		}
		// ROC curve
		Log::instance()->info( "AUC:               %7.2f\n", roc_curve->getTotalArea() );
		// Projection statistics
		if ( request.requestedProjection() ) {
			AreaStats * stats = om.getActualAreaStats();
			Log::instance()->info( "Percentage of cells predicted present: %7.2f%%\n", 
			stats->getAreaPredictedPresent() / (double) stats->getTotalArea() * 100 );
			Log::instance()->info( "Total number of cells: %d\n", stats->getTotalArea() );
			Log::instance()->info( "Done.\n" );
			delete stats;
		}
		if (!dealoutput(request.getprojectionFile(),request.getmapFormat()))
		{
			returnvalue = 4;
		}
		else
		{
			Log::instance()->info( "Create PNG. Done.\n" );
			returnvalue = 9;
			Log::instance()->info( "All Finished, Clear the var.\n" );
		}
	}
	catch ( std::exception& e ) {
		Log::instance()->info( "Exception occurred: %s", e.what() );
	}
	catch ( ... ) {
		Log::instance()->info( "Unknown error occurred\n" );
	}
	return returnvalue;
}
/***********************/
/*** show algorithms ***/
//
// Print available algorithms.
// Returns the option number associated with 'Quit' that is
// equal to the number of algorithms.
//
int
showAlgorithms( AlgMetadata const **availables )
{
	if ( ! *availables )
	{
		printf( "Could not find any algorithms.\n" );
		return 0;
	}
	printf( "\nChoose an algorithm between:\n" );
	int count = 1;
	AlgMetadata const *metadata;
	while ( ( metadata = *availables++ ) )
	{
		printf( " [%d] %s\n", count++, metadata->name.c_str() );
	}
	printf( " [q] Quit\n" );
	printf( "\n" );
	return count;
}


/**********************/
/*** read algorithm ***/
//
// Let the user choose an algorithm and enter its parameters.
// Returns the choosed algorithm's metadata.
//
AlgMetadata const *
readAlgorithm( AlgMetadata const **availables )
{
	char buf[128];
	while ( 1 ) {
		int quit_option = showAlgorithms( availables );
		if ( ! quit_option ) {
			return 0;
		}
		int option = -1;
		printf( "\nOption: " );
		fgets( buf, 128, stdin );
		int first_char_ascii = (int)buf[0];
		// Quit if input is "q" or "Q"
		if ( first_char_ascii == 113 || first_char_ascii == 81 ) {
			return 0;
		}
		option = atoi( buf );
		if ( option <= 0 || option >= quit_option ) {
			return 0;
		}
		// An algorithm was choosed.
		else {
			return availables[option-1];
		}
	}
}


/***********************/
/*** read Parameters ***/
int
readParameters( AlgParameter *result, AlgMetadata const *metadata )
{
	AlgParamMetadata *param = metadata->param;
	AlgParamMetadata *end   = param + metadata->nparam;
	// Read from stdin each algorithm parameter.
	for ( ; param < end; param++, result++ )
	{
		// The resulting ID is equal the ID set in algorithm's
		// metadata.
		result->setId( param->id );
		// Informs the parameter's metadata to the user.
		printf( "\n* Parameter: %s\n\n", param->name.c_str() );
		printf( " %s\n", param->overview.c_str() );
		if ( param->type != String ) {
			if ( param->has_min ) {
				if ( param->type == Integer ) {
					printf( "%s >= %d\n", param->name.c_str(), int( param->min_val ) );
				}
				else {
					printf( " %s >= %f\n", param->name.c_str(), param->min_val );
				}
			}
			if ( param->has_max ) {
				if ( param->type == Integer ) {
					printf( "%s <= %d\n\n", param->name.c_str(), int( param->max_val ) );
				}
				else {
					printf( " %s <= %f\n\n", param->name.c_str(), param->max_val );
				}
			}
		}
		printf( "Enter with value [%s]: ", param->typical.c_str() );
		// Read parameter's value or use the "typical" value
		// if the user does not enter a new value.
		char value[64];
		*value = 0;
		if ( fgets( value, 64, stdin ) && ( *value >= ' ' ) ) {
			// Remove line feed to avoid problems with string parameters 
			if ( param->type == String ) {
				char * pos = strchr( value, '\n' );
				if ( pos ) {
					*pos = '\0';
				}
			}
			result->setValue( value );
		}
		else {
			result->setValue( param->typical );
		}
	}
	return metadata->nparam;
}


/*************************/
/*** extract Parameter ***/
/**
 * Search for 'id' in the 'nvet' elements of the vector 'vet'.
 * If the string 'id' is in the begining of some string vet[i]
 * then returns a pointer to the next character of vet[i],
 * otherwise returns 0.
 */
char *
extractParameter( char *id, int nvet, char **vet )
{
	int length = strlen( id );
	char **end = vet + nvet;
	while ( vet < end )
		if ( ! strncmp( id, *vet++, length ) )
			return *(vet-1) + length;
	return 0;
}


/********************/
/*** map Callback ***/
/**
 * Shows the map creation progress.
 */
void
modelCallback( float progress, void *extra_param )
{
	Log::instance()->info( "Model creation: %07.4f%% \r", 100 * progress );
}


/********************/
/*** map Callback ***/
/**
 * Shows the map creation progress.
 */
void
mapCallback( float progress, void *extra_param )
{
	Log::instance()->info( "Map creation: %07.4f%% \r", 100 * progress );
}
