// Your name and Section

#include<bits/stdc++.h>
using namespace std;

// Process Control Block to hold process ID and time quantum spent.
struct PCB{int pid;int spent_quantum;};

// Multilevel Queue Scheduling which returns the process ID of currently running
// process and the queue level in which it is present
pair<int, int> MLQS(queue<int> &q0, queue<int> &q1, queue<int> &q2)
{
    int current_q = -1, current_pid = -1;
    if(!q0.empty())
    {
        current_q = 0;
        current_pid = q0.front();
    }
    else if(!q1.empty())
    {
        current_q = 1;
        current_pid = q1.front();
    }
    else if(!q2.empty())
    {
        current_q = 2;
        current_pid = q2.front();
    }
    return make_pair(current_q, current_pid);
}

int main(void)
{
    /*
    ram_size - size of ram in bytes.
    num_hdd - number of harddisks used.
    page_size - size of the page in bytes.
    pid - Process ID
    current_pid - pid of the currently running process.
    current_q - queue level corresponding to current_pid.
    user_input - command provided by the user.
    frames - no. of frames present in the ram, calculated by ram_size/page_size
    unallocated_frame = frame that is unoccupied
    num_proc_cpu - no. of processes currently present in the scheduler
    num_proc_io - no. of processes currently present in the i/o queues
    */
    int ram_size, num_hdd, page_size, pid, current_pid, current_q;
    string user_input;
    cout << "How much RAM is there on the simulated computer?" << endl;
    cin >> ram_size;
    cout << "What is the size of the page/frame?" << endl;
    cin >> page_size;
    cout << "How many harddisks does the simulated computer have?" <<endl;
    cin >> num_hdd;

    int frames = ram_size/page_size;
    int unallocated_frame;
    int num_proc_cpu = 0;
    int num_proc_io = 0;

    // State of scheduler returned by function MLQS
    // Return type (current_q, current_pid)
    pair<int, int> state;

    // Harddisk
    /*
    Vector of harddisk details. Each hard disk is in the form of list.
    The list holds process and the file used by process as a pair.
    */
    vector<list<pair<int, string>>> hdd(num_hdd);

    // RAM
    /*
    Vector of frames. Each frame is in the form of pair.
    Each pair stores process ID and the page no.
    */
    vector<pair<int, int>> ram(frames); //pid, pageno.

    // A queue of unoccupied frames in the RAM.
    queue<int> free_frames;
    for(int i = 0;i < frames;i++)
        free_frames.push(i);

    // Process Table holding the Process Control Blocks.
    map<int, PCB> process_table;

    // Multilevel queue
    queue<int> level0; // RR, Q=1
    queue<int> level1; // RR, Q=2
    queue<int> level2; // FCFS

    // Offset bits i.e. the bits remaining after the page no. in logical address
    int offset_bits = 0;
    if(page_size == 0 || page_size == 1)
        offset_bits = 1;
    else{
    // Calculating the no. of offset bits.
        int temp = page_size-1;
        while(temp > 0){temp /= 2;offset_bits++;}
    }

    // processes start from 1.
    pid = 1;

    /* Page Table Holder
     A map of page tables using process ID as key and page table as value.
    Each page table is a map using page as key and pair of respective frame and
    valid/invalid bit as value.
    */
    map<int, map<int, pair<int, bool>>> page_tables;

    // LRU Page Replacement manager queue.
    /*
    Doubly Linked List of entries containg pairs of pid made with pair of resepective
    page and frames.
    */
    list<pair<int, pair<int, int>>> get_lru;
    //        pid      page  frame

    // Beginning of simulation
    cout << "Begin the simulation by creating the first process using command A." << endl;
    scanf("\n");

    // Infinite loop to take infinite user_inputs.
    while(true)
    {
        // Take whole line as input.
        getline(cin, user_input);

        // Convert the whole line input into input stringstream
        istringstream iss(user_input);

        // Temporary word to hold each word in input
        string word;

        // vector of strings to hold each word in command
        vector<string> command;

        // Parsing each word in vector of strings.
        while(iss >> word) {
            command.push_back(word);
        }

        // Switch on first letter of first word of each command.
        switch(command[0][0])
        {
            case 'A':

                // Create a new PCB for the process.
                PCB process;
                process.pid = pid;
                process.spent_quantum = 0;

                // Appending PCB to process table map.
                process_table[pid] = process;

                // Get an unallocated frame to insert page 0.
                // If free frames are available
                if(!free_frames.empty())
                {
                    unallocated_frame = free_frames.front();
                    free_frames.pop();
                }// If free frames are not available, use LRU list
                else if(!get_lru.empty())
                {
                    // Get the frame no. from the first entry in get_lru
                    unallocated_frame = (get_lru.front().second).second;

                    // Search for the page in first entry of get_lru in page_table
                    int a = get_lru.front().first;
                    auto itr = page_tables[a].find((get_lru.front().second).first);

                    // Set the boolean to be false for the presence of the page
                    // in ram is not anymore.
                    ((*itr).second).second = false;

                    // erase the first entry from the get_lru.
                    get_lru.erase(get_lru.begin());
                }

                // Allocate memory for page #0
                page_tables[pid].insert(make_pair(0, make_pair(unallocated_frame, true)));

                // Enter the page to unallocated frame in ram.
                ram[unallocated_frame] = make_pair(pid, 0);

                // Push the current frame status with pid and page to get_lru
                get_lru.push_back(make_pair(pid, make_pair(0, unallocated_frame)));

                // Push the new process to level 0
                level0.push(pid);

                // Set the current queue as level 0.
                current_q = 0;

                // Set the current process id as the first process in level 0
                current_pid = level0.front();

                // Increment the process ID variable for next process.
                pid++;

                // No. of processes in CPU has also now increased
                num_proc_cpu++;

                cout << "New process created and enqueued at level 0." << endl;
                cout << endl;
                break;


            case 'Q':
                // Check if current_pid is -1 which may returned by MLQS on account
                // of every queue being empty.
                if(current_pid == -1){
                    cout << "No process currently running." << endl;
                    break;
                }

                /*
                If current q is 0 then after single time quantum process in its
                front will be pushed to the lower level1 and its spent_quantum
                corresponding to new queue will be set to 0.
                new current_q and current_pid are derived from MLQS according
                to the state of the three queues.
                */
                if(current_q == 0)
                {
                    level1.push(current_pid);
                    level0.pop();
                    process_table[current_pid].spent_quantum = 0;
                    state = MLQS(level0, level1, level2);
                    current_q = state.first;
                    current_pid = state.second;
                }
                /*
                If current_q is level1 then if the spent_quantum of process at
                front is 2 then it will be pushed to the next level2, otherwise
                its spent_quantum is incremented by one.
                */
                else if(current_q == 1)
                {
                    if(process_table[current_pid].spent_quantum == 2)
                    {
                        level2.push(current_pid);
                        level1.pop();
                        state = MLQS(level0, level1, level2);
                        current_q = state.first;
                        current_pid = state.second;
                        process_table[current_pid].spent_quantum += 1;
                    }
                    else
                        process_table[current_pid].spent_quantum += 1;

                }
                /* If current_q is level2 then only spent_quantum is incremented
                and the process completes whenever t is input.
                */
                else if(current_q == 2)
                    process_table[current_pid].spent_quantum += 1;
                cout << "One time quantum completed for the currently running process." << endl;
                cout << endl;
                break;


            case 't':{
                // Currently running process terminates.Release the memory used by the process.
                // When the current process terminates, enqueue another process already in queue.


                // Check if current_pid is -1 which may returned by MLQS on account
                // of every queue being empty.
                if(current_pid == -1){
                    cout << "No process currently running." << endl;
                    break;
                }

                // Memory Release using the page_tables
                // Popping the current process from the respective queue.
                switch(current_q)
                {
                    case 0:
                        level0.pop();
                        break;
                    case 1:
                        level1.pop();
                        break;
                    case 2:
                        level2.pop();
                        break;
                }

                // Remove page entries to corresponding process from the page_table
                // and the free frames added to free_frames queue.
                for(auto i = page_tables[current_pid].begin();i != page_tables[current_pid].end();i++)
                    free_frames.push((i->second).second);

                // eraes the page table corresponding to process.
                page_tables[current_pid].clear();

                // erase the corresponding page table entry for process.
                auto p = page_tables.find(current_pid);
                page_tables.erase(p);

                // erase the process control block from the process table
                auto z = process_table.find(current_pid);
                process_table.erase(z);

                // Remove resepective process entries from get_lru
                for(auto i = get_lru.begin();i != get_lru.end();++i)
                {
                    if(i->first == current_pid)
                        get_lru.erase(i--);
                }
                //get_lru.remove_if([](pair<int, pair<int, int>> n){ return n.first == current_pid; });

                // Remove process instances from the I/O queues as well
                for(int i = 0;i < num_hdd;i++)
                {
                    for(auto x = hdd[i].begin();x != hdd[i].end();x++)
                    {
                        if(x->first == current_pid)
                            hdd[i].erase(x);
                    }
                }

                // Retrieving next current_q and current_pid from MLQS.
                state = MLQS(level0, level1, level2);
                current_q = state.first;
                current_pid = state.second;

                // Decrement the no. of processes in CPU.
                num_proc_cpu--;
                cout << "Currently running process terminated."<< endl;
                cout << endl;
                break;
            }


            case 'd':{
                // Current process requests for I/O hence needs to be pre-empted
                // and enqued to the corresponding harddisk io queue.

                // Check for the validity of the command.
                if(current_pid == -1 || num_proc_cpu == 0){
                    cout << "No process currently running." << endl;
                    break;
                }

                // Retieve the queue no. of the process.
                int q_no;
                process_table[current_pid].spent_quantum = 0;
                if(level0.front() == current_pid)
                    q_no = 0;
                else if(level1.front() == current_pid)
                    q_no = 1;
                else if(level2.front() == current_pid)
                    q_no = 2;

                // Pre-empt the process from the respective queue.
                switch(q_no)
                {
                    case 0:
                        level0.pop();
                        break;
                    case 1:
                        level1.pop();
                        break;
                    case 2:
                        level2.pop();
                        break;
                }

                // Extract the hard disk no. from the command.
                int num_hdd = stoi(command[1]);

                // Extract the file name from the command.
                string file_name = command[2];

                // Push the process id with file name to the respective hdd queue.
                hdd[num_hdd].push_back(make_pair(current_pid, file_name));

                // Retrieving the new current_q and current_pid after pre-empting
                // the current process.
                state = MLQS(level0, level1, level2);
                current_q = state.first;
                current_pid = state.second;
                num_proc_cpu--;
                num_proc_io++;
                cout << "Currently running process preempted and sent for I/O."<<endl;
                cout << endl;
                break;
            }

            case 'D':{

                // Checking the validity of the command.
                if(num_proc_io <= 0){
                    cout << "No process currently running." << endl;
                    break;
                }

                // Extract the hardisk number from the command.
                int hdd_num = stoi(command[1]);

                // Variable to hold the process id of process that will be completed.
                int now_free;

                // Pushing the process present at top of respective hdd queue to the level 0.
                if(!hdd[hdd_num].empty())
                {
                    now_free = hdd[hdd_num].front().first;

                    // erase the process instance from the hdd queue.
                    hdd[hdd_num].erase(hdd[hdd_num].begin());

                    //enqueue the pid process to the scheduler.
                    level0.push(now_free);
                    state = MLQS(level0, level1, level2);
                    current_q = state.first;
                    current_pid = state.second;
                }
                num_proc_cpu++;
                num_proc_io--;
                cout << "Process currently using I/O has finished using it." << endl;
                cout << endl;
                break;
            }

            case 'm':{
                // Checking the validity of the command.
                if(current_pid == -1 || num_proc_cpu == 0){
                    cout << "No process currently running." << endl;
                    break;
                }

                // Extract binary logical address from the command.
                string logical_addresss = command[1];

                // Initiating empty page no. string.
                string page_no = "";

                // Parsing page no. from logical address to the empty page_no. string.
                for(int i = 0;i < (int)command[1].size() - offset_bits;i++)
                    page_no += command[1][i];

                // Converting the extracted binary page_no. to integer.
                int page = std::stoi(page_no, nullptr, 2);

                // Check for this page in page table for current_pid i.e.
                // if already borrowed or not
                // If already there then continue
                // else borrow the page and make an entry in page table and update the ram content

                // Find the page for corresponding process in page table.
                auto p = page_tables[current_pid].find(page);

                // Variable to hold instance of get_lru entry.
                auto l = get_lru.end();

                // If page already present in page table and valid bit is set.
                if(p != page_tables[current_pid].end() && (p->second).second == true)
                {
                    // Find the position of frame entry in get_lru
                    for(auto i = get_lru.begin();i != get_lru.end();i++)
                    {
                        if(i->first == current_pid && (i->second).first == page)
                        {
                            l = i;
                            break;
                        }
                    }

                    // Push the frame entry at the back of get_lru
                    get_lru.push_back(make_pair(current_pid, make_pair(page, (l->second).second)));

                    // Remove the frame entry from the current position.
                    if(l != get_lru.end())
                        get_lru.erase(l);

                    cout << "The page was already present." << endl;
                    break;
                }
                else
                {
                    // Getting the unallocated_frame for new frame insertion.
                    if(!free_frames.empty())
                    {
                        unallocated_frame = free_frames.front();
                        free_frames.pop();
                        get_lru.push_back(make_pair(current_pid, make_pair(page, unallocated_frame)));
                    }
                    else
                    {
                        unallocated_frame = (get_lru.front().second).second;
                        int a = get_lru.front().first;
                        auto itr = page_tables[a].find((get_lru.front().second).first);
                        ((*itr).second).second = false;
                        get_lru.push_back(make_pair(current_pid, make_pair(page, unallocated_frame)));
                        get_lru.erase(get_lru.begin());
                    }

                    // If page present in page table but valid bit not set.
                    if(p != page_tables[current_pid].end() && !(p->second).second)
                    {
                        // Set the valid bit.
                        (p->second).second = true;

			// Set the value of frame
			(p->second).first = unallocated_frame;

                        // Allocate the unallocated_frame in RAM.
                        ram[unallocated_frame] = make_pair(pid, page);
                    }

                    // If page not present in the page table.
                    if(p ==page_tables[current_pid].end())
                    {
                        // Make a new page table entry for the new page setting the valid bit
                        page_tables[current_pid].insert(make_pair(page, make_pair(unallocated_frame, true)));

                        // Allocate the unallocated frame in RAM.
                        ram[unallocated_frame] = make_pair(pid, page);
                    }
                }
                cout << "The required page for the currently running process has been loaded into the main memory." << endl;
                cout << endl;
                break;
            }


            case 'S':
                // Switch case for the second words in command.
                switch(command[1][0])
                {
                    case 'r':
                    {
                        cout << "The process currently using the CPU is: " << current_pid << endl;
                        cout << endl;

                        // Temporary queues to hold the data of the Multilevel queues.
                        queue<int> temp1, temp2, temp3;

                        cout << "The processes in the queue level0 are: ";

                        // No. of processes present in level0
                        int s = (int)level0.size();
                        for(int i = 0;i < s;i++)
                        {
                            // Push each process in temp1 and pop() from level0.
                            temp1.push(level0.front());

                            cout << level0.front() << " ";
                            level0.pop();
                        }

                        // Set the level0 to hold queue temp1
                        level0 = temp1;
                        cout << endl;

                        cout << "The processes in the queue level1 are: ";

                        // No. of processes present in level1
                        s = (int)level1.size();
                        for(int i = 0;i < s;i++)
                        {
                            // Push each process in temp2 and pop() from level1.
                            temp2.push(level1.front());

                            cout << level1.front() << " ";
                            level1.pop();
                        }
                        level1 = temp2;
                        cout << endl;

                        cout << "The processes in the queue level2 are: ";

                        // No. of processes present in level2
                        s = (int)level2.size();
                        for(int i = 0;i < s;i++)
                        {
                            // Push each process in temp3 and pop() from level2.
                            temp3.push(level2.front());

                            cout << level2.front() << " ";
                            level2.pop();
                        }
                        level2 = temp3;
                        cout << endl;
                        break;
                    }
                    case 'i':
                    {
                        // Print the state of processes in I/O.

                        cout << "The following processes are currently using the harddisks: " << endl;

                        // For each hard disk in hdd vector, process that is at front, is currently using the I/O.
                        for(int i = 0;i < num_hdd;i++)
                        {
                            if(!hdd[i].empty()) cout << "Process " << hdd[i].front().first << " is using file " << hdd[i].front().second << " in harddisk #" << i << endl;
                        }
                        cout << endl;

                        cout << "The following processes are in the I/O queue: " << endl;

                        // For each hardisk in hdd, processes except the first
                        // process in queue, are the processes in the respective
                        // queues of the harddisk.
                        for(int i = 0;i < num_hdd;i++)
                        {
                            if(!hdd[i].empty() && hdd[i].size() != 1)
                            {
                                cout << "For harddisk " << i << " the process in the queue are: " << endl;
                                auto x = hdd[i].begin();
                                x++;
                                for(;x != hdd[i].end();x++)
                                    cout << x->first << " ";
                                cout << endl;
                            }
                            else
                            // If a queue corresponding to hdd is empty or contains only one process
                            // then I/O queue corresponding to that hdd is empty.
                                cout << "No process in I/O queue for harddisk " << i << endl;
                        }

                        break;
                    }
                    case 'm':
                    {
                        // Print the state of the ram.

                        cout << "The state of memory is as follows: " << endl;

                        // Temporary array of frames, pid and page no.
                        vector<pair<int, pair<int, int>>> mem_stat;

                        // All the page tables in page table hold the frame no.
                        // corresponding to pages of process.
                        // All those page entries are fetched which valid bit set
                        // i.e. page are present in frames of RAM.
                        for(auto i = page_tables.begin();i != page_tables.end();i++)
                        {
                            for(auto x = (i->second).begin();x != (i->second).end();x++)
                            {
                                if((x->second).second)
                                    mem_stat.push_back(make_pair((x->second).first, make_pair(i->first, x->first)));
                            }
                        }

                        // Sorting the mem_stat in the order of frames.
                        sort(mem_stat.begin(), mem_stat.end());

                        for(int i = 0;i < (int)mem_stat.size();i++)
                            cout << "Frame " << mem_stat[i].first << " is occupied by page " << mem_stat[i].second.second << " of process " << mem_stat[i].second.first << endl;
                        break;
                    }
                }
                cout << endl;
                break;
            default:
                // Whenever another command is entered, a help content appears.
                cout << "Command not found !!" << endl;
                cout << endl;
                cout << "Help: " << endl;
                cout << "A                      to create a new process." << endl;
                cout << "Q                      to complete one time quantum of currently running process." << endl;
                cout << "d disknumber filename  to send currently running process for I/O for filename to hardisk disknumber." << endl;
                cout << "D disknumber           Process using I/O on harddisk disknumber finishes I/O." << endl;
                cout << "m address              Load the page with logical address as \"address\" to main memory." << endl;
                cout << "t                      terminate the currently running process." << endl;
                cout << "S r                    Print the current state of process scheduler." << endl;
                cout << "S i                    Print the current state of I/O queue." << endl;
                cout << "S m                    Print the current state of the main memory." << endl;
                break;
        }
    }
}
