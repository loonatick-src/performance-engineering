import matplotlib.pyplot as plt
import numpy as np

def originalScaling():
  sizes = [128, 256, 512, 1024, 2048]
  sequential = [4.06075, 8.03402, 14.9976, 29.8789, 57.3822]
  plt.plot(sizes, sequential, label="Sequential")
  plt.xticks(sizes, sizes)
  plt.ylabel('Time in seconds')
  plt.xlabel('Output image height')
  plt.legend()
  plt.show()

def randVsRandr():
  sizes = [1, 2, 4, 8, 16]
  rand = [0.945178, 12.7659, 18.3465, 17.0725, 17.6324]
  randr = [0.536634, 0.385408, 0.17928, 0.124971, 0.0838557]

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

  plt.plot(sizes, shared_ptr, label="shared_ptr")
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
  runs = 2147483647
  types =     ['ray hit',      'ray miss']
  box_times = [52.3946/runs  , 36.0235/runs]
  obox_times= [89.5574/runs  , 56.6143/runs]
  ind = np.arange(len(types))
  width = 0.35

  fig, ax = plt.subplots()
  rects1 = ax.bar(ind - width/2, box_times, width, label='Optimized')
  rects2 = ax.bar(ind + width/2, obox_times, width, label='Original')

  ax.set_ylabel('Seconds per call')
  ax.set_title('Ray type')
  ax.set_xticks(ind)
  ax.set_xticklabels(types)
  ax.legend()
  fig.tight_layout()
  plt.show()
  
  
def parseResultFile(filename):
  content = open(filename, "r").read()
  entries = [x for x in [[l for l in x.split("\n") if l != ''] for x in content.split("CONFIG")] if len(x) > 1]
  parsed = [
    {
      'threads': int(entry[0].split(" = ")[1]),
      'width': int(entry[1].split(" = ")[1]),
      'height': int(entry[2].split(" = ")[1]),
      'max_depth': int(entry[3].split(" = ")[1]),
      'samples_per_pixel': int(entry[4].split(" = ")[1]),
      'seconds_to_render': float(entry[5].split(" ")[-2])
    }
    for entry in entries
  ]
  for p in parsed:
    p['total_rays'] = p['width'] * p['height'] * p['samples_per_pixel']
    p['seconds_per_ray'] = p['seconds_to_render'] / p['total_rays']
    p['rays_per_second'] = p['total_rays'] / p['seconds_to_render']

  return parsed

def original_versions():
  parsed = parseResultFile("../measurements/v0_results.txt")
  
  sizes =  [128, 256, 512, 1024, 2048]
  sequential =  [next(e for e in parsed if e['threads'] == 1 and e['height'] == s)['seconds_to_render'] for s in sizes]

  plt.plot(sizes, sequential, label="Base framework")
  plt.xticks(sizes, sizes)
  plt.ylabel('Time in seconds')
  plt.xlabel('Output image height')
  plt.legend()
  plt.show()

def parallel_versions(filename):
  base = parseResultFile("../measurements/v0_results.txt")
  parsed = parseResultFile(filename)
  
  _sizes = np.array([22, 32, 45, 64, 90, 128, 181, 256, 362])
  sizes = _sizes ** 2
  sequential =  [next(e for e in base if e['threads'] == 1 and e['height'] == s)['seconds_to_render'] for s in _sizes]
  omp_1 =  [next(e for e in parsed if e['threads'] == 1 and e['height'] == s)['seconds_to_render'] for s in _sizes]
  omp_2 =  [next(e for e in parsed if e['threads'] == 2 and e['height'] == s)['seconds_to_render'] for s in _sizes]
  omp_4 =  [next(e for e in parsed if e['threads'] == 4 and e['height'] == s)['seconds_to_render'] for s in _sizes]
  omp_8 =  [next(e for e in parsed if e['threads'] == 8 and e['height'] == s)['seconds_to_render'] for s in _sizes]
  omp_16 = [next(e for e in parsed if e['threads'] == 16 and e['height'] == s)['seconds_to_render'] for s in _sizes]

  plt.plot(sizes, sequential, label="Base framework")
  plt.plot(sizes, omp_1, label="Omp 1")
  plt.plot(sizes, omp_2, label="Omp 2")
  plt.plot(sizes, omp_4, label="Omp 4")
  plt.plot(sizes, omp_8, label="Omp 8")
  plt.plot(sizes, omp_16, label="Omp 16")
  plt.xticks(sizes, sizes)
  plt.ylabel('Time in seconds')
  plt.xlabel('Output image height')
  plt.legend()
  plt.show()


# boxIntersectionBars()
# parallel_versions("../measurements/v4_results.txt")

sharedPtr()