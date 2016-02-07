# -*- coding: utf-8 -*-
# Tetris AI v0.1
# Related to tetris project of Organization Laboratory
# Author: Frank-the-Obscure (Frank Hu)
# This file contains constants and class of Tetromino

# define constants
MAX_HEIGHT = 20
MAX_WIDTH = 10
BLANK_STATUS = [[0 for i in range(0, MAX_WIDTH)] for j in range(0, MAX_HEIGHT)]

# define tetrominos
class Tetromino:
    def __init__(self, name):
        """Init name, images, max_rotate, max_width.

        images: 2d matrix
        max_rotate: to reduce duplicates
        max_width: to reduce invalid position in searching"""
        self.name = name
        if name is 's':
            self.images = [
              [0, 1, 1, 0,
               1, 1, 0, 0,
               0, 0, 0, 0,
               0, 0, 0, 0],

              [0, 1, 0, 0,
               0, 1, 1, 0,
               0, 0, 1, 0,
               0, 0, 0, 0],

              [0, 0, 0, 0,
               0, 1, 1, 0,
               1, 1, 0, 0,
               0, 0, 0, 0],

              [1, 0, 0, 0,
               1, 1, 0, 0,
               0, 1, 0, 0,
               0, 0, 0, 0]
              ]
            self.max_rotate = 2
            self.max_width = [8, 9, 8, 9]

        elif name is 'z':
            self.images = [
              [1, 1, 0, 0,
               0, 1, 1, 0,
               0, 0, 0, 0,
               0, 0, 0, 0],

              [0, 0, 1, 0,
               0, 1, 1, 0,
               0, 1, 0, 0,
               0, 0, 0, 0],

              [0, 0, 0, 0,
               1, 1, 0, 0,
               0, 1, 1, 0,
               0, 0, 0, 0],

              [0, 1, 0, 0,
               1, 1, 0, 0,
               1, 0, 0, 0,
               0, 0, 0, 0]]
            self.max_rotate = 2
            self.max_width = [8, 9, 8, 9]

        elif name is 'l':
            self.images = [
              [0, 0, 1, 0,
               1, 1, 1, 0,
               0, 0, 0, 0,
               0, 0, 0, 0],

              [0, 1, 0, 0,
               0, 1, 0, 0,
               0, 1, 1, 0,
               0, 0, 0, 0],

              [0, 0, 0, 0,
               1, 1, 1, 0,
               1, 0, 0, 0,
               0, 0, 0, 0],

              [1, 1, 0, 0,
               0, 1, 0, 0,
               0, 1, 0, 0,
               0, 0, 0, 0]]
            self.max_rotate = 4
            self.max_width = [8, 9, 8, 9]

        elif name is 'j':
            self.images = [
              [1, 0, 0, 0,
               1, 1, 1, 0,
               0, 0, 0, 0,
               0, 0, 0, 0],

              [0, 1, 1, 0,
               0, 1, 0, 0,
               0, 1, 0, 0,
               0, 0, 0, 0],

              [0, 0, 0, 0,
               1, 1, 1, 0,
               0, 0, 1, 0,
               0, 0, 0, 0],

              [0, 1, 0, 0,
               0, 1, 0, 0,
               1, 1, 0, 0,
               0, 0, 0, 0]]
            self.max_rotate = 4
            self.max_width = [8, 9, 8, 9]

        elif name is 'i':
            self.images = [
              [0, 0, 0, 0,
               1, 1, 1, 1,
               0, 0, 0, 0,
               0, 0, 0, 0],

              [0, 0, 1, 0,
               0, 0, 1, 0,
               0, 0, 1, 0,
               0, 0, 1, 0],

              [0, 0, 0, 0,
               0, 0, 0, 0,
               1, 1, 1, 1,
               0, 0, 0, 0],

              [0, 1, 0, 0,
               0, 1, 0, 0,
               0, 1, 0, 0,
               0, 1, 0, 0]]
            self.max_rotate = 2
            self.max_width = [7, 10, 7, 10]

        elif name is 'o':
            self.images = [
             [0, 1, 1, 0,
              0, 1, 1, 0,
              0, 0, 0, 0,
              0, 0, 0, 0],

             [0, 1, 1, 0,
              0, 1, 1, 0,
              0, 0, 0, 0,
              0, 0, 0, 0],

             [0, 1, 1, 0,
              0, 1, 1, 0,
              0, 0, 0, 0,
              0, 0, 0, 0],

             [0, 1, 1, 0,
              0, 1, 1, 0,
              0, 0, 0, 0,
              0, 0, 0, 0]]
            self.max_rotate = 1
            self.max_width = [9, 9, 9, 9]

        elif name is 't':
            self.images =[
              [0, 1, 0, 0,
               1, 1, 1, 0,
               0, 0, 0, 0,
               0, 0, 0, 0],

              [0, 1, 0, 0,
               0, 1, 1, 0,
               0, 1, 0, 0,
               0, 0, 0, 0],

              [0, 0, 0, 0,
               1, 1, 1, 0,
               0, 1, 0, 0,
               0, 0, 0, 0],

              [0, 1, 0, 0,
               1, 1, 0, 0,
               0, 1, 0, 0,
               0, 0, 0, 0]]
            self.max_rotate = 4
            self.max_width = [8, 9, 8, 9]

    def clean_up(self, rotate):
        """Clean up mino to provide a minimum 2d-matrix.

        rotate: 0, 1, 2, 3
        Output:
            column matrix: nested 2d-matrix
            profile: list of mino profile, 0 is bottom, -1 means 1 block up
        """

        if self.name is 's':
            return [
                ([[0, 1], [1, 1], [1, 0]], [0, 0, -1]),
                ([[1, 1, 0], [0, 1, 1]], [-1, 0])
                   ][rotate]
        elif self.name is 'z':
            return [
                ([[1, 0], [1, 1], [0, 1]], [-1, 0, 0]),
                ([[0, 1, 1], [1, 1, 0]], [0, -1])
                    ][rotate]
        elif self.name is 'l':
            return [
                ([[0, 1], [0, 1], [1, 1]], [0, 0, 0]),
                ([[1, 1, 1], [0, 0, 1]], [0, 0]),
                ([[1, 1], [1, 0], [1, 0]], [0, -1, -1]),
                ([[1, 0, 0], [1, 1, 1]], [-2, 0])
                  ][rotate]
        elif self.name is 'j':
            return [
                ([[1, 1], [0, 1], [0, 1]], [0, 0, 0]),
                ([[1, 1, 1], [1, 0, 0]], [0, -2]),
                ([[1, 0], [1, 0], [1, 1]], [-1, -1, 0]),
                ([[0, 0, 1], [1, 1, 1]], [0, 0])
                  ][rotate]
        elif self.name is 'i':
            return [
                ([[1], [1], [1], [1]], [0, 0, 0, 0]),
                ([[1, 1, 1, 1]], [0])
                    ][rotate]
        elif self.name is 'o':
            return [
                ([[1, 1], [1, 1]], [0, 0])
                    ][rotate]
        elif self.name is 't':
            return [
                ([[0, 1], [1, 1], [0, 1]], [0, 0, 0]),
                ([[1, 1, 1], [0, 1, 0]], [0, -1]),
                ([[1, 0], [1, 1], [1, 0]], [-1, 0, -1]),
                ([[0, 1, 0], [1, 1, 1]], [-1, 0])
                    ][rotate]

if __name__ == '__main__':
    ts = Tetromino('t')
    # print(ts.images)
    print(ts.clean_up(0))
    print(ts.clean_up(1))
    print(ts.clean_up(2))
    print(ts.clean_up(3))
