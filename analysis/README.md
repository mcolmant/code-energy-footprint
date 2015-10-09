## About

This C library allows to instrument each call of a program with a very low overhead.
It has to be linked during the compilation of the targeted program.

## Use case

We want to analyze the power consumption of calls of a program called ```example```.
We have to:
* Compile the program with required flags: ```gcc -finstrument-functions -c example.c```
* Link the programs: ```gcc -o example example.o trace.o```

The program can now be used in PowerAPI to monitor the power consumption of functions over the time for the example program.

For this purpose, we have to use the ```powerapi-code-energy-analysis``` project and to built it with ```sbt-native-packager```.
Then edit the ```code-energy-analysis.conf``` file with your own parameters and add an entry inside the ```powerapi.code-energy-analysis.workloads``` variable.

In our example, it should be defined as:
```
powerapi.code-energy-analysis.workloads = [
  { name = "example", binary-path = "/home/powerapi/example", script = "scripts/start-example.bash" }
]
```

Next, create the script ```scripts/start-example.bash``` and add:
```
#!/bin/bash

# This line is required. Don't edit.
cd results/$1

# Edit with the correct binary path (should be the same as the one written inside the configuration file)
/home/powerapi/example

exit 0
```

To finish, execute the command ```sudo ./bin/powerapi-code-energy-analysis``` and check the results written inside ```results/powers.txt```.

