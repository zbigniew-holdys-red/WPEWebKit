 #
 # Copyright (C) 2016 TATA ELXSI
 # Copyright (C) 2016 Metrological
 # All rights reserved.
 #
 # Redistribution and use in source and binary forms, with or without
 # modification, are permitted provided that the following conditions
 # are met:
 # 1. Redistributions of source code must retain the above copyright
 #    notice, this list of conditions and the following disclaimer.
 # 2. Redistributions in binary form must reproduce the above copyright
 #    notice, this list of conditions and the following disclaimer in the
 #    documentation and/or other materials provided with the distribution.
 #
 # THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 # "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 # LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 # PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 # HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 # SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 # LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 # DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 # THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 # (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 # OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 #/
 #  OCDM_INCLUDE_DIR - The OCDM include directory
 #  OCDM_LIB - The libraries needed to use OCDM


#**************************************************************
#Find the OCDM LIBRARIES and INCLUDES
#
#************************************************************
find_library(OCDM_LIBRARIES NAMES ocdm)
find_path(OCDM_INCLUDE_DIRS NAMES open_cdm.h PATH_SUFFIXES opencdm)

include(FindPackageHandleStandardArgs)
set(OCDM_LIBRARIES ${OCDM_LIBRARIES} CACHE PATH "Path to OCDM library")
set(OCDM_INCLUDE_DIRS ${OCDM_INCLUDE_DIRS} CACHE PATH "Path to OCDM include")

FIND_PACKAGE_HANDLE_STANDARD_ARGS(OCDM DEFAULT_MSG OCDM_LIBRARIES OCDM_INCLUDE_DIRS)
mark_as_advanced(OCDM_INCLUDE_DIRS OCDM_LIBRARIES)
