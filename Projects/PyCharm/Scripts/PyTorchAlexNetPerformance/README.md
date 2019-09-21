# How to run PyTorchAlexNetPerformance.py

1. Create a virtual environment to install all needed packages:
```bash
virtualenv --python=python3 venv
```

2. Activate virtual environment:
```bash
source venv/bin/activate
```

3. Install needed pip packages:
```bash
pip install -r requirements.txt
```

## Run using CPU:
```bash
python PyTorchAlexNetPerformance.py
```

## Run using GPU:
```bash
python PyTorchAlexNetPerformance.py --gpu
```
