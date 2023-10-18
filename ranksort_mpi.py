#!/usr/bin/env python
# coding: utf-8
""" Ranksort with MPI
by
6488004
6488089
6688XXX
 """


import random
import time
from mpi4py import MPI

ARRAY_SIZE_1 = 10000
ARRAY_SIZE_2 = 100000
ARRAY_SIZE_3 = 500000
ARRAY_SIZE_4 = 1000000
ARRAY_SIZE_5 = 2000000
THRESHOLD = 1000


def compare(a, b):
    return a - b


def local_sort(array):
    array.sort()


def ranksort(array):
    ranks = [0] * len(array)
    for i in range(len(array)):
        rank = 0
        for j in range(len(array)):
            if array[j] < array[i]:
                rank += 1
            elif array[j] == array[i] and j < i:
                rank += 1
        ranks[i] = rank
    sorted_array = [0] * len(array)
    for i in range(len(array)):
        sorted_array[ranks[i]] = array[i]
    return sorted_array


def parallel_ranksort(array, comm):
    rank = comm.Get_rank()
    size = comm.Get_size()
    if len(array) <= THRESHOLD:
        local_sort(array)
    else:
        subarrays = [array[i:i+len(array)//size]
                     for i in range(0, len(array), len(array)//size)]
        local_subarray = subarrays[rank]
        local_sorted_subarray = ranksort(local_subarray)
        sorted_subarrays = comm.allgather(local_sorted_subarray)
        merged_subarray = []
        for i in range(len(array)):
            min_val = float('inf')
            min_index = -1
            for j in range(size):
                if len(sorted_subarrays[j]) > 0 and sorted_subarrays[j][0] < min_val:
                    min_val = sorted_subarrays[j][0]
                    min_index = j
            merged_subarray.append(min_val)
            sorted_subarrays[min_index].pop(0)
        return merged_subarray


if __name__ == '__main__':
    array_sizes = [ARRAY_SIZE_1, ARRAY_SIZE_2,
                   ARRAY_SIZE_3, ARRAY_SIZE_4, ARRAY_SIZE_5]
    num_nodes_list = [2, 4, 8, 12, 16]
    comm = MPI.COMM_WORLD
    for array_size in array_sizes:
        array = [random.randint(0, 99999) for _ in range(array_size)]
        # print(array)
        for num_nodes in num_nodes_list:
            if comm.Get_rank() == 0:
                start_time = time.time()
            subarray_size = array_size // num_nodes
            local_array = [0] * subarray_size
            comm.Scatter(array, local_array, root=0)
            sorted_local_array = parallel_ranksort(local_array, comm)
            sorted_array = comm.gather(sorted_local_array, root=0)
            if comm.Get_rank() == 0:
                merged_array = []
                for subarray in sorted_array:
                    merged_array += subarray
                end_time = time.time()
                print(
                    f"Execution time for {num_nodes} nodes and {array_size} elements: {end_time - start_time} seconds")


# In[ ]:
