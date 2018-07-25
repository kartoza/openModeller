#ifndef _EXCEPTIONS_HH
#define _EXCEPTIONS_HH

// Microsoft VC8 requires <string> before <stdexcept> in order for
// this code to compile.
#include <string>
#include <stdexcept>

//
// The class OmException is the base
// class for all non-stdlib exceptions
// generated by the OpenModeller library.
//
class OmException : public std::runtime_error {
public:
  OmException( const std::string& msg ) :
    std::runtime_error( msg )
  {}

};

//
// Tag all exceptions from the configuration
// functionality with this.
//
class ConfigurationException : public OmException {
public:
  ConfigurationException( const std::string& msg ) :
    OmException( msg )
  {}
};

class AttributeNotFound : public ConfigurationException {
public:
  AttributeNotFound( const std::string& attrName ) :
    ConfigurationException( "Attribute " + attrName + " not found"),
    attrName( attrName )
  {}
  ~AttributeNotFound() throw() {}
  const std::string& getName() const {
    return attrName;
  }
private:
  std::string attrName;
};

class SubsectionNotFound : public ConfigurationException {
public:
  SubsectionNotFound( const std::string& sectionName ) :
    ConfigurationException( "Subsection " + sectionName + " not found"),
    sectionName( sectionName )
  {}
  ~SubsectionNotFound() throw() {}
  const std::string& getName() const {
    return sectionName;
  }
private:
  std::string sectionName;
};

class InvalidType : public ConfigurationException {
public:
  InvalidType( const std::string& msg ) :
    ConfigurationException( msg )
  {}
};

class XmlParseException : public ConfigurationException {
public:
  XmlParseException( const std::string& msg ) :
    ConfigurationException( msg )
  {}
};

//
// Simple IO Exception -- thrown for any File IO problem.
//
class FileIOException : public OmException {
public:
  FileIOException( const std::string& msg, const std::string& filename ) :
    OmException( msg ),
    filename( filename )
  {}
  ~FileIOException() throw() {}
  std::string getFilename() {
    return filename;
  }
private:
  std::string filename;
};

//
// Graphics Driver Exceptions -- thrown for non file io graphic problems.
//
class GraphicsDriverException : public OmException {
public:
  GraphicsDriverException( const std::string& msg ) :
    OmException( msg )
  {}
};

//
// Invalid Parameter Exception -- thrown in the library when an
// a parameter value is invalid
//
class InvalidParameterException : public OmException {
public:
  InvalidParameterException( const std::string& msg ) :
    OmException( msg )
  {}
};

//
// Invalid Sampling Configuration Exception -- thrown when clients try
// to get a pseudo absence sample without specifying the Environment
// object
class SamplerException : public OmException {
public:
  SamplerException( const std::string& msg ) :
    OmException( msg )
  {}
};


#endif
