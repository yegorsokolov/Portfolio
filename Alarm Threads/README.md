Contributors:
First/Last Name ... YorkU id
Yegor Sokolov ... 220392288
Muddassir Choudary ... 216536310
Omar Shaikh ... 218749382
Mohammad Zarif ... 217444340

1. First copy the files "alarm_mutex.c" and "errors.h" into your own directory.

2. To compile the program "alarm_mutex.c" using gcc, use the following command:

      gcc new_alarm_mutex.c -lpthread (Linux)
      gcc new_alarm_mutex.c -lpthread -o a.exe (Windows)

   This will produce an executable file named "a.exe" in the same directory as "alarm_mutex.c".

3. Type "a.exe" (rather than "a.out") to run the executable code on Windows:

      ./a.out (Linux)
      .\a.exe (Windows)

4. You will see a prompt of the form:

      alarm>

   Now type the commands and observe the expected outputs. Below is an example script of test commands and corresponding outputs.

-------------------------------------------------------------------------------
Commands and Expected Output
-------------------------------------------------------------------------------

alarm> Start_Alarm(1001): 30 Meeting at 5 PM
Start_Alarm(1001) Request Inserted Into Alarm List: 1741577149 30 Meeting at 5 PM
New Display Alarm Thread created at 1741577152
Alarm (1001) Assigned to Display Thread(12345) at 1741577152: 1741577182 Meeting at 5 PM

alarm> Change_Alarm(1001): 60 Meeting at 6 PM
Change_Alarm(1001) Request Inserted Into Alarm List: 1741577187 60 Meeting at 6 PM
Alarm(1001) Changed at 1741577190: 1741577250 Meeting at 6 PM

alarm> Cancel_Alarm(1001)
Cancel_Alarm(1001) Request Inserted Into Alarm List: 1741577135 N/A
Alarm(1001) Cancelled at 1741577140: 1741577250 Meeting at 6 PM

alarm> Suspend_Alarm(1001)
Suspend_Alarm(1001) Request Inserted Into Alarm List: 1741577190 N/A
Alarm(1001) Suspended at 1741577192: 1741577250 Meeting at 5 PM

alarm> Reactivate_Alarm(1001)
Reactivate_Alarm(1001) Request Inserted Into Alarm List: 1741577207 N/A
Alarm(1001) Reactivated at 1741577210: 1741577250 Meeting at 5 PM

alarm> View_Alarms
View_Alarms Request Inserted Into Alarm List: 1741577218
View Alarms at 1741577220:
1. Alarm(1001): Type<Start_Alarm> 1741577149 30 Meeting at 5 PM
2. Alarm(1002): Type<Change_Alarm> ...
(etc. if any)

alarm>
Exiting Program.

-------------------------------------------------------------------------------