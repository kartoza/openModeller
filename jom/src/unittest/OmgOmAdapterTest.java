package unittest;

import java.io.BufferedWriter;
import java.io.FileWriter;
import junit.framework.TestCase;
import org.om.omg.jni.OmgOmAdapter;

public class OmgOmAdapterTest extends TestCase
{
  /*
   * Test method for 'org.om.omg.jni.OmgOmAdapter.sayHello(String)'
   */
  public void testSayHello()
  {
    try
    {
      System.out.println("Testing testSayHello");
      String retval = null;
      OmgOmAdapter myAdapter = new OmgOmAdapter();
      System.out.println("Asking lib to say 'Hello World' ");
      retval = myAdapter.sayHello("Hello World");
      System.out.println("Lib returned " + retval);
    }
    catch (Exception e)
    {
      fail(e.getMessage());
    }
  }

  /*
   * Test method for 'org.om.omg.jni.OmgOmAdapter.createModel(String)'
   */
  public void testCreateModel()
  {
    try
    {
      System.out.println("Testing testCreateModel");
      String myResult = null;
      OmgOmAdapter myAdapter = new OmgOmAdapter();
      String myDir = System.getProperty("user.dir");
      myResult = myAdapter.createModel(myDir + "/testdata/bioclim-param.xml");
      if (myResult == "")
      {
        fail("createModel produced no output!");
      }
      BufferedWriter out = new BufferedWriter(new FileWriter("/tmp/model.xml"));
      out.write(myResult);
      out.close();
    }
    catch (Exception e)
    {
      fail(e.getMessage());
    }
  }

  /*
   * Test method for 'org.om.omg.jni.OmgOmAdapter.createModel(String)'
   * This test ensures that a null string is returned when bogus
   * input filename is passed to om.
   */
  public void testCreateModelExpectingFailure()
  {
    try
    {
      System.out.println("Testing testCreateModelExpectingFailure");
      String myResult = null;
      OmgOmAdapter myAdapter = new OmgOmAdapter();
      String myDir = System.getProperty("user.dir");
      myResult = myAdapter.createModel(myDir + "/testdata/BOGUSNAME.xml");
      if (myResult==null)
      {
        System.out.println("Result was: \n"+myResult);
        fail("testCreateModelExpectingFailure produced output when it was expected to fail!");        
      }
    }
    catch (Exception e)
    {
      fail(e.getMessage());
    }
  }  
  
  /*
   * Test method for 'org.om.omg.jni.OmgOmAdapter.projectModel(String, String,
   * String)'
   */
  public void testProjectModel()
  {
    try
    {
      System.out.println("Testing testProjectModel");
      OmgOmAdapter myAdapter = new OmgOmAdapter();
      String myDir = System.getProperty("user.dir");
      if (!myAdapter.projectModel("/tmp/model.xml", myDir
          + "/testdata/environment.xml", myDir + "/testdata/testout.tif"))
      {
        fail("Model projection failed!");
      }
    }
    catch (Exception e)
    {
      fail(e.getMessage());
    }
  }

  /*
   * Test method for 'org.om.omg.jni.OmgOmAdapter.projectModel(String, String,
   * String)' This test passes some bogus data and ensures that false is
   * returned!
   */
  public void testProjectModelExpectingFailure()
  {
    try
    {
      System.out.println("Testing testProjectModel");
      OmgOmAdapter myAdapter = new OmgOmAdapter();
      String myDir = System.getProperty("user.dir");
      if (myAdapter.projectModel("/tmp/model.xml", myDir
          + "/testdata/environment987987897.xml", // some bogus name
          // (intentional!)
          myDir + "/testdata/testout.tif"))
      {
        fail("testProjectModelExpectingFailure (model should have returned false!");
      }
    }
    catch (Exception e)
    {
      fail(e.getMessage());
    }
  }
}
