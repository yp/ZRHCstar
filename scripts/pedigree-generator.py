#!/usr/bin/python3

# Script to generate a pedigree of a given size
import sys
import random
import logging
from optparse import OptionParser

def parse_command_line():
    usage= "usage: %prog [options]"
    parser= OptionParser(usage=usage)
    parser.add_option("-p", "--pedigree-size",
                      action="store", dest="ped_size",
                      type="int", default=100,
                      help="the desired number of individuals",
                      metavar="SIZE")
    parser.add_option("-f", "--pedigree-founders",
                      action="store", dest="ped_founders",
                      type="int", default=2,
                      help="the desired number of initial founders",
                      metavar="SIZE")
    parser.add_option("-d", "--dot-file",
                      action="store", dest="dot_file",
                      type="string", default=None,
                      help="if specified, write a DOT-representation of the pedigree to the given file",
                      metavar="FILE")
    parser.add_option("-c", "--additional-child-probability",
                      action="store", dest="p_add_child",
                      type="float", default=0.4,
                      help="the probability that a nuclear family has an additional children",
                      metavar="[0,1]")
    parser.add_option("-r", "--generation-reusing-probability",
                      action="store", dest="p_reuse_ind",
                      type="float", default=0.2,
                      help="the probability that a generation is composed by different nuclear families",
                      metavar="[0,1]")
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

def create_founder(gender,
                   pedigree, fathers, mothers, genders):
    ind= len(pedigree) + 1
    logging.debug("Creating new founder %4d", ind)
    pedigree.add(ind)
    fathers[ind]= 0
    mothers[ind]= 0
    genders[ind]= gender
    return ind

def extract_or_create(base_set, gender,
                      pedigree, fathers, mothers, genders):
    if len(base_set) == 0:
        ind= create_founder(gender, pedigree, fathers, mothers, genders)
    else:
        ind= base_set.pop()

    return ind





options = parse_command_line()

ped_size = options.ped_size
ped_founders = options.ped_founders

p_add_child = options.p_add_child
p_reuse_ind = options.p_reuse_ind

seed= options.seed
log_level= logging.DEBUG if options.verbose else logging.INFO
random.seed(seed)
logging.basicConfig(level=log_level,
                    format='%(levelname)-6s [%(asctime)s]  %(message)s')

logging.info("PEDIGREE GENERATION")

logging.info("Pedigree size: %d", ped_size)
logging.info("No. of initial founders: %d", ped_founders)
logging.info("Additional child probability:   %f", p_add_child)
logging.info("Individual reusing probability: %f", p_reuse_ind)
logging.info("Seed: %d", seed)


pedigree = { ind for ind in range(1, ped_founders+1) }
fathers  = { ind:0 for ind in pedigree }
mothers  = { ind:0 for ind in pedigree }

genders = { ind:((ind-1)%2)+1 for ind in pedigree }

males   = { ind for ind, sex in genders.items() if sex == 1 }
females = { ind for ind, sex in genders.items() if sex == 2 }

logging.info("Starting pedigree generation...")
while len(pedigree) < ped_size:
    logging.info("Current pedigree size: %4d/ %4d",
                 len(pedigree), ped_size)
    new_gen= False
    children= set()
    parents= set()
    while len(pedigree) < ped_size and not new_gen:
        rnd_choice= random.randint(1, 3)
        if len(pedigree) == ped_size - 1:
            rnd_choice= 3
        if rnd_choice == 1 and len(males) == 0:
            rnd_choice= 2
        if rnd_choice == 2 and len(females) == 0:
            rnd_choice= 1
        logging.debug("Choice: %s",
                      "create a new founder for mother"
                      if rnd_choice == 1
                      else ("create a new founder for father" if rnd_choice == 2
                            else "extract old individuals as parents")
                      )
        if rnd_choice == 1:
            father= extract_or_create(males, 1,
                                      pedigree, fathers, mothers, genders)
            mother= create_founder(2, pedigree, fathers, mothers, genders)
        elif rnd_choice == 2:
            father= create_founder(1, pedigree, fathers, mothers, genders)
            mother= extract_or_create(females, 2,
                                      pedigree, fathers, mothers, genders)
        else:
            father= extract_or_create(males, 1,
                                      pedigree, fathers, mothers, genders)
            mother= extract_or_create(females, 2,
                                      pedigree, fathers, mothers, genders)
        logging.debug("Current parents: father= %4d  -  mother= %4d", father, mother)
        
        p_child = 1.0
        while len(pedigree) < ped_size and random.random() < p_child:
            ind= len(pedigree)+1
            pedigree.add(ind)
            children.add(ind)
            fathers[ind]=father
            mothers[ind]=mother
            gender = random.randint(1,2)
            genders[ind]=gender
            p_child = p_add_child
            logging.debug("  new child= %4d   gender= %s", ind, "male" if gender == 1 else "female")
        
        if random.random() < p_reuse_ind:
            logging.debug("Re-inserting individual %4d", father)
            males.add(father)
            parents.add(father)
        if random.random() < p_reuse_ind:
            logging.debug("Re-inserting individual %4d", mother)
            females.add(mother)
            parents.add(mother)
        
        new_gen= random.random() < 0.5

    logging.debug("New generation: %s", children)

    males = males - parents
    males.update({ind for ind in children if genders[ind] == 1})
    logging.debug("Males:   %s", males)

    females = females - parents
    females.update({ind for ind in children if genders[ind] == 2})
    logging.debug("Females: %s", females)
    
logging.info("Saving the pedigree...")
for ind in pedigree:
    print("0",ind,fathers[ind],mothers[ind],genders[ind],sep="\t")


if options.dot_file:
    logging.info("Saving a DOT-representation of the pedigree to file '%s'...",
                 options.dot_file)
    with open(options.dot_file, 'w') as dot:
        print("digraph G {", file=dot)
        for ind in pedigree:
            print("n{0} [label=\"{0}\", shape={1}];"
                  .format(ind,
                          "box" if genders[ind] == 1
                          else "ellipse"),
                  file=dot)
        for ind in pedigree:
            if fathers[ind] > 0:
                print("n{} -> n{};".format(fathers[ind], ind),
                      file=dot)
            if mothers[ind] > 0:
                print("n{} -> n{};".format(mothers[ind], ind),
                      file=dot)
        dot.write("}")

logging.info("Terminated!")
