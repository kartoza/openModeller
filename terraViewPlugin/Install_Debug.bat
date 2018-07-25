echo on

if not exist %TERRALIB_HOME%\terraView\windows\plugins mkdir %TERRALIB_HOME%\terraView\windows\plugins

copy "%EXPAT_HOME%\Libs\libexpat.dll" "%TERRALIB_HOME%\terraView\windows\plugins"
copy "%EXPAT_HOME%\Libs\libexpatw.dll" "%TERRALIB_HOME%\terraView\windows\plugins"

copy %GDAL_HOME%\bin\gdal*.dll %TERRALIB_HOME%\terraView\windows\plugins

copy %GSL_HOME%\bin\libgsl.dll %TERRALIB_HOME%\terraView\windows\plugins
copy %GSL_HOME%\bin\libgslcblas.dll %TERRALIB_HOME%\terraView\windows\plugins

copy %PROJ_HOME%\proj.dll %TERRALIB_HOME%\terraView\windows\plugins

copy om_config.txt %TERRALIB_HOME%\terraView\windows\plugins
copy wkt_defs.txt %TERRALIB_HOME%\terraView\windows\plugins

copy %OM_HOME%\windows\vc7\build_debug\libopenmodeller_debug.dll %TERRALIB_HOME%\terraView\windows\plugins
copy %OM_HOME%\windows\vc7\build_debug\libopenmodeller_debug.exp %TERRALIB_HOME%\terraView\windows\plugins
copy %OM_HOME%\windows\vc7\build_debug\libopenmodeller_debug.lib %TERRALIB_HOME%\terraView\windows\plugins

if not exist %TERRALIB_HOME%\terraView\windows\plugins\algorithm mkdir %TERRALIB_HOME%\terraView\windows\plugins\algorithm
copy %OM_HOME%\windows\vc7\build_debug\om_mindist_debug.dll %TERRALIB_HOME%\terraView\windows\plugins\algorithm
copy %OM_HOME%\windows\vc7\build_debug\om_mindist_debug.exp %TERRALIB_HOME%\terraView\windows\plugins\algorithm
copy %OM_HOME%\windows\vc7\build_debug\om_mindist_debug.lib %TERRALIB_HOME%\terraView\windows\plugins\algorithm

copy %OM_HOME%\windows\vc7\build_debug\om_garp_debug.dll %TERRALIB_HOME%\terraView\windows\plugins\algorithm
copy %OM_HOME%\windows\vc7\build_debug\om_garp_debug.exp %TERRALIB_HOME%\terraView\windows\plugins\algorithm
copy %OM_HOME%\windows\vc7\build_debug\om_garp_debug.lib %TERRALIB_HOME%\terraView\windows\plugins\algorithm

copy %OM_HOME%\windows\vc7\build_debug\om_garp_best_subsets_debug.dll %TERRALIB_HOME%\terraView\windows\plugins\algorithm
copy %OM_HOME%\windows\vc7\build_debug\om_garp_best_subsets_debug.exp %TERRALIB_HOME%\terraView\windows\plugins\algorithm
copy %OM_HOME%\windows\vc7\build_debug\om_garp_best_subsets_debug.lib %TERRALIB_HOME%\terraView\windows\plugins\algorithm

copy %OM_HOME%\windows\vc7\build_debug\om_distance_to_average_debug.dll %TERRALIB_HOME%\terraView\windows\plugins\algorithm
copy %OM_HOME%\windows\vc7\build_debug\om_distance_to_average_debug.exp %TERRALIB_HOME%\terraView\windows\plugins\algorithm
copy %OM_HOME%\windows\vc7\build_debug\om_distance_to_average_debug.lib %TERRALIB_HOME%\terraView\windows\plugins\algorithm

copy %OM_HOME%\windows\vc7\build_debug\om_dg_garp_debug.dll %TERRALIB_HOME%\terraView\windows\plugins\algorithm
copy %OM_HOME%\windows\vc7\build_debug\om_dg_garp_debug.exp %TERRALIB_HOME%\terraView\windows\plugins\algorithm
copy %OM_HOME%\windows\vc7\build_debug\om_dg_garp_debug.lib %TERRALIB_HOME%\terraView\windows\plugins\algorithm

copy %OM_HOME%\windows\vc7\build_debug\om_dg_garp_bs_debug.dll %TERRALIB_HOME%\terraView\windows\plugins\algorithm
copy %OM_HOME%\windows\vc7\build_debug\om_dg_garp_bs_debug.exp %TERRALIB_HOME%\terraView\windows\plugins\algorithm
copy %OM_HOME%\windows\vc7\build_debug\om_dg_garp_bs_debug.lib %TERRALIB_HOME%\terraView\windows\plugins\algorithm

copy %OM_HOME%\windows\vc7\build_debug\om_dg_garp_bs2_debug.dll %TERRALIB_HOME%\terraView\windows\plugins\algorithm
copy %OM_HOME%\windows\vc7\build_debug\om_dg_garp_bs2_debug.exp %TERRALIB_HOME%\terraView\windows\plugins\algorithm
copy %OM_HOME%\windows\vc7\build_debug\om_dg_garp_bs2_debug.lib %TERRALIB_HOME%\terraView\windows\plugins\algorithm

copy %OM_HOME%\windows\vc7\build_debug\om_bioclim_debug.dll %TERRALIB_HOME%\terraView\windows\plugins\algorithm
copy %OM_HOME%\windows\vc7\build_debug\om_bioclim_debug.exp %TERRALIB_HOME%\terraView\windows\plugins\algorithm
copy %OM_HOME%\windows\vc7\build_debug\om_bioclim_debug.lib %TERRALIB_HOME%\terraView\windows\plugins\algorithm



echo off