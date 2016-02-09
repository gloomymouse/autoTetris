# -*- coding: utf-8 -*-
# Tetris AI - Nuwa v1.2.2
# Related to tetris project of Organization Laboratory
# Author: Frank Hu (Frank-the-Obscure @ GitHub)


from tetris import *
import numpy as np


# init constants for AI
BLANK_LINE = [0] * MAX_WIDTH
BIG_UNDER_BLOCKS = 50
BIG_SCORE = 65535


def ai_action(status, tetromino, next_mino, verbose=False, next=False):
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
    for rotate in range(0, tetromino.max_rotate):
        for position in range(0, max_width_mino[rotate]):
            new_status = generate(status, tetromino, position, rotate)
            for next_rot in range(0, next_mino.max_rotate):
                for next_pos in range(0, max_width_next[next_rot]):
                    next_status = generate(new_status, next_mino,
                                           next_pos, next_rot)
                    temp_score, temp_height, temp_under_blocks = \
                            score(next_status, best_height, best_under_blocks)
                    if temp_score < best_score:
                        # update temp variables
                        best_height = temp_height
                        best_score = temp_score
                        best_under_blocks = temp_under_blocks
                        action_pos = position
                        action_rot = rotate
                        action_next_pos = next_pos
                        action_next_rot = next_rot
                        if verbose:  # output action
                            print(("mino is {}, best action is: {},{};"+\
                                   "next action is: {}, {}. score is {}.")\
                                .format(tetromino.name, action_pos, action_rot,
                                        action_next_pos, action_next_rot, 
                                        best_score))

    if verbose:  # output action
        print(('mino is {}, best action is: {},{};'+\
            'next action is: {}, {}. score is {}.')\
            .format(tetromino.name, action_pos, action_rot,
                    action_next_pos, action_next_rot, best_score))
    if next:
        return action_pos, action_rot, action_next_pos, action_next_rot
    else:
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
        1. calculate height of blocks
        2. scan each column for under blocks
        3. scan profile of top blocks
    output: score
    """

    score_status = 0

    # 0. clear status
    status, num_cleared_rows = clear(status)
    # minus cleared rows: **2 is testing
    CLEARED_ROWS = 50
    score_status -= num_cleared_rows**2 * CLEARED_ROWS

    # 1. cal height and cut-off (for speed)
    height = height_status(status)
    if height - best_height >= 4:  # guess it's a bad position and skip
        return BIG_SCORE, MAX_HEIGHT, BIG_UNDER_BLOCKS

    # add height & profile height
    score_status += height * 10
    if height >= 10:
        score_status += (height - 10) * 100

    # 2. under blocks and cut-off (for speed)
    status_T = transpose(status)
    s_underblocks, num_underblocks = score_underblocks(status, status_T)
    score_status += s_underblocks
    if num_underblocks - best_under_blocks > 2:
        # too many more under blocks, guess it's not a good position
        return BIG_SCORE, MAX_HEIGHT, BIG_UNDER_BLOCKS

    # 3. scan profile
    profile = BLANK_LINE[:]
    for column in range(0, MAX_WIDTH):
        for row in range(0, MAX_HEIGHT):
            if status_T[column][row] is not 0:
                profile[column] = MAX_HEIGHT - row
                break
    # add minor height(profile height)
    for i in profile:
        score_status += i  
    # add score of profile
    score_status += score_profile(profile)

    return score_status, height, num_underblocks


def height_status(status):
    """Calculate height of status."""
    height = 0
    for i in range(0, MAX_HEIGHT):
        for j in range(0, MAX_WIDTH):
            if status[i][j]:
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
    PUNISH_SINGLE_GAP = 5
    PUNISH_DOUBLE_GAP = 2
    while i < MAX_WIDTH - 1:
        gap_right = profile[i] - profile[i+1]
        if i is 0:  # left boundary
            if gap_right <= -2:
                added_score += gap_right**2 * PUNISH_SINGLE_GAP
                i += 1 # skip column (i+1), as it's higher than i
            elif profile[i+1] - profile[i+2] <= -2:  # 2 column gap
                added_score += (profile[i+2] - max(profile[i], profile[i+1]))\
                                **2 * PUNISH_DOUBLE_GAP
                i += 2 # skip i+1 and i+2
        elif i is (MAX_WIDTH - 2): # right boundary: only 1 column gap
            if 0 - gap_right <= -2:
                added_score += gap_right**2 * PUNISH_SINGLE_GAP 
            elif profile[i] - profile[i-1] <= -2:
                added_score += (profile[i-1] - max(profile[i], profile[i+1]))\
                                **2 * PUNISH_DOUBLE_GAP
            break  #skip last add and cycle
        else:  # not left and right
            gap_left = profile[i] - profile[i-1]
            if gap_left <= -2:
                if gap_right <= -2:  # single col gap
                    # gap are number < 0, so use max to choose smaller gap
                    added_score += (max(gap_right, gap_left)**2) * PUNISH_SINGLE_GAP
                    i += 1  # skip column (i+1), as it's higher than i
                elif profile[i+1] - profile[i+2] <= -2:  # 2 col gap
                    added_score += max(gap_left, profile[i+1] - profile[i+2])\
                                   **2 * (PUNISH_DOUBLE_GAP+1)
                    i += 2  # skip column i+1, i+2
        i += 1

    # punish max height - min height
    delta_h = max(profile) - min(profile)
    if delta_h > 3:  
        added_score += delta_h ** 2 
    return added_score


def score_underblocks(status, status_T):
    """Score under blocks.

    i: transposed status matrix, elements are columns.
    p: for each column, find under blocks and score.
    o: score added.
    """

    # parameters
    SCORE_UNDERBLOCK = 30
    SCORE_STACK = 3
    SCORE_FIRST_AVAILABLE_ROW = 80
    # fast init using numpy
    under_block_matrix = BLANK_STATUS_ARRAY.tolist()  

    num_underblocks = 0
    score = 0 
    for column in range(0, MAX_WIDTH):
        num_underblocks, stacks = \
            score_under_block_column(status_T[column], column, 
                                     under_block_matrix)
        score += num_underblocks * SCORE_UNDERBLOCK
        for up, stack in stacks:
            # the more up and stack, the more difficult to clear that block
            score += (up * stack) * SCORE_STACK

    # status of first available line
    first_available_row = MAX_HEIGHT - 1
    for row in range(0, MAX_HEIGHT):
        for col in range(0, MAX_WIDTH):
            if under_block_matrix[row][col]:
                # first under block, above one line is the first available row
                first_available_row = row - 1
                # 0-20, the less the better
                break
        if first_available_row < MAX_HEIGHT - 1:
            break
    score -= first_available_row * SCORE_FIRST_AVAILABLE_ROW
        # guess height of first available line is more important than height
    for block in status[first_available_row]:
        if block:  # the more blocks in that line, easier to clear that line.
            score -= 10
    return score, num_underblocks


def score_under_block_column(column, num_column, under_block_matrix):
    """Score under blocks for a column.
    
    i: list of column (len = MAX_HEIGHT)
    o: 
        1. num of under blocks
        2. list of (up_blocks, stacks)
    """
    flag = False
    up_blocks = 0
    flag_stack = False
    stack = 0
    num_underblocks = 0
    up_stacks = []
    for i in range(0, MAX_HEIGHT):
        if flag is False:
            # before any blocks appear
            if column[i] is not 0:
                # first block in the column
                flag = True
                up_blocks += 1
                stack += 1
        else:
            if column[i] is 0:
                num_underblocks += 1
                if flag_stack is False:
                    # new under blocks
                    flag_stack = True
                    under_block_matrix[i][num_column] = 1
                    up_stacks.append((up_blocks, stack))
                    up_blocks += 1
                else:
                    # continous under blocks
                    under_block_matrix[i][num_column] = 1
                    up_stacks.append((up_blocks, stack))
                    up_blocks += 1
            else:
                if flag_stack is True:
                    # block below under blocks, +1 stack level
                    stack += 1
                    flag_stack = False
                    up_blocks += 1
                else:
                    # continous blocks
                    up_blocks += 1
    return num_underblocks, up_stacks


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
        print(' ', end='')
        for block in row:
            if block:
                print('o', end='')
            else:
                print(' ', end='')
        print()


def main():
    """Test AI using input list or random list of tetromino.

    Input: file input with single letter as a mino.
    Output: send these minos to AI script.
    """
    test_line0 = [0 for i in range(0, MAX_WIDTH)]
    test_status = [test_line0 for i in range(0, 20)]
    MINO_LIST = 'sztjiol'
    #score(test_status)
    minolist = open('replay.txt').read()
    i = 0
    score = 0
    import time
    import random
    mino = random.choice(MINO_LIST)
    next_mino = random.choice(MINO_LIST)

    while True:
        # input from random choice
        next_mino = random.choice(MINO_LIST)
        # input from minolist
        # mino = minolist[i]
        # next_mino = minolist[i+1]
        # if input('press enter to get next tetromino.') is not '':
        #     break
            # press enter for next mino
        print('mino is {}, next mino is {}'.format(mino, next_mino))
        print_mino(mino, 0, 0)
        print_mino(next_mino, 15, 0)

        t0 = time.time()
        action_pos, action_rot = \
            ai_action(test_status, mino, next_mino, verbose=False, next=False)

        # # verbose next output
        # action_pos, action_rot, action_next_pos, action_next_rot = \
        #     ai_action(test_status, mino, next_mino, verbose=False, next=True)
        # print_mino(next_mino, action_next_pos, action_next_rot)

        print_mino(mino, action_pos, action_rot)
        print_status(test_status)            
        test_status = generate(test_status, Tetromino(mino),
                               action_pos, action_rot)

        test_status, cleaned_rows = clear(test_status)
        height = height_status(test_status)
        if height >= MAX_HEIGHT:
            print("Game over! score: {}".format(score))
            break
        # add score display
        if cleaned_rows > 0:
            if cleaned_rows is 1:
                score += 10
            elif cleaned_rows is 2:
                score += 40
            elif cleaned_rows is 3:
                score += 90
            else :
                score += 200
        t1 = time.time()
        print("AI reaction time {:.3f}s".format(t1-t0))

        i += 1
        print("number of blocks: {}, score: {}".format(i, score)) 
        mino = next_mino

        #if i > 50: 
        #    break


if __name__ == '__main__':
    # profile and state script
    # import cProfile
    # cProfile.run('main()', 'restats')
    # import pstats
    # p = pstats.Stats('restats')
    # p.sort_stats('cumulative').print_stats()

    main()
