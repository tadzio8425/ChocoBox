import requests

ESP32_IP = "192.168.1.1"

url = "http://{}/upload".format(ESP32_IP)
fin = open("eusebio.txt", 'rb')
files = {'file': fin}
try:
    r = requests.post(url, files=files)
finally:
    fin.close()