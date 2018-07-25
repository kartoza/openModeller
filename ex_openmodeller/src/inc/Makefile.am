# Copyright (C) 2004 Jens Oberender <j.obi at troja.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

omincludedir = $(includedir)/openmodeller

ominclude_HEADERS = \
	algorithm_factory.hh \
	configurable.hh \
	configuration.hh \
	environment.hh \
	Exceptions.hh \
	file_parser.hh \
	icstring.hh \
	map_format.hh \
	Model.hh \
	normalizable.hh \
	occurrence.hh \
	om_algorithm.hh \
	om_algorithm_metadata.hh \
	om_alg_parameter.hh \
	om_area_stats.hh \
	om_conf_matrix.hh \
	om_control.hh \
	om_defs.hh \
	om_log.hh \
	om_occurrences.hh \
	om_projector.hh \
	om_sampler.hh \
	om.hh \
	os_specific.hh \
	refcount.hh \
	Sample.hh

envioincludedir = $(omincludedir)/env_io

envioinclude_HEADERS = \
	env_io/header.hh \
	env_io/geo_transform.hh \
	env_io/map.hh \
	env_io/map_iter.hh \
	env_io/raster.hh \
	env_io/raster_gdal.hh

modelsincludedir = $(omincludedir)/models

modelsinclude_HEADERS = \
	models/AlgoAdapterModel.hh \
	models/AverageModel.hh \
	models/ScaledModel.hh

EXTRA_DIST = \
	random.hh \
	SampleExpr.hh \
	SampleExprVar.hh
