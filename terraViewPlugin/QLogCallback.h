#ifndef QLOGCALLBACK_H
#define QLOGCALLBACK_H

#include <openmodeller/Log.hh>

#include <qtextbrowser.h> 

#include <string>
using namespace std;

/** Although the call back needs a text stream, all we want is to get the msg.c_str() 
 * from the logger, so having the textstream is just so that we fit the 
 * g_log callback model, but its not actually used anywhere
 */
class QLogCallback : public Log::LogCallback
{

public:
	~QLogCallback() 
	{
		//output_.close();
	}

	QLogCallback() 
	{
		output_.setTextFormat(Qt::RichText);
	}

	void operator()(Log::Level, const string& msg ) 
	{
		output_.append( msg.c_str() );
	}

	void append( const QString& app)
	{
		output_.append( app );
	}

	QTextBrowser output_;
};

#endif //LOGCALLBACK_H
