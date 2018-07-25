package org.om.omg.jni;

/**
 * This is a JNI adapter class for the openModeller library.
 * @author Tim Sutton
 *
 */
public class OmgOmAdapter
{
  /**
   * @param args
   */
  static {
    System.loadLibrary ( "omjniadapter" ) ;
  }
  /* 
   * 
   * @NOTE When adding a new native method the procedure is as follows:
   * First add the prototype to the section below.
   * Next run the jni-compile ant task. This will create new protitypes in the
   * org_om_omg_jni_OmgOmAdapter.h generated stub.
   * Next implement these prototypes in omjni.cpp
   * Finally run the c-compile ant task which will rebuild the omjniadapter 
   * library with the new methods.
   * After that you can use this class in your java code.
   */
     
  /*
   * This method is just for testing.
   */
  public native String sayHello(String s);
  /**
   * Create a model using an xml configuration
   * @param theRequest An xml openModeller request file.
   * @return A string containing the serialised model.
   */
  public native String createModel(String theRequest);
  /**
   * Project a model using an xml configuration
   * @param theRequest
   * @param theEnvironment
   * @param theOutputFile
   */
  public native boolean projectModel(String theRequest, String theEnvironment, String theOutputFile);
  
  /**
   * Search for localities
   * @param theNameList - a list of names to search on, each name on its own line
   * @param theProvider - only GBIF currently supported
   * @return An xml document providing a list.
   */
  public native String localitiesSearch(String theNameList, String theProvider);
  
}
