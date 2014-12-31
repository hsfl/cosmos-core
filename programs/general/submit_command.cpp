#include "configCosmos.h"
#include "datalib.h"
#include "jsonlib.h"

int main(int argc, char *argv[])
{
	longeventstruc com;

	com.type = EVENT_TYPE_COMMAND;
	com.flag = 0;
	com.data[0] = 0;
	com.condition[0] = 0;
	com.utc = 0;
	com.utcexec = 0.;

	switch (argc)
	{
	case 6:
		{
			com.flag |= EVENT_FLAG_REPEAT;
		}
	case 5:
		{
			strcpy(com.condition, argv[4]);
			com.flag |= EVENT_FLAG_CONDITIONAL;
		}
	case 4:
		{
			switch (argv[3][0])
			{
			case '+':
				{
					double seconds = atof(&argv[3][1]);
					com.utc = currentmjd() + seconds / 86400.;
					break;
				}
			default:
				{
					com.utc = atof(argv[3]);
					break;
				}
			}
		}
	case 3:
		{
			strcpy(com.data, argv[2]);
		}
	case 2:
		{
			strcpy(com.name, argv[1]);
			break;
		}
	default:
		{
			printf("Usage: submit_command name command_string [time [condition [repeat_flag]]]\n");
			break;
		}
	}

	jstring js = {0,0,0};

	json_startout(&js);
	json_out_commandevent(&js, com);
	json_stopout(&js);
	printf("%s\n", js.string);
}
