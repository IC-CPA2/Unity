#the newly written files. 
from email.mime import image
from django.shortcuts import render
from django.shortcuts import HttpResponse
from members.models import all_info, map_info,live_database
from django.db import connection
from django.db.models import Q
from django.db.models import Count
import os


# Create your views here.
def members_list_(request):
    posts = map_info.objects.all().values('map_id','date_time','map_size','map_name')
    # posts = pathHistory.objects.distinct('map_id')
    
    
    context = {
        'posts': posts,
    }

    # test = posts.only('map_id')
    print(posts)
    # print(posts.query)
    # print(connection.queries)
    print(posts.values)

    if 'reset' in request.GET:
        yes = request.GET["reset"]
        if yes == "1":
            live_database.objects.filter(last_visited=0).delete()

            
    return render(request, 'output.html',context)

def query(request):
    maxx = 0
    maxy = 0
    minx = 100
    miny = 100
    tile_numarr =[]


    # print (minx)
    # print (sizey)

    if 'mapid' in request.GET:
        search_id = request.GET["mapid"]

        tile = all_info.objects.filter(map_id_id=search_id).values_list('tile_number')
        for i in tile:
            x = str(i)[2:4]
            x = int(x)
            y = str(i)[4:6]
            y = int(y)
            tile_numarr.append(str(y)+str(x))

            print("y,x:",y,x)

            if x > maxx:
                maxx = x
            if y > maxy:
                maxy = y
            if x < minx:
                minx = x
            if y < miny:
                miny = y
        
        sizex = maxx-minx+1
        sizey = maxy-miny+1
        
        alienlog = []
        aliensquery = live_database.objects.filter(~Q(tile_info='T'), ~Q(tile_info='U'), ~Q(tile_info='R')).values_list()

        for i in aliensquery:
            tile_num = i[0]
            x = (int(tile_num[:2]) - 40)
            y = -(int(tile_num[2:]) - 40)
            tile_num = str(x) + ', ' + str(y)
            tile_info = i[1]
            if tile_info == 'BA':
                tile_info = 'Blue Alien'
            elif tile_info == 'YA':
                tile_info = 'Yellow Alien'
            elif tile_info == 'GA':
                tile_info = 'Green Alien'
            elif tile_info == 'RA':
                tile_info = 'Red Alien'
            elif tile_info == 'PA':
                tile_info = 'Pink Alien'
            elif tile_info == 'DGA':
                tile_info = 'Dark Green Alien'
            elif tile_info == 'DBA':
                tile_info = 'Dark Blue Alien'
            elif tile_info == 'W':
                tile_info = 'Building'
            elif tile_info == 'F':
                tile_info = 'Underground Infrastructure'

            display = tile_info + ": " + tile_num
            if display not in alienlog:
                alienlog.append(display)

        grid = """
                <style>
                .grid-container {
                    display: grid;
                    grid-template-columns: auto auto;
                    gap: 10px;
                    height: auto;
                }

                .grid-container > div {
                    text-align: center;
                    font-size: 20px;
                }
                </style>"""

        mapprint = all_info.objects.filter(map_id=search_id).values
        buttons = """<button onclick="location.href = '/members'">Back to Database</button>"""
        buttons += "   "
        buttons += """<button onClick="window.location.reload();">Refresh</button>"""

    
        query = map_info.objects.filter(map_id=search_id)
        
        if len(query) != 0:
            # image_dec = """<img id="terrain" src="static/members/media/terrain.jpg" alt="terrain" height="0" width="0"/>"""
            # image_dec += """<img id="alien" src="static/members/media/alien.png" alt="alien" height="0" width="0"/>"""
            # image_dec += """<img id="rover" src="static/members/media/rover.png" alt="logo" height="0" width="0"/>"""
            printout = ""
            
            printout += """<style>
                            body{
                                background-image: url("/static/blog/media/bgabout.jpg") ;
                                background-size: cover;
                                background-repeat: no-repeat;
                            }
                            </style>"""

            image_dec = """ <img id="terrain" src="static/members//media/terrain.png" %}" alt="terrain" height="0" width="0" />
                            <img id="pink" src="static/members/media/pink.png" %}" alt="pink" height="0" width="0" />
                            <img id="blue" src="static/members/media/blue.png" %}" alt="blue" height="0" width="0" />
                            <img id="green" src="static/members/media/green.png" %}" alt="green" height="0" width="0" />
                            <img id="red" src="static/members/media/red.png" %}" alt="red" height="0" width="0" />
                            <img id="orange" src="static/members/media/orange.png" %}" alt="orange" height="0" width="0" />
                            <img id="rover" src="static/members/media/rover.png" %}" alt="logo" height="0" width="0" />
                            <img id="yellow" src="static/members/media/yellow.png" %}" alt="logo" height="0" width="0" />
                            <img id="darkblue" src="static/members/media/darkblue.png" %}" alt="logo" height="0" width="0" />
                            <img id="darkgreen" src="static/members/media/darkgreen.png" %}" alt="logo" height="0" width="0" />
                            <img id="fan" src="static/members/media/fan.png" %}" alt="logo" height="0" width="0" />
                            <img id="wall" src="static/members/media/wall.png" %}" alt="logo" height="0" width="0" />
                            <img id="start" src="static/members/media/start.png" %}" alt="logo" height="0" width="0" />"""
            printout += image_dec

            printout += """<div class="grid-container">"""
                    
            # posts = all_info.objects.get(map_id = search_id)
            posts = all_info.objects.filter(map_id=search_id).values()
            # print (posts)
            #do something with user
            # for i in posts:
            #     printout = printout + str(i)
            #     printout = printout + "<br>"
            # printout = "<canvas id="myCanvas" width="50" height="50"
            #                 style="border:1px solid #000000;">
            #                 </canvas>"

            tilenum = {}
            for k in posts:
                tilenum[str(k.get('tile_number'))] = str(k.get('tile_info'))
                # print(tilenum)
            print(maxx)
            print(maxy)
            printout += """<div>"""
            for j in range(miny,maxy+1):
                for i in range(minx,maxx+1):
                    if (str(i)+str(j) in tilenum):
                        if tilenum[str(i)+str(j)] == "PA":
                            printout = printout + """<canvas id=\""""+ str(i) + str(j) +"""\"width="50" height="50"
                                                        style="border:0.5px solid #000000;">
                                                    </canvas>
                                                    <script>
                                                        var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                        var ctx = c.getContext("2d");
                                                        var img = document.getElementById("terrain");
                                                        ctx.drawImage(img,0,0,50,50);
                                                    </script>
                                                    <script>
                                                    var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                    var ctx = c.getContext("2d");
                                                    var img = document.getElementById("pink");
                                                    ctx.drawImage(img,0,0,50,50);
                                                    </script>"""
                        elif tilenum[str(i)+str(j)] == "R":
                            printout = printout + """<canvas id=\""""+ str(i) + str(j) +"""\"width="50" height="50"
                                                        style="border:0.5px solid #000000;">
                                                    </canvas>
                                                    <script>
                                                        var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                        var ctx = c.getContext("2d");
                                                        var img = document.getElementById("terrain");
                                                        ctx.drawImage(img,0,0,50,50);
                                                    </script>
                                                    """
                        elif tilenum[str(i)+str(j)] == "BA":
                            printout = printout + """<canvas id=\""""+ str(i) + str(j) +"""\"width="50" height="50"
                                                        style="border:0.5px solid #000000;">
                                                    </canvas>
                                                    <script>
                                                        var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                        var ctx = c.getContext("2d");
                                                        var img = document.getElementById("terrain");
                                                        ctx.drawImage(img,0,0,50,50);
                                                    </script>
                                                    <script>
                                                    var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                    var ctx = c.getContext("2d");
                                                    var img = document.getElementById("blue");
                                                    ctx.drawImage(img,0,0,50,50);
                                                    </script>"""
                        elif tilenum[str(i)+str(j)] == "OA":
                            printout = printout + """<canvas id=\""""+ str(i) + str(j) +"""\"width="50" height="50"
                                                        style="border:0.5px solid #000000;">
                                                    </canvas>
                                                    <script>
                                                        var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                        var ctx = c.getContext("2d");
                                                        var img = document.getElementById("terrain");
                                                        ctx.drawImage(img,0,0,50,50);
                                                    </script>
                                                    <script>
                                                    var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                    var ctx = c.getContext("2d");
                                                    var img = document.getElementById("orange");
                                                    ctx.drawImage(img,0,0,50,50);
                                                    </script>"""
                        elif tilenum[str(i)+str(j)] == "GA":
                            printout = printout + """<canvas id=\""""+ str(i) + str(j) +"""\"width="50" height="50"
                                                        style="border:0.5px solid #000000;">
                                                    </canvas>
                                                    <script>
                                                        var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                        var ctx = c.getContext("2d");
                                                        var img = document.getElementById("terrain");
                                                        ctx.drawImage(img,0,0,50,50);
                                                    </script>
                                                    <script>
                                                    var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                    var ctx = c.getContext("2d");
                                                    var img = document.getElementById("green");
                                                    ctx.drawImage(img,0,0,50,50);
                                                    </script>"""
                        elif tilenum[str(i)+str(j)] == "RA":
                            printout = printout + """<canvas id=\""""+ str(i) + str(j) +"""\"width="50" height="50"
                                                        style="border:0.5px solid #000000;">
                                                    </canvas>
                                                    <script>
                                                        var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                        var ctx = c.getContext("2d");
                                                        var img = document.getElementById("terrain");
                                                        ctx.drawImage(img,0,0,50,50);
                                                    </script>
                                                    <script>
                                                    var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                    var ctx = c.getContext("2d");
                                                    var img = document.getElementById("red");
                                                    ctx.drawImage(img,0,0,50,50);
                                                    </script>"""
                        elif tilenum[str(i)+str(j)] == "YA":
                            printout = printout + """<canvas id=\""""+ str(i) + str(j) +"""\"width="50" height="50"
                                                        style="border:0.5px solid #000000;">
                                                    </canvas>
                                                    <script>
                                                        var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                        var ctx = c.getContext("2d");
                                                        var img = document.getElementById("terrain");
                                                        ctx.drawImage(img,0,0,50,50);
                                                    </script>
                                                    <script>
                                                    var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                    var ctx = c.getContext("2d");
                                                    var img = document.getElementById("yellow");
                                                    ctx.drawImage(img,0,0,50,50);
                                                    </script>"""
                        elif tilenum[str(i)+str(j)] == "DGA":
                            printout = printout + """<canvas id=\""""+ str(i) + str(j) +"""\"width="50" height="50"
                                                        style="border:0.5px solid #000000;">
                                                    </canvas>
                                                    <script>
                                                        var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                        var ctx = c.getContext("2d");
                                                        var img = document.getElementById("terrain");
                                                        ctx.drawImage(img,0,0,50,50);
                                                    </script>
                                                    <script>
                                                    var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                    var ctx = c.getContext("2d");
                                                    var img = document.getElementById("darkgreen");
                                                    ctx.drawImage(img,0,0,50,50);
                                                    </script>"""
                        elif tilenum[str(i)+str(j)] == "DBA":
                            printout = printout + """<canvas id=\""""+ str(i) + str(j) +"""\"width="50" height="50"
                                                        style="border:0.5px solid #000000;">
                                                    </canvas>
                                                    <script>
                                                        var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                        var ctx = c.getContext("2d");
                                                        var img = document.getElementById("terrain");
                                                        ctx.drawImage(img,0,0,50,50);
                                                    </script>
                                                    <script>
                                                    var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                    var ctx = c.getContext("2d");
                                                    var img = document.getElementById("darkblue");
                                                    ctx.drawImage(img,0,0,50,50);
                                                    </script>"""
                        elif tilenum[str(i)+str(j)] == "F":
                            printout = printout + """<canvas id=\""""+ str(i) + str(j) +"""\"width="50" height="50"
                                                        style="border:0.5px solid #000000;">
                                                    </canvas>
                                                    <script>
                                                        var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                        var ctx = c.getContext("2d");
                                                        var img = document.getElementById("terrain");
                                                        ctx.drawImage(img,0,0,50,50);
                                                    </script>
                                                    <script>
                                                    var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                    var ctx = c.getContext("2d");
                                                    var img = document.getElementById("fan");
                                                    ctx.drawImage(img,0,0,50,50);
                                                    </script>"""
                        elif tilenum[str(i)+str(j)] == "W":
                            printout = printout + """<canvas id=\""""+ str(i) + str(j) +"""\"width="50" height="50"
                                                        style="border:0.5px solid #000000;">
                                                    </canvas>
                                                    <script>
                                                        var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                        var ctx = c.getContext("2d");
                                                        var img = document.getElementById("terrain");
                                                        ctx.drawImage(img,0,0,50,50);
                                                    </script>
                                                    <script>
                                                    var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                    var ctx = c.getContext("2d");
                                                    var img = document.getElementById("wall");
                                                    ctx.drawImage(img,0,0,50,50);
                                                    </script>"""
                        elif tilenum[str(i)+str(j)] == "T":
                            printout = printout + """<canvas id=\""""+ str(i) + str(j) +"""\"width="50" height="50"
                                                        style="border:0.5px solid #000000;">
                                                    </canvas>
                                                    <script>
                                                        var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                        var ctx = c.getContext("2d");
                                                        var img = document.getElementById("terrain");
                                                        ctx.drawImage(img,0,0,50,50);
                                                    </script>"""
                        else:
                            printout = printout + """<canvas id=\""""+ str(i) + str(j) +"""\"width="50" height="50"
                                                        style="border:0.5px solid #000000;">
                                                    </canvas>
                                                    <script>
                                                        var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                        var ctx = c.getContext("2d");
                                                        var img = document.getElementById("terrain");
                                                        ctx.drawImage(img,0,0,50,50);
                                                        var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                        var ctx = c.getContext("2d");
                                                        var img = document.getElementById("question");
                                                        ctx.drawImage(img,12,12,25,25);
                                                    </script>"""

                    else:
                        printout = printout + """<canvas id=\""""+ str(i) + str(j) +"""\"width="50" height="50"
                                                    style="border:0.5px solid #000000;">
                                                </canvas>
                                                <script>
                                                    var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                    var ctx = c.getContext("2d");
                                                    var img = document.getElementById("terrain");
                                                    ctx.drawImage(img,0,0,50,50);
                                                    var c = document.getElementById(\"""" + str(i) + str(j) + """\");
                                                    var ctx = c.getContext("2d");
                                                    var img = document.getElementById("question");
                                                    ctx.drawImage(img,12,12,25,25);
                                                </script>"""
                printout = printout + "<br>"

            printout = printout + """
                                                <script>
                                                    var c = document.getElementById("4040");
                                                    var ctx = c.getContext("2d");
                                                    var img = document.getElementById("start");
                                                    ctx.drawImage(img,0,0,50,50);
                                                </script>"""
            printout = printout + "</div>"
            printout += """<script>
                            window.onload = function() {
                                if(!window.location.hash) {
                                    window.location = window.location + '#loaded';
                                    window.location.reload();
                                }
                            }
                            </script>"""

            pathlist = []
            paths = "<h4> Alien Log: </h4>"
            for k in alienlog:
                paths += k + "<br>"

            grid2 = "<div>" + paths + "</div>"
            
            buttons = "<div>" + buttons + "</div>"

            grid4 = "<div>" + "</div>"

            return HttpResponse (grid + printout + grid2 + buttons + grid4 + "</div>")
        else:
            posts = map_info.objects.all().values('map_id','date_time','map_size','map_name')
            context = {
                'posts': posts,
            }

            return render(request, 'outputmodal.html',context)  
    
    else:
        return render(request, 'output.html')

def queryclean(request):
    if request.method == 'POST':
        search_id = request.POST["mapid"]

        mapprint = all_info.objects.filter(map_id=search_id).values
        
        printout = ""
        print (mapprint)
        backbutton = """<br><br><input type=button value="Back" onClick="javascript:history.go(-1);">"""
        try:
            # posts = all_info.objects.get(map_id = search_id)
            posts = all_info.objects.filter(map_id=search_id).values()
            # print (posts[0].get('tile_info'))
            #do something with user
            # for i in posts:
            #     printout = printout + str(i)
            #     printout = printout + "<br>"
            context = {
                'tiles': posts.get('tile_info')
            }

            return render(request, 'query.html', context)
        except map_info.DoesNotExist:
            return HttpResponse("no such user" + backbutton)  
    else:
        return render(request, 'output.html')