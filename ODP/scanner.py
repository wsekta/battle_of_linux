import os
import time

while 1:
    os.system('clear')
    os.system('ps aux|grep -E "Klient|Serwer|Arbiter|Hooligan"')
    time.sleep(0.2)
