#define MAX_TOKEN_SIZE 100
#define MEM_SIZE 1000
#define FRAME_SIZE 3

void mem_init();
char *mem_get_value(char *var);
void mem_set_value(char *var, char *value);
int getNextAvailablePage();
void mem_load_line(int index, char *var_in, char *value_in);
void mem_remove_value(char *scriptName, int length);
char *mem_get_value_at_index(int index);
