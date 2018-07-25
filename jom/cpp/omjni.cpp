#include "org_om_omg_jni_OmgOmAdapter.h"
//this is very ugly but using consolexml is probably going to be an interim measure 
//until I get omgui1's omgomadapter hooked up via jni rather
#include <consolexml.hh>
#include <string>

JNIEXPORT jstring JNICALL Java_org_om_omg_jni_OmgOmAdapter_sayHello
  (JNIEnv * theEnv, jobject theJObject, jstring theString) 
{
  //convert the passed in string from a jstring to a std::string
  std::string myString(theEnv->GetStringUTFChars( theString, 0));
  myString.insert(0, "sayHello called with: ");
  //printf("%s\n", myString.c_str());
  //theEnv->ReleaseStringUTFChars( theString, myString.c_str());
  
  //create a string to pass back as a return from this function
;
  jstring myJString = theEnv->NewStringUTF(myString.c_str());
  return myJString;
}

JNIEXPORT jstring JNICALL Java_org_om_omg_jni_OmgOmAdapter_createModel
  (JNIEnv * theEnv, jobject theJObject, jstring theRequestXml)
{
  //convert the passed in strings from a jstring to a std::string
  std::string myParameters (theEnv->GetStringUTFChars( theRequestXml, 0));
  ConsoleXml myConsoleXml;
  std::string myRetVal=myConsoleXml.createModel(myParameters);

  jstring myJString = theEnv->NewStringUTF(myRetVal.c_str());
  return myJString;
}
  

JNIEXPORT jboolean JNICALL Java_org_om_omg_jni_OmgOmAdapter_projectModel
  (JNIEnv * theEnv, jobject theJObject, 
                   jstring theModelXml, 
                   jstring theEnvironmentXml, 
                   jstring theOutputFileName)
{
    bool myResult=true;
    //convert the passed in strings from a jstring to a std::string
    std::string myModel (theEnv->GetStringUTFChars( theModelXml, 0));
    std::string myEnvironment (theEnv->GetStringUTFChars( theEnvironmentXml, 0));
    std::string myOutputFile (theEnv->GetStringUTFChars( theOutputFileName, 0));
    ConsoleXml myConsoleXml;
    try
    {
      myResult=myConsoleXml.projectModel(myModel,myEnvironment,myOutputFile);
    }
    catch (...)
    {
      myResult=false;
    }
    return myResult;
}  

JNIEXPORT jstring JNICALL Java_org_om_omg_jni_OmgOmAdapter_localitiesSearch
  (JNIEnv * theEnv, jobject theJObject, jstring theNameList, jstring theProvider)
{
    std::string myRetVal;
    //convert the passed in strings from a jstring to a std::string
    std::string myNameList (theEnv->GetStringUTFChars( theNameList, 0));
    std::string myProvider (theEnv->GetStringUTFChars( theProvider, 0));
    ConsoleXml myConsoleXml;
    try
    {
      myRetVal="Do Me";

    }
    catch (...)
    {
      myRetVal="";
    }
    jstring myJString = theEnv->NewStringUTF(myRetVal.c_str());
    return myJString;
} 
