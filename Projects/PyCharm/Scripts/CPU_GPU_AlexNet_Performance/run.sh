#!/bin/sh

wget https://s3.amazonaws.com/content.udacity-data.com/nd089/Cat_Dog_data.zip
unzip Cat_Dog_data.zip

mkdir Data
mkdir Data/test
mv Cat_Dog_data/test/cat/* Data/test/
mv Cat_Dog_data/test/dog/* Data/test/

virtualenv venv
source venv/bin/activate
pip install -r requirements.txt

python PyTorchAlexNetPerformance.py -b 1 -w 8
python PyTorchAlexNetPerformance.py -b 2 -w 8
python PyTorchAlexNetPerformance.py -b 4 -w 8
python PyTorchAlexNetPerformance.py -b 8 -w 8
python PyTorchAlexNetPerformance.py -b 16 -w 8
python PyTorchAlexNetPerformance.py -b 32 -w 8
python PyTorchAlexNetPerformance.py -b 64 -w 8
python PyTorchAlexNetPerformance.py -b 128 -w 8
python PyTorchAlexNetPerformance.py -b 256 -w 8
python PyTorchAlexNetPerformance.py -b 512 -w 8
python PyTorchAlexNetPerformance.py -b 1024 -w 8
python PyTorchAlexNetPerformance.py -b 1 --gpu
python PyTorchAlexNetPerformance.py -b 2 --gpu
python PyTorchAlexNetPerformance.py -b 4 --gpu
python PyTorchAlexNetPerformance.py -b 8 --gpu
python PyTorchAlexNetPerformance.py -b 16 --gpu
python PyTorchAlexNetPerformance.py -b 32 --gpu
python PyTorchAlexNetPerformance.py -b 64 --gpu
python PyTorchAlexNetPerformance.py -b 128 --gpu
python PyTorchAlexNetPerformance.py -b 256 --gpu
python PyTorchAlexNetPerformance.py -b 512 --gpu
python PyTorchAlexNetPerformance.py -b 1024 --gpu
python PyTorchAlexNetPerformance.py -b 1 -w 8 --gpu
python PyTorchAlexNetPerformance.py -b 2 -w 8 --gpu
python PyTorchAlexNetPerformance.py -b 4 -w 8 --gpu
python PyTorchAlexNetPerformance.py -b 8 -w 8 --gpu
python PyTorchAlexNetPerformance.py -b 16 -w 8 --gpu
python PyTorchAlexNetPerformance.py -b 32 -w 8 --gpu
python PyTorchAlexNetPerformance.py -b 64 -w 8 --gpu
python PyTorchAlexNetPerformance.py -b 128 -w 8 --gpu
python PyTorchAlexNetPerformance.py -b 256 -w 8 --gpu
python PyTorchAlexNetPerformance.py -b 512 -w 8 --gpu
python PyTorchAlexNetPerformance.py -b 1024 -w 8 --gpu
