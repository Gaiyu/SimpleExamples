#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
import asyncio
import websockets
import os
import sys

img_dir = "img"
www_dir = "www"
interval = 3

def list_img(dir_name):
    for name in os.listdir(dir_name):
        if name.endswith("jpg") or name.endswith("jpeg") or name.endswith("png") or name.endswith("bmp") or name.endswith("gif"):
            yield name

async def walk_img(websocket, path):
    while True:
        imgs = list_img(img_dir)
        while True:
            try:
                f = open(img_dir + '/' + next(imgs), 'rb')
                await websocket.send(f.read())
                f.close()
            except:
                break
            else:
                await asyncio.sleep(interval)

print("*"*50)
argc=len(sys.argv)
if argc > 1:
    if os.path.exists(sys.argv[1]):
        img_dir = sys.argv[1]

if argc > 2:
    try:
        interval=int(sys.argv[2])
    except:
        pass

img_server = websockets.serve(walk_img, "0.0.0.0", 5555)
asyncio.get_event_loop().run_until_complete(img_server)
print("start websocket image server at port 5555")
print("images dir : ", img_dir)
print("display interval : ", interval)
print("*"*50)
asyncio.get_event_loop().run_forever()
