def tograph(Map):
    memo = {}
    for i in range(9):
        for j in range(9):
            if Map[i][j]=='_' or Map[i][j]=='x':
                possiblepaths = []
                if i==0:
                    if Map[1][j]=='_':
                        possiblepaths.append([str(1)+','+str(j),1])
                elif i==8:
                    if Map[7][j]=='_':
                        possiblepaths.append([str(7)+','+str(j),1])
                else:
                    if Map[i-1][j]=='_':
                        possiblepaths.append([str(i-1)+','+str(j),1])
                    if Map[i+1][j]=='_':
                        possiblepaths.append([str(i+1)+','+str(j),1])

                if j==0:
                    if Map[i][1]=='_':
                        possiblepaths.append([str(i)+','+str(1),1])
                elif j==8:
                    if Map[i][7]=='_':
                        possiblepaths.append([str(i)+','+str(7),1])
                else:
                    if Map[i][j-1]=='_':
                        possiblepaths.append([str(i)+','+str(j-1),1])
                    if Map[i][j+1]=='_':
                        possiblepaths.append([str(i)+','+str(j+1),1])
                        
                memo[str(i)+','+str(j)] = possiblepaths

    return memo

def shortestpath(start,graph):
    previousvertex = {start:""}
    unfinishednode = [start]
    unvisitednode = {start:1}
    init = {start:0}
    
    for node in graph:
        if node!=start:
            init[node]=999
            previousvertex[node]=""
            unfinishednode.append(node)
            unvisitednode[node]=1

    while len(unfinishednode)>0:
        cost = 999
        currentnode = ""
        for j in init:
            if init[j]<cost and unvisitednode[j]==1:
                cost = init[j]
                currentnode = j

        connections = graph[currentnode]

        for i in range(len(connections)):
            if cost + connections[i][1] < init[connections[i][0]]:
                init[connections[i][0]] = cost + connections[i][1]
                previousvertex[connections[i][0]]=currentnode

        unfinishednode.remove(currentnode)
        unvisitednode[currentnode] = 0
    print(init)

#ball - o
#rover - x
#obstacle - v
#clear - _

Map = [['_','_','_','_','_','_','_','_','_'],
       ['_','_','_','_','_','_','_','_','_'],
       ['_','_','_','_','_','_','_','_','_'],
       ['_','_','_','_','_','_','_','_','_'],
       ['_','_','_','v','x','v','_','_','_'],
       ['_','_','_','_','v','_','_','_','_'],
       ['_','_','_','_','_','_','_','_','_'],
       ['_','_','_','_','_','_','_','_','_'],
       ['_','_','_','_','_','_','_','_','_']]

try:
    shortestpath('4,4',tograph(Map))
except:
    print("Some blocks cannot be reached")
