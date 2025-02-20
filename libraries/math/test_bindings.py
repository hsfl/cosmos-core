import sys
import os

# Dynamically calculate the absolute path to `core/python/modules`
repo_root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
module_path = os.path.join(repo_root, "python", "modules")
if module_path not in sys.path:
    sys.path.insert(0, module_path)

import Vectors

v = Vectors.rvector(1.0, 2.0, 3.0)
print(v)  # Output: rvector(1.0, 2.0, 3.0)
v[0] = 5.0
print(v)  # Output: rvector(5.0, 2.0, 3.0)

v1 = Vectors.rvector()
print("v1=",v1,sep="")

for i in range(5):
    if i == 5:
        break
else:
    print("Loop finished without a break")

