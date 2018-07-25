/* omwsStub.h
   Generated by gSOAP 2.8.15 from omws_api.hh

Copyright(C) 2000-2013, Robert van Engelen, Genivia Inc. All Rights Reserved.
The generated code is released under ONE of the following licenses:
GPL or Genivia's license for commercial use.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
*/

#ifndef omwsStub_H
#define omwsStub_H
#ifndef WITH_NOGLOBAL
#define WITH_NOGLOBAL
#endif
#include "stdsoap2.h"
#if GSOAP_VERSION != 20815
# error "GSOAP VERSION MISMATCH IN GENERATED CODE: PLEASE REINSTALL PACKAGE"
#endif


namespace omws {

/******************************************************************************\
 *                                                                            *
 * Enumerations                                                               *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Types with Custom Serializers                                              *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Classes and Structs                                                        *
 *                                                                            *
\******************************************************************************/


#if 0 /* volatile type: do not declare here, declared elsewhere */

#endif

#if 0 /* volatile type: do not declare here, declared elsewhere */

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_omws_SOAP_ENV__Header
#define SOAP_TYPE_omws_SOAP_ENV__Header (16)
/* SOAP Header: */
struct SOAP_ENV__Header
{
public:
	char *omws__version;	/* optional element of type xsd:string */
public:
	int soap_type() const { return 16; } /* = unique id SOAP_TYPE_omws_SOAP_ENV__Header */
};
#endif

#endif

#ifndef SOAP_TYPE_omws_omws__pingResponse
#define SOAP_TYPE_omws_omws__pingResponse (21)
/* omws:pingResponse */
struct omws__pingResponse
{
public:
	int status;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:int */
public:
	int soap_type() const { return 21; } /* = unique id SOAP_TYPE_omws_omws__pingResponse */
};
#endif

#ifndef SOAP_TYPE_omws_omws__ping
#define SOAP_TYPE_omws_omws__ping (22)
/* omws:ping */
struct omws__ping
{
public:
	void *_;	/* transient */
public:
	int soap_type() const { return 22; } /* = unique id SOAP_TYPE_omws_omws__ping */
};
#endif

#ifndef SOAP_TYPE_omws_omws__getAlgorithmsResponse
#define SOAP_TYPE_omws_omws__getAlgorithmsResponse (23)
/* omws:getAlgorithmsResponse */
struct omws__getAlgorithmsResponse
{
public:
	wchar_t *om__AvailableAlgorithms;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:anyType */
public:
	int soap_type() const { return 23; } /* = unique id SOAP_TYPE_omws_omws__getAlgorithmsResponse */
};
#endif

#ifndef SOAP_TYPE_omws_omws__getAlgorithms
#define SOAP_TYPE_omws_omws__getAlgorithms (26)
/* omws:getAlgorithms */
struct omws__getAlgorithms
{
public:
	void *_;	/* transient */
public:
	int soap_type() const { return 26; } /* = unique id SOAP_TYPE_omws_omws__getAlgorithms */
};
#endif

#ifndef SOAP_TYPE_omws_omws__getLayersResponse
#define SOAP_TYPE_omws_omws__getLayersResponse (27)
/* omws:getLayersResponse */
struct omws__getLayersResponse
{
public:
	wchar_t *om__AvailableLayers;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:anyType */
public:
	int soap_type() const { return 27; } /* = unique id SOAP_TYPE_omws_omws__getLayersResponse */
};
#endif

#ifndef SOAP_TYPE_omws_omws__getLayers
#define SOAP_TYPE_omws_omws__getLayers (30)
/* omws:getLayers */
struct omws__getLayers
{
public:
	void *_;	/* transient */
public:
	int soap_type() const { return 30; } /* = unique id SOAP_TYPE_omws_omws__getLayers */
};
#endif

#ifndef SOAP_TYPE_omws_omws__createModelResponse
#define SOAP_TYPE_omws_omws__createModelResponse (33)
/* omws:createModelResponse */
struct omws__createModelResponse
{
public:
	std::string ticket;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:string */
public:
	int soap_type() const { return 33; } /* = unique id SOAP_TYPE_omws_omws__createModelResponse */
};
#endif

#ifndef SOAP_TYPE_omws_omws__createModel
#define SOAP_TYPE_omws_omws__createModel (34)
/* omws:createModel */
struct omws__createModel
{
public:
	wchar_t *om__ModelParameters;	/* optional element of type xsd:anyType */
public:
	int soap_type() const { return 34; } /* = unique id SOAP_TYPE_omws_omws__createModel */
};
#endif

#ifndef SOAP_TYPE_omws_omws__getModelResponse
#define SOAP_TYPE_omws_omws__getModelResponse (35)
/* omws:getModelResponse */
struct omws__getModelResponse
{
public:
	wchar_t *om__ModelEnvelope;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:anyType */
public:
	int soap_type() const { return 35; } /* = unique id SOAP_TYPE_omws_omws__getModelResponse */
};
#endif

#ifndef SOAP_TYPE_omws_omws__getModel
#define SOAP_TYPE_omws_omws__getModel (38)
/* omws:getModel */
struct omws__getModel
{
public:
	std::string ticket;	/* required element of type xsd:string */
public:
	int soap_type() const { return 38; } /* = unique id SOAP_TYPE_omws_omws__getModel */
};
#endif

#ifndef SOAP_TYPE_omws_omws__testModelResponse
#define SOAP_TYPE_omws_omws__testModelResponse (40)
/* omws:testModelResponse */
struct omws__testModelResponse
{
public:
	std::string ticket;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:string */
public:
	int soap_type() const { return 40; } /* = unique id SOAP_TYPE_omws_omws__testModelResponse */
};
#endif

#ifndef SOAP_TYPE_omws_omws__testModel
#define SOAP_TYPE_omws_omws__testModel (41)
/* omws:testModel */
struct omws__testModel
{
public:
	wchar_t *om__TestParameters;	/* optional element of type xsd:anyType */
public:
	int soap_type() const { return 41; } /* = unique id SOAP_TYPE_omws_omws__testModel */
};
#endif

#ifndef SOAP_TYPE_omws_omws__testResponse
#define SOAP_TYPE_omws_omws__testResponse (42)
/* omws:testResponse */
struct omws__testResponse
{
public:
	wchar_t *om__TestResultEnvelope;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:anyType */
public:
	int soap_type() const { return 42; } /* = unique id SOAP_TYPE_omws_omws__testResponse */
};
#endif

#ifndef SOAP_TYPE_omws_omws__getTestResult
#define SOAP_TYPE_omws_omws__getTestResult (45)
/* omws:getTestResult */
struct omws__getTestResult
{
public:
	std::string ticket;	/* required element of type xsd:string */
public:
	int soap_type() const { return 45; } /* = unique id SOAP_TYPE_omws_omws__getTestResult */
};
#endif

#ifndef SOAP_TYPE_omws_omws__projectModelResponse
#define SOAP_TYPE_omws_omws__projectModelResponse (47)
/* omws:projectModelResponse */
struct omws__projectModelResponse
{
public:
	std::string ticket;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:string */
public:
	int soap_type() const { return 47; } /* = unique id SOAP_TYPE_omws_omws__projectModelResponse */
};
#endif

#ifndef SOAP_TYPE_omws_omws__projectModel
#define SOAP_TYPE_omws_omws__projectModel (48)
/* omws:projectModel */
struct omws__projectModel
{
public:
	wchar_t *om__ProjectionParameters;	/* optional element of type xsd:anyType */
public:
	int soap_type() const { return 48; } /* = unique id SOAP_TYPE_omws_omws__projectModel */
};
#endif

#ifndef SOAP_TYPE_omws_omws__getProgressResponse
#define SOAP_TYPE_omws_omws__getProgressResponse (50)
/* omws:getProgressResponse */
struct omws__getProgressResponse
{
public:
	std::string progress;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:string */
public:
	int soap_type() const { return 50; } /* = unique id SOAP_TYPE_omws_omws__getProgressResponse */
};
#endif

#ifndef SOAP_TYPE_omws_omws__getProgress
#define SOAP_TYPE_omws_omws__getProgress (51)
/* omws:getProgress */
struct omws__getProgress
{
public:
	std::string tickets;	/* required element of type xsd:string */
public:
	int soap_type() const { return 51; } /* = unique id SOAP_TYPE_omws_omws__getProgress */
};
#endif

#ifndef SOAP_TYPE_omws_omws__getLogResponse
#define SOAP_TYPE_omws_omws__getLogResponse (53)
/* omws:getLogResponse */
struct omws__getLogResponse
{
public:
	std::string log;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:string */
public:
	int soap_type() const { return 53; } /* = unique id SOAP_TYPE_omws_omws__getLogResponse */
};
#endif

#ifndef SOAP_TYPE_omws_omws__getLog
#define SOAP_TYPE_omws_omws__getLog (54)
/* omws:getLog */
struct omws__getLog
{
public:
	std::string ticket;	/* required element of type xsd:string */
public:
	int soap_type() const { return 54; } /* = unique id SOAP_TYPE_omws_omws__getLog */
};
#endif

#ifndef SOAP_TYPE_omws_omws__getLayerAsUrlResponse
#define SOAP_TYPE_omws_omws__getLayerAsUrlResponse (56)
/* omws:getLayerAsUrlResponse */
struct omws__getLayerAsUrlResponse
{
public:
	std::string url;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:string */
public:
	int soap_type() const { return 56; } /* = unique id SOAP_TYPE_omws_omws__getLayerAsUrlResponse */
};
#endif

#ifndef SOAP_TYPE_omws_omws__getLayerAsUrl
#define SOAP_TYPE_omws_omws__getLayerAsUrl (57)
/* omws:getLayerAsUrl */
struct omws__getLayerAsUrl
{
public:
	std::string id;	/* required element of type xsd:string */
public:
	int soap_type() const { return 57; } /* = unique id SOAP_TYPE_omws_omws__getLayerAsUrl */
};
#endif

#ifndef SOAP_TYPE_omws_omws__getProjectionMetadataResponse
#define SOAP_TYPE_omws_omws__getProjectionMetadataResponse (58)
/* omws:getProjectionMetadataResponse */
struct omws__getProjectionMetadataResponse
{
public:
	int FileSize;	/* optional attribute of type xsd:int */
	wchar_t *om__ProjectionEnvelope;	/* optional element of type xsd:anyType */
public:
	int soap_type() const { return 58; } /* = unique id SOAP_TYPE_omws_omws__getProjectionMetadataResponse */
};
#endif

#ifndef SOAP_TYPE_omws_omws__getProjectionMetadata
#define SOAP_TYPE_omws_omws__getProjectionMetadata (61)
/* omws:getProjectionMetadata */
struct omws__getProjectionMetadata
{
public:
	std::string ticket;	/* required element of type xsd:string */
public:
	int soap_type() const { return 61; } /* = unique id SOAP_TYPE_omws_omws__getProjectionMetadata */
};
#endif

#ifndef SOAP_TYPE_omws_omws__evaluateModelResponse
#define SOAP_TYPE_omws_omws__evaluateModelResponse (63)
/* omws:evaluateModelResponse */
struct omws__evaluateModelResponse
{
public:
	std::string ticket;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:string */
public:
	int soap_type() const { return 63; } /* = unique id SOAP_TYPE_omws_omws__evaluateModelResponse */
};
#endif

#ifndef SOAP_TYPE_omws_omws__evaluateModel
#define SOAP_TYPE_omws_omws__evaluateModel (64)
/* omws:evaluateModel */
struct omws__evaluateModel
{
public:
	wchar_t *om__ModelEvaluationParameters;	/* optional element of type xsd:anyType */
public:
	int soap_type() const { return 64; } /* = unique id SOAP_TYPE_omws_omws__evaluateModel */
};
#endif

#ifndef SOAP_TYPE_omws_omws__modelEvaluationResponse
#define SOAP_TYPE_omws_omws__modelEvaluationResponse (65)
/* omws:modelEvaluationResponse */
struct omws__modelEvaluationResponse
{
public:
	wchar_t *om__ModelEvaluation;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:anyType */
public:
	int soap_type() const { return 65; } /* = unique id SOAP_TYPE_omws_omws__modelEvaluationResponse */
};
#endif

#ifndef SOAP_TYPE_omws_omws__getModelEvaluation
#define SOAP_TYPE_omws_omws__getModelEvaluation (68)
/* omws:getModelEvaluation */
struct omws__getModelEvaluation
{
public:
	std::string ticket;	/* required element of type xsd:string */
public:
	int soap_type() const { return 68; } /* = unique id SOAP_TYPE_omws_omws__getModelEvaluation */
};
#endif

#ifndef SOAP_TYPE_omws_omws__samplePointsResponse
#define SOAP_TYPE_omws_omws__samplePointsResponse (70)
/* omws:samplePointsResponse */
struct omws__samplePointsResponse
{
public:
	std::string ticket;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:string */
public:
	int soap_type() const { return 70; } /* = unique id SOAP_TYPE_omws_omws__samplePointsResponse */
};
#endif

#ifndef SOAP_TYPE_omws_omws__samplePoints
#define SOAP_TYPE_omws_omws__samplePoints (71)
/* omws:samplePoints */
struct omws__samplePoints
{
public:
	wchar_t *om__SamplingParameters;	/* optional element of type xsd:anyType */
public:
	int soap_type() const { return 71; } /* = unique id SOAP_TYPE_omws_omws__samplePoints */
};
#endif

#ifndef SOAP_TYPE_omws_omws__getSamplingResultResponse
#define SOAP_TYPE_omws_omws__getSamplingResultResponse (72)
/* omws:getSamplingResultResponse */
struct omws__getSamplingResultResponse
{
public:
	wchar_t *om__Sampler;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:anyType */
public:
	int soap_type() const { return 72; } /* = unique id SOAP_TYPE_omws_omws__getSamplingResultResponse */
};
#endif

#ifndef SOAP_TYPE_omws_omws__getSamplingResult
#define SOAP_TYPE_omws_omws__getSamplingResult (75)
/* omws:getSamplingResult */
struct omws__getSamplingResult
{
public:
	std::string ticket;	/* required element of type xsd:string */
public:
	int soap_type() const { return 75; } /* = unique id SOAP_TYPE_omws_omws__getSamplingResult */
};
#endif

#ifndef SOAP_TYPE_omws_omws__runExperimentResponse
#define SOAP_TYPE_omws_omws__runExperimentResponse (76)
/* omws:runExperimentResponse */
struct omws__runExperimentResponse
{
public:
	wchar_t *om__ExperimentTickets;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:anyType */
public:
	int soap_type() const { return 76; } /* = unique id SOAP_TYPE_omws_omws__runExperimentResponse */
};
#endif

#ifndef SOAP_TYPE_omws_omws__runExperiment
#define SOAP_TYPE_omws_omws__runExperiment (79)
/* omws:runExperiment */
struct omws__runExperiment
{
public:
	char *om__ExperimentParameters;	/* optional element of type xsd:anyType */
public:
	int soap_type() const { return 79; } /* = unique id SOAP_TYPE_omws_omws__runExperiment */
};
#endif

#ifndef SOAP_TYPE_omws_omws__getResultsResponse
#define SOAP_TYPE_omws_omws__getResultsResponse (80)
/* omws:getResultsResponse */
struct omws__getResultsResponse
{
public:
	wchar_t *om__ResultSet;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:anyType */
public:
	int soap_type() const { return 80; } /* = unique id SOAP_TYPE_omws_omws__getResultsResponse */
};
#endif

#ifndef SOAP_TYPE_omws_omws__getResults
#define SOAP_TYPE_omws_omws__getResults (83)
/* omws:getResults */
struct omws__getResults
{
public:
	std::string tickets;	/* required element of type xsd:string */
public:
	int soap_type() const { return 83; } /* = unique id SOAP_TYPE_omws_omws__getResults */
};
#endif

#ifndef SOAP_TYPE_omws_omws__cancelResponse
#define SOAP_TYPE_omws_omws__cancelResponse (85)
/* omws:cancelResponse */
struct omws__cancelResponse
{
public:
	std::string cancelledTickets;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:string */
public:
	int soap_type() const { return 85; } /* = unique id SOAP_TYPE_omws_omws__cancelResponse */
};
#endif

#ifndef SOAP_TYPE_omws_omws__cancel
#define SOAP_TYPE_omws_omws__cancel (86)
/* omws:cancel */
struct omws__cancel
{
public:
	std::string tickets;	/* required element of type xsd:string */
public:
	int soap_type() const { return 86; } /* = unique id SOAP_TYPE_omws_omws__cancel */
};
#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_omws_SOAP_ENV__Code
#define SOAP_TYPE_omws_SOAP_ENV__Code (87)
/* SOAP Fault Code: */
struct SOAP_ENV__Code
{
public:
	char *SOAP_ENV__Value;	/* optional element of type xsd:QName */
	struct SOAP_ENV__Code *SOAP_ENV__Subcode;	/* optional element of type SOAP-ENV:Code */
public:
	int soap_type() const { return 87; } /* = unique id SOAP_TYPE_omws_SOAP_ENV__Code */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_omws_SOAP_ENV__Detail
#define SOAP_TYPE_omws_SOAP_ENV__Detail (89)
/* SOAP-ENV:Detail */
struct SOAP_ENV__Detail
{
public:
	char *__any;
	int __type;	/* any type of element <fault> (defined below) */
	void *fault;	/* transient */
public:
	int soap_type() const { return 89; } /* = unique id SOAP_TYPE_omws_SOAP_ENV__Detail */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_omws_SOAP_ENV__Reason
#define SOAP_TYPE_omws_SOAP_ENV__Reason (90)
/* SOAP-ENV:Reason */
struct SOAP_ENV__Reason
{
public:
	char *SOAP_ENV__Text;	/* optional element of type xsd:string */
public:
	int soap_type() const { return 90; } /* = unique id SOAP_TYPE_omws_SOAP_ENV__Reason */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_omws_SOAP_ENV__Fault
#define SOAP_TYPE_omws_SOAP_ENV__Fault (91)
/* SOAP Fault: */
struct SOAP_ENV__Fault
{
public:
	char *faultcode;	/* optional element of type xsd:QName */
	char *faultstring;	/* optional element of type xsd:string */
	char *faultactor;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *detail;	/* optional element of type SOAP-ENV:Detail */
	struct SOAP_ENV__Code *SOAP_ENV__Code;	/* optional element of type SOAP-ENV:Code */
	struct SOAP_ENV__Reason *SOAP_ENV__Reason;	/* optional element of type SOAP-ENV:Reason */
	char *SOAP_ENV__Node;	/* optional element of type xsd:string */
	char *SOAP_ENV__Role;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *SOAP_ENV__Detail;	/* optional element of type SOAP-ENV:Detail */
public:
	int soap_type() const { return 91; } /* = unique id SOAP_TYPE_omws_SOAP_ENV__Fault */
};
#endif

#endif

/******************************************************************************\
 *                                                                            *
 * Typedefs                                                                   *
 *                                                                            *
\******************************************************************************/

#ifndef SOAP_TYPE_omws__QName
#define SOAP_TYPE_omws__QName (5)
typedef char *_QName;
#endif

#ifndef SOAP_TYPE_omws__XML
#define SOAP_TYPE_omws__XML (6)
typedef char *_XML;
#endif

#ifndef SOAP_TYPE_omws_xsd__string
#define SOAP_TYPE_omws_xsd__string (8)
typedef std::string xsd__string;
#endif

#ifndef SOAP_TYPE_omws_xsd__double
#define SOAP_TYPE_omws_xsd__double (10)
typedef double xsd__double;
#endif

#ifndef SOAP_TYPE_omws_xsd__int
#define SOAP_TYPE_omws_xsd__int (11)
typedef int xsd__int;
#endif

#ifndef SOAP_TYPE_omws_XML
#define SOAP_TYPE_omws_XML (14)
typedef wchar_t *XML;
#endif

#ifndef SOAP_TYPE_omws_XML_
#define SOAP_TYPE_omws_XML_ (15)
typedef char *XML_;
#endif


/******************************************************************************\
 *                                                                            *
 * Externals                                                                  *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Server-Side Operations                                                     *
 *                                                                            *
\******************************************************************************/


SOAP_FMAC5 int SOAP_FMAC6 omws__ping(struct soap*, void *_, int &status);

SOAP_FMAC5 int SOAP_FMAC6 omws__getAlgorithms(struct soap*, void *_, struct omws__getAlgorithmsResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 omws__getLayers(struct soap*, void *_, struct omws__getLayersResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 omws__createModel(struct soap*, wchar_t *om__ModelParameters, std::string &ticket);

SOAP_FMAC5 int SOAP_FMAC6 omws__getModel(struct soap*, std::string ticket, struct omws__getModelResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 omws__testModel(struct soap*, wchar_t *om__TestParameters, std::string &ticket);

SOAP_FMAC5 int SOAP_FMAC6 omws__getTestResult(struct soap*, std::string ticket, struct omws__testResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 omws__projectModel(struct soap*, wchar_t *om__ProjectionParameters, std::string &ticket);

SOAP_FMAC5 int SOAP_FMAC6 omws__getProgress(struct soap*, std::string tickets, std::string &progress);

SOAP_FMAC5 int SOAP_FMAC6 omws__getLog(struct soap*, std::string ticket, std::string &log);

SOAP_FMAC5 int SOAP_FMAC6 omws__getLayerAsUrl(struct soap*, std::string id, std::string &url);

SOAP_FMAC5 int SOAP_FMAC6 omws__getProjectionMetadata(struct soap*, std::string ticket, struct omws__getProjectionMetadataResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 omws__evaluateModel(struct soap*, wchar_t *om__ModelEvaluationParameters, std::string &ticket);

SOAP_FMAC5 int SOAP_FMAC6 omws__getModelEvaluation(struct soap*, std::string ticket, struct omws__modelEvaluationResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 omws__samplePoints(struct soap*, wchar_t *om__SamplingParameters, std::string &ticket);

SOAP_FMAC5 int SOAP_FMAC6 omws__getSamplingResult(struct soap*, std::string ticket, struct omws__getSamplingResultResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 omws__runExperiment(struct soap*, char *om__ExperimentParameters, struct omws__runExperimentResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 omws__getResults(struct soap*, std::string tickets, struct omws__getResultsResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 omws__cancel(struct soap*, std::string tickets, std::string &cancelledTickets);

/******************************************************************************\
 *                                                                            *
 * Server-Side Skeletons to Invoke Service Operations                         *
 *                                                                            *
\******************************************************************************/

SOAP_FMAC5 int SOAP_FMAC6 soap_serve(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_request(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__ping(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getAlgorithms(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getLayers(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__createModel(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getModel(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__testModel(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getTestResult(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__projectModel(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getProgress(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getLog(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getLayerAsUrl(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getProjectionMetadata(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__evaluateModel(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getModelEvaluation(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__samplePoints(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getSamplingResult(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__runExperiment(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getResults(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__cancel(struct soap*);

/******************************************************************************\
 *                                                                            *
 * Client-Side Call Stubs                                                     *
 *                                                                            *
\******************************************************************************/


SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__ping(struct soap *soap, const char *soap_endpoint, const char *soap_action, void *_, int &status);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getAlgorithms(struct soap *soap, const char *soap_endpoint, const char *soap_action, void *_, struct omws__getAlgorithmsResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getLayers(struct soap *soap, const char *soap_endpoint, const char *soap_action, void *_, struct omws__getLayersResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__createModel(struct soap *soap, const char *soap_endpoint, const char *soap_action, wchar_t *om__ModelParameters, std::string &ticket);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getModel(struct soap *soap, const char *soap_endpoint, const char *soap_action, std::string ticket, struct omws__getModelResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__testModel(struct soap *soap, const char *soap_endpoint, const char *soap_action, wchar_t *om__TestParameters, std::string &ticket);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getTestResult(struct soap *soap, const char *soap_endpoint, const char *soap_action, std::string ticket, struct omws__testResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__projectModel(struct soap *soap, const char *soap_endpoint, const char *soap_action, wchar_t *om__ProjectionParameters, std::string &ticket);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getProgress(struct soap *soap, const char *soap_endpoint, const char *soap_action, std::string tickets, std::string &progress);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getLog(struct soap *soap, const char *soap_endpoint, const char *soap_action, std::string ticket, std::string &log);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getLayerAsUrl(struct soap *soap, const char *soap_endpoint, const char *soap_action, std::string id, std::string &url);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getProjectionMetadata(struct soap *soap, const char *soap_endpoint, const char *soap_action, std::string ticket, struct omws__getProjectionMetadataResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__evaluateModel(struct soap *soap, const char *soap_endpoint, const char *soap_action, wchar_t *om__ModelEvaluationParameters, std::string &ticket);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getModelEvaluation(struct soap *soap, const char *soap_endpoint, const char *soap_action, std::string ticket, struct omws__modelEvaluationResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__samplePoints(struct soap *soap, const char *soap_endpoint, const char *soap_action, wchar_t *om__SamplingParameters, std::string &ticket);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getSamplingResult(struct soap *soap, const char *soap_endpoint, const char *soap_action, std::string ticket, struct omws__getSamplingResultResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__runExperiment(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *om__ExperimentParameters, struct omws__runExperimentResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getResults(struct soap *soap, const char *soap_endpoint, const char *soap_action, std::string tickets, struct omws__getResultsResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__cancel(struct soap *soap, const char *soap_endpoint, const char *soap_action, std::string tickets, std::string &cancelledTickets);

} // namespace omws


#endif

/* End of omwsStub.h */
