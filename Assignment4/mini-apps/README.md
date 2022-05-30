# Load Imbalance
The function `test_imbalanced` performance a variable amount of work depending on its argument. We create an array of arguments and
map this function to that array using a parallel for loop.

The array of arguments is monotonically increasing, which means that threads that get assigned the higher indices are bound to get
more work.

## Diagnosis
### Pattern
Saturating speedup with increasing thread count.

### Performance Counters
The instruction count should in general be different for each core. Since the kernel uses the floating point functions `sin`and `asin`,
and the difference in load comes from how many times these functions are called, we can simply count the number of floating point
instuctions. No other part of the kernel uses floating point instructions.

# False Sharing
Histogram is the poster child for false sharing heheh. Many threads and few cache lines should do the trick.
### Pattern
Decreasing speedup, even slowdown, with increasing number of threads.
### Diagnosis
1. Just a proposal -- The total number of L3/DRAM references should increase dramatically as we go from one thread to more threads.
2. Does PAPI provide some derived events for false sharing?

# ccNUMA
Relying on first-touch allocation to work - allocating using `calloc`should put everything in the same NUMA domain. If we let the
thread count be larger than 16, some threads should be located in a different domain.

### Pattern
TODO
### Diagnosis
TODO
There are probably some counters specific for comms across NUMA domains.
