#!/usr/bin/env python3

import os
from multiprocessing import Process, Pipe


def process_1(file_names, pipe_1_2):
    for file_name in file_names:
        with open(file_name, "r") as file:
            for line in file:
                pipe_1_2.send(line)

    pipe_1_2.send(None)


def process_2(pipe_1_2, pipe_2_3):
    while True:
        line = pipe_1_2.recv()

        if line is None:
            break

        processed_line = line.upper().rstrip() + '\n'
        pipe_2_3.send(processed_line)

    pipe_2_3.send(None)


def process_3(pipe_2_3):
    while True:
        line = pipe_2_3.recv()

        if line is None:
            break

        print(line, end='')


def main():
    file_names = input("Enter file names (separated by spaces): ").split()
    pipe_1_2, pipe_2_3 = Pipe()

    p1 = Process(target=process_1, args=(file_names, pipe_1_2))
    p2 = Process(target=process_2, args=(pipe_1_2, pipe_2_3))
    p3 = Process(target=process_3, args=(pipe_2_3,))

    p1.start()
    p2.start()
    p3.start()

    p1.join()
    p2.join()
    p3.join()


if __name__ == '__main__':
    main()
