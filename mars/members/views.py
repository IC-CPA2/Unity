#the newly written files. 
from django.shortcuts import render
from django.shortcuts import HttpResponse
from .models import all_info, map_info,live_database
from django.db import connection
from django.db.models import Q
from django.db.models import Count


# Create your views here.
def members_list_(request):
    posts = map_info.objects.all().values('map_id','date_time','map_size')
    # posts = pathHistory.objects.distinct('map_id')

    # test = posts.only('map_id')
    print(posts)
    # print(posts.query)
    # print(connection.queries)
    print(posts.values)
    print(request.POST)
    return render(request, 'output.html',{'posts':posts})

def query(request):
    if request.method == 'POST':
        search_id = request.POST["mapid"]

        grid = """<body style="background-color:#B5F0C4;">
                 </body>
                <style>
                .grid-container {
                display: grid;
                grid-template-columns: auto auto;
                gap: 10px;
                padding: 10px;
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

        try:
            image_dec = """<img id="terrain" src="static/members/media/terrain.jpg" alt="terrain" height="0" width="0"/>"""
            image_dec += """{% load static %} <img id="pink" src="{% static "blog/media/pink.png" %}" alt="pink" height="0" width="0" />"""
            image_dec += """ {% load static %} <img id="blue" src="{% static "blog/media/blue.png" %}" alt="blue" height="0" width="0" />"""
            image_dec += """ {% load static %} <img id="green" src="{% static "blog/media/green.png" %}" alt="green" height="0" width="0" />"""
            image_dec += """{% load static %} <img id="red" src="{% static "blog/media/red.png" %}" alt="red" height="0" width="0" />"""
            image_dec += """{% load static %} <img id="rover" src="{% static "blog/media/rover.png" %}" alt="logo" height="0" width="0" />"""
            image_dec += """{% load static %} <img id="undiscovered" src="{% static "blog/media/undiscovered.png" %}" alt="undiscovered" height="0" width="0" />"""

            printout = image_dec

            printout += """<div class="grid-container">"""
                    
            # posts = all_info.objects.get(map_id = search_id)
            posts = all_info.objects.filter(map_id=search_id).values()

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

            printout += """<div>"""
            for i in (n+1 for n in range(9)):
                for j in (n+1 for n in range(9)):
                    if (str(i)+str(j) in tilenum):
                        if tilenum[str(i)+str(j)] == "0":
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
                        elif tilenum[str(i)+str(j)] == "2":
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
                                                    var img = document.getElementById("rover");
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
                                                </script>"""


                printout = printout + "<br>"

            printout = printout + "</div>"

            pathlist = []
            paths = "<h4> Paths: </h4>"
            for k in posts:
                pathlist.append(k.get('path'))
                paths += k.get('path') + "<br>"

            grid2 = "<div>" + paths + "</div>"
            
            buttons = "<div>" + buttons + "</div>"

            grid4 = "<div> 4 </div>"

            return HttpResponse (grid + printout + grid2 + buttons + grid4 + "</div>")
        except map_info.DoesNotExist:
            return HttpResponse("no such user" + buttons)  
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