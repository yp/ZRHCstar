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
#  generate-instances.sh
#
#  A simple script that generate various instances using 'pedigree-generator.py' and
#  'hc-generator.py' with various parameters.
#
#  Useful for 'serial' experimentations.
#
##########

pedigree_sizes="100 1000 10000"
genotype_lengths="20 50 100"
missing_rates="0.0 0.05 0.10 0.20"
n_conf=2

additional_ped_params=""
additional_hg_params=""

formatd="%.5d"
formatconf="%.2d"
formatf="%5.3f"

LANG=C

script_dir=${1:-`dirname ${PWD}/$0`}

for script in "pedigree-generator.py" "hc-generator.py"; do
    if test ! -x ${script_dir}/${script}; then
        echo "Script '${script}' not found in directory '${script_dir}'.  Aborting..."
        exit 1
    fi
done


seed=2095
for size in ${pedigree_sizes}; do
    size_str=`printf "${formatd}" ${size}`
    for conf in `seq 0 $((n_conf - 1))`; do
        conf_str=`printf "${formatconf}" ${conf}`
        prefix_id="size${size_str}-conf${conf_str}"
        echo "Generating pedigree structure: pedigree-${prefix_id}.txt"
        echo "Generating pedigree structure: pedigree-${prefix_id}.txt" >&2
        ${script_dir}/pedigree-generator.py -p ${size} -s ${seed} -d dot-${prefix_id}.txt ${additional_ped_params} > pedigree-${prefix_id}.txt
        for length in ${genotype_lengths}; do
            length_str=`printf "${formatd}" ${length}`
            for miss in ${missing_rates}; do
                miss_str=`printf "${formatf}" ${miss}`
                id="${prefix_id}-length${length_str}-miss${miss_str/./_}"
                echo "Generating instance: gen-ped-${id}.txt"
                echo "Generating instance: gen-ped-${id}.txt" >&2
                ${script_dir}/hc-generator.py -l ${length} -m ${miss} -s ${seed} ${additional_hg_params} < pedigree-${prefix_id}.txt > gen-ped-${id}.txt
                seed=$((seed + 122))
            done
        done
    done
done 2> generation.log
