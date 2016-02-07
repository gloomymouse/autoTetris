# Tetris AI v0.1
# Related to tetris project of Organization Laboratory
# Author: Frank-the-Obscure (Frank Hu)
# -*- coding: utf-8 -*-

from tetris import *
from copy import deepcopy
import numpy as np


BIG_UNDER_BLOCKS = 50
BIG_SCORE = 65535


def ai_action(status, tetromino, next_mino, verbose=False):
    """Choose best action for the status and the new tetromino.

    logic:
        1. generate all possible block status
        2. score each one and choose best
        3. output option
    todo: add next tetromino for this func.
    """

    #init class of tetraminos
    tetromino = Tetromino(tetromino)
    next_mino = Tetromino(next_mino)
    max_rotate_mino = tetromino.max_rotate
    max_rotate_next = next_mino.max_rotate
    max_width_mino = tetromino.max_width
    max_width_next = next_mino.max_width
    #init temp variables
    best_score = BIG_SCORE
    best_height = MAX_HEIGHT  # to reduce search
    best_under_blocks = BIG_UNDER_BLOCKS
    action_pos = 0
    action_rot = 0
    action_next_pos = 0
    action_next_rot = 0
    for rotate in range(0, max_rotate_mino):
        for position in range(0, max_width_mino[rotate]):
            new_status = generate(status, tetromino, position, rotate)
            for next_rot in range(0, max_rotate_next):
                for next_pos in range(0, max_width_next[next_rot]):
                    next_status = generate(new_status, next_mino,
                                           next_pos, next_rot)
                    temp_score, temp_height, temp_under_blocks = \
                            score(next_status, best_height, best_under_blocks)
                    if temp_score < best_score:
                        # if find better action
                        best_height = temp_height
                        best_score = temp_score
                        best_under_blocks = temp_under_blocks
                        action_pos = position
                        action_rot = rotate
                        action_next_pos = next_pos
                        action_next_rot = next_rot
    if verbose:  # output action
        print('mino is {}, best action is: {},{};\
            next action is: {}, {}. score is {}'\
            .format(tetromino.name, action_pos, action_rot,
                    action_next_pos, action_next_rot, best_score))
    return action_pos, action_rot


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
    # print('mino, pos, rotate:', tetromino.name, position, rotate) # debugging
    mino, mino_profile = tetromino.clean_up(rotate)
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


def score(status, best_height=MAX_HEIGHT, best_under_blocks=BIG_UNDER_BLOCKS):
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
    # test
    if num_cleared_rows >= 4:
        print(num_cleared_rows) #find if great clear happens
    #pprint.pprint(status)
    # 1. cal height
    height = height_status(status)
    if height - best_height >= 4:  # guess it's a bad position and skip
        return BIG_SCORE, MAX_HEIGHT, BIG_UNDER_BLOCKS

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
    if sum(under_blocks) - best_under_blocks > 2:
        return BIG_SCORE, MAX_HEIGHT, BIG_UNDER_BLOCKS
    #print('under blocks', under_blocks)

    # 3. score the status
    # 3.1 height
    # 3.2 under blocks
    # 3.3 big gaps
    # 3.4 profile
    # 3.5 status of first available line
    # 3.6 distribution of under blocks(todo)
    score_status = 0
    # minus cleared rows: **2 is testing
    score_status -= num_cleared_rows**2 * 50
    # add height & profile height
    score_status += height * 5
    if height >= 10:
        score_status += (height - 10) * 100
    for i in profile:
        score_status += i  # add minor height(profile height)

    for num in under_blocks:
        score_status += num * 50  # add under blocks

    score_status += score_profile(profile)
    """for i in range(0, MAX_WIDTH - 1):  # add big gaps
        # todo: punish more when big gap comes.
        gap_right = profile[i] - profile[i + 1]
        if i is 0 or i is (MAX_WIDTH - 2):  # boundary
            if gap_right <= -2:
                score_status += gap_right**2 * 5
        else:
            gap_left = profile[i] - profile[i - 1]
            if gap_right <= -2 and gap_left <= -2:  # a gap
                score_status += (max(gap_right, gap_left) ** 2) * 5"""
    # status of first available line
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
    score_status -= first_available_row * 50
        # guess height of first available line is more important than height
    for block in status[first_available_row]:
        if block:  # the more blocks in that line, easier to clear that line.
            score_status -= 3
    #print('score is :', score_status)
    return score_status, height, sum(under_blocks)


def height_status(status):
    """Calculate height of status."""
    height = 0
    for i in range(0, MAX_HEIGHT):
        for j in range(0, MAX_WIDTH):
            if  status[i][j]:
            # if (j and j is not MAX_WIDTH - 1) and status[i][j]:
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


def score_profile(profile):
    """Score profile of status, find gaps and punish score.

    input: profile list
    process:
        1. find 1-2 gaps and punish
        2. punish max height - min height
    output: score to be added"""

    added_score = 0
    i = 0
    while i < MAX_WIDTH - 1:
        gap_right = profile[i] - profile[i+1]
        if i is 0:  # left boundary
            if gap_right <= -2:
                added_score += gap_right**2 * 5  # parameter here
                i += 1 # skip column (i+1), as it's higher than i
            elif profile[i+1] - profile[i+2] <= -2:  # 2 column gap
                added_score += (profile[i+2] - max(profile[i], profile[i+1]))\
                                **2 * 2
                i += 2 # skip i+1 and i+2
        elif i is (MAX_WIDTH - 2): # right boundary: only 1 column gap
            if 0 - gap_right <= -2:
                added_score += gap_right**2 * 5  #parameter here
            elif profile[i] - profile[i-1] <= -2:
                added_score += (profile[i-1] - max(profile[i], profile[i+1]))\
                                ** 2 * 2
            break  #skip last add and cycle
        else:  # not left and right
            gap_left = profile[i] - profile[i-1]
            if gap_left <= -2:
                if gap_right <= -2:  # single col gap
                    # gap are number < 0, so use max to choose smaller gap
                    added_score += (max(gap_right, gap_left) ** 2) * 5
                    i += 1  # skip column (i+1), as it's higher than i
                elif profile[i+1] - profile[i+2] <= -2:  # 2 col gap
                    added_score += max(gap_left, profile[i+1] - profile[i+2])\
                                    **2 * 3
                    i += 2  # skip column i+1, i+2
        i += 1

    # punish max height - min height
    delta_h = max(profile) - min(profile)
    if delta_h > 3:  # this line lead to much more under blocks, abondon now
        added_score += delta_h ** 2
    return added_score


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
    print_mino = [[0 for i in range(0, len(mino[0]))]\
                  for j in range(0, position)] + mino
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
            action_pos, action_rot = ai_action(test_status, mino,
                                               next_mino, verbose=True)
            test_status = generate(test_status, Tetromino(mino),
                                   action_pos, action_rot)
            test_status, cleaned_rows = clear(test_status)
            print_mino(mino, action_pos, action_rot)
        else:  # any other input means end testing.
            break
        i += 1


if __name__ == '__main__':
    main()
    print(score_profile([5,0,0,0,5, 5,5,5,5,5]))
