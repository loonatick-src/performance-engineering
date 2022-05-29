def originalScaling():
  sizes = [128, 256, 512, 1024, 2048]
  sequential = [4.0751, 8.02996, 15.0113, 29.9553, 58.4266]

  import matplotlib.pyplot as plt
  import numpy as np

  plt.plot(sizes, sequential, label="Sequential")
  plt.xticks(sizes, sizes)
  plt.ylabel('Time in seconds')
  plt.xlabel('Output image height')
  plt.legend()
  plt.show()


def beforeRandAndShared():
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

def randVsRandr():
  sizes = [1, 2, 4, 8, 16]
  rand = [0.945178, 12.7659, 18.3465, 17.0725, 17.6324]
  randr = [0.536634, 0.385408, 0.17928, 0.124971, 0.0838557]

  import matplotlib.pyplot as plt
  import numpy as np

  plt.plot(sizes, rand, label="rand")
  plt.plot(sizes, randr, label="randr")
  plt.xticks(sizes, sizes)
  plt.ylabel('Time in seconds')
  plt.xlabel('Number of threads')
  plt.legend()
  plt.show()

def sharedPtr():
  sizes = [1, 2, 4, 8, 16]
  shared_ptr = [1.81601, 5.43741, 6.72079, 7.54437, 8.32837]

  import matplotlib.pyplot as plt
  import numpy as np

  plt.plot(sizes, rand, label="shared_ptr")
  plt.xticks(sizes, sizes)
  plt.ylabel('Time in seconds')
  plt.xlabel('Number of threads')
  plt.legend()
  plt.show()

def boxIntersectionBars():
  """
     box hit  took 52.3946 runs(2147483647)
    obox hit  took 89.5574 runs(2147483647)
     box miss took 36.0235 runs(2147483647)
    obox miss took 56.6143 runs(2147483647)
  """
