# See LICENSE.txt for license details.
CXX_FLAGS += -std=c++11 -O3 -Wall
PAR_FLAG = -fopenmp
#GENERATE = -D GENERATE
GENERATE = 
TIMING = -D TIMING
PARALLEL = -D PARALLEL
COMPARISON = -D COMPARISON

ifneq (,$(findstring icpc,$(CXX)))
	PAR_FLAG = -openmp
endif

ifneq ($(SERIAL), 1)
	CXX_FLAGS += $(PAR_FLAG)
endif

APPS = spmv csc_spmv csr_spmv coo_spmv csr_spmv_par csc_spmv_par coo_spmv_par csr_comp csc_comp coo_comp

.PHONY: all

all: $(APPS)

spmv: spmv.c mmio.c spmv_kernel.c
	$(CXX) $(CXX_FLAGS) $(GENERATE) $(TIMING) -o $@ $^ $(LFLAGS)

csr_spmv: spmv.c mmio.c csr_spmv_kernel.c
	$(CXX) $(CXX_FLAGS) $(GENERATE) $(TIMING) -D CSR -o $@ $^ $(LFLAGS)

csc_spmv: spmv.c mmio.c csc_spmv_kernel.c
	$(CXX) $(CXX_FLAGS) $(GENERATE) $(TIMING) -D CSC -o $@ $^ $(LFLAGS)

coo_spmv: spmv.c mmio.c coo_spmv_kernel.c
	$(CXX) $(CXX_FLAGS) $(GENERATE) $(TIMING) -D COO -o $@ $^ $(LFLAGS)

csr_spmv_par: spmv.c mmio.c csr_spmv_kernel.c
	$(CXX) $(CXX_FLAGS) $(GENERATE) $(TIMING) $(PARALLEL) -D CSR -o $@ $^ $(LFLAGS)

csc_spmv_par: spmv.c mmio.c csc_spmv_kernel.c
	$(CXX) $(CXX_FLAGS) $(GENERATE) $(TIMING) $(PARALLEL) -D CSC -o $@ $^ $(LFLAGS)

coo_spmv_par: spmv.c mmio.c coo_spmv_kernel.c
	$(CXX) $(CXX_FLAGS) $(GENERATE) $(TIMING) $(PARALLEL) -D COO -o $@ $^ $(LFLAGS)

csr_comp: spmv.c mmio.c csr_spmv_kernel.c
	$(CXX) $(CXX_FLAGS) $(GENERATE) $(TIMING) $(COMPARISON) $(PARALLEL) -D CSR -o $@ $^ $(LFLAGS)

csc_comp: spmv.c mmio.c csc_spmv_kernel.c
	$(CXX) $(CXX_FLAGS) $(GENERATE) $(TIMING) $(COMPARISON) $(PARALLEL) -D CSC -o $@ $^ $(LFLAGS)

coo_comp: spmv.c mmio.c coo_spmv_kernel.c
	$(CXX) $(CXX_FLAGS) $(GENERATE) $(TIMING) $(COMPARISON) $(PARALLEL) -D COO -o $@ $^ $(LFLAGS)


.PHONY: clean
clean:
	rm -f $(APPS)
