string = "10415190 |    10385540 |    10412120 |    10384060 |    10437180 |    10363050 |    10381590 |    10382940 |    10425850 |    10384670 |    10414200 |    10390930 |    10439890 |    10368190 |    10391420 |    10385900"

string_solved = "14 |         17 |          6 |         17 |         20 |         14 |         31 |          4 |         27 |         25 |         30 |         18 |         20 |         17 |         19 |         11"

import matplotlib.pyplot as plt
import numpy as np

counters = [int(x) for x in string_solved.split(" | ")]
indexes = np.arange(len(counters))
plt.bar(indexes, counters)
plt.ylabel('MEM_LOAD_UOPS_L3_HIT_RETIRED_XSNP_HITM')
plt.xlabel('Core index')
plt.ylim(ymin=0)
plt.show()