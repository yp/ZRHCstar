#!/usr/bin/python3


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
#  evaluate-results.py
#
#  A program that evaluates the quality of the results computed by ZRHC-*
#  compared to the generated haplotype configuration.
#
##########

import os
import sys
import random
import logging
import operator
import functools
from optparse import OptionParser

def parse_command_line():
    usage= "usage: %prog [options]"
    parser= OptionParser(usage=usage)
    parser.add_option("-o", "--original",
                      action="store", dest="original",
                      type="string", default=None,
                      help="the file containing the genotyped pedigree and the "
                      "original haplotype configuration (as produced by 'hc-generator.py')",
                      metavar="FILE")
    parser.add_option("-r", "--result",
                      action="store", dest="result",
                      type="string", default=None,
                      help="the file containing the haplotype configuration computed "
                      "by ZRHC-*",
                      metavar="FILE")
    parser.add_option("-f", "--full-stats",
                      action="store_true", dest="full",
                      default=False,
                      help="print statistics for each individual")
    parser.add_option("-H", "--show-header",
                      action="store_true", dest="header",
                      default=False,
                      help="print the header")
    parser.add_option("-n", "--dont-normalize-founders",
                      action="store_true", dest="no_norm_founders",
                      default=False,
                      help="swap founders' haplotypes such that the minimum amount of "
                      "phase error is obtained")
    parser.add_option("-c", "--expensive-checks",
                      action="store_true", dest="checks",
                      default=False,
                      help="perform additional computational-expensive consistency checks")
    parser.add_option("-v", "--verbose",
                      action="store_true", dest="verbose",
                      default=False,
                      help="print additional log messages")
    (options, args) = parser.parse_args()

    return(options)


def compute_errors(genotype, orig_hp, orig_hm, res_hp, res_hm):
    orig_gen= [ encoding[str(hp) + ' ' + str(hm)] for hp,hm in zip(orig_hp, orig_hm) ]
    res_gen= [ encoding[str(hp) + ' ' + str(hm)] for hp,hm in zip(res_hp, res_hm) ]
    gen_dif= [ 0 if orig == res else 1
               for orig,res in zip(orig_gen, res_gen) ]
    pat_dif= [ 0 if orig == res else 1
               for orig,res in zip(orig_hp, res_hp) ]
    mat_dif= [ 0 if orig == res else 1
               for orig,res in zip(orig_hm, res_hm) ]
    mask_pat_dif= [ 0 if g == 0 else diff
                    for diff, g in zip(pat_dif, genotype) ]
    mask_mat_dif= [ 0 if g == 0 else diff
                    for diff, g in zip(mat_dif, genotype) ]
    err_gen= sum(gen_dif)
    err_pat_hap= sum(pat_dif)
    err_mat_hap= sum(mat_dif)
    err_mask_pat_hap= sum(mask_pat_dif)
    err_mask_mat_hap= sum(mask_mat_dif)

    return ( err_gen,
             err_pat_hap, err_mat_hap,
             err_mask_pat_hap, err_mask_mat_hap )

def compute_errors_with_swap(genotype, orig_hp, orig_hm, res_hp, res_hm, can_swap):
    if ( (can_swap) and
         (orig_ped[individual][1] == '0') and
         (orig_ped[individual][2] == '0') ):
        ris1= compute_errors(genotype, orig_hp, orig_hm, res_hp, res_hm)
        ris2= compute_errors(genotype, orig_hp, orig_hm, res_hm, res_hp)
        return ris1 if sum(ris1) < sum(ris2) else ris2
    else:
        return compute_errors(genotype, orig_hp, orig_hm, res_hp, res_hm)



options = parse_command_line()

log_level= logging.DEBUG if options.verbose else logging.INFO

logging.basicConfig(level=log_level,
                    format='%(levelname)-6s [%(asctime)s]  %(message)s')

logging.info("EVALUATION OF ZRHC-* RESULTS")

if ( not options.original or
     not os.path.isfile(options.original) or
     not os.access(options.original, os.R_OK) ):
    logging.fatal("Original file not specified or not accessible. Given '%s'",
                  options.original)
    sys.exit(1)

logging.info("Original file: '%s'", options.original)


if ( not options.result or
     not os.path.isfile(options.result) or
     not os.access(options.result, os.R_OK) ):
    logging.fatal("Result file not specified or not accessible. Given '%s'",
                  options.result)
    sys.exit(1)

logging.info("Result file:   '%s'", options.result)



# Read the original haplotype configuration
logging.info("Reading original file '%s'...", options.original)
missing_str= []
with open(options.original, 'r') as orig:
    missing_str= [ line.strip()
                    for line in orig
                    if not line.startswith('#')  ]
