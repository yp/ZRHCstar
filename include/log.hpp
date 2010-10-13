/**
 *
 *                              ZRHC-*
 * Zero-Recombinant Haplotype Configuration with missing genotypes
 *
 * Copyright (C) 2010  Yuri Pirola <yuri.pirola(-at-)gmail.com>
 *
 * Distributed under the terms of the GNU General Public License (GPL)
 *
 *
 * This file is part of ZRHC-* (ZRHCstar).
 *
 * ZRHC-* is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ZRHC-* is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ZRHC-*.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/
/**
 * log.hpp
 *
 * Definition of commodity function to use Log4Cxx
 *
 * The following logging levels are defined:
 * TRACE
 * DEBUG
 * INFO
 * WARN
 * ERROR
 * FATAL
 *
 **/

#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <string>
#include <cstdlib>
#include <log4cxx/logger.h>


int initialize_logger(void);

#define _ROOT_LOG(level, msg) LOG4CXX_ ## level(log4cxx::Logger::getRootLogger(), msg)

#define ROOT_TRACE( msg ) _ROOT_LOG(TRACE, msg)
#define ROOT_DEBUG( msg ) _ROOT_LOG(DEBUG, msg)
#define ROOT_INFO(  msg ) _ROOT_LOG(INFO, msg)
#define ROOT_WARN(  msg ) _ROOT_LOG(WARN, msg)
#define ROOT_ERROR( msg ) _ROOT_LOG(ERROR, msg)
#define ROOT_FATAL( msg ) _ROOT_LOG(FATAL, msg)


#define _STD_LOG( level, msg ) LOG4CXX_ ## level(logger, msg)

#define TRACE( msg ) _STD_LOG(TRACE, msg)
#define DEBUG( msg ) _STD_LOG(DEBUG, msg)
#define INFO(  msg ) _STD_LOG(INFO, msg)
#define WARN(  msg ) _STD_LOG(WARN, msg)
#define ERROR( msg ) _STD_LOG(ERROR, msg)
#define FATAL( msg ) _STD_LOG(FATAL, msg)


#define _NAMED_LOG( level, name, msg ) LOG4CXX_ ## level(log4cxx::Logger::getLogger((name)), msg)

#define N_TRACE( name, msg ) _NAMED_LOG( TRACE, name, msg)
#define N_DEBUG( name, msg ) _NAMED_LOG( DEBUG, name, msg)
#define N_INFO(  name, msg ) _NAMED_LOG( INFO, name, msg)
#define N_WARN(  name, msg ) _NAMED_LOG( WARN, name, msg)
#define N_ERROR( name, msg ) _NAMED_LOG( ERROR, name, msg)
#define N_FATAL( name, msg ) _NAMED_LOG( FATAL, name, msg)


#endif // __LOG_HPP__
