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
#  hc-generator.py
#
#  A program that generates a random zero-recombinant set of (partially
#  missing) genotypes given a pedigree structure.
#
##########

import sys
import random
import logging
from optparse import OptionParser

def parse_command_line():
    usage= "usage: %prog [options]"
    parser= OptionParser(usage=usage)
    parser.add_option("-l", "--genotype_length",
                      action="store", dest="length",
                      type="int", default=100,
                      help="the desired genotype length",
                      metavar="LENGTH")
    parser.add_option("-m", "--missing_genotype_probability",
                      action="store", dest="missing",
                      type="float", default=0.1,
                      help="the probability that a single-locus genotype has not been called",
                      metavar="PROBABILITY [0,1]")
    parser.add_option("-s", "--seed",
                      action="store", dest="seed",
                      type="int", default=122295,
                      help="the seed of the random generator",
                      metavar="INT")
    parser.add_option("-v", "--verbose",
                      action="store_true", dest="verbose",
                      default=False,
                      help="print additional log messages")
    (options, args) = parser.parse_args()
    return(options)

options = parse_command_line()

length= options.length
missing_genotype_prob= options.missing
seed= options.seed
log_level= logging.DEBUG if options.verbose else logging.INFO
allele1= "1"
allele2= "2"
homo1= "1 1"
homo2= "2 2"
heter= "1 2"
missing= "0 0"
pheno= "phenotype"

random.seed(seed)
logging.basicConfig(level=log_level,
                    format='%(levelname)-6s [%(asctime)s]  %(message)s')

logging.info("ZERO-RECOMBINANT GENOTYPED PEDIGREE GENERATION WITH MISSING DATA")

logging.info("Haplotype encoding: "
             "allele1= '%s', allele2= '%s'",
             allele1, allele2)
logging.info("Genotype encoding:  "
             "homo1= '%s', homo2= '%s', heter= '%s', missing= '%s'",
             homo1, homo2, heter, missing)
logging.info("Genotype length: %d", length)
logging.info("Missing genotype probability: %f", missing_genotype_prob)
logging.info("Seed: %d", seed)



# Compute the mapping between haplotypes and genotypes
mapping={}
mapping[(allele1, allele1)]= homo1
mapping[(allele1, allele2)]= heter
mapping[(allele2, allele1)]= heter
mapping[(allele2, allele2)]= homo2


pedigree= set()
fathers= {}
mothers= {}
genders= {}
for f in sys.stdin:
    f= f.rstrip()
    (family, ind, father, mother, gender)= f.split()
    pedigree.add(ind)
    fathers[ind]= father
    mothers[ind]= mother
    genders[ind]= gender
    logging.debug("Read individual %4s.  Father=%4s  Mother=%4s  Gender= %s",
                  ind, father, mother, "male" if gender == 1 else "female")

logging.info("Pedigree size: %6d", len(pedigree))

hp={}
hm={}
gpp={} # Grand-parental sources
gpm={}

# Compute haplotypes of founders
founders=[ ind for ind in pedigree if fathers[ind] == "0" and mothers[ind] == "0" ]
logging.info("No. of founders: %4d", len(founders))
for ind in founders:
    logging.debug("Computing the haplotypes of founder %7s", ind)
    hp[ind]= [random.choice((allele1, allele2)) for i in range(length)]
    hm[ind]= [random.choice((allele1, allele2)) for i in range(length)]
    gpp[ind]= 0
    gpm[ind]= 0

genotyped= set(founders)
remaining= pedigree - genotyped

# Compute the haplotypes of the remaining individuals
logging.info("Not-haplotyped individuals: %4d", len(remaining))
while len(remaining)>0:
    for ind in remaining:
        if fathers[ind] in genotyped and mothers[ind] in genotyped:
            logging.debug("Computing the haplotypes of individual %4s", ind)
            genotyped.add(ind)
            gpp[ind]= random.randint(0,1)
            gpm[ind]= random.randint(0,1)
            hp[ind]= hp[fathers[ind]] if gpp[ind] == 0 else hm[fathers[ind]]
            hm[ind]= hp[mothers[ind]] if gpm[ind] == 0 else hm[mothers[ind]]
    remaining -= genotyped
    logging.info("Not-haplotyped individuals: %4d", len(remaining))

# Computes the corresponding genotypes
logging.info("Computing the corresponding genotypes...")
genotypes={}
for ind in pedigree:
    genotypes[ind]= [mapping[(a,b)] for a,b in zip(hp[ind],hm[ind])]
    genotypes[ind]= [g if (random.random() >= missing_genotype_prob) else missing
                     for g in genotypes[ind] ]

# Print haplotype configuration as comment
logging.info("Saving the generated haplotype configuration...")
for ind in pedigree:
    print("# GENERATED_HAPLOTYPES", 0, ind, fathers[ind], mothers[ind], genders[ind], pheno,
          "\t".join(["{}|{}".format(a,b)
                      for a,b in zip(hp[ind], hm[ind]) ] ),
          sep="\t")

# Print the genotyped pedigree
logging.info("Saving the genotyped pedigree...")
for ind in pedigree:
    print(0, ind, fathers[ind], mothers[ind], genders[ind], pheno,
          "\t".join(genotypes[ind]), sep="\t")

logging.info("Terminated!")
