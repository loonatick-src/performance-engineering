# TODO
## Taking into account NUMA topology
`likwid-topology` shows the, well, topology of the hardware on your
node. This includes the caches, threads per core, and NUMA nodes.
When constructing performance models, how do we take into account NUMA
topology?

## Non-allocating stores
Write-back vs write-through is what I thought when I saw this for the
first time. This is mentioned as an explicit optimisation in the
roofline paper. Do we really have  control over write-through vs
write-back?

## Hardware Locality
Memory locality one of the oft-cited optimisations. How exactly
is the performance impact due to this measured? Once measured,
how do we optimise for it?

## Data Sheets
Vendor-provided performance manuals and data sheets are one way to
construct roofline models. Gotta locate and store them for reference.

## Hardware Threads
(As opposed to virtual threads?) What exactly is meant by hwthread?
How does hyperthreading (i.e. more than one thread per core on Intel
archs)
affect performance? As in what is the difference between 6 cores per socket with
2 threads per core, and 12 threads per socket with 1 thread per core?

# Roofline
Calculating this requires some measurements/input data. This can come
either from data sheets and manuals, or from benchmarks.

See [https://github.com/RRZE-HPC/likwid/wiki/Tutorial%3A-Empirical-Roofline-Model][this tutorial] for more.
## Maximum Performance
### Data Sheet
`<cores> * <simd-lanes> * <ops-per-cycle> * <num-fma> * <clock-speed>`.
This is the peak theoretical performance, but may not necessarily be
the "achievable" peak FP performance. NB if your application's comute
does not use FP, then you will have to look at the data on the relevant
functional units instead of FP units.

### Benchmarking
The idea is to run a synthetic kernel that executes so many flops
on values in registers that everything else, e.g.load operations and
loop bounds checking, are negligible.

You need to use a stream size that fits in the L1 cache for each
core. `likwid` ships with benchmarks for different kinds of peak
floating point performances by the names of `peakflops_*`, e.g.
`peakflops_avx_fma`, `peakflops_avx512`, `peakflops_avx512_fma` et cetera.


## Maximum Data Throughput
### Data Sheet
`<sockets> * <mem_channels> * <mem_frequency> * <mem_controllers> * <data_transfer_unit_size>`

### Benchmarking
STREAM, TheBandwidthBenchmark, or `likwid-bench`.

NB authors of the roofline paper note in the appendix that STREAM
does not really measure peak memory throughput, and used a modified
stream benchmark instead.

For the roofline model, it might be beneficial to use a benchmark
kernel that somehow relates to your application code. This is because
memory bandwidth can be a moving target; the bandwidth ceiling will
be more realistic if you choose a streaming kernel with a read/write
ratio that is similar to the kernel that you want to analyse. Else
your model might be too optimistic.

`likwid-bench` comes with various benchmarks
- `load*`
- `copy*`
- `stream*` (axpy, but assign result to third vector)
- `triad*` (`A[i] = B[i] + C[i] * D[i]`)
For proper data placement (local to hwthread), use `-W` for workgroup
definitions, which performs proper parallel first-touch placement.
With `-w`, the first thread in the WG that allocates and initializes
the memory, which would shit up the locality because of the first-touch
is performed only by that thread.

## Constructing Roofline
LIKWID stands for "Like I Know What I'm Doing", but here you know
what you're doing, yeah?

## Placing your kernel
We need AI and performance. Use `likwid-perfctr` with the groups
`MEM_DP` and `MEM_SP` for double and single precision respectively.
It is beneficial to use the Marker API around the application
region of interest to get more meaningful measures.
