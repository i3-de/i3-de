import argparse,shutil,db,os,util

p = argparse.ArgumentParser(
    prog="i3de-sensible",
    description="Starts the 'sensible' program for a specific program type."
)

p.add_argument("type",
               help="Type of program",
               choices=[
                   "bar",
                   "compositor",
                   "notify"
               ])

args = p.parse_args()

match args.type:
    case "bar":
        env_path = "DE_DEF_BAR"
        choices = ["polybar","lemonbar","dzen2","i3bar"]
    case "compositor":
        env_path = "DE_DEF_COMPOSITOR"
        choices = ["picom","xcompmgr"]
    case "notify":
        env_path = "DE_DEF_NOTIFY"
        choices = ["twmn","dunst"]
    case _:
        db.FATAL("The type you specified has not been implemented.")

chose = None
if os.environ.get(env_path):
    chose = os.environ.get(env_path)
else:
    for i in choices:
        if shutil.which(i) != None:
            chose = i
            break
if chose == None:
    db.FATAL("Could not find a sensible bar.")
db.INFO(chose)

args = []

match chose:
    case _:
        pass

util.silentOpen([chose] + args,ifno=chose)
exit(0)