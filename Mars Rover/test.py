def choose_next(ang_inp,curr_sq):
    ##assuming 9x9 grid starting with index 0. 
    #should work provide values 
    """
    0 1 2 3 4 5 6 7 8
    | | | | | | | | | 0
    | | | | | | | | | 1
    | | | | | | | | | 2
    | | | | | | | | | 3
    """
    end_x = 8
    end_y = 8 
    end_coords = int(str(end_x)+str(end_y))
    new_val = int(curr_sq)
    if ang_inp == "0" and (new_val%10)>=1:
        new_val -= 1
    if ang_inp == "45" and (new_val%10)<end_y and (new_val-9)<=end_coords:
        new_val += 9
    elif ang_inp == "90" and (new_val<=end_coords-10):
        new_val += 10
    elif ang_inp == "135" and (new_val%10)<end_y and (new_val<=end_coords-11):
        new_val += 11
    elif ang_inp == "180" and (new_val%10)<end_y:
        new_val += 1
    elif ang_inp == "225" and (new_val>=10) and (new_val%10)<8:#complete if conditions
        new_val -= 9
    elif ang_inp == "270" and (new_val>=10):
        new_val -= 10
    elif ang_inp == "315" and (new_val>=11) and (new_val%10)>=1:
        new_val -= 11 
    return str(new_val)


print(choose_next("45","44"))