# Tetris AI v0.1
# Related to tetris project of Organization Laboratory
# Author: Frank-the-Obscure (Frank Hu)
# -*- coding: utf-8 -*-

from tetris_const import *
from copy import deepcopy
import numpy as np

def ai_action(status, tetromino, next_mino, verbose=False):
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
    if verbose:  # output action
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
    under_block_matrix = deepcopy(BLANK_STATUS)
        # init it with blank matrix: must using deepcopy for nested list
    profile = [0] * MAX_WIDTH # store profile
    for column in range(0, MAX_WIDTH):
        flag = False
        for row in range(0, MAX_HEIGHT):
            if flag is True and status_T[column][row] is 0:
                under_block_matrix[row][column] = 1
                under_blocks[column] += 1
            elif flag is False and status_T[column][row] is not 0:
                flag = True
                # calculate relative height, max height of blocks is 0
                profile[column] = MAX_HEIGHT - row
    #print('under blocks', under_blocks)

    # 3. score the status
    # 3.1 height
    # 3.2 under blocks
    # 3.3 big gaps
    # 3.4 profile
    # 3.5 status of first available line
    # 3.6 distribution of under blocks(todo)
    score_status = 0
    score_status -= num_cleared_rows * 20  # minus cleared rows
    score_status += height * 5  # add height
    for i in profile:
        score_status += i  # add minor height(profile height)

    for num in under_blocks:
        score_status += num * 10  # add under blocks

    for i in range(0, MAX_WIDTH - 1):  # add big gaps
        # todo: punish more when big gap comes.
        gap_right = profile[i] - profile[i + 1]
        if i is 0 or i is (MAX_WIDTH - 2):  # boundary
            if gap_right <= -2:
                score_status += gap_right ** 2 * 5
        else:
            gap_left = profile[i] - profile[i - 1]
            if gap_right <= -2 and gap_left <= -2:  # a gap
                score_status += (max(gap_right, gap_left) ** 2) * 5

    first_available_row = MAX_HEIGHT - 1
    for row in under_block_matrix:
        for block in row:
            if block:
                # first under block, above one line is the first available row
                first_available_row = under_block_matrix.index(row) - 1
                # 0-20, the less the better
                break
        if first_available_row < MAX_HEIGHT - 1:
            break
    score_status -= first_available_row * 10
        # guess this height is more important than general height
    for block in status[first_available_row]:
        if block:  # the more blocks in that line, easier to clear that line.
            score_status -= 3
    #print('score is :', score_status)
    return score_status


def height_status(status):
    """Calculate height of status."""
    height = 0
    for i in range(0, MAX_HEIGHT):
        for j in range(0, MAX_WIDTH):
            if (j is not 0 or j is not MAX_WIDTH - 1) and status[i][j] is not 0:
                # ignore side height
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


def print_status(status):
    """Print status matrix."""
    for row in status:
        print('|', end='')
        for block in row:
            if block:
                print('*', end='')
            else:
                print(' ', end='')
        print('|')


def print_mino(tetromino, position, rotate):
    """print mino with position and rotate."""

    mino, mino_profile = Tetromino(tetromino).clean_up(rotate)
    print_mino = [[0 for i in range(0, len(mino[0]))] for j in range(0, position)] + mino
    print_mino = transpose(print_mino)
    # print row matrix for mino
    for row in print_mino:
        print('|', end='')
        for block in row:
            if block:
                print('o', end='')
            else:
                print(' ', end='')
        print()


def main():
    """Testing AI using input list of tetromino.

    Input: file input with single letter as a mino.
    Output: send these minos to AI script.
    """
    test_line0 = [0 for i in range(0, MAX_WIDTH)]
    test_status = [test_line0 for i in range(0, 20)]
    #score(test_status)
    minolist = open('replay.txt').read()
    i = 0
    while True:
        mino = minolist[i]
        next_mino = minolist[i+1]
        if input('press enter to get next tetromino.') == '':
            # press enter for next mino
            print('mino is', mino)
            print_status(test_status)
            action_position, action_rotate =  ai_action(test_status, mino, next_mino)
            test_status = generate(test_status, mino, action_position, action_rotate)
            test_status, cleaned_rows = clear(test_status)
            print_mino(mino, action_position, action_rotate)
        else:  # any other input means end testing.
            break
        i += 1


if __name__ == '__main__':
    main()
