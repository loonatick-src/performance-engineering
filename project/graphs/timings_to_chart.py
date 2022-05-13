sizes = [128, 256, 512, 1024, 2048]
sequential = [4.39333, 10.1078, 16.1195, 32.6979, 61.5664]
omp_4 = [5.40449, 10.1135, 19.3992, 27.7449, 52.7681]
omp_8 = [6.00997, 11.7645, 19.8323, 30.4133, 57.8063]
omp_16 = [6.26695, 11.4344, 20.9347, 42.2966, 79.7227]

import matplotlib.pyplot as plt
import numpy as np

plt.plot(sizes, sequential, label="Sequential")
plt.plot(sizes, omp_4, label="Omp 4")
plt.plot(sizes, omp_8, label="Omp 8")
plt.plot(sizes, omp_16, label="Omp 16")
plt.xticks(sizes, sizes)
plt.ylabel('Time in seconds')
plt.xlabel('Output image height')
plt.legend()
plt.show()
