#!/bin/bash

sudo opcontrol --shutdown
sudo opcontrol --deinit
sudo modprobe oprofile timer=1
sudo opcontrol --no-vmlinux
sudo opcontrol --init
sudo opcontrol --start
sudo opcontrol --status

