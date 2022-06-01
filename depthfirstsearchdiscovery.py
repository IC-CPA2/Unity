#DFS is working, however I realized backtracking in DFS to avoid visited paths might be more inefficient then just
#driving over the visited paths, so maybe optimize with a combination of Djikstra when we have discovered some patches of location
#mid running the program.

command = []
coordpath = []
memo = {}

def dfs9x9(array):
    global command, coordpath, memo
    memo = {'4,4':1}
    command = []
    coordpath = []
    def dfstemp(array,curr,point,command):
        global coordpath, memo
        memo[str(curr[0])+','+str(curr[1])] = 1
        coordpath.append(curr)
        if point=="up":
            if curr[0]-1>=0 and array[curr[0]-1][curr[1]]=='_' and str(curr[0]-1)+','+str(curr[1]) not in memo:
                command.append("Drive Straight")
                dfstemp(array,[curr[0]-1,curr[1]],"up",command)
            if curr[1]+1<9 and array[curr[0]][curr[1]+1]=='_' and str(curr[0])+','+str(curr[1]+1) not in memo:
                command.append("Rotate 90 Degrees")
                dfstemp(array,curr,"right",command)
            if curr[0]+1<9 and array[curr[0]+1][curr[1]]=='_' and str(curr[0]+1)+','+str(curr[1]) not in memo:
                command.append("Rotate 180 Degrees")
                dfstemp(array,curr,"down",command)
            if curr[1]-1>=0 and array[curr[0]][curr[1]-1]=='_' and str(curr[0])+','+str(curr[1]-1) not in memo:
                command.append("Rotate -90 Degrees")
                dfstemp(array,curr,"left",command)
        elif point=="right":
            if curr[1]+1<9 and array[curr[0]][curr[1]+1]=='_' and str(curr[0])+','+str(curr[1]+1) not in memo:
                command.append("Drive Straight")
                dfstemp(array,[curr[0],curr[1]+1],"right",command)
            if curr[0]+1<9 and array[curr[0]+1][curr[1]]=='_' and str(curr[0]+1)+','+str(curr[1]) not in memo:
                command.append("Rotate 90 Degrees")
                dfstemp(array,curr,"down",command)
            if curr[0]-1>=0 and array[curr[0]-1][curr[1]]=='_' and str(curr[0]-1)+','+str(curr[1]) not in memo:
                command.append("Rotate -90 Degrees")
                dfstemp(array,curr,"up",command)
        elif point=="down":
            if curr[0]+1<9 and array[curr[0]+1][curr[1]]=='_' and str(curr[0]+1)+','+str(curr[1]) not in memo:
                command.append("Drive Straight")
                dfstemp(array,[curr[0]+1,curr[1]],"down",command)
            if curr[1]+1<9 and array[curr[0]][curr[1]+1]=='_' and str(curr[0])+','+str(curr[1]+1) not in memo:
                command.append("Rotate -90 Degrees")
                dfstemp(array,curr,"right",command)
            if curr[1]-1>=0 and array[curr[0]][curr[1]-1]=='_' and str(curr[0])+','+str(curr[1]-1) not in memo:
                command.append("Rotate 90 Degrees")
                dfstemp(array,curr,"left",command)
        elif point=="left":
            if curr[1]-1>=0 and array[curr[0]][curr[1]-1]=='_' and str(curr[0])+','+str(curr[1]-1) not in memo:
                command.append("Drive Straight")
                dfstemp(array,[curr[0],curr[1]-1],"left",command)
            if curr[0]+1<9 and array[curr[0]+1][curr[1]]=='_' and str(curr[0]+1)+','+str(curr[1]) not in memo:
                command.append("Rotate -90 Degrees")
                dfstemp(array,curr,"down",command)
            if curr[0]-1>=0 and array[curr[0]-1][curr[1]]=='_' and str(curr[0]-1)+','+str(curr[1]) not in memo:
                command.append("Rotate 90 Degrees")
                dfstemp(array,curr,"up",command)
    dfstemp(array,[4,4],"up",command)
    print(command)
    print(coordpath)

Map = [['_','_','_','_','_','v','_','_','_'],
       ['_','_','_','_','_','v','v','v','v'],
       ['_','_','_','_','_','v','_','_','_'],
       ['_','_','_','_','_','v','_','_','_'],
       ['_','_','_','v','x','v','_','_','_'],
       ['_','_','_','_','v','_','_','_','_'],
       ['_','_','_','_','_','_','_','_','_'],
       ['_','_','_','_','_','_','_','_','_'],
       ['_','_','_','_','_','_','_','_','_']]

dfs9x9(Map)
