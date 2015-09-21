## About

This C library, paired up with [PowerAPI](http://powerapi.org), allows to get the energy footprint of a piece of code associated to a label that can be used as a target in PowerAPI.

## How to compile?

You have first to edit the ```variables.c``` file with your own parameters. 
These parameters should correspond to the ones used in PowerAPI (check the PowerAPI's documentation).
Secondly, check the installation of [libpfm4](http://sourceforge.net/p/perfmon2/libpfm4/ci/master/tree/) and check the ```LIBPFM_LDFLAGS``` in the ```Makefile```.

## How to use?

First, you need to update your C program with these directives:
* ```configure_all_pc(label)``` for opening all the needed counters for representing the piece of code, and for sending the file descriptors to PowerAPI.
* ```start_measure(label)``` for starting to feed the counters with the instructions executed before the last directive presented below.
* ```stop_measure(label)``` for stopping to feed the counters.

You have now to start PowerAPI with the ```libpfm-core-code``` module and starting a new monitoring with a code label as target.
Then, execute the C program and check the output from PowerAPI. 
You should get the energy footprint over the time of your program.

An example of the usage of these directives is described in ```example.c```.

## Acknowledgements

Thanks to [libancillary](https://github.com/mhaberler/libancillary) for exchanging file descriptors between programs.

