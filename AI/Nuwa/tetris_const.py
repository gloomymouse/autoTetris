# Tetris AI v0.1
# Related to tetris project of Organization Laboratory
# Author: Frank-the-Obscure (Frank Hu)
# -*- coding: utf-8 -*-

# define constants
MAX_HEIGHT = 20
MAX_WIDTH = 10
BLANK_STATUS = [[0 for i in range(0, MAX_WIDTH)] for j in range(0, MAX_HEIGHT)]

# define tetrominos
class Tetromino:
    def __init__(self, name):
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
               0, 0, 0, 0]]
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
    def clean_up(self, rotate):
        """Clean up mino to provide a minimum 2d-matrix.

        rotate: 0, 1, 2, 3
        Output:
            column matrix: nested 2d-matrix
            profile: list of mino profile, 0 is bottom
        """
        shape = self.images[rotate]
        row_matrix = [[shape[i], shape[i+1], shape[i+2], shape[i+3]]
                 for i in [0, 4, 8, 12]] # transfer to matrix
        # clean up rows
        for i in range(len(row_matrix) - 1, -1, -1):
            flag = 0
            for block in row_matrix[i]:
                if block is 1:
                    flag = 1
                    break
            if flag is 0: # blank row
                del row_matrix[i]
        import numpy as np
        col_matrix = np.array(row_matrix).T.tolist()
        # clean up columns
        for i in range(len(col_matrix) - 1, -1, -1):
            flag = 0
            for block in col_matrix[i]:
                if block == 1:
                    flag = 1
                    break
            if flag == 0: # blank col
                del col_matrix[i]

        # Calculate profile from col_matrix.
        profile = []
        for column in col_matrix:
            last_block = 0
            for i in range(len(column) - 1, -1, -1):
                if column[i] is 1:
                    last_block = i
                    break
            profile.append(last_block + 1 - len(column))

        return col_matrix, profile


if __name__ == '__main__':
    ts = Tetromino('t')
    # print(ts.images)
    print(ts.clean_up(0))
    print(ts.clean_up(1))
    print(ts.clean_up(2))
    print(ts.clean_up(3))
