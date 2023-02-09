import pkg_resources

Import("env")
installed = {pkg.key for pkg in pkg_resources.working_set}

if 'psutil' in installed:
    pass
else:
    env.Execute("$PYTHONEXE -m pip install psutil")
