# Tetris AI v0.1
# Related to tetris project of Organization Laboratory
# Author: Frank-the-Obscure (Frank Hu)
# -*- coding: utf-8 -*-

# define constants
MAX_HEIGHT = 20
MAX_WIDTH = 10

import pprint

def ai_action(status, tetromino, next_mino):
    """Choose best action for the status and the new tetromino.

    logic:
        1. generate all possible block status
        2. score each one and choose best
        3. output option
    todo: add next tetromino for this func.
    """
    print('hello, tetris')
    #init temp variables
    best_score = 0
    action_position = 0
    action_shape = 0
    for position in range(0, MAX_WIDTH):
        for shape in range(0, SHAPES[tetromino]):
            print(status, tetromino, position, shape)
            new_status = generate(status, tetromino, position, shape)
            temp_score = score(new_status)
            if temp_score >= best_score:
                best_score = temp_score
                action_position = position
                action_shape = shape
    return action_position, action_shape


def generate(status, tetromino, position, shape):
    """Generate new status after adding a tetromino.

    logic:
        1. adding mino
        2. clear lines if needed
    """
    pass


def score(status):
    """Score the status.

    input: status: an 2d-matrix, from top, line first
    process:
        1. height of blocks
        2. scan each column for under blocks
        3. scan profile of top blocks
    output: score
    """
    pprint.pprint(status) # for debugging

    # 1. cal height
    height = 0
    for i in range(0, MAX_HEIGHT):
        for j in status[i]:
            if j is not 0:
                height = MAX_HEIGHT - i
                break
        if height is not 0: # find max height
            break
    print('Height is', height) # test height func.

    # 2. under blocks
    import numpy as np
    status_T = np.array(status).T.tolist() # transpose array by numpy
    pprint.pprint(status_T)
    #find under blocks and profile (scan for each column)
    under_blocks = [0] * MAX_WIDTH #store under blocks using a list
    profile = [0] * MAX_WIDTH # store profile
    for column in range(0, len(status_T)):
        flag = False
        for block in status_T[column]:
            if flag is True and block is 0:
                under_blocks[column] += 1
            elif flag is False and block is 1:
                flag = True
                # calculate relative height, max height of blocks is 0
                profile[column] = MAX_HEIGHT - height - status_T[column].index(block)
    pprint.pprint(under_blocks)
    print(profile)



def build_test_case():
    """Build test cases.
    """
    pass


def main():
    test_line0 = [0 for i in range(0, MAX_WIDTH)]
    test_line1 = [1 for i in range(0, MAX_WIDTH)]
    test_line_r = [0, 1] * 5
    test_status = [test_line0 for i in range(0, 10)]
    test_status += [test_line0, test_line0, test_line_r, test_line0, test_line1]
    test_status += [test_line0 for i in range(0, MAX_HEIGHT - len(test_status))]
    score(test_status)

main()
