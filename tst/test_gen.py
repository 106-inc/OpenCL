import numpy as np
import numpy.random as rand
import argparse

min_val = -5
max_val = 5
vec_size = 10

parser = argparse.ArgumentParser(description= 'Hello, Im data-generator!')

parser.add_argument('min_val', metavar= 'INPUT', type = int, help = 'minimal value for your array')
parser.add_argument('max_val', metavar= 'INPUT', type = int, help = 'maximum value for your array')
parser.add_argument('vec_size', metavar= 'INPUT', type = int, help = 'size of your array')

args = parser.parse_args()


vec = np.random.randint(args.min_val, args.max_val, args.vec_size)

res = ' '.join(map(str, vec))

print(args.vec_size)
print(res)

