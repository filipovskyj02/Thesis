This directory contains the cpp core enginge used for the cpp language benchmarks, just like all the persistance benchmarks.
The persistance methods can be swapped by uncommenting the wanted logger (the file logger requires a text file name in it's constructor, others do not), otherwise all the methods are the same, so persistance methods can be hotswapped.
Postgres requires a running db server on the local machine with the correct table (described in the thesis) created.
Kdb requires a running kdb+ instance locally as well.
