Low Energy Adaptive Clustering Hierarchy (LEACH) in ns3
==========================================================

#####What?
A simulation in [ns3](http://www.nsnam.org/) of the clustering and energy profiles of
a Wireless sensor network WSN based on the LEACH , mainting a low energy footprint to
extend the lifetime of the clusters and hence the lifetime of the network.

#####How?
Download and extract the project into your ns3 root src directory and execute the build 
by running
```bash
$./waf
```

then

```bash
$./waf --run scratch/leach
```

#####Limitations?
All config files variables are currently hard coded in the consts file and the simulation
is known to work with ns-allinone-3.19, other versions not tested with it yet.

#####Simulation

[Sim](https://asciinema.org/a/9544)