logging.debug("Parsing the original genotype configuration "
              "to get the genotypes...")
genotypes= {}
encoding= { '0 0':0,
            '1 1':1,
            '2 2':2,
            '1 2':3,
            '2 1':3
            }
for r in missing_str:
    split_r= r.split("\t", 6)
    genotypes[ split_r[1] ]= [ encoding[single]
                                  for single in split_r[6].split("\t") ]
del missing_str

logging.info("Re-reading original file '%s'...", options.original)
orig_ped_str= []
with open(options.original, 'r') as orig:
    orig_ped_str= [ line.replace('# GENERATED_HAPLOTYPES', '', 1).strip()
                    for line in orig
                    if line.startswith('# GENERATED_HAPLOTYPES')  ]
logging.info("Parsing the original haplotype configuration...")
orig_ped= {}
for r in orig_ped_str:
    split_r= r.split("\t", 6)
    comb_hap= split_r[6].split("\t")
    pat_hap= [ int(single.split('|')[0]) for single in comb_hap ]
    mat_hap= [ int(single.split('|')[1]) for single in comb_hap ]
    orig_ped[split_r[1]]= (split_r[1], split_r[2], split_r[3], pat_hap, mat_hap)
del orig_ped_str;


# Read the computed haplotype configuration
logging.info("Reading result file '%s'...", options.result)
res_ped_str= []
with open(options.result, 'r') as result:
    res_ped_str= [ line.strip()
                   for line in result
                   if not line.startswith('#')  ]

logging.debug("Parsing the computed haplotype configuration...")
res_ped= {}
for r in res_ped_str:
    split_r= r.split("\t", 6)
    comb_hap= split_r[6].split("\t")
    pat_hap= [ int(single.split('|')[0]) for single in comb_hap ]
    mat_hap= [ int(single.split('|')[1]) for single in comb_hap ]
    res_ped[split_r[1]]= (split_r[1], split_r[2], split_r[3], pat_hap, mat_hap)

logging.info("Checking (basic) consistency...")
if ( orig_ped.keys() != res_ped.keys() or
     orig_ped.keys() != genotypes.keys() or 
     res_ped.keys() != genotypes.keys() ):
    logging.fatal("The two pedigrees refer to different individuals.")
    sys.exit(1)
for individual in iter(orig_ped):
    if orig_ped[individual][0:3] != res_ped[individual][0:3]:
        logging.fatal("The parents of '%s' are different in the two pedigrees. "
                      "Original: (%s) - Result: (%s).",
                      individual,
                      ",".join(orig_ped[individual][0:3]),
                      ",".join(res_ped[individual][0:3]) )
        sys.exit(1)
    if options.checks:
        pat_hap= orig_ped[individual][3]
        mat_hap= orig_ped[individual][4]
        if ( (orig_ped[individual][1] != '0') and
             (pat_hap != orig_ped[orig_ped[individual][1]][3]) and
             (pat_hap != orig_ped[orig_ped[individual][1]][4]) ):
            logging.fatal("The paternal haplotype of individual '%s' has not been "
                          "inherited from father '%s' in the original haplotype configuration.",
                          individual, orig_ped[individual][1])
            sys.exit(1)
            if ( (orig_ped[individual][2] != '0') and
                 (mat_hap != orig_ped[orig_ped[individual][2]][3]) and
                 (mat_hap != orig_ped[orig_ped[individual][2]][4]) ):
                logging.fatal("The maternal haplotype of individual '%s' has not been "
                              "inherited from mother '%s' in the original haplotype configuration.",
                              individual, orig_ped[individual][2])
                sys.exit(1)
                pat_hap= res_ped[individual][3]
                mat_hap= res_ped[individual][4]
                if ( (res_ped[individual][1] != '0') and
                     (pat_hap != res_ped[res_ped[individual][1]][3]) and
                     (pat_hap != res_ped[res_ped[individual][1]][4]) ):
                    logging.fatal("The paternal haplotype of individual '%s' has not been "
                                  "inherited from father '%s' in the computed haplotype configuration.",
                                  individual, res_ped[individual][1])
                    sys.exit(1)
                    if ( (res_ped[individual][2] != '0') and
                         (mat_hap != res_ped[res_ped[individual][2]][3]) and
                         (mat_hap != res_ped[res_ped[individual][2]][4]) ):
                        logging.fatal("The maternal haplotype of individual '%s' has not been "
                                      "inherited from mother '%s' in the computed haplotype configuration.",
                                      individual, res_ped[individual][2])
                        sys.exit(1)

logging.info("Computing differences...")
    
