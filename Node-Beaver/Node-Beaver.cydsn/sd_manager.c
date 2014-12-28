#include "sd_manager.h"



FS_FILE* pfile;
uint8_t sd_ok = 0;
DataPacket sd_queue[SD_QUEUE_LENGTH]; // sd_queue does not roll
uint16_t sd_tail = 0; // sd_queue writes when full. sd_head is always 0;



CY_ISR(power_interrupt)
{
	sd_stop();
} // CY_ISR(power_interrupt)




void sd_init(Time time)
{
	/* power_isr note:
		Triggers unexpectedly due to floating pin/environmental voltages and
		capacitance. power isr is disabled for prototyping only.
	*/
	//power_isr_StartEx(power_interrupt);
	FS_Init();
	sd_ok = 1;
	char date_str[32], run_str[64];

	if(FS_GetNumVolumes() == 1)
	{
		if(FS_ATTR_DIRECTORY != FS_GetFileAttributes("logs")) // if logs not a dir
			if(FS_MkDir("logs"))
			{
				sd_ok = 0;
				return;
			} // if logs folder can't be created

		// get time and date for naming day folder
		sprintf(date_str, "\\logs\\%u-%u-%u", time.month, time.day, time.year);

		if(FS_ATTR_DIRECTORY != FS_GetFileAttributes(date_str)) // if day not a dir
			if(FS_MkDir(date_str))
			{
				sd_ok = 0;
				return;
			} // if day folder can't be created

		sprintf(run_str, "%s\\%u-%u-%u.csv", date_str, time.hour, time.minute,
			time.second);

		pfile = FS_FOpen(run_str, "w"); // open test file

		if(pfile == NULL)
		{
			sd_ok = 0;
			return;
		} // if file does not exist

		// Set file time here
		// FS_SetFileTime()
	} // if a single file volume exists

	// test writing
	FS_FWrite("HELLO,", 1, 5, pfile);
	

	sd_stop(); // for testing
} // sd_init()



void sd_push(const DataPacket* data_queue, uint16_t data_head,
	uint16_t data_tail)
{
	if(!sd_ok)
		return;
	//push to queue


		uint16_t pos;
		for(pos=data_head; pos!=data_tail; pos=(pos+1)%DATA_QUEUE_LENGTH)
		{
			//push to queue
			sd_queue[sd_tail] = data_queue[data_head];	
			sd_tail++;

			//if queue is full, write to sd
			if(sd_tail == SD)
				sd_write();
		} // for all messages in data queue
} // sd_push()



void sd_write()
{
	// write queue to sd and clear queue
	sd_tail = 0;
} // sd_write()



void sd_stop()
{
	sd_write();
	FS_FClose(pfile);
} // sd_stop()

