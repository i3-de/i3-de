import subprocess,db

def silentOpen(args,ifno=None):
    if ifno:
        if subprocess.run(["pgrep","-x",ifno],stdout=subprocess.DEVNULL).returncode == 0:
            db.WARN(f"Process was not launched; {ifno} was running.")
            return
    return subprocess.Popen(
        args=args,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        stdin=subprocess.DEVNULL,
        start_new_session=True
    )