tot_gen= 0
tot_het= 0
tot_hom= 0
tot_mis= 0
    
tot_abs_err_gen= 0
tot_abs_err_pat_hap= 0
tot_abs_err_mat_hap= 0
tot_abs_err_mask_pat_hap= 0
tot_abs_err_mask_mat_hap= 0
tot_err_gen= 0.0
tot_err_pat_hap= 0.0
tot_err_mat_hap= 0.0
tot_err_mask_pat_hap= 0.0
tot_err_mask_mat_hap= 0.0

if options.full and options.header:
    print('"input file"', '"result file"',
          '"individual id"',
          '"father id"', '"mother id"',
          '"genotype_length"',
          '"heterozygous"', '"homozygous"', '"missing"',
          '"genotype errors"',
          '"paternal haplotype errors"', '"maternal haplotype errors"',
          '"paternal haplotype errors wo missing"', '"maternal haplotype errors wo missing"',
          sep="\t")
for individual in iter(orig_ped):
    
    genotype= genotypes[individual]
    ( err_gen,
      err_pat_hap, err_mat_hap,
      err_mask_pat_hap, err_mask_mat_hap ) = compute_errors_with_swap(genotype,
                                                                      orig_ped[individual][3],
                                                                      orig_ped[individual][4],
                                                                      res_ped[individual][3],
                                                                      res_ped[individual][4],
                                                                      not options.no_norm_founders)
      
    n_gen= len(genotype)
    n_het= sum( (g == 3) for g in genotype )
    n_hom= sum( (g == 1 or g == 2) for g in genotype )
    n_mis= sum( (g == 0) for g in genotype )
    
    if options.full:
        print(options.original,
              options.result,
              individual,
              orig_ped[individual][1],
              orig_ped[individual][2],
              len(genotypes[individual]),
              n_het, n_hom, n_mis,
              err_gen,
              err_pat_hap, err_mat_hap,
              err_mask_pat_hap, err_mask_mat_hap,
              sep="\t")

    tot_gen += n_gen
    tot_het += n_het
    tot_hom += n_hom
    tot_mis += n_mis
    
    tot_abs_err_gen += err_gen
    tot_abs_err_pat_hap += err_pat_hap
    tot_abs_err_mat_hap += err_mat_hap
    tot_abs_err_mask_pat_hap += err_mask_pat_hap
    tot_abs_err_mask_mat_hap += err_mask_mat_hap
    tot_err_gen += (err_gen / n_mis) if n_mis > 0 else 0
    tot_err_pat_hap += (err_pat_hap / (n_het + n_mis)) if (n_het + n_mis) > 0 else 0
    tot_err_mat_hap += (err_pat_hap / (n_het + n_mis)) if (n_het + n_mis) > 0 else 0
    tot_err_mask_pat_hap += (err_mask_pat_hap / n_het) if n_het > 0 else 0
    tot_err_mask_mat_hap += (err_mask_pat_hap / n_het) if n_het > 0 else 0

n_indiv= len(orig_ped.keys())

# tot_gen /= n_indiv
# tot_het /= n_indiv
# tot_hom /= n_indiv
# tot_mis /= n_indiv
tot_err_gen /= n_indiv
tot_err_pat_hap /= n_indiv
tot_err_mat_hap /= n_indiv
tot_err_mask_pat_hap /= n_indiv
tot_err_mask_mat_hap /= n_indiv
          
if not options.full:
    if options.header:
        print('"input file"',
              '"result file"',
              '"pedigree size"',
              '"tot genotype length"',
              '"tot heterozygous loci"',
              '"tot homozygous loci"',
              '"tot missing genotypes"',
              '"tot genotype errors"',
              '"tot paternal haplotype errors"',
              '"tot maternal haplotype errors"',
              '"tot paternal haplotype errors wo missing"',
              '"tot maternal haplotype errors wo missing"',
              '"avg genotype errors"',
              '"avg paternal haplotype errors"',
              '"avg maternal haplotype errors"',
              '"avg paternal haplotype errors wo missing"',
              '"avg maternal haplotype errors wo missing"',
              sep="\t")

    print(options.original,
          options.result,
          n_indiv,
          tot_gen,
          tot_het,
          tot_hom,
          tot_mis,
          tot_abs_err_gen,
          tot_abs_err_pat_hap,
          tot_abs_err_mat_hap,
          tot_abs_err_mask_pat_hap,
          tot_abs_err_mask_mat_hap,
          tot_err_gen,
          tot_err_pat_hap,
          tot_err_mat_hap,
          tot_err_mask_pat_hap,
          tot_err_mask_mat_hap,
          sep="\t")
