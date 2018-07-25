#include <jni.h>
#include <gdal_version.h>
#include <gdal.h>
#include <gdal_priv.h>
/* Header for class org_ecoinformatics_seek_garp_GarpJniGlue */
#ifndef _Included_org_ioz_openModeller_webservice_util_ExecuteAlgorithm
#define _Included_org_ioz_openModeller_webservice_util_ExecuteAlgorithm
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     org_ecoinformatics_seek_garp_GarpJniGlue
 * Method:    PresampleLayers
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT int JNICALL Java_org_ioz_openModeller_web_util_ExecuteAlgorithm_RunAlgorithm
  (JNIEnv *, jobject, jstring);

#ifdef __cplusplus
}
#endif
#endif

#ifndef _Included_org_ioz_openModeller_invoke
#define _Included_org_ioz_openModeller_invoke
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     org_ecoinformatics_seek_garp_GarpJniGlue
 * Method:    PresampleLayers
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
 */

JNIEXPORT int JNICALL Java_org_ioz_openModeller_invoke_RunAlgorithm
  (JNIEnv *, jobject, jstring);
#ifdef __cplusplus
}
#endif
#endif

