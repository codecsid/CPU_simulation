# Your Name and Section

# Queue Class for implementation of queues in the program.
class Queue:
    def __init__(self):
        self.items = []

    def front(self):
        return self.items[0]

    def isEmpty(self):
        return self.items == []

    def enqueue(self, item):
        self.items.append(item)

    def pop(self):
        return self.items.pop(0)

    def size(self):
        return len(self.items)

    def iterate(self):
        for x in self.items:
            print x,
        print ""

# Compator for sort
def comp(elem):
    return elem["frame"]

# Scheduler state function
def queue(q0, q1, q2):
    q_cur = -1
    pid_cur = -1
    if not q0.isEmpty():
        q_cur = 0
        pid_cur = q0.front()
    elif not q1.isEmpty():
        q_cur = 1
        pid_cur = q1.front()
    elif not q2.isEmpty():
        q_cur = 2
        pid_cur = q2.front()
    return (q_cur, pid_cur)

print "How much RAM do you need?"
ram_size = (int)(raw_input())
print "What is the page/frame size?"
page_size = (int)(raw_input())
print "How many harddisks you need?"
num_hdd = (int)(raw_input())

frames = ram_size/page_size

hdd = [[]for row in range(num_hdd)] #list of lists of dictionaries(pid, filename)

ram = [{}for row in range(frames)]

free_frames = Queue()

t_table = {} #dictionary of pid and t_elap

q0 = Queue()
q1 = Queue()
q2 = Queue()

pg_tbl_dic = {} # dictionary of dictionaries of dictionary(int, bool)

fr_lru_list = []  # list of dictionaries(pid, page, frame)

#Initializations
pid = 1
unallocated_frame = 0
num_proc_cpu = 0
num_proc_io = 0
pqid = -1
ppid = -1

for i in range(frames):
    free_frames.enqueue(i)

mem_addr = 0
if(page_size == 0 or page_size == 1):
    mem_addr = 1
else:
    temp = page_size - 1
    while temp > 0:
        temp = temp / 2
        mem_addr += 1

print "CPU started !!"

