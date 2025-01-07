#!/bin/bash

make

return_val=$(./idle_and_audio)

sudo ./backlight $return_val #argument outputted by idle_and_audio == $return_val
