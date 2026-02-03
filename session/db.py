import colors

colors = {
    "reset":colors.RESET,
    "INFO":colors.ITALIC + colors.TEAL,
    "WARN":colors.YELLOW,
    "ERROR":colors.RED,
    "FATAL":colors.MAGENTA
}

def MSG(type,msg):
    print(f"[{colors[type]}{type}{colors['reset']}] {msg}")

def INFO(msg):
    MSG("INFO",msg)
def WARN(msg):
    MSG("WARN",msg)
def ERROR(msg):
    MSG("ERROR",msg)
def FATAL(msg):
    MSG("FATAL",msg)
    exit(1)