# Argument Loop
while True:
    user_input = raw_input()

    args = user_input.split()

    if args[0] == 'A':

        t_table[pid] = {"t_elap" : 0}

        # Find unoccupied frames in free_frames
        # If not found, get the least recently used from fr_lru_list
        # and set its valid bit to False
        if not free_frames.isEmpty():
            unallocated_frame = free_frames.front()
            free_frames.pop()
        elif not len(fr_lru_list) == 0:
            unallocated_frame = fr_lru_list[0]["frame"]
            a = fr_lru_list[0]["pid"]
            pg_tbl_dic[a][fr_lru_list[0]["page"]]["valid"] = False
            fr_lru_list.pop(0)

        # Make page table entry
        pg_tbl_dic[pid] = {0: {"frame": unallocated_frame, "valid" : True}}

        # Allocate ram frame
        ram[unallocated_frame] = {"process" : pid, "page" : 0}

        # Append in fr_lru_list
        fr_lru_list.append({"pid" : pid, "page" : 0, "frame" : unallocated_frame})
        q0.enqueue(pid)

        pqid = 0
        ppid = q0.front()
        pid += 1
        num_proc_cpu += 1

        print "Process", str(pid-1), "created."

    elif args[0] == 'Q':

        if ppid == -1:
            print "No process currently running"
            break

        # Deciding what changes to be made in queues after each time quantum
        if pqid == 0:
            q1.enqueue(ppid)
            q0.pop()
            t_table[ppid]["t_elap"] = 0
            pqid, ppid = queue(q0, q1, q2)
        elif pqid == 1:
            if t_table[ppid]["t_elap"] == 1:
                q2.enqueue(ppid)
                q1.pop()
                pqid, ppid = queue(q0, q1, q2)
                t_table[ppid]["t_elap"] = 0
            else:
                t_table[ppid]["t_elap"] += 1
        elif pqid == 2:
            t_table[ppid]["t_elap"] += 1

        print "One time quantum completed for the currently running process."

    elif args[0] == 't':
        if ppid == -1:
            print "No process currently running."
            continue

        # Popping the current process from current queue.
        if pqid == 0:
            q0.pop()
        elif pqid == 1:
            q1.pop()
        elif pqid == 2:
            q2.pop()

        # Pushing the new free frames from ram in free_frames queue
        for each_page in pg_tbl_dic[ppid]:
            free_frames.enqueue(pg_tbl_dic[ppid][each_page]["frame"])

        # Remove process entry from page table and time table
        pg_tbl_dic.pop(ppid)
        t_table.pop(ppid)

        # Removing current process entries from fr_lru_list and hdd
        fr_lru_list[:] = [d for d in fr_lru_list if d.get('pid') != ppid]

        for hd in hdd:
            hd[:] = [d for d in hd if d.get('pid') != ppid]

        print "Terminated Current Process", ppid

        pqid, ppid = queue(q0, q1, q2)

        num_proc_cpu -= 1

    elif args[0] == 'd':

        if ppid == -1 or num_proc_cpu == 0:
            print "0 running processes."
            continue

        q_no = -1
        t_table[ppid]["t_elap"] = 0

        # Finding the current queue
        if q0.front() == ppid:
            q_no = 0
        elif q1.front() == ppid:
            q_no = 1
        elif q2.front() == ppid:
            q_no = 2

        # Popping the current process from the current queue.
        if q_no == 0:
            q0.pop()
        elif q_no == 1:
            q1.pop()
        elif q_no == 2:
            q2.pop()

        # Finding the harddisk no and filename from argument
        hd_no = (int)(user_input.split()[1])
        file_name = args[2]

        # Append the current process to the respective hdd.
        hdd[hd_no].append({"pid" : ppid, "file_name": file_name})

        print "current process", ppid, "enqueued for I/O"
        pqid, ppid = queue(q0, q1, q2)

        num_proc_cpu-=1
        num_proc_io+=1

    elif args[0] == 'D':

        if num_proc_io <= 0:
            print "No process currently running."
            continue

        hd_no = (int)(user_input.split()[1])
        now_free = -1

        # Remove the current process in I/O
        # and push it to q0
        if not len(hdd[hd_no]) == 0:
            now_free = hdd[hd_no][0]["pid"]
            hdd[hd_no].pop(0)
            q0.enqueue(now_free)
            pqid, ppid = queue(q0, q1, q2)

        num_proc_cpu += 1
        num_proc_io -= 1
        print  "Process currently using I/O has finished using it."

    elif args[0] == 'm':
        if ppid == -1 or num_proc_cpu == 0:
            print "No process currently running."
            continue

        logical_address = args[1]
        page_no = ""

        # Extracting the page_no as string from logical_address
        for i in range(len(logical_address) - mem_addr):
            page_no += logical_address[i]

        # page_no string to int conversion
        page = int(page_no, 2)
        frame = -1
        index = 0
        rem_index = -1

        # if page already in page table and ram
        if page in pg_tbl_dic[ppid]:
            if pg_tbl_dic[ppid][page]["valid"] == True:
                for x in fr_lru_list:
                    if x["pid"] == ppid and x["page"] == page:
                        frame = x["frame"]
                        rem_index = index
                        break
                    index+=1

                fr_lru_list.append({"pid":ppid, "page":page, "frame":frame})
                if not rem_index == -1:
                    fr_lru_list.pop(rem_index)

                print "The page was already present."
            else:
                # If page already in page table but not in ram
                if pg_tbl_dic[ppid][page]["valid"] == False:
                    pg_tbl_dic[ppid][page]["valid"] = True
                    if not free_frames.isEmpty():
                        unallocated_frame = free_frames.front()
                        free_frames.pop()

                    else:
                        unallocated_frame = fr_lru_list[0]["frame"]
                        a = fr_lru_list[0]["pid"]
                        pg_tbl_dic[a][fr_lru_list[0]["page"]]["valid"] = False
                        fr_lru_list.pop(0)

                    pg_tbl_dic[ppid][page]["frame"] = unallocated_frame
                    fr_lru_list.append({"pid":ppid, "page":page, "frame":unallocated_frame})
                    ram[unallocated_frame] = {"process":pid, "page":page}
                    print "The required page for the currently running process has been loaded into the main memory."

        else:
            if not free_frames.isEmpty():
                unallocated_frame = free_frames.front()
                free_frames.pop()

            else:
                unallocated_frame = fr_lru_list[0]["frame"]
                a = fr_lru_list[0]["pid"]
                pg_tbl_dic[a][fr_lru_list[0]["page"]]["valid"] = False
                fr_lru_list.pop(0)

            fr_lru_list.append({"pid":ppid, "page":page, "frame":unallocated_frame})

            # If page is not present anywhere
            if page not in pg_tbl_dic[ppid]:
                pg_tbl_dic[ppid][page] = {"frame":unallocated_frame, "valid":True}
                ram[unallocated_frame] = {"process":pid, "page":page}

            print "The required page for the currently running process has been loaded into the main memory."

    if args[0] == 'S':
        if args[1][0] == 'r':
            print "The process currently using the CPU is: " + str(ppid)
            print "The processes in q0 are: ",
            q0.iterate()
            print "The processes in q1 are: ",
            q1.iterate()
            print "The processes in q2 are: ",
            q2.iterate()
        elif args[1][0] == 'i':
            print "The following processes are currently using the harddisks: "
            for i in range(num_hdd):
                if not len(hdd[i]) == 0:
                    print "Process " + str(hdd[i][0]["pid"]) + " using file " + hdd[i][0]["file_name"]

            print "The following processes are in the I/O queue: "
            for i in range(num_hdd):
                print "For harddisk " + str(i) + " the processes in the queue are:"
                for j in range(1, len(hdd[i])):
                    print str(hdd[i][j]["pid"]) + " "
        elif args[1][0] == 'm':
            print "The state of memory is as follows: "

            cur_mem = []

            for process in pg_tbl_dic:
                for page in pg_tbl_dic[process]:
                    if pg_tbl_dic[process][page]["valid"] == True:
                        cur_mem.append({"pid":process, "page":page, "frame": pg_tbl_dic[process][page]["frame"]})

            cur_mem.sort(key=comp)
            for x in cur_mem:
                print "Frame " + str(x["frame"]) + " contains page " + str(x["page"]) + " of process " + str(x["pid"])
        print ""
