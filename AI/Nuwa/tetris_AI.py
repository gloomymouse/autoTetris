# Tetris AI v0.1
# Related to tetris project of Organization Laboratory
# Author: Frank-the-Obscure (Frank Hu)
# -*- coding: utf-8 -*-

import pprint
from tetris_const import *
import numpy as np

def ai_action(status, tetromino, next_mino):
    """Choose best action for the status and the new tetromino.

    logic:
        1. generate all possible block status
        2. score each one and choose best
        3. output option
    todo: add next tetromino for this func.
    """
    #init temp variables
    best_score = 65535
    action_position = 0
    action_rotate = 0
    for position in range(0, MAX_WIDTH):
        for rotate in range(0, 4): # todo: revise with mino.rotate
            if is_valid_status(tetromino, position, rotate):
                new_status = generate(status, tetromino, position, rotate)
                temp_score = score(new_status)
                if temp_score < best_score:
                    best_score = temp_score
                    action_position = position
                    action_rotate = rotate
    print('mino is {}, best action is: {},{}. score is {}'.format(
        tetromino, action_position, action_rotate, best_score))
    return action_position, action_rotate


def is_valid_status(tetromino, position, rotate):
    """Determine if this is valid status. (i.e. not to the right boundary.)"""
    mino, mino_profile = Tetromino(tetromino).clean_up(rotate)
    mino_width = len(mino)
    if position + mino_width > MAX_WIDTH:
        return False
    else:
        return True


def generate(status, tetromino, position, rotate):
    """Generate new status after adding a tetromino.

    logic:
        1. adding mino
        2. clear lines if needed
    """
    # prepare shape and profiles of status and mino
    status_profile = profile_status(status)
    # print('mino, pos, rotate:', tetromino, position, rotate) # debugging
    mino, mino_profile = Tetromino(tetromino).clean_up(rotate)
    #calculate for columns
    final_mino_height = 0
    for i in range(0, len(mino)): #len(mino) is the width of mino
        mino_height = status_profile[position + i] + mino_profile[i]
        if mino_height > final_mino_height:
            final_mino_height = mino_height
    status_T = transpose(status)
    # adding mino
    for i in range(0, len(mino)):
        for j in range(0, len(mino[i])):
            if mino[i][j] is 1:
                status_T[position + i]\
                        [MAX_HEIGHT - final_mino_height - len(mino[i]) + j] = 1
                # line number is the same direction for mino and status
    status = transpose(status_T)
    return status


def clear(status):
    """Clear status, return new status and lines cleaned."""
    for i in range(MAX_HEIGHT - 1, -1, -1):
        if 0 not in status[i]:
            del status[i]
    num_cleared_rows = MAX_HEIGHT - len(status)
    for i in range(MAX_HEIGHT - len(status)):
        status.insert(0, [0] * MAX_WIDTH)

    return status, num_cleared_rows


def transpose(matrix):
    return np.array(matrix).T.tolist() # transpose array by numpy


def score(status):
    """Score the status.

    input: status: an 2d-matrix, from top, line first
    process:
        0. clear rows
        1. height of blocks
        2. scan each column for under blocks
        3. scan profile of top blocks
    output: score
    """
    #pprint.pprint(status) # for debugging

    # 0. clear status
    status, num_cleared_rows = clear(status)
    #pprint.pprint(status)
    # 1. cal height
    height = height_status(status)

    # 2. under blocks
    status_T = transpose(status)
    #find under blocks and profile (scan for each column)
    under_blocks = [0] * MAX_WIDTH #store under blocks using a list
    profile = [0] * MAX_WIDTH # store profile
    for column in range(0, len(status_T)):
        flag = False
        for block in status_T[column]:
            if flag is True and block is 0:
                under_blocks[column] += 1
            elif flag is False and block is not 0:
                flag = True
                # calculate relative height, max height of blocks is 0
                profile[column] = MAX_HEIGHT - status_T[column].index(block)
    #print('under blocks', under_blocks)
    #print('profile', profile)

    # 3. score the status
    score_status = 0
    score_status -= num_cleared_rows * 50 # minus cleared rows
    score_status += height * 10 # add height
    for num in under_blocks:
        score_status += num * 1 # add under blocks
    '''for i in range(0, MAX_WIDTH - 1, 2): # add big gaps
        # todo: punish more when big gap comes.
        gap = profile[i] - profile[i + 1]
        if gap >= 2:
            score_status += gap * 5
        elif gap <= -2:
            score_status -= gap * 5'''
    #print('score is :', score_status)
    return score_status


def height_status(status):
    """Calculate height of status."""
    height = 0
    for i in range(0, MAX_HEIGHT):
        for j in status[i]:
            if j is not 0:
                height = MAX_HEIGHT - i
                break
        if height is not 0: # find max height
            break
    return height


def profile_status(status):
    """Calculate profile of status."""
    height = height_status(status)
    status_T = transpose(status)
    #find under blocks and profile (scan for each column)
    profile = [0] * MAX_WIDTH # store profile
    for column in range(0, len(status_T)):
        flag = False
        for block in status_T[column]:
            if flag is False and block is not 0:
                flag = True
                # calculate relative height, max height of blocks is 0
                profile[column] = MAX_HEIGHT - status_T[column].index(block)
                break
    return profile


def build_test_case():
    """Build test cases.
    """
    pass


def main():
    test_line0 = [0 for i in range(0, MAX_WIDTH)]
    test_line1 = [1 for i in range(0, MAX_WIDTH)]
    test_line01 = [0, 1] + [1, 1] * 4
    test_line_r = [0, 1] * 5
    test_status = [test_line0 for i in range(0, 12)]
    test_status += [test_line0, test_line0, test_line_r, test_line01, test_line1]
    test_status += [test_line0 for i in range(0, MAX_HEIGHT - len(test_status))]
    #score(test_status)
    #profile_status(test_status)
    #generate(test_status, 'i', 0, 1)
    ai_action(test_status, 'i', 'l')
    """
    generate(test_status, 'i', 0, 1)
    generate(test_status, 'i', 0, 2)
    generate(test_status, 'i', 0, 3)
    generate(test_status, 'z', 0, 0)
    generate(test_status, 'z', 0, 1)
    generate(test_status, 'z', 0, 2)
    generate(test_status, 'z', 0, 3)"""

if __name__ == '__main__':
    main()
