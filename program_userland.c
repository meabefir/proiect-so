#include <unistd.h>
#include <stdio.h>

#define m_size 100
#define indent_size 4
#define padding (indent_size / 2)

struct proc_data
{
	pid_t pid;
	pid_t ppid;
	int children_count;
	int children_pids[m_size];
	suseconds_t utime;
	suseconds_t stime;
	uint64_t uticks;
	uint64_t sticks;
	char comm[17];
	u_int flags;
	uid_t u_id;
};

int fill_line = 0;
int last_child[100];

void l(int lvl, char special_char)
{
	int actual_level = lvl;
	lvl *= indent_size;
	while (lvl)
	{
		if (lvl % indent_size == 0)
		{
			if (lvl == indent_size)
				printf("%c", (char)special_char);
			else if (!last_child[actual_level - lvl / indent_size])
				printf("|");
			else
				printf(" ");
		}
		else
		{
			if (fill_line && lvl <= indent_size - padding)
				printf("-");
			else
				printf(" ");
		}
		lvl--;
	}
}	

void print_proc_data(struct proc_data *p_data, int lvl)
{
	l(lvl, '|');printf("\n");
	fill_line = 1;
	l(lvl, '>');printf("pid: %d ppid: %d\n", p_data->pid, p_data->ppid);
	fill_line = 0;
	l(lvl, '|');printf("user ticks: %d\n", p_data->uticks);
	l(lvl, '|');printf("system ticks: %d\n", p_data->sticks);
	l(lvl, '|');printf("comm: %s\n", p_data->comm);
	l(lvl, '|');printf("flags: %d\n", p_data->flags);
	l(lvl, '|');printf("user id: %d\n", p_data->u_id);
	l(lvl, '|');printf("nr of children processes: %d\n", p_data->children_count);
	int i;
	for (i = 0; i < p_data->children_count; i++)
	{
		struct proc_data child_proc_data;
		int res = syscall(331, p_data->children_pids[i], (void*)(&child_proc_data));
		if (res == -1)
			continue;
		last_child[lvl] = i == p_data->children_count - 1;
		print_proc_data(&child_proc_data, lvl+1);
		last_child[lvl] = 0;
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("am nevoie de un pid!\n");
		return 0;
	}	

	int arg = atoi(argv[1]);
	
	struct proc_data p_data;
	int res = syscall(331, arg, (void*)(&p_data));
	if (res == -1)
		return 0;

	print_proc_data(&p_data, 0);

	return 0;	
}




