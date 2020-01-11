# Class that holds all the information about the procces. 
class procStatus:
    name = ""
    groups = []
    ppid = 0
    threads = 0 
    umask = ""

    # Override default str method to enable using print() method on this class
    def __str__(self):
        return "Name: " + self.name + ", Groups: " + str(self.groups) + ", PPid: " + str(self.ppid) + ", Threads: " + str(self.threads) + ", Umask: " + self.umask


# Builds a path for the status file of the requested process
def __proc_status_path(pid):
    return "/proc/" + str(pid) + "/status"


# Parsing method for each attribute we want to get about the process
def __parse_name(line, proc_status):
    proc_status.name = line.split()[1]

def __parse_groups(line, proc_status):
    words = line.split()
    for i in range(1, len(words)):
        proc_status.groups.append(int(words[i]))

def __parse_ppid(line, proc_status):
    proc_status.ppid = int(line.split()[1])

def __parse_threads(line, proc_status):
    proc_status.threads = int(line.split()[1])

def __parse_umask(line, proc_status):
    proc_status.umask = line.split()[1]


# Dictionary that is used to route the relevent line from the status file to the correct parser function
proc_param_parsers = {
    "Name:" : __parse_name,
    "Umask:" : __parse_umask,
    "PPid:" : __parse_ppid,
    "Groups:" : __parse_groups,
    "Threads:" : __parse_threads,
}


# This method builds and returns a procStatus class with all the attributes of the requested process (by pid)
def get_process_info(pid):
    with open(__proc_status_path(pid)) as proc_file:
        proc_status = procStatus()
        parsers = dict(proc_param_parsers)

        # read the status file line-by-line and parse the relevent attributes
        for line in proc_file.readlines():
            key_word = line.split()[0]
            if key_word in parsers:
                parsers[key_word](line, proc_status)
                del parsers[key_word]
        
        # verify that all attributes were found and return the complete procStatus class
        if len(parsers) != 0:
            print("Error: failed to find the following process attributes:")
            for key in parsers.keys():
                print(key)
            raise NameError("Process attribute(s) not found")
        else:
            return proc_status