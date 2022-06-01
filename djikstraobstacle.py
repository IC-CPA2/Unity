#The DFS Algorithm is working, however I realised that when using DFS sometimes backtracking to avoid visited paths might 
#be more inefficient than just crossing over the visited paths so maybe have to integrate with Djikstra is make backtracking 
#cases more efficient.

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

                if i-1>=0 and j-1>=0:
                    if Map[i-1][j-1]=='_':
                        possiblepaths.append([str(i-1)+','+str(j-1),1.4])
                if i-1>=0 and j+1<=8:
                    if Map[i-1][j+1]=='_':
                        possiblepaths.append([str(i-1)+','+str(j+1),1.4])
                if i+1<=8 and j-1>=0:
                    if Map[i+1][j-1]=='_':
                        possiblepaths.append([str(i+1)+','+str(j-1),1.4])
                if i+1<=8 and j+1<=8:
                    if Map[i+1][j+1]=='_':
                        possiblepaths.append([str(i+1)+','+str(j+1),1.4])
                        
                memo[str(i)+','+str(j)] = possiblepaths

    return memo

def shortestpath(start,graph):
    graph["void"]=[]
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
        currentnode = "void"
        for j in init:
            if init[j]<cost and unvisitednode[j]==1:
                cost = init[j]
                currentnode = j

        connections = graph[currentnode]

        for i in range(len(connections)):
            if cost + connections[i][1] < init[connections[i][0]]:
                init[connections[i][0]] = cost + connections[i][1]
                previousvertex[connections[i][0]]=currentnode

        if currentnode!="void":
            unfinishednode.remove(currentnode)
        else:
            break
        unvisitednode[currentnode] = 0
    return previousvertex

def pathfinding(start,end,graph):
    retpath = []
    vertex = shortestpath(start,graph)
    curr = end
    while True:
        if vertex[curr]=='' and curr!=start:
            return []
        elif vertex[curr]=='' and curr==start:
            retpath.reverse()
            return retpath
        else:
            retpath.append(curr)
            curr = vertex[curr]
            

#ball - o
#rover - x
#obstacle - v
#clear - _

Map = [['_','_','_','_','_','v','_','_','_'],
       ['_','_','_','_','_','v','v','v','v'],
       ['_','_','_','_','_','v','_','_','_'],
       ['_','_','_','_','_','v','_','_','_'],
       ['_','_','_','v','x','v','_','_','_'],
       ['_','_','_','_','v','_','_','_','_'],
       ['_','_','_','_','_','_','_','_','_'],
       ['_','_','_','_','_','_','_','_','_'],
       ['_','_','_','_','_','_','_','_','_']]

print(pathfinding('4,4','5,7',tograph(Map)))
