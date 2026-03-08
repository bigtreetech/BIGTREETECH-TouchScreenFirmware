import importlib.util

Import("env")

if importlib.util.find_spec('psutil') is None:
    env.Execute("$PYTHONEXE -m pip install psutil")
