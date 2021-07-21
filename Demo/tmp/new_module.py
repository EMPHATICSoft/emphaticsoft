#!/usr/bin/env python

import os

tplDir = os.environ['SRT_PRIVATE_CONTEXT']+'/Demo/templates/'
if not os.path.isdir(tplDir):
  tplDir = os.environ['SRT_PUBLIC_CONTEXT']+'/Demo/templates/'

if not os.path.isdir(tplDir):
  print 'Unable to find templates.'
  exit(1)

module = raw_input('Module name? ')
namespace = raw_input('Namespace? ')

reps = {}
reps['MODULENAME'] = module
reps['NAMESPACE'] = namespace
reps['FCLNAME'] = module.lower()
reps['AUTHOR'] = os.environ['USER']
reps['INCLUDEGUARD'] = namespace.upper()+'_'+module.upper()+'_H'

for ext in ['.cxx', '.h', '.fcl', '_module.cc']:
  fin = open(tplDir+'Template'+ext)
  fout = open(module+ext, 'w')

  for line in fin:
    for key, val in reps.items():
      line = line.replace(key, val)
    fout.write(line)

  print 'Created', fout.name
