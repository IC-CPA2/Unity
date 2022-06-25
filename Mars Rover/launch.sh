#!/usr/bin/env bash

python3 manage.py runserver &
python3 manage.py nests &
python3 arduino_tcp.py


