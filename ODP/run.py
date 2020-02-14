#!/usr/bin/python3
import os
import signal

os.system("cd .. && make -s")

os.environ["CAMP"] = "/tmp/"
os.environ["RECRUIT_DLY"] = "1"
os.environ["RECRUIT_MAX"] = "5"
os.environ["RING_REG"] = "1"
os.environ["TOTEM_OP"] = "1"
os.environ["TOTEM_CL"] = "1"

#os.system("./Klient &")

os.system("./Serwer &")

for i in range(5):
    os.system("./Klient &")

input("press enter")

os.killpg(os.getpgid(os.getpid()), signal.SIGTERM)
