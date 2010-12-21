#!/bin/bash

##########
#
#                               ZRHC-*
#  Zero-Recombinant Haplotype Configuration with missing genotypes
#
#  Copyright (C) 2010  Yuri Pirola <yuri.pirola(-at-)gmail.com>
#
#  Distributed under the terms of the GNU General Public License (GPL)
#
#
#  This file is part of ZRHC-* (ZRHCstar).
#
#  ZRHC-* is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  ZRHC-* is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with ZRHC-*.  If not, see <http://www.gnu.org/licenses/>.
#
##########

##########
#
#  execute-ZRHC.sh
#
#  A simple script that execute ZRHC in the 'create-read' mode on
#  every genotyped pedigree of the current directory.
#  
#  Useful for 'serial' experimentations.
#
##########


ZRHC_exe='./bin/ZRHCstar'
SAT_cmd='./bin/cryptominisat %%INPUT%% %%OUTPUT%% >> execution.log'

pedigrees="`ls gen-ped-*.txt`"
pedigrees=${*:-${pedigrees}}

LANG=C

for pedigree in ${pedigrees}; do
    echo "`date`  --  Executing on ${pedigree}"
    nice time -f "%U %s %E %x %M %C" -o ${pedigree/#gen-/time-} ${ZRHC_exe} -3 -p ${pedigree} -h ${pedigree/#gen-/hap-} -c "${SAT_cmd}"
done
