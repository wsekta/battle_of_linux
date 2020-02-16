#!/usr/bin/python3
import os
import signal
import time

os.system("cd .. && make -s")

os.environ["CAMP"] = "/tmp/"
os.environ["RECRUIT_DLY"] = "50"
os.environ["RECRUIT_MAX"] = "1"
os.environ["RING_REG"] = "50"
os.environ["TOTEM_OP"] = "50"
os.environ["TOTEM_CL"] = "50"

#os.system("./Klient &")
os.system("./Serwer &")

for i in range(3):
    os.system("./Klient &")

input("press enter")

os.system("killall Klient")
os.system("killall Hooligan")
os.killpg(os.getpgid(os.getpid()), signal.SIGTERM)
