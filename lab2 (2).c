
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>


#define bfs 10240
extern int errno;

struct lfile {

	char* name;
	char* path;
	long size;
	ino_t ind;
	int count;

};


struct lfile entries[bfs];
int j = 0;

char* nm;

int checkInode(int ind) {
	int i = 0;
	int result = 0;
	for (i = 0; i < j; i++)
	{
		if (entries[i].ind == ind) result = 1;
	}
	return result;
}

int checkName(char* nms) {
	int i = 0;
	int result = 0;
	for (i = 0; i < j; i++)
	{
		if (strcmp (nms, entries[i].name) == 0) 
		
		{
			entries[i].count++;
          	result = entries[i].count;
		}
	}
	 
	return result;
}


void listdir(const char *name, int level)
{

	DIR *dir;
	struct dirent *entry;

	if (!(dir = opendir(name)))
	{
		printf("%s: %s %s\n", nm, strerror(errno), name);
		return;
	}
	if (!(entry = readdir(dir)))
	{
		printf("%s: %s %s\n", nm, strerror(errno), name);
		return;
	}



	do {


		if (entry->d_type == DT_DIR) {
			char* path;
			path = (char*) malloc (strlen(name) + strlen (entry->d_name)+2);
			path[0]=0;
			strcat(path,  name);
			strcat(path,  "/");
			strcat(path,  entry->d_name);
			
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;

			listdir(path, level + 1);
		}
		else
		{
			char* path;
			path = (char*) malloc (strlen(name) + strlen (entry->d_name)+2);
			strcpy(path, name);
			strcat(path, "/");
			strcat(path, entry->d_name);

			struct stat sb;
			if (lstat(path, &sb) != -1)

			{
				if ((S_ISLNK(sb.st_mode) <= 0) && (checkInode(sb.st_ino) == 0) )

				{
					 int count=0;

					entries[j].name = (char*) malloc (strlen(entry->d_name) + 100);
					

					strcpy(entries[j].name, entry->d_name);
					
					
					 if (count = checkName (entry->d_name))
					 
					 {
						 char str[2];
						 
						 sprintf (str, "%d", count);
						 
						 strcat(entries[j].name, str);
						 
					 }
					
				
					
					
                    entries[j].path = (char*) malloc (strlen(path) + 1);
					strcpy(entries[j].path, path);
					entries[j].size = sb.st_size;
					entries[j].ind = sb.st_ino;
					entries[j].count = 0;

					j++;
				}
			}
			else
			{
				printf("%s: %s %s\n", nm, strerror(errno), path);
			}


		}
	} while (entry = readdir(dir));
	if (closedir(dir) == -1) printf("%s: %s %s\n", nm, strerror(errno), name);
}


int cmp1(const void* p1, const void* p2)
{
	struct lfile *pa = p1;
	struct lfile *pb = p2;

	return (pb->size - pa->size);

}

int cmp2(const void* p1, const void* p2)
{
	struct lfile *pa = p1;
	struct lfile *pb = p2;

	return strcmp(pa->name, pb->name);

}




int main(int argc, char** argv)
{
	int i = 0;
	nm = (char*) malloc (strlen(argv[0]) + 1);
	nm[0] = 0;
	strcat(nm, argv[0]);


	for (i = 0; i < strlen(nm); i++)
	{
		nm[i] = nm[i + 2];
	}


	listdir(argv[1], 0);



	switch (argv[2][0])
	{
	case '1':
	{
		qsort(entries, j, sizeof(struct lfile), cmp1);
		break;
	}
	case '2':
	{
		qsort(entries, j, sizeof(struct lfile), cmp2);
		break;
	}
	default:
	{
		qsort(entries, j, sizeof(struct lfile), cmp2);
	}
	}




	char* arg;
	arg = (char*) malloc (strlen(argv[3]) + 1);
	arg[0] = 0;
	strcpy(arg, argv[3]);
	char* dest;
    dest = (char*) malloc (strlen(arg) + 2);
    dest[0] = 0;
	strcat(dest, arg);
	strcat(dest, "/");




	for (i = 0; i < j; i++) {



		strcat(dest, entries[i].name);

		int in_fd = open(entries[i].path, O_RDONLY);

		if (in_fd != -1)
		{

			mode_t md = S_IRUSR | S_IWUSR | S_IXUSR;
			int out_fd = creat(dest, md);

			if (out_fd != -1)
			{
				char buf[bfs /10 * 4];

				while (1) {
					ssize_t result = read(in_fd, &buf[0], sizeof(buf));
					if (!result) break;
					if (write(out_fd, &buf[0], result) == -1)
					{
						printf("%s: %s %s\n", nm, strerror(errno), entries[i].path);
						break;
					}
				}
				
				if (close(out_fd) == -1) printf("%s: %s %s\n", nm, strerror(errno), dest);
			}
			else
			{
				printf("%s: %s %s\n", nm, strerror(errno), dest);
			}


			
			if (close(in_fd) == -1) printf("%s: %s %s\n", nm, strerror(errno), entries[i].path);

		}
		else
		{
			printf("%s: %s %s\n", nm, strerror(errno), entries[i].path);

		}

		int k = strlen(dest) - 1;
		while (dest[k] != '/') k--;
		dest[k + 1] = 0;
	}

	return 0;
}


